
#include "HCLI.hh"
#include "lib/Platform.hh"
#include "lib/core/Config.hh"
#include "lib/core/Error.hh"
#include "lib/core/Log.hh"
#include "lib/core/Scope.hh"
#include "lib/core/ServiceLocator.hh"
#include "lib/net/Asio.hh"

using namespace hyperion;

int main(int argc, char** argv) {
    log::init(log::LoggerOptions{
        .enableColors = true,
        .formatPattern = "hcli> %v",
    });

    auto config = Config::fromEnv(Config::LogConfigFields::skip);
    ServiceLocator<Config>::set(&config);
    log::setLogLevel(config.logging().cliLevel);

    Platform::init();

    log::info(
        "hyperion cli version {}.{}.{}", config.version().major,
        config.version().minor, config.version().patch
    );
    ON_SCOPE_EXIT { log::info("cya"); };

    try {
        HCLI{config}.parse(argc, argv);
    } catch (const Exception& e) {
        log::error(
            "unhandled exception in main: {} - {}", fmt::underlying(e.code()),
            e.message()
        );
        return static_cast<int>(e.code());
    } catch (const asio::system_error& e) {
        log::error("ASIO system error in main: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        log::error("unhandled exception in main: {}", e.what());
        throw;
    } catch (...) {
        log::error("unhandled unknown exception in main");
        throw;
    }
    return 0;
}
