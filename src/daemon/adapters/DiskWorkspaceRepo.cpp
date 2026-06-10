#include "DiskWorkspaceRepo.hh"

namespace hyperion {

namespace {

Path workspaceConfigPath(const Config& config) {
    return Path::join(config.rootPath(), "var/lib/workspaces.json");
}

}  // namespace

DiskWorkspaceRepo::DiskWorkspaceRepo(const Config& config)
    : m_config(config), m_workspaceConfigPath(workspaceConfigPath(config)) {
    ensureFileSystem();
}

std::vector<WorkspaceConfig> DiskWorkspaceRepo::list() const {
    std::vector<WorkspaceConfig> configs;
    auto repo = loadRepo();
    auto& workspaces = repo["workspaces"];

    for (const auto& ws : workspaces) {
        configs.push_back(WorkspaceConfig{ws["name"].get<Str>()});
    }
    return configs;
}

Opt<WorkspaceConfig> DiskWorkspaceRepo::get(const Str& name) {
    auto repo = loadRepo();
    auto& workspaces = repo["workspaces"];

    for (const auto& ws : workspaces) {
        if (ws["name"] == name) {
            return WorkspaceConfig{ws["name"].get<Str>()};
        }
    }
    return {};
}

void DiskWorkspaceRepo::add(const WorkspaceConfig& workspace) {
    auto repo = loadRepo();
    auto& workspaces = repo["workspaces"];

    for (const auto& ws : workspaces) {
        if (ws["name"] == workspace.name) {
            throw WorkspaceError{
                ErrorCode::duplicate, "Workspace with name '{}' already exists",
                workspace.name
            };
        }
    }

    workspaces.push_back({{"name", workspace.name}});
    saveRepo(repo);
}

void DiskWorkspaceRepo::remove(const Str& name) {
    auto repo = loadRepo();
    auto& workspaces = repo["workspaces"];

    auto it = std::remove_if(
        workspaces.begin(), workspaces.end(),
        [&name](const nlohmann::json& ws) { return ws["name"] == name; }
    );

    if (it == workspaces.end()) {
        log::warn("Workspace '{}' not found in repo, nothing to remove", name);
        return;
    }

    workspaces.erase(it, workspaces.end());
    saveRepo(repo);
}

void DiskWorkspaceRepo::ensureFileSystem() {
    if (not m_workspaceConfigPath.exists()) {
        createWorkspacesConfig();
    } else if (not m_workspaceConfigPath.isFile()) {
        throw WorkspaceError{
            ErrorCode::workspaceError, "Expected '{}' to be a file",
            m_workspaceConfigPath.str()
        };
    }
}

void DiskWorkspaceRepo::createWorkspacesConfig() {
    nlohmann::json root;

    root["meta"] = nlohmann::json::object();
    root["workspaces"] = nlohmann::json::array();

    saveRepo(root);
}

void DiskWorkspaceRepo::saveRepo(const nlohmann::json& repo) {
    constexpr u32 indent = 4;
    File{m_workspaceConfigPath}.write(repo.dump(indent));
}

nlohmann::json DiskWorkspaceRepo::loadRepo() const {
    if (not m_workspaceConfigPath.exists()) {
        throw WorkspaceError{
            ErrorCode::fileNotFound, "Workspace config file '{}' not found",
            m_workspaceConfigPath.str()
        };
    }
    try {
        return nlohmann::json::parse(File{m_workspaceConfigPath}.read());
    } catch (const nlohmann::json::parse_error& e) {
        throw WorkspaceError{
            ErrorCode::workspaceError, "Failed to parse workspace config: {}",
            e.what()
        };
    }
}

}  // namespace hyperion
