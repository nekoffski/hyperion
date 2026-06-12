#include "Daemon.hh"

#include "api/Server.hh"
#include "internal/core/ServiceLocator.hh"

namespace hyperion {

Daemon::Daemon(const Config& config)
    : m_config(config),
      m_workspaceRepo(config),
      m_workspaceManager(m_workspaceRepo, config) {
    ServiceLocator<WorkspaceManager>::set(&m_workspaceManager);
}

void Daemon::start() {
    m_workers.add<Server>(m_config);

    m_workers.start();
    m_workers.join();
}

void Daemon::stop() { m_workers.cancel(); }

}  // namespace hyperion
