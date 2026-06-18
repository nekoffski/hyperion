#pragma once

#include <memory>

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/core/OS.hh"
#include "internal/runtime/Process.hh"
#include "internal/runtime/Signal.hh"

namespace hyperion {

class Platform : public StaticClass {
   public:
    static void init();

   private:
    static std::unique_ptr<ProcessManager::Impl> s_processManager;
    static std::unique_ptr<SignalManager::Impl> s_signalManager;
};

}  // namespace hyperion
