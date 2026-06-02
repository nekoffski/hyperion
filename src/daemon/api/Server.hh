#pragma once

#include "lib/asio/Asio.hh"
#include "lib/core/Config.hh"
#include "lib/runtime/Thread.hh"

namespace hyperion {

class Server : public Thread {
   public:
    explicit Server(const Config& config);
    ~Server() override;

    void cancel() override;

   private:
    void run() override;

    void startAcceptor();
    asio::awaitable<void> acceptConnection();
    asio::awaitable<void> handleClient(asio::ip::tcp::socket socket);

    const Config& m_config;

    asio::io_context m_io;
    asio::ip::tcp::acceptor m_acceptor;
};

}  // namespace hyperion
