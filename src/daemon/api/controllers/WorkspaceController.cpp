#include "WorkspaceController.hh"

#include "internal/api/cmd/Ack.hh"

namespace hyperion {

WorkspaceController::WorkspaceController(WorkspaceManager& wm)
    : m_workspaceManager(wm) {}

asio::awaitable<std::unique_ptr<api::ApiMessage>> WorkspaceController::on(
    const api::WorkspaceCreateRequest& req
) {
    m_workspaceManager.create(req.config());
    co_return std::make_unique<api::Ack>();
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> WorkspaceController::on(
    const api::WorkspaceDeleteRequest& req
) {
    m_workspaceManager.remove(req.workspaceName());
    co_return std::make_unique<api::Ack>();
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> WorkspaceController::on(
    const api::WorkspaceListRequest& req
) {
    const auto& workspaces = m_workspaceManager.list();
    co_return std::make_unique<api::WorkspaceListResponse>(workspaces);
}

}  // namespace hyperion