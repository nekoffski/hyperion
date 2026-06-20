#include <gtest/gtest.h>

#include "internal/core/Error.hh"
#include "ksl/Ast.hh"
#include "ksl/Lexer.hh"
#include "ksl/Parser.hh"

using namespace hyperion;
using namespace hyperion::ksl;

static KernelDef parseKSL(const Str& src) {
    Lexer lex{src};
    auto tokens = lex.tokenize();
    Parser parser{std::move(tokens)};
    return parser.parse();
}

TEST(ParserTest, EmptyKernel) {
    auto def = parseKSL("kernel foo() {}");
    EXPECT_EQ(def.name, "foo");
    EXPECT_TRUE(def.params.empty());
    EXPECT_TRUE(def.body.empty());
}

TEST(ParserTest, KernelName) {
    auto def = parseKSL("kernel my_kernel() {}");
    EXPECT_EQ(def.name, "my_kernel");
}

TEST(ParserTest, ScalarI32Param) {
    auto def = parseKSL("kernel k(width: i32) {}");
    ASSERT_EQ(def.params.size(), 1u);
    EXPECT_EQ(def.params[0].name, "width");
    ASSERT_TRUE(std::holds_alternative<ScalarType>(def.params[0].type));
    EXPECT_EQ(std::get<ScalarType>(def.params[0].type), ScalarType::i32);
}

TEST(ParserTest, AllScalarParamTypes) {
    auto def = parseKSL("kernel k(a: i32, b: u32, c: f32, d: f64) {}");
    ASSERT_EQ(def.params.size(), 4u);
    EXPECT_EQ(std::get<ScalarType>(def.params[0].type), ScalarType::i32);
    EXPECT_EQ(std::get<ScalarType>(def.params[1].type), ScalarType::u32);
    EXPECT_EQ(std::get<ScalarType>(def.params[2].type), ScalarType::f32);
    EXPECT_EQ(std::get<ScalarType>(def.params[3].type), ScalarType::f64);
}

TEST(ParserTest, ReadOnlyBufferParam) {
    auto def = parseKSL("kernel k(buf: ReadOnly Buffer<f32>) {}");
    ASSERT_EQ(def.params.size(), 1u);
    ASSERT_TRUE(std::holds_alternative<BufferType>(def.params[0].type));
    auto& bt = std::get<BufferType>(def.params[0].type);
    EXPECT_EQ(bt.elementType, ScalarType::f32);
    EXPECT_EQ(bt.qualifier, BufferQualifier::ReadOnly);
}

TEST(ParserTest, WriteOnlyBufferParam) {
    auto def = parseKSL("kernel k(buf: WriteOnly Buffer<f32>) {}");
    auto& bt = std::get<BufferType>(def.params[0].type);
    EXPECT_EQ(bt.qualifier, BufferQualifier::WriteOnly);
}

TEST(ParserTest, ReadWriteBufferParam) {
    auto def = parseKSL("kernel k(buf: ReadWrite Buffer<i32>) {}");
    auto& bt = std::get<BufferType>(def.params[0].type);
    EXPECT_EQ(bt.qualifier, BufferQualifier::ReadWrite);
    EXPECT_EQ(bt.elementType, ScalarType::i32);
}

TEST(ParserTest, MultipleParams) {
    auto def = parseKSL(
        "kernel k(inp: ReadOnly Buffer<f32>, out: WriteOnly Buffer<f32>, "
        "n: i32) {}"
    );
    ASSERT_EQ(def.params.size(), 3u);
    EXPECT_EQ(def.params[0].name, "inp");
    EXPECT_EQ(def.params[1].name, "out");
    EXPECT_EQ(def.params[2].name, "n");
}

TEST(ParserTest, LetStatement) {
    auto def = parseKSL("kernel k() { let x: i32 = 0; }");
    ASSERT_EQ(def.body.size(), 1u);
    auto& stmt = std::get<VarDeclStmt>(def.body[0]->data);
    EXPECT_EQ(stmt.name, "x");
    EXPECT_EQ(stmt.type, ScalarType::i32);
    EXPECT_EQ(std::get<IntLiteralExpr>(stmt.init->data).value, 0);
}

