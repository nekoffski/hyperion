#include <gtest/gtest.h>

#include "internal/core/Error.hh"
#include "ksl/Lexer.hh"
#include "ksl/Token.hh"

using namespace hyperion;
using namespace hyperion::ksl;

static std::vector<Token> lex(const Str& src) {
    Lexer lex{src};
    auto tokens = lex.tokenize();
    // Remove Eof for cleaner assertions
    if (!tokens.empty() && tokens.back().kind == TokenKind::Eof) {
        tokens.pop_back();
    }
    return tokens;
}

static TokenKind kindOf(const Str& src) { return lex(src).at(0).kind; }

TEST(LexerTest, EmptySourceYieldsOnlyEof) {
    Lexer l{""};
    auto tokens = l.tokenize();
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Eof);
}

TEST(LexerTest, WhitespaceOnlyYieldsOnlyEof) {
    Lexer l{"   \n\t  "};
    auto tokens = l.tokenize();
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Eof);
}

TEST(LexerTest, IntegerLiteral) {
    auto tokens = lex("42");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
    EXPECT_EQ(tokens[0].value, "42");
}

TEST(LexerTest, ZeroIntegerLiteral) {
    auto tokens = lex("0");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
    EXPECT_EQ(tokens[0].value, "0");
}

TEST(LexerTest, FloatLiteral) {
    auto tokens = lex("3.14");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::FloatLiteral);
    EXPECT_EQ(tokens[0].value, "3.14");
}

TEST(LexerTest, FloatLiteralZero) {
    auto tokens = lex("0.0");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::FloatLiteral);
}

TEST(LexerTest, IntFollowedByDotWithoutDigitIsNotFloat) {
    // "1." — the dot has no following digit, so "1" is an int
    auto tokens = lex("1.");
    ASSERT_GE(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
    EXPECT_EQ(tokens[0].value, "1");
}

TEST(LexerTest, StatementKeywords) {
    EXPECT_EQ(kindOf("kernel"), TokenKind::KwKernel);
    EXPECT_EQ(kindOf("let"), TokenKind::KwLet);
    EXPECT_EQ(kindOf("for"), TokenKind::KwFor);
    EXPECT_EQ(kindOf("in"), TokenKind::KwIn);
    EXPECT_EQ(kindOf("range"), TokenKind::KwRange);
    EXPECT_EQ(kindOf("if"), TokenKind::KwIf);
    EXPECT_EQ(kindOf("else"), TokenKind::KwElse);
    EXPECT_EQ(kindOf("as"), TokenKind::KwAs);
}

TEST(LexerTest, TypeKeywords) {
    EXPECT_EQ(kindOf("i32"), TokenKind::KwI32);
    EXPECT_EQ(kindOf("u32"), TokenKind::KwU32);
    EXPECT_EQ(kindOf("f32"), TokenKind::KwF32);
    EXPECT_EQ(kindOf("f64"), TokenKind::KwF64);
}

TEST(LexerTest, QualifierKeywords) {
    EXPECT_EQ(kindOf("ReadOnly"), TokenKind::KwReadOnly);
    EXPECT_EQ(kindOf("WriteOnly"), TokenKind::KwWriteOnly);
    EXPECT_EQ(kindOf("ReadWrite"), TokenKind::KwReadWrite);
}

TEST(LexerTest, BuiltinKeywords) {
    EXPECT_EQ(kindOf("globalId"), TokenKind::KwGlobalId);
    EXPECT_EQ(kindOf("globalSize"), TokenKind::KwGlobalSize);
    EXPECT_EQ(kindOf("clamp"), TokenKind::KwClamp);
    EXPECT_EQ(kindOf("min"), TokenKind::KwMin);
    EXPECT_EQ(kindOf("max"), TokenKind::KwMax);
}

TEST(LexerTest, BufferKeyword) {
    EXPECT_EQ(kindOf("Buffer"), TokenKind::KwBuffer);
}

TEST(LexerTest, PlainIdentifier) {
    auto tokens = lex("myVar");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Identifier);
    EXPECT_EQ(tokens[0].value, "myVar");
}

TEST(LexerTest, IdentifierWithUnderscore) {
    auto tokens = lex("some_name");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Identifier);
    EXPECT_EQ(tokens[0].value, "some_name");
}

TEST(LexerTest, IdentifierStartingWithUnderscore) {
    auto tokens = lex("_x");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Identifier);
}

TEST(LexerTest, Delimiters) {
    EXPECT_EQ(kindOf("("), TokenKind::LParen);
    EXPECT_EQ(kindOf(")"), TokenKind::RParen);
    EXPECT_EQ(kindOf("{"), TokenKind::LBrace);
    EXPECT_EQ(kindOf("}"), TokenKind::RBrace);
    EXPECT_EQ(kindOf("["), TokenKind::LBracket);
    EXPECT_EQ(kindOf("]"), TokenKind::RBracket);
    EXPECT_EQ(kindOf(","), TokenKind::Comma);
    EXPECT_EQ(kindOf(":"), TokenKind::Colon);
    EXPECT_EQ(kindOf(";"), TokenKind::Semicolon);
}

