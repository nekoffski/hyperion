#include "Server.hh"

#include "lib/core/Log.hh"
#include "lib/core/Time.hh"
#include "lib/net/Session.hh"

namespace hyperion {

Server::Server(const Config& config)
    : m_config(config),
      m_acceptor(m_io, {asio::ip::tcp::v4(), config.daemon().port}) {}

Server::~Server() { m_io.stop(); }

void Server::cancel() {
    log::info("stopping Server...");
    m_io.stop();
}

void Server::run() {
    log::info(
        "Server thread starting, listening on port {}", m_config.daemon().port
    );
    startAcceptor();

    log::debug("ASIO context running");
    m_io.run();
}

asio::awaitable<void> Server::handleClient(asio::ip::tcp::socket socket) {
    Session session{std::move(socket)};

    for (;;) {
        try {
            auto buffer = co_await session.read();

        } catch (const NetError& e) {
            log::error(
                "{} - Received malformed message, dropping session",
                session.ident()
            );
            break;
        }
    }
}

void Server::startAcceptor() {
    asio::co_spawn(
        m_io,
        [this]() -> asio::awaitable<void> {
            for (;;) {
                try {
                    co_await acceptConnection();
                } catch (const asio::system_error& e) {
                    log::error("error accepting connection: {}", e.what());
                    break;
                } catch (const std::exception& e) {
                    log::error(
                        "unexpected error accepting connection: {}", e.what()
                    );
                    break;
                }
            }
        },
        asio::detached
    );
}

asio::awaitable<void> Server::acceptConnection() {
    asio::ip::tcp::socket socket{m_io};
    co_await m_acceptor.async_accept(socket, asio::use_awaitable);

    asio::co_spawn(
        m_io,
        [this, socket = std::move(socket)]() mutable -> asio::awaitable<void> {
            auto ident = socket.remote_endpoint().address().to_string();
            log::info("{} - accepted connection", ident);

            try {
                co_await handleClient(std::move(socket));
            } catch (const Exception& e) {
                log::error(
                    "{} - error in client session: {} - {}", ident,
                    fmt::underlying(e.code()), e.message()
                );
            } catch (const asio::system_error& e) {
                if (e.code() == asio::error::eof) {
                    log::info("{} - Client disconnected", ident);
                } else {
                    log::error(
                        "{} - ASIO error in client session: {}", ident, e.what()
                    );
                }
            } catch (const std::exception& e) {
                log::error(
                    "{} - unexpected error in client session: {}", ident,
                    e.what()
                );
            }
        },
        asio::detached
    );
}

}  // namespace hyperion