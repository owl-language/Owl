#ifndef eval_hpp
#define eval_hpp
#include "runtime/memobject.hpp"
#include "../tokens/tokens.hpp"
#include "../ast/ast.hpp"
#include "../tools/tracer.hpp"

class Evaluator {
    public:
        Object stringOp(TokenType op, Object left, Object right);
        Object mathOp(TokenType op, Object left, Object right);
        Object relOp(TokenType op, float left, float right);
};

#endif