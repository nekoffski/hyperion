#pragma once

#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/FileSystem.hh"
#include "workspace/WorkspaceRepo.hh"

namespace hyperion {

class DiskWorkspaceRepo : public WorkspaceRepo {
   public:
    explicit DiskWorkspaceRepo(const Config& config);

    std::vector<WorkspaceConfig> list() const override;
    Opt<WorkspaceConfig> get(const Str& name) override;
    void add(const WorkspaceConfig& workspace) override;
    void remove(const Str& name) override;

   private:
    void ensureFileSystem();
    void createWorkspacesConfig();

    void saveRepo(const nlohmann::json& repo);
    nlohmann::json loadRepo() const;

    const Config& m_config;

    Path m_workspaceConfigPath;
};

}  // namespace hyperion
