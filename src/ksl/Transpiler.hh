#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/models/Compute.hh"

namespace hyperion::ksl {

class Transpiler : public StaticClass {
   public:
    static Str transpile(const Str& source, KernelType target);
};

}  // namespace hyperion::ksl
