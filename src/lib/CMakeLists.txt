add_library(hyperion SHARED
    # -- core
    "core/Config.cpp"
    "core/String.cpp"
    "core/Core.cpp"
    "core/Log.cpp"
    "core/String.cpp"
    "core/Error.cpp"
    "core/FileSystem.cpp"
    "core/Random.cpp"
    "core/Profiler.cpp"
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
)

set_target_properties(hyperion PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(hyperion PUBLIC
    $<$<CONFIG:Debug>:hyperion_ENABLE_PROFILING>
    $<$<CONFIG:Debug>:hyperion_DEBUG_BUILD>
)
