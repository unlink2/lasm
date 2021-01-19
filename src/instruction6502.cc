#include "instruction6502.h"
#include "parser.h"
#include "stmt.h"
#include "interpreter.h"
#include "utility.h"

namespace lasm {
    /**
     * Immediate
     */
    InstructionParser6502Immediate::InstructionParser6502Immediate(char immediate,
            InstructionSet6502 *is):
        immediate(immediate),
        is(is) {}

    std::shared_ptr<Stmt> InstructionParser6502Immediate::parse(Parser *parser) {
        auto name = parser->previous();
        if (parser->match(std::vector<TokenType> {HASH})) {
            // immediate
            auto expr = parser->expression();
            std::vector<std::shared_ptr<Expr>> args;

            args.push_back(expr);

            auto info = std::make_shared<InstructionInfo>(InstructionInfo(is->immediate));
            info->addOpcode(immediate);

            parser->consume(SEMICOLON, MISSING_SEMICOLON);

            return std::make_shared<InstructionStmt>(InstructionStmt(name, info, args));
        }
        return std::shared_ptr<Stmt>(nullptr);
    }

    InstructionResult Immediate6502Generator::generate(Interpreter *interpreter,
            std::shared_ptr<InstructionInfo> info, InstructionStmt *stmt) {
        auto value = interpreter->evaluate(stmt->args[0]);

        const unsigned int size = 2;

        std::shared_ptr<char[]> data(new char[size]);
        data[0] = info->getOpcode();
        if (!value.isScalar()) {
            // handle first pass
            if (value.isNil() && interpreter->getPass() == 0) {
                value = LasmObject(NUMBER_O, (lasmNumber)0);
            } else {
                throw LasmException(TYPE_ERROR, stmt->name);
            }
        } else if (value.toNumber() > 0xFF) {
            throw LasmException(VALUE_OUT_OF_RANGE, stmt->name);
        }
        data[1] = value.toNumber() & 0xFF;
        interpreter->setAddress(interpreter->getAddress()+size);
        return InstructionResult(data, size, interpreter->getAddress()-size, stmt->name);
    }

    /**
     * Absolute
     */
    InstructionParser6502AbsoluteOrZp::InstructionParser6502AbsoluteOrZp(char absolute, InstructionSet6502 *is):
        absolute(absolute), is(is) {}

    std::shared_ptr<Stmt> InstructionParser6502AbsoluteOrZp::parse(Parser *parser) {
        auto name = parser->previous();
        auto expr = parser->expression();
        std::vector<std::shared_ptr<Expr>> args;

        args.push_back(expr);

        auto info = std::make_shared<InstructionInfo>(InstructionInfo(is->absolute));
        info->addOpcode(absolute);

        parser->consume(SEMICOLON, MISSING_SEMICOLON);

        return std::make_shared<InstructionStmt>(InstructionStmt(name, info, args));
    }

    InstructionResult AbsoluteOrZp6502Generator::generate(Interpreter *interpreter,
            std::shared_ptr<InstructionInfo> info,
            InstructionStmt *stmt) {

        const unsigned int size = 3;

        auto value = interpreter->evaluate(stmt->args[0]);

        std::shared_ptr<char[]> data(new char[size]);
        data[0] = info->getOpcode();
        if (!value.isScalar()) {
            // handle first pass
            if (value.isNil() && interpreter->getPass() == 0) {
                value = LasmObject(NUMBER_O, (lasmNumber)0);
            } else {
                throw LasmException(TYPE_ERROR, stmt->name);
            }
        } else if (value.toNumber() > 0xFFFF) {
            throw LasmException(VALUE_OUT_OF_RANGE, stmt->name);
        }
        data[1] = HI(value.toNumber());
        data[2] = LO(value.toNumber());
        interpreter->setAddress(interpreter->getAddress()+size);
        return InstructionResult(data, size, interpreter->getAddress()-size, stmt->name);
    }


    /**
     * Instruction set
     */

    InstructionSet6502::InstructionSet6502() {
        // TODO add all instructions
        addInstruction("lda", std::make_shared<InstructionParser6502Immediate>(InstructionParser6502Immediate(0x69, this)));
        addInstruction("lda", std::make_shared<InstructionParser6502AbsoluteOrZp>(InstructionParser6502AbsoluteOrZp(0x6D, this)));
    }

    InstructionResult InstructionSet6502::generate(Interpreter *interpreter,
            std::shared_ptr<InstructionInfo> info,
            InstructionStmt *stmt) {
        return info->getGenerator()->generate(interpreter, info, stmt);
    }
};
