#include <gtest/gtest.h>

#include "internal/core/Core.hh"

using namespace hyperion;

// getAlignedValue

TEST(TypesTest, GetAlignedValueAlreadyAligned) {
    EXPECT_EQ(getAlignedValue<u64>(256, 256), 256);
}

TEST(TypesTest, GetAlignedValueRoundsUp) {
    EXPECT_EQ(getAlignedValue<u64>(257, 256), 512);
}

TEST(TypesTest, GetAlignedValueZero) {
    EXPECT_EQ(getAlignedValue<u64>(0, 64), 0);
}

// Range::aligned

TEST(TypesTest, RangeAlignedBothAligned) {
    auto r = Range::aligned(256, 512, 256);
    EXPECT_EQ(r.offset, 256);
    EXPECT_EQ(r.size, 512);
}

TEST(TypesTest, RangeAlignedRoundsUpOffset) {
    auto r = Range::aligned(100, 512, 256);
    EXPECT_EQ(r.offset, 256);
}

TEST(TypesTest, RangeAlignedRoundsUpSize) {
    auto r = Range::aligned(256, 300, 256);
    EXPECT_EQ(r.size, 512);
}

// Wrapper

TEST(TypesTest, WrapperGet) {
    Wrapper<int> w{42};
    EXPECT_EQ(w.get(), 42);
}

TEST(TypesTest, WrapperEqualityTrue) {
    Wrapper<int> a{10};
    Wrapper<int> b{10};
    EXPECT_EQ(a, b);
}

TEST(TypesTest, WrapperEqualityFalse) {
    Wrapper<int> a{10};
    Wrapper<int> b{20};
    EXPECT_NE(a, b);
}

TEST(TypesTest, WrapperHash) {
    Wrapper<int> w{99};
    std::hash<Wrapper<int>> h;
    EXPECT_EQ(h(w), std::hash<int>{}(99));
}

// Tag

TEST(TypesTest, TagGet) {
    Tag<int> t{7};
    EXPECT_EQ(t.get(), 7);
}

TEST(TypesTest, TagEquality) {
    Tag<std::string> a{"hello"};
    Tag<std::string> b{"hello"};
    EXPECT_EQ(a, b);
}

TEST(TypesTest, TagInequality) {
    Tag<std::string> a{"hello"};
    Tag<std::string> b{"world"};
    EXPECT_NE(a, b);
}

// Rect

TEST(TypesTest, RectFields) {
    Rect<int> r{1, 2, 3, 4};
    EXPECT_EQ(r.x, 1);
    EXPECT_EQ(r.y, 2);
    EXPECT_EQ(r.w, 3);
    EXPECT_EQ(r.h, 4);
}