TEST(ParserTest, LetStatementF32) {
    auto def = parseKSL("kernel k() { let s: f32 = 0.0; }");
    auto& stmt = std::get<VarDeclStmt>(def.body[0]->data);
    EXPECT_EQ(stmt.type, ScalarType::f32);
    EXPECT_DOUBLE_EQ(std::get<FloatLiteralExpr>(stmt.init->data).value, 0.0);
}

TEST(ParserTest, VariableAssignment) {
    auto def = parseKSL("kernel k() { let s: f32 = 0.0; s = 1.0; }");
    ASSERT_EQ(def.body.size(), 2u);
    auto& assign = std::get<AssignStmt>(def.body[1]->data);
    EXPECT_EQ(std::get<VarTarget>(assign.target).name, "s");
}

TEST(ParserTest, BufferWrite) {
    auto def =
        parseKSL("kernel k(out: WriteOnly Buffer<f32>) { out[0] = 1.0; }");
    auto& assign = std::get<AssignStmt>(def.body[0]->data);
    auto& target = std::get<BufferTarget>(assign.target);
    EXPECT_EQ(target.bufferName, "out");
    EXPECT_EQ(std::get<IntLiteralExpr>(target.index->data).value, 0);
}

TEST(ParserTest, ForLoop) {
    auto def = parseKSL("kernel k() { for i in range(0, 10) {} }");
    auto& forStmt = std::get<ForStmt>(def.body[0]->data);
    EXPECT_EQ(forStmt.var, "i");
    EXPECT_EQ(std::get<IntLiteralExpr>(forStmt.rangeStart->data).value, 0);
    EXPECT_EQ(std::get<IntLiteralExpr>(forStmt.rangeEnd->data).value, 10);
    EXPECT_TRUE(forStmt.body.empty());
}

TEST(ParserTest, NestedForLoops) {
    auto def = parseKSL(
        "kernel k() { for i in range(0, 3) { for j in range(0, 3) {} } }"
    );
    auto& outer = std::get<ForStmt>(def.body[0]->data);
    EXPECT_EQ(outer.var, "i");
    ASSERT_EQ(outer.body.size(), 1u);
    auto& inner = std::get<ForStmt>(outer.body[0]->data);
    EXPECT_EQ(inner.var, "j");
}

TEST(ParserTest, IfStatement) {
    auto def = parseKSL("kernel k() { if x {} }");
    auto& ifStmt = std::get<IfStmt>(def.body[0]->data);
    EXPECT_FALSE(ifStmt.elseBody.has_value());
    EXPECT_EQ(std::get<VarRefExpr>(ifStmt.condition->data).name, "x");
}

TEST(ParserTest, IfElseStatement) {
    auto def = parseKSL("kernel k() { if x {} else {} }");
    auto& ifStmt = std::get<IfStmt>(def.body[0]->data);
    EXPECT_TRUE(ifStmt.elseBody.has_value());
}

// ─── Expressions ─────────────────────────────────────────────────────────────

