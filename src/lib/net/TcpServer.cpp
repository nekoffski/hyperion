#include "TcpServer.hh"

namespace hyperion {

TcpServer::TcpServer(asio::io_context& io, uint16_t port)
    : m_io(io), m_acceptor(io, {asio::ip::tcp::v4(), port}) {}

void TcpServer::asyncAccept() {
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

asio::awaitable<void> TcpServer::acceptConnection() {
    asio::ip::tcp::socket socket{m_io};
    co_await m_acceptor.async_accept(socket, asio::use_awaitable);

    asio::co_spawn(
        m_io,
        [this, socket = std::move(socket)]() mutable -> asio::awaitable<void> {
            auto ident = socket.remote_endpoint().address().to_string();
            log::info("{} - accepted connection", ident);

            try {
                TcpSession session(std::move(socket));
                co_await onClient(session);
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