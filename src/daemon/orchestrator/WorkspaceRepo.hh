#pragma once

#include <vector>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/orchestrator/Workspace.hh"

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