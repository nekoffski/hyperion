#pragma once

#include "internal/core/Core.hh"

namespace hyperion::ksl {

enum class TokenKind {
    IntLiteral,
    FloatLiteral,
    Identifier,
    KwKernel,
    KwLet,
    KwFor,
    KwIn,
    KwRange,
    KwIf,
    KwElse,
    KwAs,
    KwI32,
    KwU32,
    KwF32,
    KwF64,
    KwReadOnly,
    KwWriteOnly,
    KwReadWrite,
    KwGlobalId,
    KwGlobalSize,
    KwClamp,
    KwMin,
    KwMax,
    KwBuffer,
    LParen,     // (
    RParen,     // )
    LBrace,     // {
    RBrace,     // }
    LBracket,   // [
    RBracket,   // ]
    Comma,      // ,
    Colon,      // :
    Semicolon,  // ;
    Equals,     // =
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Percent,    // %
    Lt,         // <
    Gt,         // >
    LtEq,       // <=
    GtEq,       // >=
    EqEq,       // ==
    BangEq,     // !=
    Bang,       // !
    AmpAmp,     // &&
    PipePipe,   // ||
    // End of file
    Eof
};

struct Token {
    TokenKind kind;
    Str value;
    u32 line{1};
    u32 column{1};
};

}  // namespace hyperion::ksl
