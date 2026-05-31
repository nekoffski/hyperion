#include "Platform.hh"

#include "lib/core/Log.hh"
#include "lib/core/ServiceLocator.hh"
#include "lib/runtime/unix/UnixProcess.hh"
#include "lib/runtime/unix/UnixSignal.hh"

namespace hyperion {

std::unique_ptr<ProcessManager::Impl> Platform::s_processManager = nullptr;
std::unique_ptr<SignalManager::Impl> Platform::s_signalManager = nullptr;

constexpr Platform::OS Platform::os() {
#ifdef HYPERION_PLATFORM_WINDOWS
    return Platform::OS::windows;
#elif defined(HYPERION_PLATFORM_DARWIN)
    return Platform::OS::darwin;
#else
    return Platform::OS::linux;
#endif
}

Str toString(Platform::OS os) {
    switch (os) {
        case Platform::OS::linux:
            return "Linux";
        case Platform::OS::windows:
            return "Windows";
        case Platform::OS::darwin:
            return "Darwin";
    }
    return "Unknown";
}

void Platform::init() {
    log::debug("Detected platform: {}", toString(os()));

#if defined(HYPERION_PLATFORM_LINUX) || defined(HYPERION_PLATFORM_DARWIN)
    s_processManager = std::make_unique<UnixProcess>();
    s_signalManager = std::make_unique<UnixSignalManager>();

#else
#error "Unsupported platform"
#endif

    ProcessManager::setImpl(*s_processManager);
    SignalManager::setImpl(*s_signalManager);
}

}  // namespace hyperion
