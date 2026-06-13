#include "ksl/Transpiler.hh"

#include "internal/core/Error.hh"
#include "ksl/CxxEmitter.hh"
#include "ksl/Lexer.hh"
#include "ksl/OpenCLEmitter.hh"
#include "ksl/Parser.hh"

namespace hyperion::ksl {

Str Transpiler::transpile(const Str& source, KernelType target) {
    Lexer lexer{source};
    auto tokens = lexer.tokenize();

    Parser parser{std::move(tokens)};
    auto kernel = parser.parse();

    switch (target) {
        case KernelType::cxx: {
            CxxEmitter emitter;
            return emitter.emit(kernel);
        }
        case KernelType::opencl: {
            OpenCLEmitter emitter;
            return emitter.emit(kernel);
        }
    }

    throw KslError{ErrorCode::invalidArgument, "unknown KernelType value"};
}

}  // namespace hyperion::ksl
