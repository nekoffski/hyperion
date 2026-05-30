#include "Config.hh"

#include <boost/algorithm/string.hpp>
#include <toml++/toml.hpp>

#include "FileSystem.hh"
#include "Log.hh"

namespace hyperion {

namespace {

class Reader : public NonCopyable, public NonMovable {
   public:
    explicit Reader(const toml::table& tbl) : m_tbl(tbl) {}

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
            log::debug(
                "Config: read {}.{} = [ {} ]", stanza, key,
                fmt::join(result, ",")
            );
            return result;
        } else {
            auto v = raw->value<T>();
            log::expect(
                v.has_value(),
                "Config file must contain a {} field in the [{}] table", key,
                stanza
            );
            log::debug("Config: read {}.{} = {}", stanza, key, *v);
            return *v;
        }
    }

   private:
    const toml::table& m_tbl;
};

}  // namespace

Config Config::fromFile(const Path& path) {
    Config cfg;

    cfg.parseVersionFile(Path::join(path, "etc/version"));

    try {
        cfg.parseFields(Path::join(path, "etc/hyperion.toml"));
    } catch (const toml::parse_error& e) {
        log::panic("Failed to parse config file: {}", e.what());
    } catch (const std::bad_optional_access& e) {
        log::panic("Failed to parse config file: {}", e.what());
    }

    return cfg;
}

using StrVec = std::vector<Str>;

static log::Level parseLogLevel(const Str& levelStr) {
    static const std::unordered_map<Str, log::Level> levelMap = {
        {"trace", log::Level::trace}, {"debug", log::Level::debug},
        {"info", log::Level::info},   {"warn", log::Level::warn},
        {"error", log::Level::error}, {"critical", log::Level::critical},
        {"off", log::Level::off},
    };

    auto it = levelMap.find(boost::algorithm::to_lower_copy(levelStr));
    log::expect(
        it != levelMap.end(),
        "Invalid log level '{}', must be one of: trace, debug, info, warn, "
        "error, critical, off",
        levelStr
    );
    return it->second;
}

void Config::parseFields(const Path& path) {
    log::expect(
        path.isFile(), "Config path {} does not exist or is not a file",
        path.str()
    );

    auto tbl = toml::parse_file(path.str());
    Reader r{tbl};

    m_daemon.port = r.read<u16>("daemon", "port");
    m_daemon.pidfile = r.read<Str>("daemon", "pid_file");

    m_logging.level = parseLogLevel(r.read<Str>("logging", "level"));
    m_logging.file = r.read<Str>("logging", "file");
}

void Config::parseVersionFile(const Path& path) {
    log::expect(
        path.isFile(), "Version file {} does not exist or is not a file",
        path.str()
    );

    // vx.y.z
    File f{path};
    auto versionStr = f.read();

    if (versionStr.back() == '\n') versionStr.pop_back();
    if (versionStr.front() == 'v') versionStr.erase(0, 1);

    std::vector<Str> parts;
    boost::algorithm::split(parts, versionStr, boost::is_any_of("."));

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

const Config::Version& Config::version() const { return m_version; }
const Config::Daemon& Config::daemon() const { return m_daemon; }
const Config::Logging& Config::logging() const { return m_logging; }

}  // namespace hyperion
