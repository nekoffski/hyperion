#pragma once

#include "Process.hh"
#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/core/Error.hh"

namespace hyperion {

DEFINE_SUB_ERROR(SignalError, RuntimeError);

enum class Signal { terminate, interrupt, hangup, kill, quit, count };

class SignalManager : public StaticClass {
   public:
    using Handler = void (*)();

    class Impl : public NonCopyable, public NonMovable {
       public:
        virtual ~Impl() = default;

        virtual void send(Signal signal, Pid pid) = 0;
        virtual void registerHandler(Signal signal, Handler handler) = 0;
    };

    static void send(Signal signal, Pid pid);
    static void registerHandler(Signal signal, Handler handler);

    static void setImpl(Impl& impl);

   private:
    static Impl* s_impl;
};

}  // namespace hyperion
