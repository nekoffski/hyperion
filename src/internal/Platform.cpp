#include "Platform.hh"

#include "internal/core/Log.hh"
#include "internal/core/ServiceLocator.hh"
#include "internal/runtime/unix/UnixProcess.hh"
#include "internal/runtime/unix/UnixSignal.hh"

namespace hyperion {

std::unique_ptr<ProcessManager::Impl> Platform::s_processManager = nullptr;
std::unique_ptr<SignalManager::Impl> Platform::s_signalManager = nullptr;

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
