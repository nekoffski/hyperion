set (HYPERION_DAEMON_SOURCES
    "main.cpp"
    "PidFile.cpp"
    "Daemon.cpp"
    # -- api
    "api/Server.cpp"
    "api/Controller.cpp"
    # -- orchestrator
    "orchestrator/WorkspaceManager.cpp"
    # -- workspace
    "workspace/DiskWorkspaceRepo.cpp"
)

add_executable(hyperiond ${HYPERION_DAEMON_SOURCES})
target_link_libraries(hyperiond PRIVATE hyperion)
target_include_directories(hyperiond PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
