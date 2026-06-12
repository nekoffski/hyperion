#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/FileSystem.hh"
#include "internal/runtime/Process.hh"

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
