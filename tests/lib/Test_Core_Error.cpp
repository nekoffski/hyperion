#include <gtest/gtest.h>

#include "lib/core/Error.hh"

using namespace hyperion;

TEST(ErrorTest, ExceptionMessage) {
    Exception e{"error: {}", 42};
    EXPECT_EQ(e.message(), "error: 42");
}

TEST(ErrorTest, ExceptionDefaultCode) {
    Exception e{"oops"};
    EXPECT_EQ(e.code(), ErrorCode::none);
}

TEST(ErrorTest, ExceptionWithCode) {
    Exception e{ErrorCode::invalidArgument, "bad arg: {}", "x"};
    EXPECT_EQ(e.code(), ErrorCode::invalidArgument);
    EXPECT_EQ(e.message(), "bad arg: x");
}

TEST(ErrorTest, WhatMatchesMessage) {
    Exception e{"something went wrong"};
    EXPECT_STREQ(e.what(), "something went wrong");
}

TEST(ErrorTest, RuntimeErrorIsException) {
    RuntimeError e{"runtime fail"};
    EXPECT_TRUE((std::is_base_of_v<Exception, RuntimeError>));
    EXPECT_EQ(e.message(), "runtime fail");
}

TEST(ErrorTest, RuntimeErrorWithCode) {
    RuntimeError e{ErrorCode::ioError, "io failed"};
    EXPECT_EQ(e.code(), ErrorCode::ioError);
}

TEST(ErrorTest, CatchAsStdException) {
    try {
        throw Exception{ErrorCode::outOfRange, "out of range"};
    } catch (const std::exception& ex) {
        EXPECT_STREQ(ex.what(), "out of range");
    }
}

TEST(ErrorTest, CatchSubclassAsBaseException) {
    try {
        throw RuntimeError{"runtime"};
    } catch (const Exception& ex) {
        EXPECT_EQ(ex.message(), "runtime");
    }
}
