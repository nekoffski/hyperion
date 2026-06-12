#pragma once

#include <array>

#include "internal/core/Singleton.hh"
#include "internal/runtime/Signal.hh"

namespace hyperion {

class UnixSignalManager : public SignalManager::Impl,
                          public UniqueInstance<UnixSignalManager> {
    using Handlers =
        std::array<SignalManager::Handler, static_cast<u16>(Signal::count)>;

   public:
    void send(Signal signal, Pid pid) override;
    void registerHandler(
        Signal signal, SignalManager::Handler handler
    ) override;

   private:
    static Handlers m_handlers;
};

}  // namespace hyperion
