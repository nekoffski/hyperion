#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/core/Error.hh"
#include "ksl/Ast.hh"
#include "ksl/CxxEmitter.hh"

using namespace hyperion;
using namespace hyperion::ksl;
using ::testing::HasSubstr;

static Str emitKernel(KernelDef kernel) {
    CxxEmitter e;
    return e.emit(kernel);
}

static KernelDef emptyKernel(Str name = "test") {
    return KernelDef{std::move(name), {}, {}};
}

TEST(CxxEmitterTest, ScalarI32MapsToInt) {
    KernelDef k{"k", {Param{"n", ScalarType::i32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("int n"));
}

TEST(CxxEmitterTest, ScalarU32MapsToUnsignedInt) {
    KernelDef k{"k", {Param{"n", ScalarType::u32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("unsigned int n"));
}

TEST(CxxEmitterTest, ScalarF32MapsToFloat) {
    KernelDef k{"k", {Param{"v", ScalarType::f32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("float v"));
}

TEST(CxxEmitterTest, ScalarF64MapsToDouble) {
    KernelDef k{"k", {Param{"v", ScalarType::f64}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("double v"));
}

TEST(CxxEmitterTest, ReadOnlyBufferIsConstPtr) {
    KernelDef k{
        "k",
        {Param{"buf", BufferType{ScalarType::f32, BufferQualifier::ReadOnly}}},
        {}
    };
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("const float* buf"));
}

TEST(CxxEmitterTest, WriteOnlyBufferIsNonConstPtr) {
    KernelDef k{
        "k",
        {Param{"out", BufferType{ScalarType::f32, BufferQualifier::WriteOnly}}},
        {}
    };
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("float* out"));
    EXPECT_THAT(out, ::testing::Not(HasSubstr("const float* out")));
}

TEST(CxxEmitterTest, ReadWriteBufferIsNonConstPtr) {
    KernelDef k{
        "k",
        {Param{"rw", BufferType{ScalarType::i32, BufferQualifier::ReadWrite}}},
        {}
    };
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("int* rw"));
}

TEST(CxxEmitterTest, EmptyKernelHasFunctionSignature) {
    auto out = emitKernel(emptyKernel("foo"));
    EXPECT_THAT(out, HasSubstr("void foo("));
    EXPECT_THAT(out, HasSubstr("__ksl_gid_0"));
    EXPECT_THAT(out, HasSubstr("__ksl_gid_1"));
    EXPECT_THAT(out, HasSubstr("__ksl_gid_2"));
    EXPECT_THAT(out, HasSubstr("__ksl_gsz_0"));
}

TEST(CxxEmitterTest, PreambleIncludesAlgorithmAndCstddef) {
    auto out = emitKernel(emptyKernel());
    EXPECT_THAT(out, HasSubstr("#include <algorithm>"));
    EXPECT_THAT(out, HasSubstr("#include <cstddef>"));
}

TEST(CxxEmitterTest, GlobalIdMapsToKslGidParam) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(0));
    Block body;
    body.push_back(makeVarDecl(
        "x", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalId, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("__ksl_gid_0"));
}

TEST(CxxEmitterTest, GlobalIdDim1MapsToKslGid1) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(1));
    Block body;
    body.push_back(makeVarDecl(
        "y", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalId, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("__ksl_gid_1"));
}

TEST(CxxEmitterTest, GlobalSizeMapsToKslGszParam) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(2));
    Block body;
    body.push_back(makeVarDecl(
        "s", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalSize, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("__ksl_gsz_2"));
}

TEST(CxxEmitterTest, ClampMapsToStdClamp) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("x"));
    args.push_back(makeIntLit(0));
    args.push_back(makeIntLit(10));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Clamp, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("std::clamp("));
}

TEST(CxxEmitterTest, MinMapsToStdMin) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("a"));
    args.push_back(makeVarRef("b"));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Min, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("std::min("));
}

TEST(CxxEmitterTest, MaxMapsToStdMax) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("a"));
    args.push_back(makeVarRef("b"));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Max, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("std::max("));
}

TEST(CxxEmitterTest, VarDeclEmitsTypedDeclaration) {
    Block body;
    body.push_back(makeVarDecl("sum", ScalarType::f32, makeFloatLit(0.0)));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("float sum = "));
}

TEST(CxxEmitterTest, VarAssignEmitsAssignment) {
    Block body;
    body.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(0)));
    body.push_back(makeVarAssign("x", makeIntLit(5)));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("x = 5;"));
}

TEST(CxxEmitterTest, BufferWriteEmitsIndexedAssignment) {
    KernelDef k{
        "k",
        {Param{"out", BufferType{ScalarType::f32, BufferQualifier::WriteOnly}}},
        {}
    };
    k.body.push_back(makeBufferAssign("out", makeIntLit(0), makeFloatLit(1.0)));
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("out[0] = "));
}

TEST(CxxEmitterTest, ForLoopEmitsCStyleFor) {
    Block inner;
    Block body;
    body.push_back(
        makeFor("i", makeIntLit(0), makeIntLit(10), std::move(inner))
    );
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("for (int i = 0; i < 10; ++i)"));
}

TEST(CxxEmitterTest, IfStatementEmitsIfBlock) {
    Block thenBody;
    thenBody.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(1)));
    Block body;
    body.push_back(makeIf(makeVarRef("flag"), std::move(thenBody)));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("if (flag)"));
}

TEST(CxxEmitterTest, IfElseEmitsElseBlock) {
    Block thenBody, elseBody;
    thenBody.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(1)));
    elseBody.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(2)));
    Block body;
    body.push_back(makeIf(
        makeVarRef("flag"), std::move(thenBody),
        std::make_optional(std::move(elseBody))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("else {"));
}

TEST(CxxEmitterTest, CastEmitsStaticCast) {
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::f32, makeCast(ScalarType::f32, makeVarRef("n"))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("static_cast<float>(n)"));
}

TEST(CxxEmitterTest, FloatLiteralAlwaysHasDecimalPoint) {
    Block body;
    body.push_back(makeVarDecl("v", ScalarType::f32, makeFloatLit(0.0)));
    KernelDef k{"k", {}, std::move(body)};
    // Must have decimal point — not just "0"
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("0.0"));
}

TEST(CxxEmitterTest, BodyIsIndentedByFourSpaces) {
    Block body;
    body.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(0)));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("    int x"));
}
