#include "Process.hh"

#include "lib/core/Log.hh"

namespace hyperion {

ProcessManager::Impl* ProcessManager::s_impl = nullptr;

void ProcessManager::setImpl(Impl& impl) { s_impl = &impl; }

Pid ProcessManager::currentPid() {
    log::expect(s_impl, "ProcessManager implementation not set");
    return s_impl->currentPid();
}

Pid ProcessManager::spawn(const ProcessDescription& desc) {
    log::expect(s_impl, "ProcessManager implementation not set");
    return s_impl->spawn(desc);
}

bool ProcessManager::isRunning(Pid pid) {
    log::expect(s_impl, "ProcessManager implementation not set");
    return s_impl->isRunning(pid);
}

}  // namespace hyperion
