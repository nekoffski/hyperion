#include "Config.hh"

#include <toml++/toml.hpp>

#include "Env.hh"
#include "FileSystem.hh"
#include "Log.hh"
#include "OS.hh"
#include "ServiceLocator.hh"
#include "String.hh"

namespace hyperion {

namespace {

class Reader : public NonCopyable, public NonMovable {
   public:
    explicit Reader(
        const toml::table& tbl, Config::LogConfigFields logConfigFields
    )
        : m_tbl(tbl),
          m_logFields(logConfigFields == Config::LogConfigFields::log) {}

    template <typename T>
    T read(const Str& stanza, const Str& key) const {
        log::expect(
            m_tbl.contains(stanza), "Config file must contain a [{}] table",
            stanza
        );
        auto subTable = m_tbl.get(stanza)->as_table();
        log::expect(subTable, "Config file must contain a [{}] table", stanza);
        log::expect(
            subTable->contains(key),
            "Config file must contain a {} field in the [{}] table", key, stanza
        );

        auto raw = subTable->get(key);

        if constexpr (std::is_same_v<T, std::vector<Str>>) {
            log::expect(
                raw->is_array(), "Config file field {}.{} must be an array",
                stanza, key
            );
            std::vector<Str> result;
            for (const auto& item : *raw->as_array()) {
                log::expect(
                    item.is_string(),
                    "Config file field {}.{} must be an array of strings",
                    stanza, key
                );
                result.push_back(item.value<Str>().value_or(""));
            }
            if (m_logFields) {
                log::info(
                    "Config: read {}.{} = [ {} ]", stanza, key,
                    fmt::join(result, ",")
                );
            }
            return result;
        } else {
            auto v = raw->value<T>();
            log::expect(
                v.has_value(),
                "Config file must contain a {} field in the [{}] table", key,
                stanza
            );
            if (m_logFields) {
                log::info("Config: read {}.{} = {}", stanza, key, *v);
            }
            return *v;
        }
    }

   private:
    const toml::table& m_tbl;
    bool m_logFields;
};

}  // namespace

Config Config::fromFile(const Path& path, LogConfigFields logConfigFields) {
    Config cfg;
    cfg.m_rootPath = path;

    cfg.parseVersionFile(Path::join(path, "etc/version"));

    try {
        cfg.parseFields(Path::join(path, "etc/hyperion.toml"), logConfigFields);
    } catch (const toml::parse_error& e) {
        log::panic("Failed to parse config file: {}", e.what());
    } catch (const std::bad_optional_access& e) {
        log::panic("Failed to parse config file: {}", e.what());
    }

    return cfg;
}

Config Config::fromEnv(LogConfigFields logConfigFields) {
    auto homeDir = getEnv<Str>("HYPERION_HOME");
    log::expect(
        homeDir.has_value(), "HYPERION_HOME environment variable is not set"
    );
    return fromFile(*homeDir, logConfigFields);
}

using StrVec = std::vector<Str>;

namespace {

ComputeBackendType parseComputeBackendType(const Str& backendStr) {
    if (backendStr == "auto") {
        switch (os()) {
            case OS::windows:
            case OS::linux:
                return ComputeBackendType::opencl;
            case OS::darwin:
                return ComputeBackendType::cpu;
        }
    } else if (backendStr == "cpu") {
        return ComputeBackendType::cpu;
    } else if (backendStr == "opencl") {
        log::expect(
            os() != OS::darwin, "OpenCL backend is not supported on macOS"
        );
        return ComputeBackendType::opencl;
    }
    log::panic("Invalid backend type '{}'", backendStr);
}

log::Level parseLogLevel(const Str& levelStr) {
    static const std::unordered_map<Str, log::Level> levelMap = {
        {"trace", log::Level::trace}, {"debug", log::Level::debug},
        {"info", log::Level::info},   {"warn", log::Level::warn},
        {"error", log::Level::error}, {"critical", log::Level::critical},
        {"off", log::Level::off},
    };

    auto it = levelMap.find(levelStr);
    log::expect(
        it != levelMap.end(),
        "Invalid log level '{}', must be one of: trace, debug, info, warn, "
        "error, critical, off",
        levelStr
    );
    return it->second;
}

}  // namespace

void Config::parseFields(const Path& path, LogConfigFields logConfigFields) {
    log::expect(
        path.isFile(), "Config path {} does not exist or is not a file",
        path.str()
    );

    auto tbl = toml::parse_file(path.str());
    Reader r{tbl, logConfigFields};

    m_daemon.port = r.read<u16>("daemon", "port");
    m_daemon.pidfile = r.read<Str>("daemon", "pid_file");
    m_daemon.binPath = r.read<Str>("daemon", "bin_path");

    m_logging.cliLevel = parseLogLevel(r.read<Str>("logging", "cli_level"));
    m_logging.daemonLevel =
        parseLogLevel(r.read<Str>("logging", "daemon_level"));
    m_logging.daemonFile = r.read<Str>("logging", "daemon_file");
    m_logging.daemonErrorFile = r.read<Str>("logging", "daemon_err");

    m_runtime.backend =
        parseComputeBackendType(r.read<Str>("runtime", "backend"));
}

void Config::parseVersionFile(const Path& path) {
    log::expect(
        path.isFile(), "Version file {} does not exist or is not a file",
        path.str()
    );

    // vx.y.z
    File f{path};
    auto versionStr = f.read();

    if (versionStr.back() == '\n') {
        versionStr.pop_back();
    }
    if (versionStr.front() == 'v') {
        versionStr.erase(0, 1);
    }

    auto parts = split(versionStr, ".");

    log::expect(
        parts.size() == 3, "Version file must be in the format vx.y.z, got {}",
        versionStr
    );

    try {
        m_version.major = std::stoul(parts[0]);
        m_version.minor = std::stoul(parts[1]);
        m_version.patch = std::stoul(parts[2]);
    } catch (const std::exception& e) {
        log::panic("Failed to parse version file: {}", e.what());
    }
}

// -- getters

const Path& Config::rootPath() const { return m_rootPath; }
const Config::Version& Config::version() const { return m_version; }
const Config::Daemon& Config::daemon() const { return m_daemon; }
const Config::Logging& Config::logging() const { return m_logging; }

}  // namespace hyperion
