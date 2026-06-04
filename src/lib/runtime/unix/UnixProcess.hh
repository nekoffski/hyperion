#pragma once

#include "lib/core/Singleton.hh"
#include "lib/runtime/Process.hh"

namespace hyperion {

class UnixProcess : public ProcessManager::Impl,
                    public UniqueInstance<UnixProcess> {
   public:
    Pid currentPid() const override;
    Pid spawn(const ProcessDescription& desc) override;

    bool isRunning(Pid pid) const override;
};

}  // namespace hyperion
