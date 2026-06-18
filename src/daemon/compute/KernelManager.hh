#pragma once

#include "KernelSourceRepo.hh"

namespace hyperion {

class KernelManager : public NonCopyable, public NonMovable {
   public:
    explicit KernelManager(KernelSourceRepo& repo);

   private:
    KernelSourceRepo& m_repo;
};

}  // namespace hyperion
