#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Core.hh"
#include "FileSystem.hh"

namespace hyp {

class Config {
   public:
    struct Version {
        u32 major{0};
        u32 minor{0};
        u32 patch{0};
    };

    const Version& version() const;
    static Config fromFile(const Path& path);

   private:
    void parseFields(const Path& path);

    Version m_version;
};

}  // namespace hyp
