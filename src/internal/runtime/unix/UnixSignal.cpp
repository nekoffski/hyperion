#include "UnixSignal.hh"

#include <signal.h>
#include <unistd.h>

#include "internal/core/Log.hh"

namespace hyperion {

UnixSignalManager::Handlers UnixSignalManager::m_handlers;

namespace {

i32 toUnixSignal(Signal signal) {
    switch (signal) {
        case Signal::terminate:
            return SIGTERM;
        case Signal::interrupt:
            return SIGINT;
        case Signal::hangup:
            return SIGHUP;
        case Signal::quit:
            return SIGQUIT;
        case Signal::kill:
            return SIGKILL;
    }
    throw SignalError{
        ErrorCode::invalidEnumValue, "Invalid signal enum value: {}",
        static_cast<u16>(signal)
    };
}

Signal fromUnixSignal(i32 unixSignal) {
    switch (unixSignal) {
        case SIGTERM:
            return Signal::terminate;
        case SIGINT:
            return Signal::interrupt;
        case SIGHUP:
            return Signal::hangup;
        case SIGQUIT:
            return Signal::quit;
        case SIGKILL:
            return Signal::kill;
    }
    throw SignalError{
        ErrorCode::invalidEnumValue, "Invalid signal numeric value: {}",
        unixSignal
    };
}

}  // namespace

void UnixSignalManager::send(Signal signal, Pid pid) {
    ::kill(pid, toUnixSignal(signal));
}

void UnixSignalManager::registerHandler(
    Signal signal, SignalManager::Handler handler
) {
    auto unixSignal = toUnixSignal(signal);
    log::info("registering handler for signal: {}", unixSignal);

    struct sigaction sa{};
    sa.sa_handler = [](i32 signum) {
        auto sig = static_cast<u16>(fromUnixSignal(signum));

        if (sig >= static_cast<u16>(Signal::count)) {
            log::error(
                "Received invalid signal with numeric value: {}/{}", sig, signum
            );
            return;
        }
        if (auto cb = m_handlers[sig]; cb) {
            cb();
        }
    };

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(unixSignal, &sa, nullptr) < 0) {
        throw SignalError{
            "Failed to register signal handler: {}", std::strerror(errno)
        };
    }

    m_handlers[static_cast<u16>(signal)] = handler;
}

}  // namespace hyperion