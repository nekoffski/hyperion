#include "ApiServer.hh"

#include "lib/core/Log.hh"
#include "lib/core/Time.hh"

namespace hyperion {

ApiServer::ApiServer(const Config& config)
    : m_config(config),
      m_acceptor(m_io, {asio::ip::tcp::v4(), config.daemon().port}) {}

void ApiServer::cancel() {
    log::info("stopping ApiServer...");
    m_io.stop();
}

void ApiServer::run() {
    log::info("ApiServer thread starting");

    asio::co_spawn(
        m_io,
        [this]() -> asio::awaitable<void> {
            while (true) {
                asio::ip::tcp::socket socket(m_io);
                co_await m_acceptor.async_accept(socket, asio::use_awaitable);
                log::info(
                    "accepted connection from {}",
                    socket.remote_endpoint().address().to_string()
                );
            }
        },
        asio::detached
    );

    m_io.run();
}

}  // namespace hyperion