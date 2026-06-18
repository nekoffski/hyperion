#pragma once

#include "KernelSource.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/models/Workspace.hh"

namespace hyperion {

class KernelSourceRepo : public NonCopyable, public NonMovable {
   public:
    virtual ~KernelSourceRepo() = default;

    virtual bool has(
        const Workspace& workspace, const Str& name, KernelSourceFormat format
    ) const = 0;

    virtual KernelSource get(
        const Workspace& workspace, const Str& name, KernelSourceFormat format
    ) const = 0;

    virtual void add(
        const Workspace& workspace, const Str& name, const KernelSource& source
    ) = 0;

    virtual void remove(
        const Workspace& workspace, const Str& name, KernelSourceFormat format
    ) = 0;
};

}  // namespace hyperion
