#pragma once

#include "Payload.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/core/RTTI.hh"

namespace hyperion {

using MessageKindUnderlying = u16;

template <typename MessageKind>
    requires std::is_enum_v<MessageKind>
class Message : public RTTI {
   public:
    virtual ~Message() = default;

    virtual std::string_view name() const = 0;
    virtual MessageKind kind() const = 0;

    virtual void serialize(PayloadWriter& writer) const {
        writer.write(static_cast<MessageKindUnderlying>(kind()));
        serializeImpl(writer);
    }
    virtual void deserialize(PayloadReader& reader) { deserializeImpl(reader); }

   private:
    virtual void serializeImpl(PayloadWriter& writer) const = 0;
    virtual void deserializeImpl(PayloadReader& reader) = 0;
};

template <typename T, typename MessageKind, MessageKind MKind>
    requires std::is_enum_v<MessageKind>
class MessageImpl : public Message<MessageKind> {
   public:
    using Kind = MessageKind;
    constexpr static MessageKind kindValue{MKind};

    explicit MessageImpl(const std::string& name) : m_name(name) {}

    MessageKind kind() const final override { return MKind; }
    std::type_index type() const final override { return typeid(T); }
    std::string_view name() const final override { return m_name; }

   private:
    std::string m_name;
};

}  // namespace hyperion
