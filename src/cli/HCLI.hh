#pragma once

#include <CLI/CLI.hpp>
#include <memory>
#include <vector>

#include "Command.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

class HCLI : public NonCopyable, public NonMovable {
   public:
    explicit HCLI();
    void parse(int argc, char** argv);

   private:
    CLI::App m_app;
    std::vector<std::unique_ptr<Command>> m_commands;
};

}  // namespace hyperion
