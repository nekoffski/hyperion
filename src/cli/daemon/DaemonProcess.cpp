#include "DaemonProcess.hh"

#include "lib/core/FileSystem.hh"
#include "lib/core/Log.hh"
#include "lib/core/ServiceLocator.hh"
#include "lib/core/Time.hh"
#include "lib/runtime/Signal.hh"

namespace hyperion {

DaemonProcess::DaemonProcess() : m_config(ServiceLocator<Config>::get()) {
    readStatus();
}

bool DaemonProcess::stop() {
    if (m_pid <= 0) {
        return true;
    }
    SignalManager::send(Signal::terminate, m_pid);

    constexpr auto interval = 500ms;
    constexpr int attemps = 10;

    for (int i = 0; i < attemps; ++i) {
        readStatus();
        if (not m_status.running) {
            return true;
        }
        std::this_thread::sleep_for(interval);
    }
    return false;
}

bool DaemonProcess::start() {
    if (m_status.running) {
        return true;
    }

    if (not m_config.daemon().binPath.isFile()) {
        m_status.running = false;
        m_status.details = fmt::format(
            "Daemon binary {} does not exist or is not a file",
            m_config.daemon().binPath.str()
        );
        return false;
    }

    try {
        auto pid = ProcessManager::spawn({
            .bin = m_config.daemon().binPath.str(),
            .args = {},
            .env = {},
            .stdout = m_config.logging().daemonFile,
            .stderr = m_config.logging().daemonErrorFile,
        });

        if (pid <= 0) {
            m_status.running = false;
            m_status.details = "Failed to spawn daemon process";
            return false;
        }

        constexpr auto interval = 500ms;
        constexpr int attemps = 10;

        for (int i = 0; i < attemps; ++i) {
            readStatus();
            if (m_status.running) {
                if (m_pid != pid) {
                    log::panic(
                        "PID mismatch after starting daemon: expected {}, got "
                        "{}",
                        pid, m_pid
                    );
                }
                return true;
            }
            std::this_thread::sleep_for(interval);
        }

    } catch (const ProcessError& e) {
        m_status.running = false;
        m_status.details =
            fmt::format("Failed to spawn daemon process: {}", e.message());
        return false;
    }

    m_status.running = false;
    m_status.details = "Daemon process did not start within expected time";

    return false;
}

Pid DaemonProcess::pid() const { return m_pid; }

const DaemonProcess::Status& DaemonProcess::status() const { return m_status; }

void DaemonProcess::readStatus() {
    auto pidPath = m_config.daemon().pidfile;

    if (not pidPath.isFile()) {
        m_status.running = false;
        m_status.details = "PID file does not exist";
        return;
    }

    File pidFile{pidPath};
    auto pidStr = pidFile.read();

    try {
        m_pid = static_cast<Pid>(std::stoul(pidStr));
    } catch (const std::exception& e) {
        m_status.running = false;
        m_status.details = fmt::format(
            "Failed to parse PID from file {}: {}", pidPath.str(), e.what()
        );
        return;
    }

    if (ProcessManager::isRunning(m_pid)) {
        m_status.running = true;
        m_status.details = fmt::format("Daemon is running with PID {}", m_pid);
    } else {
        m_status.running = false;
        m_status.details = fmt::format(
            "Daemon is not running, but PID file exists ({}), manual "
            "intervention may be required",
            pidPath.str()
        );
    }
}

}  // namespace hyperion
