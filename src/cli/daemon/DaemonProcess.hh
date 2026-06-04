#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/Singleton.hh"
#include "lib/runtime/Process.hh"

namespace hyperion {

class DaemonProcess : public Singleton<DaemonProcess> {
   public:
    struct Status {
        bool running;
        std::string details;
    };

    DaemonProcess();

    const Status& status() const;

    bool stop();
    bool start();

    Pid pid() const;

   private:
    void readStatus();

    const Config& m_config;
    Status m_status;
    Pid m_pid{0u};
};

}  // namespace hyperion
