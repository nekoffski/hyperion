set (HYPERION_SOURCES
    "main.cpp"
)

find_package(CLI11 REQUIRED)

add_executable(hyperion-cli ${HYPERION_SOURCES})
target_link_libraries(hyperion-cli PRIVATE hyperion CLI11::CLI11)
