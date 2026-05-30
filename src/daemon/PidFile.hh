#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/FileSystem.hh"
#include "lib/runtime/Process.hh"

namespace hyperion {

class PidFile : public NonCopyable, public NonMovable {
   public:
    explicit PidFile(const Path& path);
    ~PidFile();

    Pid getPid() const;

   private:
    Path m_path;
    Pid m_pid;
};

}  // namespace hyperion
