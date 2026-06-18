#include "OS.hh"

namespace hyperion {

Str toString(OS os) {
    switch (os) {
        case OS::linux:
            return "Linux";
        case OS::windows:
            return "Windows";
        case OS::darwin:
            return "Darwin";
    }
    return "Unknown";
}

}  // namespace hyperion
