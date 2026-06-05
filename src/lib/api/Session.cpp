#include "Session.hh"

namespace hyperion::api {

ApiSession::ApiSession(TcpSession& session) : m_session(session) {}

std::string_view ApiSession::ident() const { return m_session.ident(); }

asio::awaitable<std::unique_ptr<ApiMessage>> ApiSession::read(
    ApiMessageDeserializer& deserializer
) {
    auto buffer = co_await m_session.read();
    PayloadReader reader{buffer};
    co_return deserializer.deserialize(reader);
}

asio::awaitable<void> ApiSession::write(const ApiMessage& message) {
    PayloadWriter writer;
    message.serialize(writer);
    co_await m_session.write(writer.getBuffer());
}

}  // namespace hyperion::api