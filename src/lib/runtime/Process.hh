#pragma once

#include <memory>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

using Pid = u16;

class ProcessManager : public StaticClass {
   public:
    class Impl : public NonCopyable, public NonMovable {
       public:
        virtual ~Impl() = default;

        virtual Pid getPid() const = 0;
    };

    static void setImpl(Impl& impl);

    static Pid getPid();

   private:
    static Impl* s_impl;
};

}  // namespace hyperion
