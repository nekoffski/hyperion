#pragma once

#include "WorkspaceRepo.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/FileSystem.hh"
#include "lib/core/Singleton.hh"

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
