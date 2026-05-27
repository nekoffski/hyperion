#include "Config.hh"

#include <toml++/toml.hpp>

#include "FileSystem.hh"
#include "Log.hh"

namespace hyp {

namespace {

class Reader : public NonCopyable, public NonMovable {
   public:
    explicit Reader(const toml::table& tbl) : m_tbl(tbl) {}

    template <typename T>
    T read(const std::string& stanza, const std::string& key) const {
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

        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            log::expect(
                raw->is_array(), "Config file field {}.{} must be an array",
                stanza, key
            );
            std::vector<std::string> result;
            for (const auto& item : *raw->as_array()) {
                log::expect(
                    item.is_string(),
                    "Config file field {}.{} must be an array of strings",
                    stanza, key
                );
                result.push_back(item.value<std::string>().value_or(""));
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

    log::expect(
        path.isFile(), "Config path {} does not exist or is not a file",
        path.str()
    );

    try {
        cfg.parseFields(path);
    } catch (const toml::parse_error& e) {
        log::panic("Failed to parse config file: {}", e.what());
    } catch (const std::bad_optional_access& e) {
        log::panic("Failed to parse config file: {}", e.what());
    }

    return cfg;
}

using StrVec = std::vector<std::string>;

void Config::parseFields(const Path& path) {
    auto tbl = toml::parse_file(path.str());
    Reader r{tbl};

    m_version.major = r.read<u32>("version", "major");
    m_version.minor = r.read<u32>("version", "minor");
    m_version.patch = r.read<u32>("version", "patch");
}

// -- getters

const Config::Version& Config::version() const { return m_version; }

}  // namespace hyp
