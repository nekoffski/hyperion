add_executable(hyperion_tests
    "Test_Core_FlatMap.cpp"
    "Test_Core_String.cpp"
    "Test_Core_Types.cpp"
    "Test_Core_Enum.cpp"
    "Test_Core_Error.cpp"
    "Test_Core_Functional.cpp"
    "Test_Core_Scoped.cpp"
    "Test_Messaging_Payload.cpp"
    "Test_Api_Cmd.cpp"
)

target_link_libraries(hyperion_tests PRIVATE
    hyperion
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(hyperion_tests)
