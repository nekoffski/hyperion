#include "DaemonClient.hh"

#include "lib/api/cmd/Error.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/core/ServiceLocator.hh"

namespace hyperion {

DaemonClient::DaemonClient(asio::io_context& io, const Config& config)
    : m_config(config), m_session(io) {}

asio::awaitable<Opt<Str>> DaemonClient::checkHealth() {
    auto request = std::make_unique<api::HealthRequest>();
    auto response = co_await sendMessage(*request);

    if (response->kind() == api::ApiMessageKind::healthResponse) {
        if (response->as<api::HealthResponse>()->uuid() == request->uuid()) {
            co_return std::nullopt;
        } else {
            co_return "Health check failed: UUID mismatch";
        }
    } else if (response->kind() == api::ApiMessageKind::errorResponse) {
        auto& errorResponse = static_cast<api::ErrorResponse&>(*response);
        co_return fmt::format(
            "Health check failed: {}", fmt::underlying(errorResponse.code())
        );

    } else {
        co_return fmt::format(
            "Unexpected response kind: {}", fmt::underlying(response->kind())
        );
    }
    co_return std::nullopt;
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> DaemonClient::sendMessage(
    const api::ApiMessage& message
) {
    if (not m_session.connected()) {
        co_await m_session.connect({
            .ip = "127.0.0.1",
            .port = m_config.daemon().port,
        });
    }

    api::ApiSession session{m_session};
    co_await session.write(message);
    co_return co_await session.read(api::ApiMessageDeserializer::get());
}

}  // namespace hyperion
