#pragma once

#include <vector>

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "ksl/Ast.hh"
#include "ksl/Token.hh"

namespace hyperion::ksl {

class Parser : public NonCopyable {
   public:
    explicit Parser(std::vector<Token> tokens);

    KernelDef parse();

   private:
    const Token& current() const;
    const Token& lookAhead(u32 offset = 1) const;
    Token consume();
    Token expect(TokenKind kind, const Str& context);
    bool check(TokenKind kind) const;
    bool match(TokenKind kind);
    bool isAtEnd() const;

    KernelDef parseKernel();
    std::vector<Param> parseParamList();
    Param parseParam();
    Type parseParamType();
    ScalarType parseScalarType();
    Block parseBlock();
    StmtPtr parseStmt();
    StmtPtr parseLetStmt();
    StmtPtr parseAssignOrBufferAssign();
    StmtPtr parseForStmt();
    StmtPtr parseIfStmt();

    // Expressions (precedence climbing)
    ExprPtr parseExpr();
    ExprPtr parseOrExpr();
    ExprPtr parseAndExpr();
    ExprPtr parseEqExpr();
    ExprPtr parseRelExpr();
    ExprPtr parseAddExpr();
    ExprPtr parseMulExpr();
    ExprPtr parseCastExpr();
    ExprPtr parseUnaryExpr();
    ExprPtr parsePrimaryExpr();

    std::vector<Token> m_tokens;
    u32 m_pos{0};
};

}  // namespace hyperion::ksl
