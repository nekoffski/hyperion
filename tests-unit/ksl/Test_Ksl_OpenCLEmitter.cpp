#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/core/Error.hh"
#include "ksl/Ast.hh"
#include "ksl/OpenCLEmitter.hh"

using namespace hyperion;
using namespace hyperion::ksl;
using ::testing::HasSubstr;
using ::testing::Not;

static Str emitKernel(KernelDef kernel) {
    OpenCLEmitter e;
    return e.emit(kernel);
}

TEST(OpenCLEmitterTest, SignatureHasKernelQualifier) {
    KernelDef k{"foo", {}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("__kernel void foo("));
}

TEST(OpenCLEmitterTest, NoExtraGidParameters) {
    // OpenCL kernels get global IDs via built-ins, not extra params
    KernelDef k{"foo", {}, {}};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, Not(HasSubstr("__ksl_gid")));
    EXPECT_THAT(out, Not(HasSubstr("__ksl_gsz")));
}

TEST(OpenCLEmitterTest, ScalarI32MapsToInt) {
    KernelDef k{"k", {Param{"n", ScalarType::i32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("int n"));
}

TEST(OpenCLEmitterTest, ScalarU32MapsToUint) {
    KernelDef k{"k", {Param{"n", ScalarType::u32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("uint n"));
}

TEST(OpenCLEmitterTest, ScalarF32MapsToFloat) {
    KernelDef k{"k", {Param{"v", ScalarType::f32}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("float v"));
}

TEST(OpenCLEmitterTest, ScalarF64MapsToDouble) {
    KernelDef k{"k", {Param{"v", ScalarType::f64}}, {}};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("double v"));
}

TEST(OpenCLEmitterTest, ReadOnlyBufferIsGlobalConstPtr) {
    KernelDef k{
        "k",
        {Param{"inp", BufferType{ScalarType::f32, BufferQualifier::ReadOnly}}},
        {}
    };
    EXPECT_THAT(
        emitKernel(std::move(k)), HasSubstr("__global const float* inp")
    );
}

TEST(OpenCLEmitterTest, WriteOnlyBufferIsGlobalNonConstPtr) {
    KernelDef k{
        "k",
        {Param{"out", BufferType{ScalarType::f32, BufferQualifier::WriteOnly}}},
        {}
    };
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("__global float* out"));
    EXPECT_THAT(out, Not(HasSubstr("const")));
}

TEST(OpenCLEmitterTest, ReadWriteBufferIsGlobalNonConstPtr) {
    KernelDef k{
        "k",
        {Param{"rw", BufferType{ScalarType::i32, BufferQualifier::ReadWrite}}},
        {}
    };
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("__global int* rw"));
}

TEST(OpenCLEmitterTest, GlobalIdMapsToGetGlobalId) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(0));
    Block body;
    body.push_back(makeVarDecl(
        "x", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalId, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("get_global_id(0)"));
}

TEST(OpenCLEmitterTest, GlobalIdDim1MapsCorrectly) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(1));
    Block body;
    body.push_back(makeVarDecl(
        "y", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalId, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("get_global_id(1)"));
}

TEST(OpenCLEmitterTest, GlobalSizeMapsToGetGlobalSize) {
    std::vector<ExprPtr> args;
    args.push_back(makeIntLit(0));
    Block body;
    body.push_back(makeVarDecl(
        "s", ScalarType::i32,
        makeBuiltinCall(BuiltinFn::GlobalSize, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("get_global_size(0)"));
}

TEST(OpenCLEmitterTest, ClampMapsToNativeClamp) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("x"));
    args.push_back(makeIntLit(0));
    args.push_back(makeIntLit(10));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Clamp, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("clamp("));
    EXPECT_THAT(out, Not(HasSubstr("std::clamp")));
}

TEST(OpenCLEmitterTest, MinMapsToNativeMin) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("a"));
    args.push_back(makeVarRef("b"));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Min, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("min("));
    EXPECT_THAT(out, Not(HasSubstr("std::min")));
}

TEST(OpenCLEmitterTest, MaxMapsToNativeMax) {
    std::vector<ExprPtr> args;
    args.push_back(makeVarRef("a"));
    args.push_back(makeVarRef("b"));
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::i32, makeBuiltinCall(BuiltinFn::Max, std::move(args))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("max("));
    EXPECT_THAT(out, Not(HasSubstr("std::max")));
}

TEST(OpenCLEmitterTest, CastEmitsCStyleCast) {
    Block body;
    body.push_back(makeVarDecl(
        "v", ScalarType::f32, makeCast(ScalarType::f32, makeVarRef("n"))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("(float)(n)"));
    EXPECT_THAT(out, Not(HasSubstr("static_cast")));
}

TEST(OpenCLEmitterTest, ForLoopEmitsCStyleFor) {
    Block inner;
    Block body;
    body.push_back(
        makeFor("i", makeIntLit(0), makeIntLit(8), std::move(inner))
    );
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(
        emitKernel(std::move(k)), HasSubstr("for (int i = 0; i < 8; ++i)")
    );
}

TEST(OpenCLEmitterTest, IfElseEmitted) {
    Block thenBody, elseBody;
    thenBody.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(1)));
    elseBody.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(2)));
    Block body;
    body.push_back(makeIf(
        makeVarRef("c"), std::move(thenBody),
        std::make_optional(std::move(elseBody))
    ));
    KernelDef k{"k", {}, std::move(body)};
    auto out = emitKernel(std::move(k));
    EXPECT_THAT(out, HasSubstr("if (c)"));
    EXPECT_THAT(out, HasSubstr("else {"));
}

TEST(OpenCLEmitterTest, BodyIsIndentedByFourSpaces) {
    Block body;
    body.push_back(makeVarDecl("x", ScalarType::i32, makeIntLit(0)));
    KernelDef k{"k", {}, std::move(body)};
    EXPECT_THAT(emitKernel(std::move(k)), HasSubstr("    int x"));
}
