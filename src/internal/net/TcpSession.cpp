#include "TcpSession.hh"

#include <array>

#include "internal/core/Error.hh"
#include "internal/core/Log.hh"
#include "internal/core/String.hh"

namespace hyperion {

TcpSession::TcpSession(asio::ip::tcp::socket socket)
    : m_socket(std::move(socket)),
      m_ident(
          fmt::format(
              "{}:{}", m_socket.remote_endpoint().address().to_string(),
              m_socket.remote_endpoint().port()
          )
      ),
      m_connected(m_socket.is_open()) {
    log::trace("{} - TcpSession created", m_ident);
}

TcpSession::TcpSession(asio::io_context& io)
    : m_socket(io), m_ident("<notconnected>"), m_connected(false) {}

TcpSession::~TcpSession() {
    log::trace("{} - TcpSession destroyed", m_ident);

    if (m_connected) {
        try {
            m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
            m_socket.close();
        } catch (const asio::system_error& e) {
            log::error(
                "{} - error shutting down socket: {}", m_ident, e.what()
            );
        }
    }
}

asio::awaitable<PayloadBuffer> TcpSession::read() {
    std::array<u8, headerSize> header{};
    co_await asio::async_read(
        m_socket, asio::buffer(header), asio::use_awaitable
    );

    if (log::level() >= log::Level::debug) [[unlikely]] {
        log::debug(
            "{} - received message header: {}", m_ident, toHexString(header)
        );
    }

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

    if (log::level() >= log::Level::debug) [[unlikely]] {
        log::debug(
            "{} - received message payload: {}", m_ident, toHexString(buffer)
        );
    }
    co_return buffer;
}

asio::awaitable<void> TcpSession::write(const PayloadBufferView& buffer) {
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

    if (log::level() >= log::Level::debug) [[unlikely]] {
        log::debug(
            "{} - writing message: preamble={:#06x}, size={}, header='{}', "
            "payload='{}'",
            m_ident, protocolPreamble, size, toHexString(header),
            toHexString(buffer)
        );
    }

    const std::array<asio::const_buffer, 2> buffers{
        asio::buffer(header),
        asio::buffer(buffer.data(), buffer.size()),
    };
    co_await asio::async_write(m_socket, buffers, asio::use_awaitable);
}

asio::awaitable<void> TcpSession::connect(const Addr& addr) {
    co_await m_socket.async_connect(
        asio::ip::tcp::endpoint(asio::ip::make_address(addr.ip), addr.port),
        asio::use_awaitable
    );

    m_ident = fmt::format(
        "{}:{}", m_socket.remote_endpoint().address().to_string(),
        m_socket.remote_endpoint().port()
    );
    m_connected = true;
}

bool TcpSession::connected() const { return m_connected; }

std::string_view TcpSession::ident() const { return m_ident; }

}  // namespace hyperion
