#include "Daemon.hh"

#include "api/Server.hh"

namespace hyperion {

void Daemon::start(const Config& config) {
    m_workers.add<Server>(config);

    m_workers.start();
    m_workers.join();
}

void Daemon::stop() { m_workers.cancel(); }

}  // namespace hyperion
