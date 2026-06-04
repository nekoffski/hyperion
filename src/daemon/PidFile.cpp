#include "PidFile.hh"

#include "lib/core/Log.hh"

namespace hyperion {

PidFile::PidFile(const Path& path)
    : m_path(path), m_pid(ProcessManager::currentPid()) {
    log::expect(
        not path.isFile(),
        "Pid file {} already exists, is another instance running?", path.str()
    );

    log::info("Creating pid file at {} with pid {}", path.str(), m_pid);
    File{m_path}.write(std::to_string(m_pid));
}

PidFile::~PidFile() {
    if (m_path.isFile()) {
        File{m_path}.remove();
        log::info("Removed pid file at {}", m_path.str());
    } else {
        log::warn("Pid file {} does not exist, cannot remove", m_path.str());
    }
}

Pid PidFile::getPid() const { return m_pid; }

}  // namespace hyperion
