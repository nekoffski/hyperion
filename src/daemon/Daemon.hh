#pragma once

#include "lib/core/Config.hh"
#include "lib/core/Singleton.hh"
#include "lib/runtime/Thread.hh"

namespace hyperion {

class Daemon : public UniqueInstance<Daemon> {
   public:
    void start(const Config& config);
    void stop();

   private:
    ThreadGroup m_workers;
};

}  // namespace hyperion