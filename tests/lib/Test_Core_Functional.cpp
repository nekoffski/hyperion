#include <gtest/gtest.h>

#include <ranges>
#include <variant>

#include "lib/core/Functional.hh"

using namespace hyperion;

// GuardCall

TEST(FunctionalTest, GuardCallInvokesOnDestruction) {
    bool called = false;
    {
        GuardCall g{[&]() { called = true; }};
    }
    EXPECT_TRUE(called);
}

TEST(FunctionalTest, GuardCallDefaultConstructorDoesNotCrash) {
    EXPECT_NO_THROW({ GuardCall g{}; });
}

// Overloader

TEST(FunctionalTest, OverloaderSelectsCorrectLambda) {
    Overloader o{
        [](int x) { return x * 2; },
        [](std::string s) { return (int)s.size(); },
    };
    EXPECT_EQ(o(5), 10);
    EXPECT_EQ(o(std::string{"hello"}), 5);
}

TEST(FunctionalTest, OverloaderWithVariant) {
    using V = std::variant<int, std::string>;
    Overloader visitor{
        [](int x) -> std::string { return "int:" + std::to_string(x); },
        [](const std::string& s) -> std::string { return "str:" + s; },
    };
    EXPECT_EQ(std::visit(visitor, V{42}), "int:42");
    EXPECT_EQ(std::visit(visitor, V{std::string{"hi"}}), "str:hi");
}

// LazyEvaluator

TEST(FunctionalTest, LazyEvaluatorEvaluatesOnConversion) {
    int callCount = 0;
    auto lazy = lazyEvaluate([&]() {
        ++callCount;
        return 99;
    });
    EXPECT_EQ(callCount, 0);
    int val = lazy;
    EXPECT_EQ(callCount, 1);
    EXPECT_EQ(val, 99);
}

TEST(FunctionalTest, LazyEvaluateMacro) {
    int x = 5;
    auto lazy = LAZY_EVALUATE(x * 2);
    x = 10;
    int result = lazy;
    EXPECT_EQ(result, 20);
}

// toVector

TEST(FunctionalTest, ToVectorFromRange) {
    std::vector<int> src = {1, 2, 3, 4, 5};
    auto result = src | std::views::filter([](int x) { return x % 2 == 0; }) |
                  toVector<int>();
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 4);
}

TEST(FunctionalTest, ToVectorPreservesOrder) {
    std::vector<int> src = {5, 3, 1, 4, 2};
    auto result = src | toVector<int>();
    EXPECT_EQ(result, src);
}
