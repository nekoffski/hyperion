#pragma once

#include "lib/api/Session.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/Singleton.hh"
#include "lib/models/Workspace.hh"
#include "lib/net/Asio.hh"

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
