#include "interpreter.h"

namespace lasm {
    Interpreter::Interpreter(BaseError &onError):
        onError(onError) {
    }

    void Interpreter::interprete(std::shared_ptr<Expr> expr) {
        try {
        } catch (LasmException &e) {
            onError.onError(e.getType(), e.getToken(), &e);
        }
    }

    LasmObject Interpreter::evaluate(std::shared_ptr<Expr> expr) {
        return std::any_cast<LasmObject>(expr->accept(this));
    }

    std::any Interpreter::visitBinary(BinaryExpr *expr) {
        auto left = evaluate(expr->left);
        auto right = evaluate(expr->right);

        switch (expr->op->getType()) {
            case MINUS:
                // first number decides auto-cast
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(NUMBER_O, left.toNumber() - right.toNumber());
                } else if (left.isReal() && left.isScalar()) {
                    return LasmObject(REAL_O, left.toReal() - right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
                break;
            case SLASH:
                if (left.isNumber() && left.isScalar()) {
                    // integer division by 0 is not valid!
                    if (right.toNumber() == 0) {
                        throw LasmDivisionByZero(expr->op);
                    }
                    return LasmObject(NUMBER_O, left.toNumber() / right.toNumber());
                } else if (left.isReal() && left.isScalar()) {
                    return LasmObject(REAL_O, left.toReal() / right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
                break;
            case STAR:
                // first number decides auto-cast
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(NUMBER_O, left.toNumber() * right.toNumber());
                } else if (left.isReal() && left.isScalar()) {
                    return LasmObject(REAL_O, left.toReal() * right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
                break;
            case PLUS:
                // first number decides auto-cast
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(NUMBER_O, left.toNumber() - right.toNumber());
                } else if (left.isReal() && left.isScalar()) {
                    return LasmObject(REAL_O, left.toReal() - right.toReal());
                } else if (left.isString() && right.isString()) {
                    // string cat
                    return LasmObject(STRING_O, left.toString() + right.toString());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O, STRING_O}, left.getType(), expr->op);
                }
                break;

            // comparison
            case GREATER:
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toNumber() > right.toNumber());
                } else if (right.isReal() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toReal() > right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
            case LESS:
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toNumber() < right.toNumber());
                } else if (right.isReal() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toReal() < right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
            case GREATER_EQUAL:
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toNumber() >= right.toNumber());
                } else if (right.isReal() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toReal() >= right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
            case LESS_EQUAL:
                if (left.isNumber() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toNumber() <= right.toNumber());
                } else if (right.isReal() && left.isScalar()) {
                    return LasmObject(BOOLEAN_O, left.toReal() <= right.toReal());
                } else {
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, left.getType(), expr->op);
                }
            case BANG_EQUAL:
                return LasmObject(BOOLEAN_O, !left.isEqual(right));
            case EQUAL_EQUAL:
                return LasmObject(BOOLEAN_O, left.isEqual(right));
            default:
                break;
        }

        // should be unreacbable
        return LasmObject(NIL_O, nullptr);
    }

    std::any Interpreter::visitUnary(UnaryExpr *expr) {
        auto right = evaluate(expr->right);

        switch (expr->op->getType()) {
            case MINUS:
                if (right.isReal()) {
                    return LasmObject(REAL_O, -right.toReal());
                } else if (right.isNumber()) {
                    return LasmObject(NUMBER_O, -right.toNumber());
                } else {
                    // type error!
                    throw LasmTypeError(std::vector<ObjectType> {NUMBER_O, REAL_O}, right.getType(), expr->op);
                }
                break;
            case BANG:
                return LasmObject(BOOLEAN_O, !right.isTruthy());
            default:
                break;
        }

        // should be unreacbable
        return LasmObject(NIL_O, nullptr);
    }

    std::any Interpreter::visitLiteral(LiteralExpr *expr) {
        return expr->value;
    }

    std::any Interpreter::visitGrouping(GroupingExpr *expr) {
        return evaluate(expr->expression);
    }
}
