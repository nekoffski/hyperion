#pragma once

#include <memory>

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/runtime/Process.hh"
#include "internal/runtime/Signal.hh"

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
