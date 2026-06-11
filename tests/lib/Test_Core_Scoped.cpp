#include <gtest/gtest.h>

#include "lib/core/Scoped.hh"

using namespace hyperion;

TEST(ScopedTest, DestructorCalledOnScopeExit) {
    bool destroyed = false;
    {
        Scoped<int> s{42, [&](int&) { destroyed = true; }};
        EXPECT_FALSE(s.empty());
    }
    EXPECT_TRUE(destroyed);
}

TEST(ScopedTest, GetReturnsResource) {
    Scoped<int> s{99, [](int&) {}};
    EXPECT_EQ(s.get(), 99);
    EXPECT_EQ(*s, 99);
}

TEST(ScopedTest, DefaultConstructedIsEmpty) {
    Scoped<int> s;
    EXPECT_TRUE(s.empty());
}

TEST(ScopedTest, MoveTransfersOwnership) {
    bool destroyed = false;
    Scoped<int> a{1, [&](int&) { destroyed = true; }};
    Scoped<int> b = std::move(a);

    EXPECT_TRUE(a.empty());
    EXPECT_FALSE(b.empty());
    EXPECT_FALSE(destroyed);
}

TEST(ScopedTest, MoveDestructorOnlyCalledOnce) {
    int count = 0;
    {
        Scoped<int> a{1, [&](int&) { ++count; }};
        Scoped<int> b = std::move(a);
    }
    EXPECT_EQ(count, 1);
}

TEST(ScopedTest, MoveAssignmentCallsOldDestructor) {
    int count = 0;
    Scoped<int> a{1, [&](int&) { ++count; }};
    Scoped<int> b{2, [&](int&) { ++count; }};
    a = std::move(b);
    EXPECT_EQ(count, 1);
}
