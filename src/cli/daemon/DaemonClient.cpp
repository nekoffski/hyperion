#include "DaemonClient.hh"

#include "internal/api/cmd/Ack.hh"
#include "internal/api/cmd/Error.hh"
#include "internal/api/cmd/Health.hh"
#include "internal/api/cmd/Workspace.hh"
#include "internal/core/ServiceLocator.hh"

namespace hyperion {

namespace {

template <typename T>
const T& expectResponse(const api::ApiMessage& message) {
    if (message.is<T>()) {
        return message.as<T>();
    }
    if (message.kind() == api::ApiMessageKind::errorResponse) {
        const auto& errorResponse = message.as<api::ErrorResponse>();
        throw Exception(
            fmt::format("Received error response: {}", errorResponse.details())
        );
    } else {
        throw Exception(fmt::format("Unexpected response {}", message.name()));
    }
}

}  // namespace

DaemonClient::DaemonClient(asio::io_context& io, const Config& config)
    : m_config(config), m_session(io) {}

asio::awaitable<bool> DaemonClient::healthy() {
    api::HealthRequest req;
    auto res = co_await sendMessage(req);
    const auto& health = expectResponse<api::HealthResponse>(*res);
    co_return health.uuid() == req.uuid();
}

asio::awaitable<void> DaemonClient::createWorkspace(
    const WorkspaceConfig& config
) {
    auto res = co_await sendMessage(api::WorkspaceCreateRequest{config});
    expectResponse<api::Ack>(*res);
}

asio::awaitable<void> DaemonClient::deleteWorkspace(const Str& name) {
    auto res = co_await sendMessage(api::WorkspaceDeleteRequest{name});
    expectResponse<api::Ack>(*res);
}

asio::awaitable<std::vector<WorkspaceConfig>> DaemonClient::listWorkspaces() {
    auto res = co_await sendMessage(api::WorkspaceListRequest{});
    const auto& listRes = expectResponse<api::WorkspaceListResponse>(*res);
    co_return listRes.workspaces();
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
    auto msg = co_await session.read(api::ApiMessageDeserializer::get());
    log::expect(msg != nullptr, "Received null message, could not deserialize");
    co_return msg;
}

}  // namespace hyperion
