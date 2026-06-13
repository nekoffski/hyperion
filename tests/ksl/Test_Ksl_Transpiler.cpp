#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/core/Error.hh"
#include "internal/models/Compute.hh"
#include "ksl/Transpiler.hh"

using namespace hyperion;
using namespace hyperion::ksl;
using ::testing::HasSubstr;
using ::testing::Not;

TEST(TranspilerTest, CxxTargetReturnsCxxSource) {
    auto out = Transpiler::transpile("kernel k() {}", KernelType::cxx);
    EXPECT_THAT(out, HasSubstr("void k("));
    EXPECT_THAT(out, HasSubstr("#include <algorithm>"));
    EXPECT_THAT(out, Not(HasSubstr("__kernel")));
}

TEST(TranspilerTest, OpenCLTargetReturnsOpenCLSource) {
    auto out = Transpiler::transpile("kernel k() {}", KernelType::opencl);
    EXPECT_THAT(out, HasSubstr("__kernel void k("));
    EXPECT_THAT(out, Not(HasSubstr("#include")));
}

TEST(TranspilerTest, SameSourceProducesDifferentOutputsForDifferentTargets) {
    const Str src = "kernel k(n: i32) {}";
    auto cxx = Transpiler::transpile(src, KernelType::cxx);
    auto ocl = Transpiler::transpile(src, KernelType::opencl);
    EXPECT_NE(cxx, ocl);
}

TEST(TranspilerTest, SyntaxErrorThrowsKslError) {
    EXPECT_THROW(
        Transpiler::transpile("not a kernel", KernelType::cxx), KslError
    );
}

TEST(TranspilerTest, LexerErrorThrowsKslError) {
    EXPECT_THROW(
        Transpiler::transpile("kernel k() { @ }", KernelType::cxx), KslError
    );
}

TEST(TranspilerTest, EmptySourceThrowsKslError) {
    EXPECT_THROW(Transpiler::transpile("", KernelType::cxx), KslError);
}

TEST(TranspilerTest, ScalarParamCxxRoundTrip) {
    auto out = Transpiler::transpile(
        "kernel scale(data: ReadOnly Buffer<f32>, "
        "             result: WriteOnly Buffer<f32>, "
        "             factor: f32) {}",
        KernelType::cxx
    );
    EXPECT_THAT(out, HasSubstr("const float* data"));
    EXPECT_THAT(out, HasSubstr("float* result"));
    EXPECT_THAT(out, HasSubstr("float factor"));
}

TEST(TranspilerTest, ScalarParamOpenCLRoundTrip) {
    auto out = Transpiler::transpile(
        "kernel scale(data: ReadOnly Buffer<f32>, "
        "             result: WriteOnly Buffer<f32>, "
        "             factor: f32) {}",
        KernelType::opencl
    );
    EXPECT_THAT(out, HasSubstr("__global const float* data"));
    EXPECT_THAT(out, HasSubstr("__global float* result"));
    EXPECT_THAT(out, HasSubstr("float factor"));
}

TEST(TranspilerTest, ForLoopCxxRoundTrip) {
    auto out = Transpiler::transpile(
        "kernel k() { for i in range(0, 10) {} }", KernelType::cxx
    );
    EXPECT_THAT(out, HasSubstr("for (int i = 0; i < 10; ++i)"));
}

TEST(TranspilerTest, ForLoopOpenCLRoundTrip) {
    auto out = Transpiler::transpile(
        "kernel k() { for i in range(0, 10) {} }", KernelType::opencl
    );
    EXPECT_THAT(out, HasSubstr("for (int i = 0; i < 10; ++i)"));
}

static const Str kGaussianKSL = R"(
kernel gaussian_filter(
    input:   ReadOnly  Buffer<f32>,
    output:  WriteOnly Buffer<f32>,
    weights: ReadOnly  Buffer<f32>,
    width:   i32,
    height:  i32,
    radius:  i32
) {
    let x: i32 = globalId(0);
    let y: i32 = globalId(1);
    let sum: f32 = 0.0;
    for ky in range(-radius, radius + 1) {
        for kx in range(-radius, radius + 1) {
            let nx: i32 = clamp(x + kx, 0, width - 1);
            let ny: i32 = clamp(y + ky, 0, height - 1);
            let wi: i32 = (ky + radius) * (2 * radius + 1) + (kx + radius);
            sum = sum + input[ny * width + nx] * weights[wi];
        }
    }
    output[y * width + x] = sum;
}
)";

TEST(TranspilerTest, GaussianFilterCxxOutput) {
    auto out = Transpiler::transpile(kGaussianKSL, KernelType::cxx);

    EXPECT_THAT(out, HasSubstr("void gaussian_filter("));
    EXPECT_THAT(out, HasSubstr("const float* input"));
    EXPECT_THAT(out, HasSubstr("float* output"));
    EXPECT_THAT(out, HasSubstr("const float* weights"));
    EXPECT_THAT(out, HasSubstr("int width"));
    EXPECT_THAT(out, HasSubstr("int height"));
    EXPECT_THAT(out, HasSubstr("int radius"));
    EXPECT_THAT(out, HasSubstr("__ksl_gid_0"));
    EXPECT_THAT(out, HasSubstr("__ksl_gid_1"));
    EXPECT_THAT(out, HasSubstr("std::clamp("));
    EXPECT_THAT(out, HasSubstr("for (int ky ="));
    EXPECT_THAT(out, HasSubstr("for (int kx ="));
    EXPECT_THAT(out, HasSubstr("output["));
}

TEST(TranspilerTest, GaussianFilterOpenCLOutput) {
    auto out = Transpiler::transpile(kGaussianKSL, KernelType::opencl);

    EXPECT_THAT(out, HasSubstr("__kernel void gaussian_filter("));
    EXPECT_THAT(out, HasSubstr("__global const float* input"));
    EXPECT_THAT(out, HasSubstr("__global float* output"));
    EXPECT_THAT(out, HasSubstr("__global const float* weights"));
    EXPECT_THAT(out, HasSubstr("int width"));
    EXPECT_THAT(out, HasSubstr("get_global_id(0)"));
    EXPECT_THAT(out, HasSubstr("get_global_id(1)"));
    EXPECT_THAT(out, HasSubstr("clamp("));
    EXPECT_THAT(out, Not(HasSubstr("std::clamp")));
    EXPECT_THAT(out, HasSubstr("for (int ky ="));
    EXPECT_THAT(out, HasSubstr("for (int kx ="));
    EXPECT_THAT(out, Not(HasSubstr("__ksl_gid")));
}
