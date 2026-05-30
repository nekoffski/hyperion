#pragma once

#include "lib/runtime/Process.hh"

namespace hyperion {

class UnixProcess : public ProcessManager::Impl {
   public:
    Pid getPid() const override;
};

}  // namespace hyperion
