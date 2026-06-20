add_executable(hyperiond_tests
    "test_placeholder.cpp"
)

target_link_libraries(hyperiond_tests PRIVATE
    hyperiond_core
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(hyperiond_tests)
