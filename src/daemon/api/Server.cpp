#include "Server.hh"

#include "lib/api/MessageDeserializer.hh"
#include "lib/api/Session.hh"
#include "lib/core/Log.hh"
#include "lib/core/Time.hh"
#include "lib/net/TcpSession.hh"

namespace hyperion {

Server::Server(const Config& config)
    : TcpServer(io(), config.daemon().port), m_config(config) {}

Server::~Server() { io().stop(); }

void Server::cancel() {
    log::info("stopping Server...");
    io().stop();
}

void Server::run() {
    log::info(
        "Server thread starting, listening on port {}", m_config.daemon().port
    );
    TcpServer::asyncAccept();

    log::debug("ASIO context running");
    io().run();
}

asio::awaitable<void> Server::onClient(TcpSession& s) {
    auto& deserializer = api::ApiMessageDeserializer::get();
    api::ApiSession session{s};

    for (;;) {
        try {
            auto message = co_await session.read(deserializer);
            log::expect(
                message != nullptr, "Received null message from client: {}",
                session.ident()
            );

            auto response = co_await m_apiController.handleMessage(*message);
            log::expect(
                response != nullptr,
                "API controller returned null response for client: {}",
                session.ident()
            );

            co_await session.write(*response);

        } catch (const NetError& e) {
            log::error(
                "{} - Received malformed message, dropping session - {}",
                session.ident(), e.message()
            );
            break;
        } catch (const MessagingError& e) {
            log::error(
                "{} - Serialization error: {}, dropping request",
                session.ident(), e.message()
            );
        }
    }
}

}  // namespace hyperion
