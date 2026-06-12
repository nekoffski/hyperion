#include <gtest/gtest.h>

#include "internal/core/Enum.hh"

enum class Perms : int {
    none = 0,
    read = 1 << 0,
    write = 1 << 1,
    exec = 1 << 2,
};
hyp_BIT_ENUM(Perms)

    using namespace hyperion;

TEST(EnumTest, CheckFlagTrue) {
    auto flags = Perms::read | Perms::write;
    EXPECT_TRUE(checkFlag(flags, Perms::read));
    EXPECT_TRUE(checkFlag(flags, Perms::write));
}

TEST(EnumTest, CheckFlagFalse) {
    auto flags = Perms::read;
    EXPECT_FALSE(checkFlag(flags, Perms::write));
    EXPECT_FALSE(checkFlag(flags, Perms::exec));
}

TEST(EnumTest, BitwiseOr) {
    auto result = Perms::read | Perms::exec;
    EXPECT_TRUE(checkFlag(result, Perms::read));
    EXPECT_TRUE(checkFlag(result, Perms::exec));
    EXPECT_FALSE(checkFlag(result, Perms::write));
}

TEST(EnumTest, BitwiseAnd) {
    auto flags = Perms::read | Perms::write | Perms::exec;
    auto masked = flags & (Perms::read | Perms::write);
    EXPECT_TRUE(checkFlag(masked, Perms::read));
    EXPECT_TRUE(checkFlag(masked, Perms::write));
    EXPECT_FALSE(checkFlag(masked, Perms::exec));
}

TEST(EnumTest, BitwiseOrAssign) {
    Perms flags = Perms::read;
    flags |= Perms::write;
    EXPECT_TRUE(checkFlag(flags, Perms::read));
    EXPECT_TRUE(checkFlag(flags, Perms::write));
}

TEST(EnumTest, BitwiseAndAssign) {
    Perms flags = Perms::read | Perms::write;
    flags &= Perms::read;
    EXPECT_TRUE(checkFlag(flags, Perms::read));
    EXPECT_FALSE(checkFlag(flags, Perms::write));
}

TEST(EnumTest, NoneFlag) { EXPECT_FALSE(checkFlag(Perms::none, Perms::read)); }
