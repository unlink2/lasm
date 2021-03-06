#ifndef __STMT_H__
#define __STMT_H__

#include <iostream>
#include <any>
#include <memory>
#include "expr.h"
#include "instruction.h"
#include "environment.h"

namespace lasm {
    enum StmtType {
        EXPRESSION_STMT,
        LET_STMT,
        BLOCK_STMT,
        IF_STMT,
        WHILE_STMT,
        FUNCTION_STMT,
        RETURN_STMT,
        LABEL_STMT,
        INSTRUCTION_STMT,
        DIRECTIVE_STMT,
        ORG_STMT,
        ALIGN_STMT,
        DEFINE_BYTE_STMT,
        FILL_STMT,
        ENUM_STMT,
        BSS_STMT,
        INCLUDE_STMT,
        INCBIN_STMT
    };

    class StmtVisitor;

    class Stmt {
        public:
            Stmt(StmtType type):
                type(type) {}

            virtual ~Stmt() {}

            template<typename T>
            T* castTo() {
                return static_cast<T>(this);
            }

            virtual std::any accept(StmtVisitor *visitor) { return std::any(nullptr); }

            StmtType getType() {
                return type;
            }
        private:
            StmtType type;
    };

    class ExpressionStmt: public Stmt {
        public:
            ExpressionStmt(std::shared_ptr<Expr> expr):
                Stmt::Stmt(EXPRESSION_STMT), expr(expr) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Expr> expr;
    };

    class LetStmt: public Stmt {
        public:
            LetStmt(std::shared_ptr<Token> name, std::shared_ptr<Expr> init):
                Stmt::Stmt(LET_STMT), name(name), init(init) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> name;
            std::shared_ptr<Expr> init;
    };

    class BlockStmt: public Stmt {
        public:
            BlockStmt(std::vector<std::shared_ptr<Stmt>> statements):
                Stmt::Stmt(BLOCK_STMT), statements(statements) {
            }

            virtual std::any accept(StmtVisitor *visitor);

            std::vector<std::shared_ptr<Stmt>> statements;
    };

    class IfStmt: public Stmt {
        public:
            IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch,
                    std::shared_ptr<Stmt> elseBranch):
                Stmt::Stmt(IF_STMT), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Stmt> thenBranch;
            std::shared_ptr<Stmt> elseBranch;
    };

    class WhileStmt: public Stmt {
        public:
            WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body):
                Stmt::Stmt(WHILE_STMT), condition(condition), body(body) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Stmt> body;
    };

    class FunctionStmt: public Stmt {
        public:
            FunctionStmt(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Token>> params,
                    std::vector<std::shared_ptr<Stmt>> body):
                Stmt::Stmt(FUNCTION_STMT), name(name), params(params), body(body) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> name;
            std::vector<std::shared_ptr<Token>> params;
            std::vector<std::shared_ptr<Stmt>> body;
    };

    class ReturnStmt: public Stmt {
        public:
            ReturnStmt(std::shared_ptr<Token> keyword, std::shared_ptr<Expr> value):
                Stmt::Stmt(RETURN_STMT), keyword(keyword), value(value) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> keyword;
            std::shared_ptr<Expr> value;
    };

    class LabelStmt: public Stmt {
        public:
            LabelStmt(std::shared_ptr<Token> name):
                Stmt::Stmt(LABEL_STMT), name(name) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> name;
    };

    class InstructionStmt: public Stmt {
        public:
            InstructionStmt(std::shared_ptr<Token> name, std::shared_ptr<InstructionInfo> info, std::vector<std::shared_ptr<Expr>> args):
                Stmt::Stmt(INSTRUCTION_STMT), name(name), info(info), args(args) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> name;
            std::shared_ptr<InstructionInfo> info;
            std::vector<std::shared_ptr<Expr>> args;
            // use this to makr an instruction as not
            // fully resolved on the first pass
            bool fullyResolved = true;
    };

    class DirectiveStmt: public Stmt {
        public:
            DirectiveStmt(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Expr>> args,
                    Directive *directive):
                Stmt::Stmt(DIRECTIVE_STMT), name(name), args(args), directive(directive) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> name;
            std::vector<std::shared_ptr<Expr>> args;
            Directive *directive;
    };

    class AlignStmt: public Stmt {
        public:
            AlignStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> alignTo, std::shared_ptr<Expr> fillValue):
                Stmt::Stmt(ALIGN_STMT), token(token), alignTo(alignTo), fillValue(fillValue) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> alignTo;
            std::shared_ptr<Expr> fillValue;
    };

    class OrgStmt: public Stmt {
        public:
            OrgStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> address):
                Stmt::Stmt(ORG_STMT), token(token), address(address) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> address;
    };

    class FillStmt: public Stmt {
        public:
            FillStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> fillAddress, std::shared_ptr<Expr> fillValue):
                Stmt::Stmt(FILL_STMT), token(token), fillAddress(fillAddress), fillValue(fillValue) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> fillAddress;
            std::shared_ptr<Expr> fillValue;
    };

    class DefineByteStmt: public Stmt {
        public:
            DefineByteStmt(std::shared_ptr<Token> token, std::vector<std::shared_ptr<Expr>> values,
                    unsigned int size, Endianess endianess):
                Stmt::Stmt(DEFINE_BYTE_STMT), token(token), values(values), size(size), endianess(endianess) {
            }

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::vector<std::shared_ptr<Expr>> values;
            unsigned int size;
            Endianess endianess;
    };

    class BssStmt: public Stmt {
        public:
            BssStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> startAddress,
                    std::vector<std::shared_ptr<LetStmt>> declarations):
                Stmt::Stmt(BSS_STMT), token(token), startAddress(startAddress), declarations(declarations) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> startAddress;
            std::vector<std::shared_ptr<LetStmt>> declarations;
    };

    class IncbinStmt: public Stmt {
        public:
            IncbinStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> filePath):
                Stmt::Stmt(INCBIN_STMT), token(token), filePath(filePath) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> filePath;

            // do not re-read once this is not null
            std::shared_ptr<char[]> data = std::shared_ptr<char[]>(nullptr);
            unsigned long size; 
    };

    class IncludeStmt: public Stmt {
        public:
            IncludeStmt(std::shared_ptr<Token> token, std::shared_ptr<Expr> filePath):
                Stmt::Stmt(INCLUDE_STMT), token(token), filePath(filePath) {}

            virtual std::any accept(StmtVisitor *visitor);

            std::shared_ptr<Token> token;
            std::shared_ptr<Expr> filePath;

            std::vector<std::shared_ptr<Stmt>> stmts;
            bool wasparsed = false; // set to true to not re-parse
    };

    class StmtVisitor {
        public:
            virtual ~StmtVisitor () {}

            virtual std::any visitExpression(ExpressionStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitLet(LetStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitBlock(BlockStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitIf(IfStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitWhile(WhileStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitFunction(FunctionStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitReturn(ReturnStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitLabel(LabelStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitInstruction(InstructionStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitAlign(AlignStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitOrg(OrgStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitFill(FillStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitDefineByte(DefineByteStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitBss(BssStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitIncbin(IncbinStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitInclude(IncludeStmt *stmt) { return std::any(nullptr); };
            virtual std::any visitDirective(DirectiveStmt *stmt) { return std::any(nullptr); };
    };
}

#endif 
