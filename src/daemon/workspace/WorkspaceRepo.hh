#pragma once

#include <vector>

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/models/Workspace.hh"

namespace hyperion {

class WorkspaceRepo : public NonCopyable, public NonMovable {
   public:
    virtual ~WorkspaceRepo() = default;

    virtual std::vector<WorkspaceConfig> list() const = 0;
    virtual Opt<WorkspaceConfig> get(const Str& name) = 0;
    virtual void add(const WorkspaceConfig& workspace) = 0;
    virtual void remove(const Str& name) = 0;
};

}  // namespace hyperion