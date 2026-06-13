add_library(ksl SHARED
    "Lexer.cpp"
    "Parser.cpp"
    "CxxEmitter.cpp"
    "OpenCLEmitter.cpp"
    "Transpiler.cpp"
)

target_include_directories(ksl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(ksl
    PUBLIC
        hyperion
        fmt::fmt
)

set_target_properties(ksl PROPERTIES POSITION_INDEPENDENT_CODE ON)
