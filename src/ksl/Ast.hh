#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "internal/core/Core.hh"

namespace hyperion::ksl {

enum class ScalarType { i32, u32, f32, f64 };
enum class BufferQualifier { ReadOnly, WriteOnly, ReadWrite };

struct BufferType {
    ScalarType elementType;
    BufferQualifier qualifier;
};

using Type = std::variant<ScalarType, BufferType>;

struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using Block = std::vector<StmtPtr>;

struct IntLiteralExpr {
    i64 value;
};
struct FloatLiteralExpr {
    f64 value;
};
struct VarRefExpr {
    Str name;
};

enum class BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    And,
    Or
};

struct BinaryExpr {
    BinaryOp op;
    ExprPtr lhs;
    ExprPtr rhs;
};

enum class UnaryOp { Neg, Not };

struct UnaryExpr {
    UnaryOp op;
    ExprPtr operand;
};

struct BufferIndexExpr {
    Str bufferName;
    ExprPtr index;
};

enum class BuiltinFn { GlobalId, GlobalSize, Clamp, Min, Max };

struct BuiltinCallExpr {
    BuiltinFn fn;
    std::vector<ExprPtr> args;
};

struct CastExpr {
    ScalarType targetType;
    ExprPtr operand;
};

using ExprData = std::variant<
    IntLiteralExpr, FloatLiteralExpr, VarRefExpr, BinaryExpr, UnaryExpr,
    BufferIndexExpr, BuiltinCallExpr, CastExpr>;

struct Expr {
    ExprData data;
};

struct VarDeclStmt {
    Str name;
    ScalarType type;
    ExprPtr init;
};

struct VarTarget {
    Str name;
};
struct BufferTarget {
    Str bufferName;
    ExprPtr index;
};

using AssignTarget = std::variant<VarTarget, BufferTarget>;

struct AssignStmt {
    AssignTarget target;
    ExprPtr value;
};

struct ForStmt {
    Str var;
    ExprPtr rangeStart;
    ExprPtr rangeEnd;
    Block body;
};

struct IfStmt {
    ExprPtr condition;
    Block thenBody;
    std::optional<Block> elseBody;
};

using StmtData = std::variant<VarDeclStmt, AssignStmt, ForStmt, IfStmt>;

struct Stmt {
    StmtData data;
};

struct Param {
    Str name;
    Type type;
};

struct KernelDef {
    Str name;
    std::vector<Param> params;
    Block body;
};

inline ExprPtr makeIntLit(i64 v) {
    return std::make_unique<Expr>(Expr{IntLiteralExpr{v}});
}

inline ExprPtr makeFloatLit(f64 v) {
    return std::make_unique<Expr>(Expr{FloatLiteralExpr{v}});
}

inline ExprPtr makeVarRef(Str name) {
    return std::make_unique<Expr>(Expr{VarRefExpr{std::move(name)}});
}

inline ExprPtr makeBinary(BinaryOp op, ExprPtr lhs, ExprPtr rhs) {
    return std::make_unique<Expr>(
        Expr{BinaryExpr{op, std::move(lhs), std::move(rhs)}}
    );
}

inline ExprPtr makeUnary(UnaryOp op, ExprPtr operand) {
    return std::make_unique<Expr>(Expr{UnaryExpr{op, std::move(operand)}});
}

inline ExprPtr makeBufferIndex(Str buf, ExprPtr idx) {
    return std::make_unique<Expr>(
        Expr{BufferIndexExpr{std::move(buf), std::move(idx)}}
    );
}

inline ExprPtr makeCast(ScalarType t, ExprPtr operand) {
    return std::make_unique<Expr>(Expr{CastExpr{t, std::move(operand)}});
}

inline ExprPtr makeBuiltinCall(BuiltinFn fn, std::vector<ExprPtr> args) {
    return std::make_unique<Expr>(Expr{BuiltinCallExpr{fn, std::move(args)}});
}

inline StmtPtr makeVarDecl(Str name, ScalarType type, ExprPtr init) {
    return std::make_unique<Stmt>(
        Stmt{VarDeclStmt{std::move(name), type, std::move(init)}}
    );
}

inline StmtPtr makeVarAssign(Str name, ExprPtr value) {
    return std::make_unique<Stmt>(
        Stmt{AssignStmt{VarTarget{std::move(name)}, std::move(value)}}
    );
}

inline StmtPtr makeBufferAssign(Str buf, ExprPtr idx, ExprPtr value) {
    return std::make_unique<Stmt>(Stmt{AssignStmt{
        BufferTarget{std::move(buf), std::move(idx)}, std::move(value)
    }});
}

inline StmtPtr makeFor(Str var, ExprPtr start, ExprPtr end, Block body) {
    return std::make_unique<Stmt>(Stmt{ForStmt{
        std::move(var), std::move(start), std::move(end), std::move(body)
    }});
}

inline StmtPtr makeIf(
    ExprPtr cond, Block thenBody, std::optional<Block> elseBody = {}
) {
    return std::make_unique<Stmt>(
        Stmt{IfStmt{std::move(cond), std::move(thenBody), std::move(elseBody)}}
    );
}

}  // namespace hyperion::ksl
