#include "Daemon.hh"
#include "PidFile.hh"
#include "lib/Platform.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/Env.hh"
#include "lib/core/Log.hh"
#include "lib/core/Scope.hh"
#include "lib/core/ServiceLocator.hh"
#include "lib/runtime/Thread.hh"

using namespace hyperion;

int main() {
    log::init(log::LoggerOptions{
        .enableColors = true,
        .formatPattern = "[%Y-%m-%d %T] [Th: %t] %-5l [hyperion]: %v",
    });
    log::info("starting hyperion daemon");
    ON_SCOPE_EXIT { log::info("hyperion daemon finished, cya"); };

    try {
        Platform::init();

        auto homeDir = getEnv<Str>("HYPERION_HOME");
        log::expect(
            homeDir.has_value(), "HYPERION_HOME environment variable is not set"
        );

        auto config = Config::fromFile(*homeDir);
        ServiceLocator<Config>::set(&config);

        PidFile pidFile{config.daemon().pidfile};

        log::info(
            "Starting hyperiond version {}.{}.{}", config.version().major,
            config.version().minor, config.version().patch
        );

        Daemon daemon;
        ServiceLocator<Daemon>::set(&daemon);

        SignalManager::registerHandler(Signal::interrupt, []() {
            log::info("received interrupt signal, shutting down...");
            ServiceLocator<Daemon>::get().stop();
        });

        SignalManager::registerHandler(Signal::terminate, []() {
            log::info("received terminate signal, shutting down...");
            ServiceLocator<Daemon>::get().stop();
        });

        daemon.start(config);

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
