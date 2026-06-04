#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/Singleton.hh"

namespace hyperion {

class DaemonClient : public Singleton<DaemonClient> {
   public:
    explicit DaemonClient();

   private:
    const Config& m_config;
};

}  // namespace hyperion
