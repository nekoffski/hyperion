#include "HCLI.hh"

#include "cmd/DaemonCommand.hh"
#include "cmd/WorkspaceCommand.hh"
#include "internal/core/Log.hh"
#include "internal/net/Asio.hh"

namespace hyperion {

HCLI::HCLI(const Config& config)
    : m_app("hyperion CLI"), m_daemonClient(m_io, config) {
    m_commands.emplace_back(
        std::make_unique<DaemonCommand>(m_app, m_daemonClient)
    );
    m_commands.emplace_back(
        std::make_unique<WorkspaceCommand>(m_app, m_daemonClient)
    );
}

void HCLI::parse(int argc, char** argv) {
    try {
        m_app.parse(argc, argv);
    } catch (const CLI::CallForHelp& e) {
        std::exit(m_app.exit(e));
    } catch (const CLI::CallForAllHelp& e) {
        std::exit(m_app.exit(e));
    } catch (const CLI::ParseError& e) {
        log::panic("Failed to parse command line arguments: {}", e.what());
    }

    asio::co_spawn(
        m_io,
        [&]() -> asio::awaitable<void> {
            try {
                for (const auto& cmd : m_commands) {
                    if (cmd->isChosen()) {
                        co_return co_await cmd->execute();
                    }
                }
                log::error("ERR: Unknown command, exiting");
            } catch (const Exception& e) {
                log::error(
                    "unhandled exception in command execution: {} - {}",
                    fmt::underlying(e.code()), e.message()
                );
            } catch (const std::exception& e) {
                log::error(
                    "unhandled exception in command execution: {}", e.what()
                );
            } catch (...) {
                log::error("unhandled unknown exception in command execution");
            }
        },
        asio::detached
    );

    m_io.run();
}

}  // namespace hyperion
