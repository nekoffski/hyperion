#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

enum class JobStatus { scheduled, running, completed, failed, cancelled };

enum class JobCategory {};

enum class JobType {
    noop,
    delayedNoop,
    executeKernel,
    executeImagePipeline,
};

}  // namespace hyperion
