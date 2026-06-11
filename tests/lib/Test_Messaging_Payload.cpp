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
