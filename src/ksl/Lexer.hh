#pragma once

#include <vector>

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "ksl/Token.hh"

namespace hyperion::ksl {

class Lexer : public NonCopyable {
   public:
    explicit Lexer(const Str& source);

    std::vector<Token> tokenize();

   private:
    void skipWhitespaceAndComments();
    Token readNumber();
    Token readIdentifierOrKeyword();

    bool isAtEnd() const;
    char peek(u32 offset = 0) const;
    char advance();

    static TokenKind classifyKeyword(const Str& word);

    const Str& m_source;
    u32 m_pos{0};
    u32 m_line{1};
    u32 m_column{1};
};

}  // namespace hyperion::ksl
