#include "UnixProcess.hh"

#include <unistd.h>

namespace hyperion {

Pid UnixProcess::getPid() const { return getpid(); }

}  // namespace hyperion
