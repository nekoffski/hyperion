#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Config.hh"
#include "internal/core/Core.hh"
#include "internal/core/Singleton.hh"
#include "internal/runtime/Process.hh"

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
