#pragma once

#include "Addr.hh"
#include "lib/asio/Asio.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/messaging/Payload.hh"

namespace hyperion {

class TcpSession : public NonCopyable, public NonMovable {
   public:
    static constexpr u16 protocolPreamble = 0xd34f;
    static constexpr u16 headerSize = 4;
    static constexpr u16 maxPayloadSize = 1024u * 8u;

    explicit TcpSession(asio::ip::tcp::socket socket);
    explicit TcpSession(asio::io_context& io);

    ~TcpSession();

    std::string_view ident() const;

    asio::awaitable<PayloadBuffer> read();
    asio::awaitable<void> write(const PayloadBufferView& buffer);

    asio::awaitable<void> connect(const Addr& addr);

    bool connected() const;

   private:
    asio::ip::tcp::socket m_socket;
    std::string m_ident;
    bool m_connected;
};

}  // namespace hyperion
