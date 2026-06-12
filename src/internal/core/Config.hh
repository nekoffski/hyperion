#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Concepts.hh"
#include "Core.hh"
#include "FileSystem.hh"
#include "Log.hh"

namespace hyperion {

class Config {
   public:
    enum class LogConfigFields { skip, log };

    struct Version {
        u32 major{0};
        u32 minor{0};
        u32 patch{0};
    };

    struct Daemon {
        u16 port;
        Path pidfile;
        Path binPath;
    };

    struct Logging {
        log::Level daemonLevel;
        log::Level cliLevel;
        Path daemonFile;
        Path daemonErrorFile;
    };

    const Path& rootPath() const;

    const Version& version() const;
    const Daemon& daemon() const;
    const Logging& logging() const;

    static Config fromFile(
        const Path& path, LogConfigFields logConfigFields = LogConfigFields::log
    );
    static Config fromEnv(
        LogConfigFields logConfigFields = LogConfigFields::log
    );

   private:
    void parseFields(const Path& path, LogConfigFields logConfigFields);
    void parseVersionFile(const Path& path);

    Version m_version;
    Daemon m_daemon;
    Logging m_logging;
    Path m_rootPath;
};

}  // namespace hyperion
