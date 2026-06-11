#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/FlatMap.hh"

namespace hyperion {

enum class JobStatus {
    scheduled,
    running,
    completed,
    failed,
    cancelled,
    count
};

enum class JobType {
    noop = 0,
    delayedNoop,
    executeKernel,
    executeImagePipeline,
    count
};

using JobArgs = FlatMap<Str, Str>;

struct JobConfig {
    JobType type;
    JobArgs args;
};

struct JobShortDescription {
    JobType type;
    JobStatus status;
    Str uuid;
};

struct JobDescription {
    JobType type;
    JobStatus status;
    Str uuid;
    JobArgs args;
};

}  // namespace hyperion
