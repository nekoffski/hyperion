#include "ksl/OpenCLEmitter.hh"

#include <fmt/core.h>

#include <sstream>

#include "internal/core/Error.hh"

namespace hyperion::ksl {

namespace {

Str scalarTypeToOpenCL(ScalarType t) {
    switch (t) {
        case ScalarType::i32:
            return "int";
        case ScalarType::u32:
            return "uint";
        case ScalarType::f32:
            return "float";
        case ScalarType::f64:
            return "double";
    }
    throw KslError{"unknown scalar type"};
}

Str binaryOpToOpenCL(BinaryOp op) {
    switch (op) {
        case BinaryOp::Add:
            return "+";
        case BinaryOp::Sub:
            return "-";
        case BinaryOp::Mul:
            return "*";
        case BinaryOp::Div:
            return "/";
        case BinaryOp::Mod:
            return "%";
        case BinaryOp::Eq:
            return "==";
        case BinaryOp::Ne:
            return "!=";
        case BinaryOp::Lt:
            return "<";
        case BinaryOp::Le:
            return "<=";
        case BinaryOp::Gt:
            return ">";
        case BinaryOp::Ge:
            return ">=";
        case BinaryOp::And:
            return "&&";
        case BinaryOp::Or:
            return "||";
    }
    throw KslError{"unknown binary operator"};
}

Str emitExprOCL(const Expr& expr);

Str emitExprOCL(const Expr& expr) {
    return std::visit(
        [](const auto& node) -> Str {
            using T = std::decay_t<decltype(node)>;

            if constexpr (std::is_same_v<T, IntLiteralExpr>) {
                return std::to_string(node.value);

            } else if constexpr (std::is_same_v<T, FloatLiteralExpr>) {
                auto s = fmt::format("{}", node.value);
                if (s.find('.') == Str::npos && s.find('e') == Str::npos) {
                    s += ".0";
                }
                return s;

            } else if constexpr (std::is_same_v<T, VarRefExpr>) {
                return node.name;

            } else if constexpr (std::is_same_v<T, BinaryExpr>) {
                return "(" + emitExprOCL(*node.lhs) + " " +
                       binaryOpToOpenCL(node.op) + " " +
                       emitExprOCL(*node.rhs) + ")";

            } else if constexpr (std::is_same_v<T, UnaryExpr>) {
                Str op = node.op == UnaryOp::Neg ? "-" : "!";
                return "(" + op + emitExprOCL(*node.operand) + ")";

            } else if constexpr (std::is_same_v<T, BufferIndexExpr>) {
                return node.bufferName + "[" + emitExprOCL(*node.index) + "]";

            } else if constexpr (std::is_same_v<T, BuiltinCallExpr>) {
                switch (node.fn) {
                    case BuiltinFn::GlobalId: {
                        const auto* lit =
                            std::get_if<IntLiteralExpr>(&node.args[0]->data);
                        if (!lit) {
                            throw KslError{
                                "globalId() argument must be an integer literal"
                            };
                        }
                        return "get_global_id(" + std::to_string(lit->value) +
                               ")";
                    }
                    case BuiltinFn::GlobalSize: {
                        const auto* lit =
                            std::get_if<IntLiteralExpr>(&node.args[0]->data);
                        if (!lit) {
                            throw KslError{
                                "globalSize() argument must be an integer "
                                "literal"
                            };
                        }
                        return "get_global_size(" + std::to_string(lit->value) +
                               ")";
                    }
                    case BuiltinFn::Clamp:
                        return "clamp(" + emitExprOCL(*node.args[0]) + ", " +
                               emitExprOCL(*node.args[1]) + ", " +
                               emitExprOCL(*node.args[2]) + ")";
                    case BuiltinFn::Min:
                        return "min(" + emitExprOCL(*node.args[0]) + ", " +
                               emitExprOCL(*node.args[1]) + ")";
                    case BuiltinFn::Max:
                        return "max(" + emitExprOCL(*node.args[0]) + ", " +
                               emitExprOCL(*node.args[1]) + ")";
                }
                throw KslError{"unknown built-in function"};

            } else if constexpr (std::is_same_v<T, CastExpr>) {
                return "(" + scalarTypeToOpenCL(node.targetType) + ")(" +
                       emitExprOCL(*node.operand) + ")";
            }
            throw KslError{"unknown expression node type"};
        },
        expr.data
    );
}

Str emitBlockOCL(const Block& block, u32 indent);

Str emitStmtOCL(const Stmt& stmt, u32 indent) {
    Str pad(indent * 4u, ' ');

    return std::visit(
        [&](const auto& node) -> Str {
            using T = std::decay_t<decltype(node)>;

            if constexpr (std::is_same_v<T, VarDeclStmt>) {
                return pad + scalarTypeToOpenCL(node.type) + " " + node.name +
                       " = " + emitExprOCL(*node.init) + ";\n";

            } else if constexpr (std::is_same_v<T, AssignStmt>) {
                return std::visit(
                    [&](const auto& target) -> Str {
                        using TT = std::decay_t<decltype(target)>;
                        if constexpr (std::is_same_v<TT, VarTarget>) {
                            return pad + target.name + " = " +
                                   emitExprOCL(*node.value) + ";\n";
                        } else {
                            return pad + target.bufferName + "[" +
                                   emitExprOCL(*target.index) +
                                   "] = " + emitExprOCL(*node.value) + ";\n";
                        }
                    },
                    node.target
                );

            } else if constexpr (std::is_same_v<T, ForStmt>) {
                Str r = pad + "for (int " + node.var + " = " +
                        emitExprOCL(*node.rangeStart) + "; " + node.var +
                        " < " + emitExprOCL(*node.rangeEnd) + "; ++" +
                        node.var + ") {\n";
                r += emitBlockOCL(node.body, indent + 1);
                r += pad + "}\n";
                return r;

            } else if constexpr (std::is_same_v<T, IfStmt>) {
                Str r = pad + "if (" + emitExprOCL(*node.condition) + ") {\n";
                r += emitBlockOCL(node.thenBody, indent + 1);
                r += pad + "}";
                if (node.elseBody) {
                    r += " else {\n";
                    r += emitBlockOCL(*node.elseBody, indent + 1);
                    r += pad + "}";
                }
                r += "\n";
                return r;
            }
            throw KslError{"unknown statement node type"};
        },
        stmt.data
    );
}

Str emitBlockOCL(const Block& block, u32 indent) {
    Str result;
    for (const auto& stmt : block) {
        result += emitStmtOCL(*stmt, indent);
    }
    return result;
}

}  // namespace

Str OpenCLEmitter::emit(const KernelDef& kernel) const {
    std::ostringstream out;

    out << "__kernel void " << kernel.name << "(\n";

    std::vector<Str> params;
    for (const auto& p : kernel.params) {
        std::visit(
            [&](const auto& t) {
                using T = std::decay_t<decltype(t)>;
                if constexpr (std::is_same_v<T, ScalarType>) {
                    params.push_back(
                        "    " + scalarTypeToOpenCL(t) + " " + p.name
                    );
                } else {
                    Str constPart = t.qualifier == BufferQualifier::ReadOnly
                                        ? "const "
                                        : "";
                    params.push_back(
                        "    __global " + constPart +
                        scalarTypeToOpenCL(t.elementType) + "* " + p.name
                    );
                }
            },
            p.type
        );
    }

    for (u32 i = 0; i < params.size(); ++i) {
        out << params[i];
        if (i + 1 < params.size()) {
            out << ",";
        }
        out << "\n";
    }

    out << ") {\n";
    out << emitBlockOCL(kernel.body, 1);
    out << "}\n";

    return out.str();
}

}  // namespace hyperion::ksl
