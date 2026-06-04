#include "DaemonClient.hh"

#include "lib/core/ServiceLocator.hh"

namespace hyperion {

DaemonClient::DaemonClient() : m_config(ServiceLocator<Config>::get()) {}

}  // namespace hyperion
