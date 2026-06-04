set (HYPERION_SOURCES
    "main.cpp"
    "HCLI.cpp"
    "Command.cpp"
    # -- cmd
    "cmd/DaemonCommand.cpp"
    # -- daemon
    "daemon/DaemonProcess.cpp"
)

find_package(CLI11 REQUIRED)

add_executable(hyperion-cli ${HYPERION_SOURCES})
target_link_libraries(hyperion-cli PRIVATE hyperion CLI11::CLI11)
target_include_directories(hyperion-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
