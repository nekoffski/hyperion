#pragma once

#include "Message.hh"
#include "MessageDeserializer.hh"
#include "internal/core/Concepts.hh"
#include "internal/net/TcpSession.hh"

namespace hyperion::api {

class ApiSession : public NonCopyable, public NonMovable {
   public:
    explicit ApiSession(TcpSession& session);

    std::string_view ident() const;

    asio::awaitable<std::unique_ptr<ApiMessage>> read(
        ApiMessageDeserializer& deserializer
    );
    asio::awaitable<void> write(const ApiMessage& message);

   private:
    TcpSession& m_session;
};

}  // namespace hyperion::api
