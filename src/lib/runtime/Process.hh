#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/core/Error.hh"
#include "lib/core/FileSystem.hh"

namespace hyperion {

DEFINE_SUB_ERROR(ProcessError, RuntimeError);

using Pid = i64;

struct ProcessDescription {
    Str bin;
    std::vector<Str> args;
    std::unordered_map<Str, Str> env;
    Opt<Path> stdout;
    Opt<Path> stderr;
    Opt<Path> stdin;
};

class ProcessManager : public StaticClass {
   public:
    class Impl : public NonCopyable, public NonMovable {
       public:
        virtual ~Impl() = default;

        virtual Pid currentPid() const = 0;
        virtual bool isRunning(Pid pid) const = 0;
        virtual Pid spawn(const ProcessDescription& desc) = 0;
    };

    static void setImpl(Impl& impl);

    static Pid currentPid();
    static Pid spawn(const ProcessDescription& desc);

    static bool isRunning(Pid pid);

   private:
    static Impl* s_impl;
};

}  // namespace hyperion
