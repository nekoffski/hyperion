#pragma once

#include "Payload.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/RTTI.hh"

namespace hyperion {

template <typename MessageKind>
    requires std::is_enum_v<MessageKind>
class Message : public RTTI {
   public:
    virtual ~Message() = default;

    virtual std::string_view name() const = 0;
    virtual MessageKind kind() const = 0;

    virtual void serialize(PayloadWriter& writer) const = 0;
    virtual void deserialize(PayloadReader& reader) = 0;
};

template <typename T, typename MessageKind, MessageKind MKind>
    requires std::is_enum_v<MessageKind>
class MessageImpl : public Message<MessageKind> {
   public:
    using Kind = MessageKind;

    explicit MessageImpl(const std::string& name) : m_name(name) {}

    static MessageKind sKind() { return MKind; }
    MessageKind kind() const final override { return MKind; }
    std::type_index type() const final override { return typeid(T); }
    std::string_view name() const final override { return m_name; }

   private:
    std::string m_name;
};

}  // namespace hyperion
