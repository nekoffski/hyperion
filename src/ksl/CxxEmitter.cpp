#include "ksl/CxxEmitter.hh"

#include <fmt/core.h>

#include <sstream>

#include "internal/core/Error.hh"

namespace hyperion::ksl {

namespace {

Str scalarTypeToCxx(ScalarType t) {
    switch (t) {
        case ScalarType::i32:
            return "int";
        case ScalarType::u32:
            return "unsigned int";
        case ScalarType::f32:
            return "float";
        case ScalarType::f64:
            return "double";
    }
    throw KslError{"unknown scalar type"};
}

Str binaryOpToCxx(BinaryOp op) {
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

Str emitExprCxx(const Expr& expr);

Str emitExprCxx(const Expr& expr) {
    return std::visit(
        [](const auto& node) -> Str {
            using T = std::decay_t<decltype(node)>;

            if constexpr (std::is_same_v<T, IntLiteralExpr>) {
                return std::to_string(node.value);

            } else if constexpr (std::is_same_v<T, FloatLiteralExpr>) {
                auto s = fmt::format("{}", node.value);
                // Ensure there is always a decimal point
                if (s.find('.') == Str::npos && s.find('e') == Str::npos) {
                    s += ".0";
                }
                return s;

            } else if constexpr (std::is_same_v<T, VarRefExpr>) {
                return node.name;

            } else if constexpr (std::is_same_v<T, BinaryExpr>) {
                return "(" + emitExprCxx(*node.lhs) + " " +
                       binaryOpToCxx(node.op) + " " + emitExprCxx(*node.rhs) +
                       ")";

            } else if constexpr (std::is_same_v<T, UnaryExpr>) {
                Str op = node.op == UnaryOp::Neg ? "-" : "!";
                return "(" + op + emitExprCxx(*node.operand) + ")";

            } else if constexpr (std::is_same_v<T, BufferIndexExpr>) {
                return node.bufferName + "[" + emitExprCxx(*node.index) + "]";

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
                        return "__ksl_gid_" + std::to_string(lit->value);
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
                        return "__ksl_gsz_" + std::to_string(lit->value);
                    }
                    case BuiltinFn::Clamp:
                        return "std::clamp(" + emitExprCxx(*node.args[0]) +
                               ", " + emitExprCxx(*node.args[1]) + ", " +
                               emitExprCxx(*node.args[2]) + ")";
                    case BuiltinFn::Min:
                        return "std::min(" + emitExprCxx(*node.args[0]) + ", " +
                               emitExprCxx(*node.args[1]) + ")";
                    case BuiltinFn::Max:
                        return "std::max(" + emitExprCxx(*node.args[0]) + ", " +
                               emitExprCxx(*node.args[1]) + ")";
                }
                throw KslError{"unknown built-in function"};

            } else if constexpr (std::is_same_v<T, CastExpr>) {
                return "static_cast<" + scalarTypeToCxx(node.targetType) +
                       ">(" + emitExprCxx(*node.operand) + ")";
            }
            throw KslError{"unknown expression node type"};
        },
        expr.data
    );
}

Str emitBlockCxx(const Block& block, u32 indent);

Str emitStmtCxx(const Stmt& stmt, u32 indent) {
    Str pad(indent * 4u, ' ');

    return std::visit(
        [&](const auto& node) -> Str {
            using T = std::decay_t<decltype(node)>;

            if constexpr (std::is_same_v<T, VarDeclStmt>) {
                return pad + scalarTypeToCxx(node.type) + " " + node.name +
                       " = " + emitExprCxx(*node.init) + ";\n";

            } else if constexpr (std::is_same_v<T, AssignStmt>) {
                return std::visit(
                    [&](const auto& target) -> Str {
                        using TT = std::decay_t<decltype(target)>;
                        if constexpr (std::is_same_v<TT, VarTarget>) {
                            return pad + target.name + " = " +
                                   emitExprCxx(*node.value) + ";\n";
                        } else {
                            return pad + target.bufferName + "[" +
                                   emitExprCxx(*target.index) +
                                   "] = " + emitExprCxx(*node.value) + ";\n";
                        }
                    },
                    node.target
                );

            } else if constexpr (std::is_same_v<T, ForStmt>) {
                Str r = pad + "for (int " + node.var + " = " +
                        emitExprCxx(*node.rangeStart) + "; " + node.var +
                        " < " + emitExprCxx(*node.rangeEnd) + "; ++" +
                        node.var + ") {\n";
                r += emitBlockCxx(node.body, indent + 1);
                r += pad + "}\n";
                return r;

            } else if constexpr (std::is_same_v<T, IfStmt>) {
                Str r = pad + "if (" + emitExprCxx(*node.condition) + ") {\n";
                r += emitBlockCxx(node.thenBody, indent + 1);
                r += pad + "}";
                if (node.elseBody) {
                    r += " else {\n";
                    r += emitBlockCxx(*node.elseBody, indent + 1);
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

Str emitBlockCxx(const Block& block, u32 indent) {
    Str result;
    for (const auto& stmt : block) {
        result += emitStmtCxx(*stmt, indent);
    }
    return result;
}

}  // namespace

Str CxxEmitter::emit(const KernelDef& kernel) const {
    std::ostringstream out;

    out << "#include <algorithm>\n"
        << "#include <cstddef>\n\n";

    out << "void " << kernel.name << "(\n";

    std::vector<Str> params;
    for (const auto& p : kernel.params) {
        std::visit(
            [&](const auto& t) {
                using T = std::decay_t<decltype(t)>;
                if constexpr (std::is_same_v<T, ScalarType>) {
                    params.push_back(
                        "    " + scalarTypeToCxx(t) + " " + p.name
                    );
                } else {
                    Str constPart = t.qualifier == BufferQualifier::ReadOnly
                                        ? "const "
                                        : "";
                    params.push_back(
                        "    " + constPart + scalarTypeToCxx(t.elementType) +
                        "* " + p.name
                    );
                }
            },
            p.type
        );
    }

    // Extra work-item parameters consumed by the JIT driver loop
    for (int dim = 0; dim < 3; ++dim) {
        params.push_back("    size_t __ksl_gid_" + std::to_string(dim));
    }
    for (int dim = 0; dim < 3; ++dim) {
        params.push_back("    size_t __ksl_gsz_" + std::to_string(dim));
    }

    for (u32 i = 0; i < params.size(); ++i) {
        out << params[i];
        if (i + 1 < params.size()) {
            out << ",";
        }
        out << "\n";
    }

    out << ") {\n";
    out << emitBlockCxx(kernel.body, 1);
    out << "}\n";

    return out.str();
}

}  // namespace hyperion::ksl
