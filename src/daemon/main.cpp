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

    Platform::init();

    auto homeDir = getEnv<Str>("HYPERION_HOME");
    log::expect(
        homeDir.has_value(), "HYPERION_HOME environment variable is not set"
    );

    auto config = Config::fromFile(*homeDir);
    ServiceLocator<Config>::set(&config);

    PidFile pidFile{config.daemon().pidfile};

    ThreadGroup tg;

    tg.start();
    tg.join();

    return 0;
}
