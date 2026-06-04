#pragma once

#include <memory>
#include <unordered_map>

#include "Message.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Error.hh"

namespace hyperion {

template <typename T, typename MessageKind>
concept MessageConcept = std::derived_from<T, Message<MessageKind>> &&
                         std::is_enum_v<MessageKind> && requires(T t) {
                             { t.sKind() } -> std::convertible_to<MessageKind>;
                         };

template <typename MessageKind>
    requires std::is_enum_v<MessageKind>
class MessageDeserializer : public NonCopyable, public NonMovable {
    using MessagePtr = std::unique_ptr<Message<MessageKind>>;
    using DeserializerCallback = std::function<MessagePtr(PayloadReader&)>;

   public:
    MessagePtr deserialize(PayloadReader& reader) const {
        const auto rawKind = reader.read<u16>();

        if (rawKind >= static_cast<u16>(MessageKind::count)) {
            throw MessagingError{
                ErrorCode::invalidMessageKind, "Invalid message kind: {}",
                rawKind
            };
        }

        const auto kind = static_cast<MessageKind>(rawKind);

        if (auto it = m_deserializers.find(kind); it != m_deserializers.end()) {
            return it->second(reader);
        }

        throw MessagingError{
            ErrorCode::deserializerNotFound,
            "No deserializer registered for message kind: {}", rawKind
        };
    }

   protected:
    template <typename T>
        requires MessageConcept<T, MessageKind>
    void registerMessage() {
        auto kind = T::sKind();
        log::expect(
            not m_deserializers.contains(kind),
            "Message kind already registered: {}", static_cast<int>(kind)
        );
        m_deserializers[kind] = [](PayloadReader& reader) -> MessagePtr {
            auto message = std::make_unique<T>();
            message->deserialize(reader);
            return message;
        };
    }

   private:
    std::unordered_map<MessageKind, DeserializerCallback> m_deserializers;
};

}  // namespace hyperion
