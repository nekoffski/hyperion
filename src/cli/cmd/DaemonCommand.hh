#pragma once

#include "Command.hh"
#include "daemon/DaemonClient.hh"

namespace hyperion {

class DaemonCommand : public Command {
   public:
    explicit DaemonCommand(CLI::App& app, DaemonClient& client);

   private:
    asio::awaitable<void> onStatus();
    void onStart();
    void onStop();
    void onRestart();

    DaemonClient& m_client;
};

}  // namespace hyperion
