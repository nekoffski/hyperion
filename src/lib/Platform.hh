#pragma once

#include <memory>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/runtime/Process.hh"
#include "lib/runtime/Signal.hh"

namespace hyperion {

#ifdef _WIN32
#define HYPERION_PLATFORM_WINDOWS
#elif __APPLE__
#define HYPERION_PLATFORM_DARWIN
#else
#define HYPERION_PLATFORM_LINUX
#endif

class Platform : public StaticClass {
   public:
    enum class OS {
        linux,
        windows,
        darwin,
    };

    constexpr static OS os();

    static void init();

   private:
    static std::unique_ptr<ProcessManager::Impl> s_processManager;
    static std::unique_ptr<SignalManager::Impl> s_signalManager;
};

}  // namespace hyperion
