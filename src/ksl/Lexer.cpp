#include "ksl/Lexer.hh"

#include <cctype>
#include <unordered_map>

#include "internal/core/Error.hh"

namespace hyperion::ksl {

namespace {
const std::unordered_map<Str, TokenKind> kKeywords = {
    {"kernel", TokenKind::KwKernel},
    {"let", TokenKind::KwLet},
    {"for", TokenKind::KwFor},
    {"in", TokenKind::KwIn},
    {"range", TokenKind::KwRange},
    {"if", TokenKind::KwIf},
    {"else", TokenKind::KwElse},
    {"as", TokenKind::KwAs},
    {"i32", TokenKind::KwI32},
    {"u32", TokenKind::KwU32},
    {"f32", TokenKind::KwF32},
    {"f64", TokenKind::KwF64},
    {"ReadOnly", TokenKind::KwReadOnly},
    {"WriteOnly", TokenKind::KwWriteOnly},
    {"ReadWrite", TokenKind::KwReadWrite},
    {"globalId", TokenKind::KwGlobalId},
    {"globalSize", TokenKind::KwGlobalSize},
    {"clamp", TokenKind::KwClamp},
    {"min", TokenKind::KwMin},
    {"max", TokenKind::KwMax},
    {"Buffer", TokenKind::KwBuffer},
};
}  // namespace

Lexer::Lexer(const Str& source) : m_source(source) {}

bool Lexer::isAtEnd() const {
    return m_pos >= static_cast<u32>(m_source.size());
}

char Lexer::peek(u32 offset) const {
    u32 i = m_pos + offset;
    return (i < static_cast<u32>(m_source.size())) ? m_source[i] : '\0';
}

char Lexer::advance() {
    char c = m_source[m_pos++];
    if (c == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    return c;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isspace(static_cast<unsigned char>(c))) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::readNumber() {
    u32 startLine = m_line, startCol = m_column;
    Str text;
    bool isFloat = false;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        text += advance();
    }

    if (!isAtEnd() && peek() == '.' &&
        std::isdigit(static_cast<unsigned char>(peek(1)))) {
        isFloat = true;
        text += advance();  // '.'
        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
            text += advance();
        }
    }

    return Token{
        isFloat ? TokenKind::FloatLiteral : TokenKind::IntLiteral, text,
        startLine, startCol
    };
}

Token Lexer::readIdentifierOrKeyword() {
    u32 startLine = m_line, startCol = m_column;
    Str text;

    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) ||
                          peek() == '_')) {
        text += advance();
    }

    auto it = kKeywords.find(text);
    auto kind = (it != kKeywords.end()) ? it->second : TokenKind::Identifier;
    return Token{kind, text, startLine, startCol};
}

// static
TokenKind Lexer::classifyKeyword(const Str& word) {
    auto it = kKeywords.find(word);
    return (it != kKeywords.end()) ? it->second : TokenKind::Identifier;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespaceAndComments();

        if (isAtEnd()) {
            tokens.push_back(Token{TokenKind::Eof, "", m_line, m_column});
            break;
        }

        u32 startLine = m_line, startCol = m_column;
        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(readNumber());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
            continue;
        }

        advance();  // consume the character

        auto make = [&](TokenKind k, Str v) -> Token {
            return Token{k, std::move(v), startLine, startCol};
        };

        switch (c) {
            case '(':
                tokens.push_back(make(TokenKind::LParen, "("));
                break;
            case ')':
                tokens.push_back(make(TokenKind::RParen, ")"));
                break;
            case '{':
                tokens.push_back(make(TokenKind::LBrace, "{"));
                break;
            case '}':
                tokens.push_back(make(TokenKind::RBrace, "}"));
                break;
            case '[':
                tokens.push_back(make(TokenKind::LBracket, "["));
                break;
            case ']':
                tokens.push_back(make(TokenKind::RBracket, "]"));
                break;
            case ',':
                tokens.push_back(make(TokenKind::Comma, ","));
                break;
            case ':':
                tokens.push_back(make(TokenKind::Colon, ":"));
                break;
            case ';':
                tokens.push_back(make(TokenKind::Semicolon, ";"));
                break;
            case '+':
                tokens.push_back(make(TokenKind::Plus, "+"));
                break;
            case '-':
                tokens.push_back(make(TokenKind::Minus, "-"));
                break;
            case '*':
                tokens.push_back(make(TokenKind::Star, "*"));
                break;
            case '/':
                tokens.push_back(make(TokenKind::Slash, "/"));
                break;
            case '%':
                tokens.push_back(make(TokenKind::Percent, "%"));
                break;
            case '<':
                if (!isAtEnd() && peek() == '=') {
                    advance();
                    tokens.push_back(make(TokenKind::LtEq, "<="));
                } else {
                    tokens.push_back(make(TokenKind::Lt, "<"));
                }
                break;
            case '>':
                if (!isAtEnd() && peek() == '=') {
                    advance();
                    tokens.push_back(make(TokenKind::GtEq, ">="));
                } else {
                    tokens.push_back(make(TokenKind::Gt, ">"));
                }
                break;
            case '=':
                if (!isAtEnd() && peek() == '=') {
                    advance();
                    tokens.push_back(make(TokenKind::EqEq, "=="));
                } else {
                    tokens.push_back(make(TokenKind::Equals, "="));
                }
                break;
            case '!':
                if (!isAtEnd() && peek() == '=') {
                    advance();
                    tokens.push_back(make(TokenKind::BangEq, "!="));
                } else {
                    tokens.push_back(make(TokenKind::Bang, "!"));
                }
                break;
            case '&':
                if (!isAtEnd() && peek() == '&') {
                    advance();
                    tokens.push_back(make(TokenKind::AmpAmp, "&&"));
                } else {
                    throw KslError{
                        ErrorCode::invalidArgument,
                        "unexpected '&' at line {}, col {}", startLine, startCol
                    };
                }
                break;
            case '|':
                if (!isAtEnd() && peek() == '|') {
                    advance();
                    tokens.push_back(make(TokenKind::PipePipe, "||"));
                } else {
                    throw KslError{
                        ErrorCode::invalidArgument,
                        "unexpected '|' at line {}, col {}", startLine, startCol
                    };
                }
                break;
            default:
                throw KslError{
                    ErrorCode::invalidArgument,
                    "unexpected character '{}' at line {}, col {}", c,
                    startLine, startCol
                };
        }
    }
    return tokens;
}

}  // namespace hyperion::ksl
