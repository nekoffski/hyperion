#pragma once

#include "adapters/DiskWorkspaceRepo.hh"
#include "internal/core/Config.hh"
#include "internal/core/Singleton.hh"
#include "internal/runtime/Thread.hh"
#include "workspace/WorkspaceManager.hh"

namespace hyperion {

class Daemon : public UniqueInstance<Daemon> {
   public:
    explicit Daemon(const Config& config);

    void start();
    void stop();

   private:
    const Config& m_config;
    ThreadGroup m_workers;

    DiskWorkspaceRepo m_workspaceRepo;
    WorkspaceManager m_workspaceManager;
};

}  // namespace hyperion