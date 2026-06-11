#include <gtest/gtest.h>

#include "lib/messaging/Payload.hh"

using namespace hyperion;

TEST(PayloadTest, RoundtripU8) {
    PayloadWriter w;
    w.write<u8>(0xAB);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<u8>(), 0xAB);
}

TEST(PayloadTest, RoundtripU32) {
    PayloadWriter w;
    w.write<u32>(0xDEADBEEF);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<u32>(), 0xDEADBEEF);
}

TEST(PayloadTest, RoundtripFloat) {
    PayloadWriter w;
    w.write<f32>(3.14f);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_FLOAT_EQ(r.read<f32>(), 3.14f);
}

TEST(PayloadTest, RoundtripString) {
    PayloadWriter w;
    w.write(std::string{"hello"});
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<std::string>(), "hello");
}

TEST(PayloadTest, RoundtripMultipleValues) {
    PayloadWriter w;
    w.write<u8>(1).write<u16>(1000).write(std::string{"world"}).write<u32>(99);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<u8>(), 1);
    EXPECT_EQ(r.read<u16>(), 1000);
    EXPECT_EQ(r.read<std::string>(), "world");
    EXPECT_EQ(r.read<u32>(), 99);
}

TEST(PayloadTest, OutParamRead) {
    PayloadWriter w;
    w.write<u32>(42);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    u32 val = 0;
    r.read(val);
    EXPECT_EQ(val, 42);
}

TEST(PayloadTest, EmptyStringRoundtrip) {
    PayloadWriter w;
    w.write(std::string{""});
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<std::string>(), "");
}

TEST(PayloadTest, RoundtripVectorOfU32) {
    const std::vector<u32> data{1, 2, 3, 4, 5};
    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<std::vector<u32>>(), data);
}

TEST(PayloadTest, RoundtripVectorOfStrings) {
    const std::vector<std::string> data{"foo", "bar", "baz"};
    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<std::vector<std::string>>(), data);
}

TEST(PayloadTest, RoundtripEmptyVector) {
    const std::vector<u32> data{};
    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    EXPECT_EQ(r.read<std::vector<u32>>(), data);
}

TEST(PayloadTest, OutParamReadVector) {
    const std::vector<u16> data{10, 20, 30};
    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    std::vector<u16> out;
    r.read(out);
    EXPECT_EQ(out, data);
}

TEST(PayloadTest, RoundtripFlatMapStringToU32) {
    FlatMap<std::string, u32> data;
    data.insert("alpha", 1);
    data.insert("beta", 2);
    data.insert("gamma", 3);

    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    auto result = r.read<FlatMap<std::string, u32>>();

    EXPECT_EQ(result.size(), data.size());
    for (const auto& [k, v] : data) {
        EXPECT_TRUE(result.contains(k));
        EXPECT_EQ(result.at(k), v);
    }
}

TEST(PayloadTest, RoundtripFlatMapU32ToString) {
    FlatMap<u32, std::string> data;
    data.insert(1, "one");
    data.insert(2, "two");

    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    auto result = r.read<FlatMap<u32, std::string>>();

    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result.at(1), "one");
    EXPECT_EQ(result.at(2), "two");
}

TEST(PayloadTest, RoundtripEmptyFlatMap) {
    FlatMap<std::string, u32> data;

    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    auto result = r.read<FlatMap<std::string, u32>>();

    EXPECT_TRUE(result.empty());
}

TEST(PayloadTest, OutParamReadFlatMap) {
    FlatMap<std::string, u32> data;
    data.insert("x", 42);

    PayloadWriter w;
    w.write(data);
    auto buf = w.getBuffer();
    PayloadBuffer owned{buf.begin(), buf.end()};

    PayloadReader r{owned};
    FlatMap<std::string, u32> out;
    r.read(out);

    EXPECT_EQ(out.size(), 1u);
    EXPECT_EQ(out.at("x"), 42u);
}
