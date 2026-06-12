#pragma once

#include "WorkspaceRepo.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Config.hh"
#include "internal/core/Core.hh"
#include "internal/core/FileSystem.hh"
#include "internal/core/Singleton.hh"

namespace hyperion {

class WorkspaceManager : public UniqueInstance<WorkspaceManager> {
   public:
    explicit WorkspaceManager(WorkspaceRepo& repo, const Config& config);

    void create(const WorkspaceConfig& config);
    std::vector<WorkspaceConfig> list() const;
    Opt<Workspace> get(const Str& name) const;
    void remove(const Str& name);

   private:
    void prepareWorkspaceDirectory(const Str& name);

    WorkspaceRepo& m_repo;
    Path m_workspacesPath;
};

}  // namespace hyperion
