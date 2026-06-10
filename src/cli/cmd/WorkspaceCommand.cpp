#include "WorkspaceCommand.hh"

namespace hyperion {

WorkspaceCommand::WorkspaceCommand(CLI::App& app, DaemonClient& client)
    : Command(app, "workspace", "Manage workspaces"), m_client(client) {
    addSubcommand(
        "create", "Create a new workspace",
        [&](CLI::App& cmd) -> asio::awaitable<void> {
            co_return co_await onCreate(cmd);
        },
        [](CLI::App& cmd) {
            cmd.add_option("--name", "Name of the workspace")->required();
        }
    );

    addSubcommand(
        "delete", "Delete an existing workspace",
        [&](CLI::App& cmd) -> asio::awaitable<void> {
            co_return co_await onDelete(cmd);
        },
        [](CLI::App& cmd) {
            cmd.add_option("--name", "Name of the workspace")->required();
        }
    );

    addSubcommand(
        "list", "List all workspaces",
        [&](CLI::App&) -> asio::awaitable<void> { co_return co_await onList(); }
    );
}

asio::awaitable<void> WorkspaceCommand::onCreate(CLI::App& cmd) {
    const auto& name = cmd.get_option("--name")->as<std::string>();
    WorkspaceConfig config{name};
    co_await m_client.createWorkspace(config);
}

asio::awaitable<void> WorkspaceCommand::onDelete(CLI::App& cmd) {
    const auto& name = cmd.get_option("--name")->as<std::string>();
    co_await m_client.deleteWorkspace(name);
}

asio::awaitable<void> WorkspaceCommand::onList() {
    const auto& workspaces = co_await m_client.listWorkspaces();
    fmt::print("\n workspaces: \n");
    for (const auto& workspace : workspaces) {
        fmt::print("\t - {}\n", workspace.name);
    }
}

}  // namespace hyperion
