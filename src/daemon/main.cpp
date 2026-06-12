#include "Daemon.hh"
#include "PidFile.hh"
#include "internal/Platform.hh"
#include "internal/core/Config.hh"
#include "internal/core/Core.hh"
#include "internal/core/Env.hh"
#include "internal/core/Log.hh"
#include "internal/core/Scope.hh"
#include "internal/core/ServiceLocator.hh"
#include "internal/runtime/Thread.hh"

using namespace hyperion;

int main() {
    log::init(
        log::LoggerOptions{
            .enableColors = true,
            .formatPattern = "[%Y-%m-%d %T] [Th: %t] %-5l [hyperion]: %v",
        }
    );
    log::info("starting hyperion daemon");
    ON_SCOPE_EXIT { log::info("hyperion daemon finished, cya"); };

    try {
        Platform::init();

        auto config = Config::fromEnv();
        ServiceLocator<Config>::set(&config);
        log::setLogLevel(config.logging().daemonLevel);

        PidFile pidFile{config.daemon().pidfile};

        log::info(
            "Starting hyperiond version {}.{}.{}", config.version().major,
            config.version().minor, config.version().patch
        );

        Daemon daemon{config};
        ServiceLocator<Daemon>::set(&daemon);

        SignalManager::registerHandler(Signal::interrupt, []() {
            log::info("received interrupt signal, shutting down...");
            ServiceLocator<Daemon>::get().stop();
        });

        SignalManager::registerHandler(Signal::terminate, []() {
            log::info("received terminate signal, shutting down...");
            ServiceLocator<Daemon>::get().stop();
        });

        daemon.start();

    } catch (const Exception& e) {
        log::error(
            "unhandled exception in main: {} - {}", fmt::underlying(e.code()),
            e.message()
        );
        return static_cast<int>(e.code());
    } catch (const std::exception& e) {
        log::error("unhandled exception in main: {}", e.what());
        throw;
    } catch (...) {
        log::error("unhandled unknown exception in main");
        throw;
    }

    return 0;
}
