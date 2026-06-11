#include <gtest/gtest.h>

#include "lib/core/String.hh"

using namespace hyperion;

// split

TEST(StringTest, SplitBasic) {
    auto result = split("a,b,c", ",");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}

TEST(StringTest, SplitNoSeparator) {
    auto result = split("abc", ",");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "abc");
}

TEST(StringTest, SplitEmptyString) {
    auto result = split("", ",");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "");
}

TEST(StringTest, SplitMulticharSeparator) {
    auto result = split("a::b::c", "::");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}

TEST(StringTest, SplitTrailingSeparator) {
    auto result = split("a,b,", ",");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[2], "");
}

// nameFromPath

TEST(StringTest, NameFromPathWithExtension) {
    EXPECT_EQ(
        nameFromPath("path/to/file.tar.gz", NameExtractionMode::withExtension),
        "file.tar.gz"
    );
}

TEST(StringTest, NameFromPathWithoutFullExtension) {
    EXPECT_EQ(
        nameFromPath(
            "path/to/file.tar.gz", NameExtractionMode::withoutFullExtension
        ),
        "file"
    );
}

TEST(StringTest, NameFromPathWithoutLastExtensionChunk) {
    EXPECT_EQ(
        nameFromPath(
            "path/to/file.tar.gz", NameExtractionMode::withoutLastExtensionChunk
        ),
        "file.tar"
    );
}

TEST(StringTest, NameFromPathNoDirectory) {
    EXPECT_EQ(
        nameFromPath("file.cpp", NameExtractionMode::withExtension), "file.cpp"
    );
}

TEST(StringTest, NameFromPathNoExtension) {
    EXPECT_EQ(
        nameFromPath("path/to/file", NameExtractionMode::withoutFullExtension),
        "file"
    );
}

// extensionFromPath

TEST(StringTest, ExtensionFromPathFull) {
    auto ext =
        extensionFromPath("path/to/file.tar.gz", ExtensionExtractionMode::full);
    ASSERT_TRUE(ext.has_value());
    EXPECT_EQ(*ext, ".tar.gz");
}

TEST(StringTest, ExtensionFromPathLastChunk) {
    auto ext = extensionFromPath(
        "path/to/file.tar.gz", ExtensionExtractionMode::lastChunk
    );
    ASSERT_TRUE(ext.has_value());
    EXPECT_EQ(*ext, ".gz");
}

TEST(StringTest, ExtensionFromPathNoExtension) {
    auto ext = extensionFromPath("path/to/file");
    EXPECT_FALSE(ext.has_value());
}

// toBinaryString

TEST(StringTest, ToBinaryStringU8) {
    EXPECT_EQ(toBinaryString<u8>(0b10110001), "10110001");
}

TEST(StringTest, ToBinaryStringU8Zero) {
    EXPECT_EQ(toBinaryString<u8>(0), "00000000");
}

TEST(StringTest, ToBinaryStringU16) {
    EXPECT_EQ(toBinaryString<u16>(0x00FF), "0000000011111111");
}

// toHexString

TEST(StringTest, ToHexStringU8) { EXPECT_EQ(toHexString<u8>(0xAB), "ab"); }

TEST(StringTest, ToHexStringU8Zero) { EXPECT_EQ(toHexString<u8>(0), "00"); }

TEST(StringTest, ToHexStringU32) {
    EXPECT_EQ(toHexString<u32>(0x0000CAFE), "0000cafe");
}

TEST(StringTest, ToBinaryStringSpan) {
    std::vector<u8> bytes = {0b11110000, 0b00001111};
    EXPECT_EQ(toBinaryString(std::span<const u8>{bytes}), "1111000000001111");
}

TEST(StringTest, ToHexStringSpan) {
    std::vector<u8> bytes = {0xDE, 0xAD};
    EXPECT_EQ(toHexString(std::span<const u8>{bytes}), "dead");
}
