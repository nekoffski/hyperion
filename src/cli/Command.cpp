#include "Command.hh"

#include "lib/core/Log.hh"

namespace hyperion {

Command::Command(
    CLI::App& app, const std::string& name, const std::string& description
)
    : m_cmd(app.add_subcommand(name, description)) {}

void Command::addSubcommand(
    const std::string& name, const std::string& description,
    Callback&& callback, std::optional<InitCallback> init
) {
    auto sub = m_cmd->add_subcommand(name, description);

    if (init) {
        init.value()(*sub);
    }

    m_subs.emplace_back(std::move(callback), sub);
}

bool Command::isChosen() const { return m_cmd->parsed(); }

asio::awaitable<void> Command::execute() {
    for (const auto& sub : m_subs) {
        if (sub.cmd->parsed()) {
            co_return co_await sub.callback(*sub.cmd);
        }
    }
    log::error("ERR: No subcommand chosen for command");
}

}  // namespace hyperion
