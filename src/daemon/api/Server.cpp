#include "Server.hh"

#include "lib/api/ApiMessageDeserializer.hh"
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

asio::awaitable<void> Server::onClient(TcpSession& session) {
    auto& deserializer = ApiMessageDeserializer::get();

    for (;;) {
        try {
            auto buffer = co_await session.read();
            PayloadReader reader{buffer};

            auto message = deserializer.deserialize(reader);

        } catch (const NetError& e) {
            log::error(
                "{} - Received malformed message, dropping session",
                session.ident()
            );
            break;
        }
    }
}

}  // namespace hyperion
