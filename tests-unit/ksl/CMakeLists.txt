add_executable(ksl_tests
    "Test_Ksl_Lexer.cpp"
    "Test_Ksl_Parser.cpp"
    "Test_Ksl_CxxEmitter.cpp"
    "Test_Ksl_OpenCLEmitter.cpp"
    "Test_Ksl_Transpiler.cpp"
)

target_link_libraries(ksl_tests PRIVATE
    ksl
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(ksl_tests)
