#pragma once

#include <memory>

#include "lib/api/Message.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/api/cmd/Workspace.hh"
#include "lib/asio/Asio.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

class Controller : public NonCopyable, public NonMovable {
   public:
    Controller();

    asio::awaitable<std::unique_ptr<api::ApiMessage>> handleMessageWithError(
        const api::ApiMessage& message
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> handleMessage(
        const api::ApiMessage& message
    );

   private:
    asio::awaitable<std::unique_ptr<api::ApiMessage>> onHealthRequest(
        const api::HealthRequest& req
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> onWorkspaceCreateRequest(
        const api::WorkspaceCreateRequest& req
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> onWorkspaceDeleteRequest(
        const api::WorkspaceDeleteRequest& req
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> onWorkspaceListRequest(
        const api::WorkspaceListRequest& req
    );
};
}  // namespace hyperion
