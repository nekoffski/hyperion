#pragma once

#include <memory>
#include <unordered_map>

#include "Message.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Error.hh"

namespace hyperion {

template <typename T, typename MessageKind>
concept MessageConcept = std::derived_from<T, Message<MessageKind>> &&
                         std::is_enum_v<MessageKind> && requires(T t) {
                             {
                                 t.kindValue
                             } -> std::convertible_to<MessageKind>;
                         };

template <typename MessageKind>
    requires std::is_enum_v<MessageKind>
class MessageDeserializer : public NonCopyable, public NonMovable {
    using MessagePtr = std::unique_ptr<Message<MessageKind>>;
    using DeserializerCallback = std::function<MessagePtr(PayloadReader&)>;

   public:
    MessagePtr deserialize(PayloadReader& reader) const {
        const auto rawKind = reader.read<MessageKindUnderlying>();

        if (rawKind >= static_cast<MessageKindUnderlying>(MessageKind::count)) {
            throw MessagingError{
                ErrorCode::invalidMessageKind, "Invalid message kind: {}",
                rawKind
            };
        }

        const auto kind = static_cast<MessageKind>(rawKind);

        if (auto it = m_deserializers.find(kind); it != m_deserializers.end()) {
            return it->second(reader);
        }

        return nullptr;
    }

   protected:
    template <typename T>
        requires MessageConcept<T, MessageKind>
    void registerMessage() {
        auto kind = T::kindValue;
        log::expect(
            not m_deserializers.contains(kind),
            "Message kind already registered: {}", fmt::underlying(kind)
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
