#include "Server.hh"

#include "internal/api/MessageDeserializer.hh"
#include "internal/api/Session.hh"
#include "internal/api/cmd/Error.hh"
#include "internal/core/Log.hh"
#include "internal/core/Time.hh"
#include "internal/net/TcpSession.hh"

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

            if (not message) {
                log::error("{} - could deserialize message", session.ident());

                api::ErrorResponse response{
                    "Serialization error, message not registered"
                };
                co_await session.write(response);
            } else {
                auto response =
                    co_await m_apiController.handleMessageWithError(*message);
                log::expect(
                    response != nullptr,
                    "API controller returned null response for client: {}",
                    session.ident()
                );

                co_await session.write(*response);
            }
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
