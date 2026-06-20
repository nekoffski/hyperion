#pragma once

#include <memory>

#include "controllers/KernelController.hh"
#include "controllers/WorkspaceController.hh"
#include "internal/api/Message.hh"
#include "internal/api/cmd/Health.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/net/Asio.hh"

namespace hyperion {

class Dispatcher : public NonCopyable, public NonMovable {
   public:
    Dispatcher();

    asio::awaitable<std::unique_ptr<api::ApiMessage>> dispatch(
        const api::ApiMessage& message
    );

   private:
    asio::awaitable<std::unique_ptr<api::ApiMessage>> dispatchImpl(
        const api::ApiMessage& message
    );

    asio::awaitable<std::unique_ptr<api::ApiMessage>> on(
        const api::HealthRequest& req
    );

    WorkspaceController m_workspaceController;
    // KernelController m_kernelController;
};
}  // namespace hyperion
