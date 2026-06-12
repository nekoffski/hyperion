#pragma once

#include "TcpSession.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/net/Asio.hh"

namespace hyperion {

class TcpServer : public NonCopyable, public NonMovable {
   public:
    explicit TcpServer(asio::io_context& io, uint16_t port);

    void asyncAccept();

   private:
    asio::awaitable<void> acceptConnection();

    virtual asio::awaitable<void> onClient(TcpSession& session) = 0;

    asio::io_context& m_io;
    asio::ip::tcp::acceptor m_acceptor;
};

}  // namespace hyperion
