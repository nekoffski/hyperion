add_library(hyperion SHARED
    "Platform.cpp"
    # -- api
    "api/cmd/Ack.cpp"
    "api/cmd/Health.cpp"
    "api/cmd/Error.cpp"
    "api/cmd/Job.cpp"
    "api/cmd/Workspace.cpp"
    "api/MessageDeserializer.cpp"
    "api/Session.cpp"
    # -- core
    "core/Config.cpp"
    "core/String.cpp"
    "core/Core.cpp"
    "core/Json.cpp"
    "core/Log.cpp"
    "core/String.cpp"
    "core/Error.cpp"
    "core/FileSystem.cpp"
    "core/Random.cpp"
    "core/Profiler.cpp"
    # -- runtime
    "runtime/Thread.cpp"
    "runtime/Process.cpp"
    "runtime/Signal.cpp"
    "runtime/unix/UnixProcess.cpp"
    "runtime/unix/UnixSignal.cpp"
    # -- net
    "net/TcpSession.cpp"
    "net/TcpServer.cpp"
    "net/Addr.cpp"
    # -- messaging
    "messaging/Payload.cpp"
)

target_include_directories(hyperion
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(hyperion
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        tomlplusplus::tomlplusplus
        stduuid::stduuid
        asio::asio
        nlohmann_json::nlohmann_json
)

target_precompile_headers(hyperion PUBLIC "core/Json.hh")

set_target_properties(hyperion PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(hyperion PUBLIC
    $<$<CONFIG:Debug>:hyperion_ENABLE_PROFILING>
    $<$<CONFIG:Debug>:hyperion_DEBUG_BUILD>
)
