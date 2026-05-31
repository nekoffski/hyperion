#pragma once

#include "lib/core/Singleton.hh"
#include "lib/runtime/Process.hh"

namespace hyperion {

class UnixProcess : public ProcessManager::Impl,
                    public UniqueInstance<UnixProcess> {
   public:
    Pid getPid() const override;
};

}  // namespace hyperion
