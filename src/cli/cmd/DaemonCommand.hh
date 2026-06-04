#pragma once

#include "Command.hh"

namespace hyperion {

class DaemonCommand : public Command {
   public:
    explicit DaemonCommand(CLI::App& app);

   private:
    void onStatus();
    void onStart();
    void onStop();
    void onRestart();
};

}  // namespace hyperion
