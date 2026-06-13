#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "ksl/Ast.hh"

namespace hyperion::ksl {

class CxxEmitter : public NonCopyable {
   public:
    Str emit(const KernelDef& kernel) const;
};

}  // namespace hyperion::ksl
