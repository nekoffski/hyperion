#pragma once

#include "internal/api/cmd/Workspace.hh"
#include "internal/core/Concepts.hh"
#include "internal/net/Asio.hh"
#include "workspace/WorkspaceManager.hh"

namespace hyperion {

class WorkspaceController : public NonCopyable, public NonMovable {
   public:
    explicit WorkspaceController(WorkspaceManager& wm);

    asio::awaitable<std::unique_ptr<api::ApiMessage>> on(
        const api::WorkspaceCreateRequest& req
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> on(
        const api::WorkspaceDeleteRequest& req
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> on(
        const api::WorkspaceListRequest& req
    );

   private:
    WorkspaceManager& m_workspaceManager;
};

}  // namespace hyperion
