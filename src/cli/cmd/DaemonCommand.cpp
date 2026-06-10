#include "DaemonCommand.hh"

#include "daemon/DaemonProcess.hh"
#include "lib/core/Log.hh"
#include "lib/runtime/Process.hh"

namespace hyperion {

DaemonCommand::DaemonCommand(CLI::App& app, DaemonClient& client)
    : Command(app, "daemon", "Manage the hyperion daemon"), m_client(client) {
    addSubcommand(
        "start", "Start the hyperion daemon",
        [&](CLI::App& cmd) -> asio::awaitable<void> {
            onStart();
            co_return;
        }
    );

    addSubcommand(
        "stop", "Stop the hyperion daemon",
        [&](CLI::App& cmd) -> asio::awaitable<void> {
            onStop();
            co_return;
        }
    );

    addSubcommand(
        "restart", "Restart the hyperion daemon",
        [&](CLI::App& cmd) -> asio::awaitable<void> {
            onRestart();
            co_return;
        }
    );

    addSubcommand(
        "status", "Show the status of the hyperion daemon",
        [&](CLI::App&) -> asio::awaitable<void> {
            co_return co_await onStatus();
        }
    );
}

void DaemonCommand::onStart() {
    auto& daemon = DaemonProcess::get();

    if (daemon.status().running) {
        log::info("Daemon is already running with PID {}", daemon.pid());
        return;
    }

    log::info("Starting daemon...");
    if (not daemon.start()) {
        log::error("Failed to start daemon");
    } else {
        log::info("Daemon started successfully with PID {}", daemon.pid());
    }
}

void DaemonCommand::onStop() {
    auto& daemon = DaemonProcess::get();

    if (const auto& status = daemon.status(); status.running) {
        log::info("Stopping daemon with PID {}...", daemon.pid());
        if (not daemon.stop()) {
            log::error("Failed to stop daemon with PID {}", daemon.pid());
        } else {
            log::info("Daemon stopped successfully");
        }
    } else {
        log::info("Daemon is not running: {}", status.details);
    }
}

void DaemonCommand::onRestart() {
    auto& daemon = DaemonProcess::get();

    if (daemon.status().running) {
        log::info("Restarting daemon with PID {}...", daemon.pid());
        if (not daemon.stop()) {
            log::error("Failed to stop daemon with PID {}", daemon.pid());
            return;
        }
        log::info("Daemon stopped successfully");
    } else {
        log::info("Daemon is not running: {}", daemon.status().details);
    }

    if (not daemon.start()) {
        log::error("Failed to start daemon");
    } else {
        log::info("Daemon started successfully with PID {}", daemon.pid());
    }
}

asio::awaitable<void> DaemonCommand::onStatus() {
    auto& daemon = DaemonProcess::get();

    if (const auto& status = daemon.status(); status.running) {
        log::info("Daemon is running with PID {}", daemon.pid());
    } else {
        log::info("Daemon is not running: {}", status.details);
        co_return;
    }

    if (co_await m_client.healthy()) {
        log::error("Daemon health check failed, invalid response");
    } else {
        log::info("Daemon health check passed");
    }
}

}  // namespace hyperion
