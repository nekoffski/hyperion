#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Core.hh"
#include "FileSystem.hh"

namespace hyperion {

class Config {
   public:
    struct Version {
        u32 major{0};
        u32 minor{0};
        u32 patch{0};
    };

    struct Daemon {
        u16 port;
        Str pidfile;
    };

    struct Logging {
        log::Level level;
        Str file;
    };

    const Version& version() const;
    const Daemon& daemon() const;
    const Logging& logging() const;

    static Config fromFile(const Path& path);

   private:
    void parseFields(const Path& path);
    void parseVersionFile(const Path& path);

    Version m_version;
    Daemon m_daemon;
    Logging m_logging;
};

}  // namespace hyperion
