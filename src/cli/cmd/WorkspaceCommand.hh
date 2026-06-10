#pragma once

#include "Command.hh"
#include "daemon/DaemonClient.hh"

namespace hyperion {

class WorkspaceCommand : public Command {
   public:
    explicit WorkspaceCommand(CLI::App& app, DaemonClient& client);

   private:
    asio::awaitable<void> onCreate(CLI::App& cmd);
    asio::awaitable<void> onDelete(CLI::App& cmd);
    asio::awaitable<void> onList();

    DaemonClient& m_client;
};

}  // namespace hyperion