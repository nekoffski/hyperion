#include "Session.hh"

#include <array>

#include "lib/core/Error.hh"

namespace hyperion {

Session::Session(asio::ip::tcp::socket socket)
    : m_socket(std::move(socket)),
      m_ident(
          m_socket.remote_endpoint().address().to_string() + ":" +
          std::to_string(m_socket.remote_endpoint().port())
      ) {
    log::trace("{} - Session created", m_ident);
}

Session::~Session() { log::trace("{} - Session destroyed", m_ident); }

asio::awaitable<PayloadBuffer> Session::read() {
    std::array<u8, headerSize> header{};
    co_await asio::async_read(
        m_socket, asio::buffer(header), asio::use_awaitable
    );

    const auto preamble = (static_cast<u16>(header[0]) << 8) | header[1];
    if (preamble != protocolPreamble) {
        throw NetError{
            ErrorCode::invalidMessagePreamble,
            "invalid preamble: expected {:#06x}, got {:#06x}", protocolPreamble,
            preamble
        };
    }

    const auto size = (static_cast<u16>(header[2]) << 8) | header[3];

    if (size == 0) {
        throw NetError{
            ErrorCode::invalidMessageSize,
            "invalid message size: expected > 0, got {}", size
        };
    }

    if (size > maxPayloadSize) {
        throw NetError{
            ErrorCode::invalidMessageSize,
            "invalid message size: expected <= {}, got {}", maxPayloadSize, size
        };
    }

    PayloadBuffer buffer(size);
    co_await asio::async_read(
        m_socket, asio::buffer(buffer), asio::use_awaitable
    );

    co_return buffer;
}

asio::awaitable<void> Session::write(const PayloadBufferView& buffer) {
    const auto size = static_cast<u16>(buffer.size());

    if (size > maxPayloadSize) {
        throw NetError{
            ErrorCode::invalidMessageSize,
            "invalid message size: expected <= {}, got {}", maxPayloadSize, size
        };
    }

    const std::array<u8, headerSize> header{
        static_cast<u8>(protocolPreamble >> 8),
        static_cast<u8>(protocolPreamble & 0xFF),
        static_cast<u8>(size >> 8),
        static_cast<u8>(size & 0xFF),
    };

    const std::array<asio::const_buffer, 2> buffers{
        asio::buffer(header),
        asio::buffer(buffer.data(), buffer.size()),
    };
    co_await asio::async_write(m_socket, buffers, asio::use_awaitable);
}

std::string_view Session::ident() const { return m_ident; }

}  // namespace hyperion
