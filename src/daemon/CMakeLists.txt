set (HYPERION_DAEMON_SOURCES
    "main.cpp"
    "PidFile.cpp"
)

add_executable(hyperiond ${HYPERION_DAEMON_SOURCES})
target_link_libraries(hyperiond PRIVATE hyperion)
