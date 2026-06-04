#pragma once

#include "lib/asio/Asio.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/messaging/Payload.hh"

namespace hyperion {

class TcpSession : public NonCopyable, public NonMovable {
   public:
    static constexpr u16 protocolPreamble = 0xd34f;
    static constexpr u16 headerSize = 4;
    static constexpr u16 maxPayloadSize = 1024;

    explicit TcpSession(asio::ip::tcp::socket socket);
    ~TcpSession();

    asio::awaitable<PayloadBuffer> read();
    asio::awaitable<void> write(const PayloadBufferView& buffer);

    std::string_view ident() const;

   private:
    asio::ip::tcp::socket m_socket;
    std::string m_ident;
};

}  // namespace hyperion
