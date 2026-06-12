#pragma once

#include <CLI/CLI.hpp>
#include <memory>
#include <vector>

#include "Command.hh"
#include "daemon/DaemonClient.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"

namespace hyperion {

class HCLI : public NonCopyable, public NonMovable {
   public:
    explicit HCLI(const Config& config);
    void parse(int argc, char** argv);

   private:
    asio::io_context m_io;
    CLI::App m_app;
    std::vector<std::unique_ptr<Command>> m_commands;
    DaemonClient m_daemonClient;
};

}  // namespace hyperion
