#include "Process.hh"

#include "lib/core/Log.hh"

namespace hyperion {

ProcessManager::Impl* ProcessManager::s_impl = nullptr;

void ProcessManager::setImpl(Impl& impl) { s_impl = &impl; }

Pid ProcessManager::getPid() {
    log::expect(s_impl, "ProcessManager implementation not set");
    return s_impl->getPid();
}

}  // namespace hyperion
