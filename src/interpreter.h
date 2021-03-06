#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <iostream>
#include <memory>
#include <vector>
#include <any>
#include "instruction.h"
#include "expr.h"
#include "object.h"
#include "error.h"
#include "stmt.h"
#include "environment.h"
#include "callable.h"
#include "instruction.h"
#include "filereader.h"

namespace lasm {
    class InterpreterCallback {
        public:
            ~InterpreterCallback() {}

            virtual void onStatementExecuted(LasmObject *object) {}
    };

    class Interpreter: public ExprVisitor, public StmtVisitor {
        public:
            Interpreter(BaseError &onError, BaseInstructionSet &is, InterpreterCallback *callback=nullptr,
                    FileReader *reader=nullptr);

            void initGlobals();

            // TODO first pass:
            // resolve labels by adding them to a speical environment
            // each variable that was not resolvable the first time around gets a pointer to said environment
            // for loops each iteration creates a new label environment
            // each block creates a new label enviormnent
            // variable names shadow labels
            // second pass:
            // now all variables should be resolved
            std::vector<InstructionResult> interprete(std::vector<std::shared_ptr<Stmt>> stmts, bool abortOnError=false,
                    int passes=2);

            void execPass(std::vector<std::shared_ptr<Stmt>> stmts);

            void execute(std::shared_ptr<Stmt> stmt);

            LasmObject evaluate(std::shared_ptr<Expr> expr);

            std::any visitBinary(BinaryExpr *expr);
            std::any visitUnary(UnaryExpr *expr);
            std::any visitLiteral(LiteralExpr *expr);
            std::any visitGrouping(GroupingExpr *expr);
            std::any visitVariable(VariableExpr *expr);
            std::any visitAssign(AssignExpr *expr);
            std::any visitLogical(LogicalExpr *expr);
            std::any visitCall(CallExpr *expr);
            std::any visitList(ListExpr *expr);
            std::any visitIndex(IndexExpr *expr);
            std::any visitIndexAssign(IndexAssignExpr *expr);

            std::any visitExpression(ExpressionStmt *stmt);
            std::any visitLet(LetStmt *stmt);
            std::any visitBlock(BlockStmt *stmt);
            std::any visitIf(IfStmt *stmt);
            std::any visitWhile(WhileStmt *stmt);
            std::any visitFunction(FunctionStmt *stmt);
            std::any visitReturn(ReturnStmt *stmt);
            std::any visitInstruction(InstructionStmt *stmt);
            std::any visitDirective(DirectiveStmt *stmt);
            std::any visitAlign(AlignStmt *stmt);
            std::any visitFill(FillStmt *stmt);
            std::any visitOrg(OrgStmt *stmt);
            std::any visitDefineByte(DefineByteStmt *stmt);
            std::any visitBss(BssStmt *stmt);
            std::any visitLabel(LabelStmt *stmt);
            std::any visitIncbin(IncbinStmt *stmt);
            std::any visitInclude(IncludeStmt *stmt);

            void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment,
                    std::shared_ptr<Environment> labels=std::shared_ptr<Environment>(nullptr));


            unsigned long getAddress() { return address; }
            void setAddress(unsigned long newAddress) { address = newAddress; }

            std::vector<InstructionResult> getCode() { return code; }
            unsigned int getPass() { return pass; }

            std::shared_ptr<Environment> getEnv() { return environment; }
            std::shared_ptr<Environment> getLabels() { return labels; }
            std::vector<std::shared_ptr<Environment>>& getLabelTable() { return labelTable; }
            std::shared_ptr<Environment> getGlobals() { return globals; }

            BaseInstructionSet& getInstructions() { return instructions; }
        private:
            void onInstructionResult(InstructionResult result);

            BaseError &onError;
            BaseInstructionSet &instructions;
            InterpreterCallback *callback;

            // interpreter environment chain
            std::shared_ptr<Environment> globals;
            std::shared_ptr<Environment> environment;

            // interpreter label chain
            std::shared_ptr<Environment> globalLabels;
            std::shared_ptr<Environment> labels;

            // flat list of all labels that were generated during assembly.
            // used for label list file
            std::vector<std::shared_ptr<Environment>> labelTable;

            Endianess getNativeByteOrder();

            unsigned long address = 0;
            unsigned short pass = 0;

            std::vector<InstructionResult> code;

            FileReader *reader;
    };
}

#endif