TEST(ParserTest, BinaryAddExpr) {
    auto def = parseKSL("kernel k() { let r: i32 = a + b; }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& bin = std::get<BinaryExpr>(decl.init->data);
    EXPECT_EQ(bin.op, BinaryOp::Add);
    EXPECT_EQ(std::get<VarRefExpr>(bin.lhs->data).name, "a");
    EXPECT_EQ(std::get<VarRefExpr>(bin.rhs->data).name, "b");
}

TEST(ParserTest, UnaryNeg) {
    auto def = parseKSL("kernel k() { let r: i32 = -1; }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& unary = std::get<UnaryExpr>(decl.init->data);
    EXPECT_EQ(unary.op, UnaryOp::Neg);
    EXPECT_EQ(std::get<IntLiteralExpr>(unary.operand->data).value, 1);
}

TEST(ParserTest, CastExpr) {
    auto def = parseKSL("kernel k() { let r: f32 = n as f32; }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& cast = std::get<CastExpr>(decl.init->data);
    EXPECT_EQ(cast.targetType, ScalarType::f32);
    EXPECT_EQ(std::get<VarRefExpr>(cast.operand->data).name, "n");
}

TEST(ParserTest, BufferIndexInExpr) {
    auto def = parseKSL(
        "kernel k(buf: ReadOnly Buffer<f32>) { let v: f32 = buf[0]; }"
    );
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& idx = std::get<BufferIndexExpr>(decl.init->data);
    EXPECT_EQ(idx.bufferName, "buf");
    EXPECT_EQ(std::get<IntLiteralExpr>(idx.index->data).value, 0);
}

TEST(ParserTest, GlobalIdCall) {
    auto def = parseKSL("kernel k() { let x: i32 = globalId(0); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& builtin = std::get<BuiltinCallExpr>(decl.init->data);
    EXPECT_EQ(builtin.fn, BuiltinFn::GlobalId);
    EXPECT_EQ(std::get<IntLiteralExpr>(builtin.args[0]->data).value, 0);
}

TEST(ParserTest, GlobalSizeCall) {
    auto def = parseKSL("kernel k() { let s: i32 = globalSize(1); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& builtin = std::get<BuiltinCallExpr>(decl.init->data);
    EXPECT_EQ(builtin.fn, BuiltinFn::GlobalSize);
}

TEST(ParserTest, ClampCall) {
    auto def = parseKSL("kernel k() { let v: i32 = clamp(x, 0, 10); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& builtin = std::get<BuiltinCallExpr>(decl.init->data);
    EXPECT_EQ(builtin.fn, BuiltinFn::Clamp);
    ASSERT_EQ(builtin.args.size(), 3u);
}

TEST(ParserTest, MinCall) {
    auto def = parseKSL("kernel k() { let v: i32 = min(a, b); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& builtin = std::get<BuiltinCallExpr>(decl.init->data);
    EXPECT_EQ(builtin.fn, BuiltinFn::Min);
}

TEST(ParserTest, MaxCall) {
    auto def = parseKSL("kernel k() { let v: i32 = max(a, b); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& builtin = std::get<BuiltinCallExpr>(decl.init->data);
    EXPECT_EQ(builtin.fn, BuiltinFn::Max);
}

TEST(ParserTest, ParenthesisedExpr) {
    auto def = parseKSL("kernel k() { let r: i32 = (1 + 2); }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    // Outer parens are transparent — result is BinaryExpr
    EXPECT_TRUE(std::holds_alternative<BinaryExpr>(decl.init->data));
}

TEST(ParserTest, OperatorPrecedenceMulBeforeAdd) {
    // a + b * c should parse as a + (b * c)
    auto def = parseKSL("kernel k() { let r: i32 = a + b * c; }");
    auto& decl = std::get<VarDeclStmt>(def.body[0]->data);
    auto& add = std::get<BinaryExpr>(decl.init->data);
    EXPECT_EQ(add.op, BinaryOp::Add);
    // rhs of add is the multiplication
    auto& mul = std::get<BinaryExpr>(add.rhs->data);
    EXPECT_EQ(mul.op, BinaryOp::Mul);
}

TEST(ParserTest, MissingKernelKeywordThrows) {
    EXPECT_THROW(parseKSL("foo() {}"), KslError);
}

TEST(ParserTest, UnknownTypeThrows) {
    EXPECT_THROW(parseKSL("kernel k(x: bool) {}"), KslError);
}

TEST(ParserTest, MissingSemicolonAfterLetThrows) {
    EXPECT_THROW(parseKSL("kernel k() { let x: i32 = 0 }"), KslError);
}

TEST(ParserTest, UnexpectedTokenInExprThrows) {
    EXPECT_THROW(parseKSL("kernel k() { let x: i32 = ; }"), KslError);
}

TEST(ParserTest, UnclosedBlockThrows) {
    EXPECT_THROW(parseKSL("kernel k() {"), KslError);
}
