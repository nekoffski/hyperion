#pragma once

#include "internal/api/Session.hh"
#include "internal/api/cmd/Health.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Config.hh"
#include "internal/core/Core.hh"
#include "internal/core/Singleton.hh"
#include "internal/models/Workspace.hh"
#include "internal/net/Asio.hh"

namespace hyperion {

class DaemonClient : public UniqueInstance<DaemonClient> {
   public:
    explicit DaemonClient(asio::io_context& io, const Config& config);

    asio::awaitable<bool> healthy();
    asio::awaitable<void> createWorkspace(const WorkspaceConfig& config);
    asio::awaitable<void> deleteWorkspace(const Str& name);
    asio::awaitable<std::vector<WorkspaceConfig>> listWorkspaces();

   private:
    asio::awaitable<std::unique_ptr<api::ApiMessage>> sendMessage(
        const api::ApiMessage& message
    );

    const Config& m_config;
    TcpSession m_session;
};

}  // namespace hyperion
