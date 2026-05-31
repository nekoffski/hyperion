#include "Signal.hh"

#include "lib/core/Log.hh"

namespace hyperion {

SignalManager::Impl* SignalManager::s_impl = nullptr;

void SignalManager::send(Signal signal, Pid pid) {
    log::expect(s_impl, "SignalManager implementation not set");
    s_impl->send(signal, pid);
}

void SignalManager::registerHandler(Signal signal, Handler handler) {
    log::expect(s_impl, "SignalManager implementation not set");
    s_impl->registerHandler(signal, handler);
}

void SignalManager::setImpl(Impl& impl) { s_impl = &impl; }

}  // namespace hyperion

