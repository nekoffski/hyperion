#pragma once

#include <span>
#include <vector>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/Error.hh"
#include "lib/core/Log.hh"
#include "lib/core/Scope.hh"

namespace hyperion {

DEFINE_SUB_ERROR(PayloadError, CoreError);

using PayloadBuffer = std::vector<u8>;
using PayloadBufferView = std::span<u8>;

class PayloadWriter : public NonCopyable, public NonMovable {
   public:
    PayloadBufferView getBuffer();

    template <typename T>
        requires std::is_arithmetic_v<T>
    PayloadWriter& write(T v) {
        write(&v, sizeof(T));
        return *this;
    }

    template <typename T>
        requires requires(PayloadWriter& bw, const T& t) { serialize(bw, t); }
    PayloadWriter& write(const T& v) {
        serialize(*this, v);
        return *this;
    }

    PayloadWriter& write(const std::string& v) {
        auto size = v.size();
        log::expect(
            size <= std::numeric_limits<u8>::max(),
            "String too long to serialize"
        );

        write(&size, 1);
        write(v.data(), size);

        return *this;
    }

   private:
    template <typename T>
    void write(const T* ptr, u8 bytes) {
        const auto bufferSize = m_buffer.size();
        m_buffer.resize(bufferSize + bytes);
        std::memcpy(m_buffer.data() + bufferSize, ptr, bytes);
    }

    PayloadBuffer m_buffer;
};

class PayloadReader : public NonCopyable, public NonMovable {
   public:
    explicit PayloadReader(PayloadBufferView view);

    template <typename T>
        requires std::is_arithmetic_v<T>
    T read() {
        return *reinterpret_cast<T*>(readImpl(sizeof(T)));
    }

    template <typename T>
        requires std::is_same_v<T, std::string>
    T read() {
        const auto size = read<u8>();
        return std::string{reinterpret_cast<char*>(readImpl(size)), size};
    }

    template <typename T>
        requires(
            std::is_default_constructible_v<T> &&
            requires(PayloadReader& bw, T& t) { deserialize(bw, t); }
        )
    T read() {
        T out;
        deserialize(*this, out);
        return out;
    }

    template <typename T>
        requires requires(PayloadReader& br) {
            { br.read<T>() } -> std::same_as<T>;
        }
    PayloadReader& read(T& out) {
        out = read<T>();
        return *this;
    }

   private:
    u8* readImpl(u8 bytes);

    u64 m_index{0u};
    PayloadBufferView m_buffer;
};

}  // namespace hyperion
