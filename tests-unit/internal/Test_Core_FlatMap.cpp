#include <gtest/gtest.h>

#include "internal/core/FlatMap.hh"

using namespace hyperion;

TEST(FlatMapTest, StartsEmpty) {
    FlatMap<int, int> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(FlatMapTest, InsertAndContains) {
    FlatMap<int, std::string> m;
    EXPECT_TRUE(m.insert(1, "one"));
    EXPECT_TRUE(m.contains(1));
    EXPECT_EQ(m.size(), 1);
}

TEST(FlatMapTest, InsertDuplicateReturnsFalse) {
    FlatMap<int, std::string> m;
    m.insert(1, "one");
    EXPECT_FALSE(m.insert(1, "uno"));
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(m.at(1), "one");
}

TEST(FlatMapTest, AtReturnsValue) {
    FlatMap<std::string, int> m;
    m.insert("a", 42);
    EXPECT_EQ(m.at("a"), 42);
}

TEST(FlatMapTest, AtThrowsOnMissingKey) {
    FlatMap<std::string, int> m;
    EXPECT_THROW(m.at("missing"), std::out_of_range);
}

TEST(FlatMapTest, ConstAtThrowsOnMissingKey) {
    const FlatMap<std::string, int> m;
    EXPECT_THROW(m.at("missing"), std::out_of_range);
}

TEST(FlatMapTest, SubscriptOperatorInsertsDefault) {
    FlatMap<int, int> m;
    m[5];
    EXPECT_TRUE(m.contains(5));
    EXPECT_EQ(m[5], 0);
}

TEST(FlatMapTest, SubscriptOperatorReturnsExisting) {
    FlatMap<int, int> m;
    m.insert(1, 10);
    m[1] = 20;
    EXPECT_EQ(m.at(1), 20);
    EXPECT_EQ(m.size(), 1);
}

TEST(FlatMapTest, InsertOrAssignUpdatesExisting) {
    FlatMap<int, std::string> m;
    m.insert(1, "one");
    EXPECT_FALSE(m.insertOrAssign(1, "ONE"));
    EXPECT_EQ(m.at(1), "ONE");
    EXPECT_EQ(m.size(), 1);
}

TEST(FlatMapTest, InsertOrAssignInsertsNew) {
    FlatMap<int, std::string> m;
    EXPECT_TRUE(m.insertOrAssign(1, "one"));
    EXPECT_EQ(m.size(), 1);
}

TEST(FlatMapTest, EraseRemovesKey) {
    FlatMap<int, int> m;
    m.insert(1, 10);
    EXPECT_TRUE(m.erase(1));
    EXPECT_FALSE(m.contains(1));
    EXPECT_TRUE(m.empty());
}

TEST(FlatMapTest, EraseReturnsFalseForMissingKey) {
    FlatMap<int, int> m;
    EXPECT_FALSE(m.erase(99));
}

TEST(FlatMapTest, GetReturnsValueRef) {
    FlatMap<int, int> m;
    m.insert(1, 42);
    auto v = m.get(1);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->get(), 42);
}

TEST(FlatMapTest, GetReturnsNulloptForMissingKey) {
    FlatMap<int, int> m;
    EXPECT_FALSE(m.get(99).has_value());
}

TEST(FlatMapTest, ConstGetReturnsValueRef) {
    FlatMap<int, int> m;
    m.insert(1, 42);
    const auto& cm = m;
    auto v = cm.get(1);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->get(), 42);
}

TEST(FlatMapTest, ClearEmptiesMap) {
    FlatMap<int, int> m;
    m.insert(1, 1);
    m.insert(2, 2);
    m.clear();
    EXPECT_TRUE(m.empty());
}

TEST(FlatMapTest, Iteration) {
    FlatMap<int, int> m;
    m.insert(1, 10);
    m.insert(2, 20);
    m.insert(3, 30);

    int sum = 0;
    for (auto& [k, v] : m) {
        sum += v;
    }
    EXPECT_EQ(sum, 60);
}
