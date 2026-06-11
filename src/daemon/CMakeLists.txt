add_library(hyperiond_core STATIC
    "PidFile.cpp"
    "Daemon.cpp"
    # -- api
    "api/Server.cpp"
    "api/Controller.cpp"
    # -- workspace
    "workspace/WorkspaceManager.cpp"
    # -- adapters
    "adapters/DiskWorkspaceRepo.cpp"
)

target_link_libraries(hyperiond_core PUBLIC hyperion)
target_include_directories(hyperiond_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(hyperiond "main.cpp")
target_link_libraries(hyperiond PRIVATE hyperiond_core)
