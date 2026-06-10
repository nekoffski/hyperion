#include "WorkspaceManager.hh"

#include "lib/core/FileSystem.hh"
#include "lib/core/Scope.hh"

namespace hyperion {

namespace {

Path workspacesPath(const Config& config) {
    return Path::join(config.rootPath(), "var/lib/workspaces");
}

}  // namespace

WorkspaceManager::WorkspaceManager(WorkspaceRepo& repo, const Config& config)
    : m_repo(repo), m_workspacesPath(workspacesPath(config)) {}

void WorkspaceManager::create(const WorkspaceConfig& config) {
    m_repo.add(config);
    ON_SCOPE_FAIL { m_repo.remove(config.name); };
    prepareWorkspaceDirectory(config.name);
}

std::vector<WorkspaceConfig> WorkspaceManager::list() const {
    return m_repo.list();
}

Opt<Workspace> WorkspaceManager::get(const Str& name) const {
    if (auto config = m_repo.get(name); config) {
        return Workspace{config->name};
    }
    return {};
}

void WorkspaceManager::remove(const Str& name) {
    m_repo.remove(name);

    const auto workspacePath = Path::join(m_workspacesPath, name);
    if (workspacePath.exists()) {
        Directory{workspacePath}.remove();
    }
}

void WorkspaceManager::prepareWorkspaceDirectory(const Str& name) {
    const auto workspacePath = Path::join(m_workspacesPath, name);
    if (workspacePath.exists()) {
        throw WorkspaceError{
            ErrorCode::workspaceError,
            "Workspace directory '{}' already exists", workspacePath.str()
        };
    }

    Directory dir{workspacePath};
    dir.create();
    dir.createSubdirectory("jobs");
}

}  // namespace hyperion
