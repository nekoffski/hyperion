#pragma once

#include <span>
#include <vector>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/Error.hh"
#include "lib/core/FlatMap.hh"
#include "lib/core/Log.hh"
#include "lib/core/Scope.hh"

namespace hyperion {

DEFINE_SUB_ERROR(PayloadError, CoreError);

namespace detail {

template <typename T>
struct isVector : std::false_type {};
template <typename T>
struct isVector<std::vector<T>> : std::true_type {};

template <typename T>
struct isFlatMap : std::false_type {};
template <typename K, typename V>
struct isFlatMap<FlatMap<K, V>> : std::true_type {};

}  // namespace detail

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

    template <typename T>
    PayloadWriter& write(const std::vector<T>& v) {
        log::expect(
            v.size() <= std::numeric_limits<u32>::max(),
            "Vector too long to serialize"
        );
        write(static_cast<u32>(v.size()));
        for (const auto& item : v) {
            write(item);
        }
        return *this;
    }

    template <typename K, typename V>
    PayloadWriter& write(const FlatMap<K, V>& m) {
        log::expect(
            m.size() <= std::numeric_limits<u32>::max(),
            "FlatMap too large to serialize"
        );
        write(static_cast<u32>(m.size()));
        for (const auto& [k, v] : m) {
            write(k);
            write(v);
        }
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
        requires detail::isVector<T>::value
    T read() {
        const auto count = read<u32>();
        T result;
        result.reserve(count);
        for (u32 i = 0; i < count; ++i) {
            result.push_back(read<typename T::value_type>());
        }
        return result;
    }

    template <typename T>
        requires detail::isFlatMap<T>::value
    T read() {
        using KeyType = typename T::value_type::first_type;
        using ValueType = typename T::value_type::second_type;
        const auto count = read<u32>();
        T result;
        result.reserve(count);
        for (u32 i = 0; i < count; ++i) {
            result.insert(read<KeyType>(), read<ValueType>());
        }
        return result;
    }

    template <typename T>
        requires(
            std::is_arithmetic_v<T> || std::is_same_v<T, std::string> ||
            (std::is_default_constructible_v<T> &&
             requires(PayloadReader& bw, T& t) { deserialize(bw, t); }) ||
            detail::isVector<T>::value || detail::isFlatMap<T>::value
        )
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
