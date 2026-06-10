#include "Controller.hh"

#include "lib/api/cmd/Ack.hh"
#include "lib/api/cmd/Error.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/core/ServiceLocator.hh"
#include "workspace/WorkspaceManager.hh"

namespace hyperion {

Controller::Controller() {}

asio::awaitable<std::unique_ptr<api::ApiMessage>>
Controller::handleMessageWithError(const api::ApiMessage& msg) {
    try {
        co_return co_await handleMessage(msg);
    } catch (const Exception& e) {
        log::error("Error handling message: {}", e.what());
        co_return std::make_unique<api::ErrorResponse>(e);
    }
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> Controller::handleMessage(
    const api::ApiMessage& msg
) {
    switch (msg.kind()) {
        case api::ApiMessageKind::healthRequest:
            co_return co_await onHealthRequest(msg.as<api::HealthRequest>());

        case api::ApiMessageKind::workspaceCreateRequest:
            co_return co_await onWorkspaceCreateRequest(
                msg.as<api::WorkspaceCreateRequest>()
            );

        case api::ApiMessageKind::workspaceDeleteRequest:
            co_return co_await onWorkspaceDeleteRequest(
                msg.as<api::WorkspaceDeleteRequest>()
            );

        case api::ApiMessageKind::workspaceListRequest:
            co_return co_await onWorkspaceListRequest(
                msg.as<api::WorkspaceListRequest>()
            );

        default:
            log::warn(
                "Received unhandled message of kind: {}",
                fmt::underlying(msg.kind())
            );
            co_return std::make_unique<api::ErrorResponse>(fmt::format(
                "Unhandled message kind: {}", fmt::underlying(msg.kind())
            ));
    }
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> Controller::onHealthRequest(
    const api::HealthRequest& req
) {
    log::debug("Received health request with uuid: {}", req.uuid());
    co_return std::make_unique<api::HealthResponse>(req.uuid());
}

asio::awaitable<std::unique_ptr<api::ApiMessage>>
Controller::onWorkspaceCreateRequest(const api::WorkspaceCreateRequest& req) {
    auto& wm = ServiceLocator<WorkspaceManager>::get();
    wm.create(req.config());
    co_return std::make_unique<api::Ack>();
}

asio::awaitable<std::unique_ptr<api::ApiMessage>>
Controller::onWorkspaceDeleteRequest(const api::WorkspaceDeleteRequest& req) {
    auto& wm = ServiceLocator<WorkspaceManager>::get();
    wm.remove(req.workspaceName());
    co_return std::make_unique<api::Ack>();
}

asio::awaitable<std::unique_ptr<api::ApiMessage>>
Controller::onWorkspaceListRequest(const api::WorkspaceListRequest& req) {
    auto& wm = ServiceLocator<WorkspaceManager>::get();
    auto res = std::make_unique<api::WorkspaceListResponse>(wm.list());
    co_return res;
}

}  // namespace hyperion
