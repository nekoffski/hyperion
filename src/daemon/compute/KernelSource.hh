#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"

namespace hyperion {

enum class KernelSourceFormat { native, cxx, opencl };

struct KernelSource {
    KernelSourceFormat format;
    Str code;
};

}  // namespace hyperion
