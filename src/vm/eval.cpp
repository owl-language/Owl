#include "eval.hpp"

Object Evaluator::stringOp(TokenType op, Object left, Object right) {
    onEnter("String Operation");
    Object retObj(" ");
    Object convObj;
    if (op == PLUS) {
        if (left.type == STRING) {
            retObj.data._value = left.data.stringValue();
            convObj = right;
            if (convObj.type == INTEGER) convObj.data._value = to_string(convObj.data.intValue());
                else if (convObj.type == REAL) convObj.data._value = to_string(convObj.data.realValue());
                retObj.data._value += convObj.data.stringValue();
            } else {
                retObj.data._value = right.data.stringValue();
                convObj = left;
                if (convObj.type == INTEGER) convObj.data._value = to_string(convObj.data.intValue());
                else if (convObj.type == REAL) convObj.data._value = to_string(convObj.data.realValue());
                retObj.data._value = convObj.data.stringValue() + retObj.data.stringValue();
            }
    } else if (op == EQUAL) {
        retObj = Object(left.data.stringValue() == right.data.stringValue());
    } else if (op == NOTEQUAL) {
        retObj = Object(left.data.stringValue() != right.data.stringValue());
    } else {
        logError("Hoot! Operation: " + tokenString[op] + " not supported on strings.");
        retObj = Object(0);
    }
    onExit();
    return retObj;
}

float divide(float a, float b) {
    if (b == 0) {
        logError("Hoot! Hoot!: Divide By Zero. (Don't do that.)");
        return 0;
    }
    return a/b;
}

Object Evaluator::mathOp(TokenType op, Object leftChild, Object rightChild) {
    onEnter("math Op");
    Object retObj;
    float leftOperand, rightOperand, result = 0;
    leftOperand = leftChild.data.realValue();
    rightOperand = rightChild.data.realValue();
    say(to_string(leftOperand) + " " + tokenString[op] + " " + to_string(rightOperand));
    switch (op) {
        case PLUS:
            result = leftOperand + rightOperand;
            break;
        case MINUS:
            result = leftOperand - rightOperand;
            break;
        case MULT:
            result = leftOperand * rightOperand;
            break;
        case DIVD:
            result = divide(leftOperand, rightOperand);
            break;
        default:
            break;
    }
    if (leftChild.type == INTEGER && rightChild.type == INTEGER) {
        retObj.type = INTEGER;
    } else {
        retObj.type = REAL;
    }
    retObj.data._value = to_string(result);
    onExit();
    return retObj;
}

Object Evaluator::relOp(TokenType op, float leftOperand, float rightOperand) {
    onEnter("rel Op");
    Object retVal;
    say(to_string(leftOperand) + " " + tokenString[op] + " " + to_string(rightOperand));
    bool result;
    switch (op) {
        case LESS:
            result = (leftOperand < rightOperand);
            break;
        case GREATER:
            result = (leftOperand > rightOperand);
            break;
        case EQUAL:
            result = (leftOperand == rightOperand);
            break;
        case NOTEQUAL:
            result = (leftOperand != rightOperand);
            break;
        case LESSEQ:
            result = (leftOperand <= rightOperand);
            break;
        case GREATEREQ:
            result = (leftOperand >= rightOperand);
            break;
    }
    retVal = Object(result);
    retVal.type = INTEGER;
    return retVal;
}