TEST(LexerTest, SingleCharOperators) {
    EXPECT_EQ(kindOf("="), TokenKind::Equals);
    EXPECT_EQ(kindOf("+"), TokenKind::Plus);
    EXPECT_EQ(kindOf("-"), TokenKind::Minus);
    EXPECT_EQ(kindOf("*"), TokenKind::Star);
    EXPECT_EQ(kindOf("/"), TokenKind::Slash);
    EXPECT_EQ(kindOf("%"), TokenKind::Percent);
    EXPECT_EQ(kindOf("<"), TokenKind::Lt);
    EXPECT_EQ(kindOf(">"), TokenKind::Gt);
    EXPECT_EQ(kindOf("!"), TokenKind::Bang);
}

TEST(LexerTest, TwoCharOperators) {
    EXPECT_EQ(kindOf("<="), TokenKind::LtEq);
    EXPECT_EQ(kindOf(">="), TokenKind::GtEq);
    EXPECT_EQ(kindOf("=="), TokenKind::EqEq);
    EXPECT_EQ(kindOf("!="), TokenKind::BangEq);
    EXPECT_EQ(kindOf("&&"), TokenKind::AmpAmp);
    EXPECT_EQ(kindOf("||"), TokenKind::PipePipe);
}

TEST(LexerTest, LineCommentIsSkipped) {
    auto tokens = lex("// this is a comment\n42");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
    EXPECT_EQ(tokens[0].value, "42");
}

TEST(LexerTest, CommentAtEndOfLineDoesNotConsumeNextLine) {
    auto tokens = lex("let // comment\nx");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].kind, TokenKind::KwLet);
    EXPECT_EQ(tokens[1].kind, TokenKind::Identifier);
    EXPECT_EQ(tokens[1].value, "x");
}

TEST(LexerTest, SingleLineColumnTracking) {
    Lexer l{"let x"};
    auto tokens = l.tokenize();
    EXPECT_EQ(tokens[0].line, 1u);
    EXPECT_EQ(tokens[0].column, 1u);
    EXPECT_EQ(tokens[1].line, 1u);
    EXPECT_EQ(tokens[1].column, 5u);
}

TEST(LexerTest, MultilineLineTracking) {
    Lexer l{"a\nb"};
    auto tokens = l.tokenize();
    EXPECT_EQ(tokens[0].line, 1u);
    EXPECT_EQ(tokens[1].line, 2u);
}

TEST(LexerTest, UnknownCharacterThrows) {
    Lexer l{"@"};
    EXPECT_THROW(l.tokenize(), KslError);
}

TEST(LexerTest, SingleAmpersandThrows) {
    Lexer l{"&"};
    EXPECT_THROW(l.tokenize(), KslError);
}

TEST(LexerTest, SinglePipeThrows) {
    Lexer l{"|"};
    EXPECT_THROW(l.tokenize(), KslError);
}

TEST(LexerTest, SimpleKernelSignatureTokens) {
    auto tokens = lex("kernel foo(x: i32) {}");
    std::vector<TokenKind> expected = {
        TokenKind::KwKernel,
        TokenKind::Identifier,  // foo
        TokenKind::LParen,
        TokenKind::Identifier,  // x
        TokenKind::Colon,      TokenKind::KwI32,  TokenKind::RParen,
        TokenKind::LBrace,     TokenKind::RBrace,
    };
    ASSERT_EQ(tokens.size(), expected.size());
    for (u32 i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].kind, expected[i]) << "at index " << i;
    }
}

TEST(LexerTest, BufferParamTokens) {
    auto tokens = lex("ReadOnly Buffer<f32>");
    std::vector<TokenKind> expected = {
        TokenKind::KwReadOnly, TokenKind::KwBuffer, TokenKind::Lt,
        TokenKind::KwF32,      TokenKind::Gt,
    };
    ASSERT_EQ(tokens.size(), expected.size());
    for (u32 i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].kind, expected[i]) << "at index " << i;
    }
}

TEST(LexerTest, ForRangeTokens) {
    auto tokens = lex("for i in range(0, n)");
    std::vector<TokenKind> expected = {
        TokenKind::KwFor,
        TokenKind::Identifier,  // i
        TokenKind::KwIn,       TokenKind::KwRange, TokenKind::LParen,
        TokenKind::IntLiteral,  // 0
        TokenKind::Comma,
        TokenKind::Identifier,  // n
        TokenKind::RParen,
    };
    ASSERT_EQ(tokens.size(), expected.size());
    for (u32 i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].kind, expected[i]) << "at index " << i;
    }
}
