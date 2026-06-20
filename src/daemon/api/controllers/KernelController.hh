#pragma once

#include "compute/KernelManager.hh"
#include "internal/api/cmd/Kernel.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"

namespace hyperion {

class KernelController : public NonCopyable, public NonMovable {
   public:
    explicit KernelController(KernelManager& kernelManager);

   private:
    KernelManager& m_kernelManager;
};

}  // namespace hyperion
