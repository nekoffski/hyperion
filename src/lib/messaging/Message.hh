#pragma once

#include "Payload.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/RTTI.hh"

namespace hyperion {

template <typename MessageKind>
class Message : public RTTI {
   public:
    virtual ~Message() = default;

    virtual std::string_view name() const = 0;
    virtual MessageKind kind() const = 0;

    virtual void serialize(PayloadWriter& writer) const = 0;
    virtual void deserialize(PayloadReader& reader) = 0;
};

template <typename T, typename MessageKind, MessageKind Kind>
class MessageImpl : public Message<MessageKind> {
   public:
    explicit MessageImpl(const std::string& name) : m_name(name) {}

    MessageKind kind() const override { return Kind; }
    std::type_index type() const override { return typeid(T); }
    std::string_view name() const override { return m_name; }

   private:
    std::string m_name;
};

}  // namespace hyperion
