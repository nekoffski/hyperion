#include "ksl/Parser.hh"

#include "internal/core/Error.hh"

namespace hyperion::ksl {

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

const Token& Parser::current() const { return m_tokens[m_pos]; }

const Token& Parser::lookAhead(u32 offset) const {
    u32 i = m_pos + offset;
    return (i < static_cast<u32>(m_tokens.size())) ? m_tokens[i]
                                                   : m_tokens.back();
}

Token Parser::consume() {
    Token t = m_tokens[m_pos];
    if (m_pos + 1 < static_cast<u32>(m_tokens.size())) {
        m_pos++;
    }
    return t;
}

Token Parser::expect(TokenKind kind, const Str& context) {
    if (current().kind != kind) {
        const auto& tok = current();
        throw KslError{
            ErrorCode::invalidArgument,
            "parse error at line {}, col {}: unexpected '{}' in {} context",
            tok.line,
            tok.column,
            tok.value.empty() ? "<eof>" : tok.value,
            context
        };
    }
    return consume();
}

bool Parser::check(TokenKind kind) const { return current().kind == kind; }

bool Parser::match(TokenKind kind) {
    if (check(kind)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const { return current().kind == TokenKind::Eof; }

KernelDef Parser::parse() {
    auto kernel = parseKernel();
    expect(TokenKind::Eof, "top-level");
    return kernel;
}

KernelDef Parser::parseKernel() {
    expect(TokenKind::KwKernel, "kernel definition");
    auto nameTok = expect(TokenKind::Identifier, "kernel name");
    expect(TokenKind::LParen, "kernel parameter list");
    auto params = parseParamList();
    expect(TokenKind::RParen, "kernel parameter list");
    auto body = parseBlock();
    return KernelDef{nameTok.value, std::move(params), std::move(body)};
}

std::vector<Param> Parser::parseParamList() {
    std::vector<Param> params;
    if (check(TokenKind::RParen)) {
        return params;
    }
    params.push_back(parseParam());
    while (match(TokenKind::Comma)) {
        if (check(TokenKind::RParen)) {
            break;  // trailing comma
        }
        params.push_back(parseParam());
    }
    return params;
}

Param Parser::parseParam() {
    auto nameTok = expect(TokenKind::Identifier, "parameter name");
    expect(TokenKind::Colon, "parameter type annotation");
    auto type = parseParamType();
    return Param{nameTok.value, std::move(type)};
}

Type Parser::parseParamType() {
    if (check(TokenKind::KwReadOnly) || check(TokenKind::KwWriteOnly) ||
        check(TokenKind::KwReadWrite)) {
        auto qualTok = consume();
        BufferQualifier qual;
        switch (qualTok.kind) {
            case TokenKind::KwReadOnly:
                qual = BufferQualifier::ReadOnly;
                break;
            case TokenKind::KwWriteOnly:
                qual = BufferQualifier::WriteOnly;
                break;
            default:
                qual = BufferQualifier::ReadWrite;
                break;
        }
        expect(TokenKind::KwBuffer, "buffer type");
        expect(TokenKind::Lt, "Buffer<T> opening bracket");
        auto elem = parseScalarType();
        expect(TokenKind::Gt, "Buffer<T> closing bracket");
        return BufferType{elem, qual};
    }
    return parseScalarType();
}

ScalarType Parser::parseScalarType() {
    auto tok = consume();
    switch (tok.kind) {
        case TokenKind::KwI32:
            return ScalarType::i32;
        case TokenKind::KwU32:
            return ScalarType::u32;
        case TokenKind::KwF32:
            return ScalarType::f32;
        case TokenKind::KwF64:
            return ScalarType::f64;
        default:
            throw KslError{
                ErrorCode::invalidArgument,
                "expected scalar type (i32/u32/f32/f64) but got '{}' "
                "at line {}, col {}",
                tok.value, tok.line, tok.column
            };
    }
}

Block Parser::parseBlock() {
    expect(TokenKind::LBrace, "block");
    Block stmts;
    while (!check(TokenKind::RBrace) && !isAtEnd()) {
        stmts.push_back(parseStmt());
    }
    expect(TokenKind::RBrace, "block");
    return stmts;
}

StmtPtr Parser::parseStmt() {
    if (check(TokenKind::KwLet)) {
        return parseLetStmt();
    }
    if (check(TokenKind::KwFor)) {
        return parseForStmt();
    }
    if (check(TokenKind::KwIf)) {
        return parseIfStmt();
    }
    return parseAssignOrBufferAssign();
}

StmtPtr Parser::parseLetStmt() {
    expect(TokenKind::KwLet, "let statement");
    auto nameTok = expect(TokenKind::Identifier, "variable name");
    expect(TokenKind::Colon, "variable type annotation");
    auto type = parseScalarType();
    expect(TokenKind::Equals, "let initializer");
    auto init = parseExpr();
    expect(TokenKind::Semicolon, "let statement end");
    return makeVarDecl(nameTok.value, type, std::move(init));
}

StmtPtr Parser::parseAssignOrBufferAssign() {
    auto nameTok = expect(TokenKind::Identifier, "assignment target");

    if (check(TokenKind::LBracket)) {
        consume();  // [
        auto idx = parseExpr();
        expect(TokenKind::RBracket, "buffer index");
        expect(TokenKind::Equals, "buffer assignment");
        auto value = parseExpr();
        expect(TokenKind::Semicolon, "buffer assignment end");
        return makeBufferAssign(
            nameTok.value, std::move(idx), std::move(value)
        );
    }

    expect(TokenKind::Equals, "variable assignment");
    auto value = parseExpr();
    expect(TokenKind::Semicolon, "variable assignment end");
    return makeVarAssign(nameTok.value, std::move(value));
}

StmtPtr Parser::parseForStmt() {
    expect(TokenKind::KwFor, "for statement");
    auto varTok = expect(TokenKind::Identifier, "loop variable");
    expect(TokenKind::KwIn, "for...in");
    expect(TokenKind::KwRange, "range");
    expect(TokenKind::LParen, "range(");
    auto start = parseExpr();
    expect(TokenKind::Comma, "range arguments separator");
    auto end = parseExpr();
    expect(TokenKind::RParen, "range)");
    auto body = parseBlock();
    return makeFor(
        varTok.value, std::move(start), std::move(end), std::move(body)
    );
}

StmtPtr Parser::parseIfStmt() {
    expect(TokenKind::KwIf, "if statement");
    auto cond = parseExpr();
    auto thenBody = parseBlock();
    std::optional<Block> elseBody;
    if (match(TokenKind::KwElse)) {
        elseBody = parseBlock();
    }
    return makeIf(std::move(cond), std::move(thenBody), std::move(elseBody));
}

ExprPtr Parser::parseExpr() { return parseOrExpr(); }

ExprPtr Parser::parseOrExpr() {
    auto lhs = parseAndExpr();
    while (check(TokenKind::PipePipe)) {
        consume();
        auto rhs = parseAndExpr();
        lhs = makeBinary(BinaryOp::Or, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseAndExpr() {
    auto lhs = parseEqExpr();
    while (check(TokenKind::AmpAmp)) {
        consume();
        auto rhs = parseEqExpr();
        lhs = makeBinary(BinaryOp::And, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseEqExpr() {
    auto lhs = parseRelExpr();
    while (check(TokenKind::EqEq) || check(TokenKind::BangEq)) {
        auto op =
            consume().kind == TokenKind::EqEq ? BinaryOp::Eq : BinaryOp::Ne;
        auto rhs = parseRelExpr();
        lhs = makeBinary(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseRelExpr() {
    auto lhs = parseAddExpr();
    while (check(TokenKind::Lt) || check(TokenKind::Gt) ||
           check(TokenKind::LtEq) || check(TokenKind::GtEq)) {
        BinaryOp op;
        switch (consume().kind) {
            case TokenKind::Lt:
                op = BinaryOp::Lt;
                break;
            case TokenKind::Gt:
                op = BinaryOp::Gt;
                break;
            case TokenKind::LtEq:
                op = BinaryOp::Le;
                break;
            default:
                op = BinaryOp::Ge;
                break;
        }
        auto rhs = parseAddExpr();
        lhs = makeBinary(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseAddExpr() {
    auto lhs = parseMulExpr();
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
        auto op =
            consume().kind == TokenKind::Plus ? BinaryOp::Add : BinaryOp::Sub;
        auto rhs = parseMulExpr();
        lhs = makeBinary(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseMulExpr() {
    auto lhs = parseCastExpr();
    while (check(TokenKind::Star) || check(TokenKind::Slash) ||
           check(TokenKind::Percent)) {
        BinaryOp op;
        switch (consume().kind) {
            case TokenKind::Star:
                op = BinaryOp::Mul;
                break;
            case TokenKind::Slash:
                op = BinaryOp::Div;
                break;
            default:
                op = BinaryOp::Mod;
                break;
        }
        auto rhs = parseCastExpr();
        lhs = makeBinary(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

ExprPtr Parser::parseCastExpr() {
    auto expr = parseUnaryExpr();
    while (check(TokenKind::KwAs)) {
        consume();
        auto targetType = parseScalarType();
        expr = makeCast(targetType, std::move(expr));
    }
    return expr;
}

ExprPtr Parser::parseUnaryExpr() {
    if (check(TokenKind::Minus)) {
        consume();
        return makeUnary(UnaryOp::Neg, parseUnaryExpr());
    }
    if (check(TokenKind::Bang)) {
        consume();
        return makeUnary(UnaryOp::Not, parseUnaryExpr());
    }
    return parsePrimaryExpr();
}

ExprPtr Parser::parsePrimaryExpr() {
    if (check(TokenKind::IntLiteral)) {
        auto tok = consume();
        return makeIntLit(std::stoll(tok.value));
    }

    if (check(TokenKind::FloatLiteral)) {
        auto tok = consume();
        return makeFloatLit(std::stod(tok.value));
    }

    if (check(TokenKind::LParen)) {
        consume();
        auto expr = parseExpr();
        expect(TokenKind::RParen, "parenthesised expression");
        return expr;
    }

    if (check(TokenKind::KwGlobalId) || check(TokenKind::KwGlobalSize)) {
        BuiltinFn fn = check(TokenKind::KwGlobalId) ? BuiltinFn::GlobalId
                                                    : BuiltinFn::GlobalSize;
        consume();
        expect(TokenKind::LParen, "globalId/globalSize argument");
        std::vector<ExprPtr> args;
        args.push_back(parseExpr());
        expect(TokenKind::RParen, "globalId/globalSize argument");
        return makeBuiltinCall(fn, std::move(args));
    }

    if (check(TokenKind::KwClamp)) {
        consume();
        expect(TokenKind::LParen, "clamp arguments");
        std::vector<ExprPtr> args;
        args.push_back(parseExpr());
        expect(TokenKind::Comma, "clamp arguments");
        args.push_back(parseExpr());
        expect(TokenKind::Comma, "clamp arguments");
        args.push_back(parseExpr());
        expect(TokenKind::RParen, "clamp arguments");
        return makeBuiltinCall(BuiltinFn::Clamp, std::move(args));
    }

    if (check(TokenKind::KwMin) || check(TokenKind::KwMax)) {
        BuiltinFn fn =
            check(TokenKind::KwMin) ? BuiltinFn::Min : BuiltinFn::Max;
        consume();
        expect(TokenKind::LParen, "min/max arguments");
        std::vector<ExprPtr> args;
        args.push_back(parseExpr());
        expect(TokenKind::Comma, "min/max arguments");
        args.push_back(parseExpr());
        expect(TokenKind::RParen, "min/max arguments");
        return makeBuiltinCall(fn, std::move(args));
    }

    if (check(TokenKind::Identifier)) {
        auto nameTok = consume();
        if (check(TokenKind::LBracket)) {
            consume();  // [
            auto idx = parseExpr();
            expect(TokenKind::RBracket, "buffer index");
            return makeBufferIndex(nameTok.value, std::move(idx));
        }
        return makeVarRef(nameTok.value);
    }

    const auto& tok = current();
    throw KslError{
        ErrorCode::invalidArgument,
        "unexpected token '{}' in expression at line {}, col {}",
        tok.value.empty() ? "<eof>" : tok.value, tok.line, tok.column
    };
}

}  // namespace hyperion::ksl
