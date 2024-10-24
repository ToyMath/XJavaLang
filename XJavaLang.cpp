#include "ast.h"
#include <iostream>
#include <map>
#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

using namespace llvm;

extern "C" {
    #include "parser.tab.h"
    int yyparse();
    extern ASTNode* ast_root;
}

extern ASTNode* ast_root;

static LLVMContext TheContext;
static Module* TheModule;
static IRBuilder<> Builder(TheContext);
static std::map<std::string, Value*> NamedValues;

void generate_code(ASTNode* node);

Value* codegen_expr(ASTNode* node);
void codegen_stmt(ASTNode* node);
void codegen_method(ASTNode* node);
void codegen_class(ASTNode* node);
void codegen_program(ASTNode* node);

void generate_code(ASTNode* node) {
    TheModule = new Module("main", TheContext);

    codegen_program(node);

    verifyModule(*TheModule);

    // LLVM IR print
    TheModule->print(outs(), nullptr);
}

void codegen_program(ASTNode* node) {
    if (node->type != NODE_PROGRAM) {
        std::cerr << "Root node is not a program\n";
        return;
    }
    codegen_class(node->program.class_node);
}

void codegen_class(ASTNode* node) {
    if (node->type != NODE_CLASS) {
        std::cerr << "Expected class node\n";
        return;
    }
    codegen_method(node->class_decl.body);
}

void codegen_method(ASTNode* node) {
    if (node->type != NODE_METHOD) {
        std::cerr << "Expected method node\n";
        return;
    }

    FunctionType *FT = FunctionType::get(Type::getInt32Ty(TheContext), false);
    Function *MainFunc = Function::Create(FT, Function::ExternalLinkage, "main", TheModule);

    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", MainFunc);
    Builder.SetInsertPoint(BB);

    ASTNode* stmt_list = node->method_decl.body;
    while (stmt_list) {
        codegen_stmt(stmt_list->stmt_list.stmt);
        stmt_list = stmt_list->stmt_list.next;
    }

    // return 0
    Builder.CreateRet(ConstantInt::get(TheContext, APInt(32, 0)));

    verifyFunction(*MainFunc);
}

void codegen_stmt(ASTNode* node) {
    switch (node->type) {
        case NODE_VAR_DECL: {
            std::string var_name = node->var_name;
            AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(TheContext), 0, var_name.c_str());
            NamedValues[var_name] = Alloca;
            break;
        }
        case NODE_ASSIGN: {
            std::string var_name = node->assign.var_name;
            Value* var = NamedValues[var_name];
            if (!var) {
                std::cerr << "Unknown variable name: " << var_name << "\n";
                return;
            }
            Value* val = codegen_expr(node->assign.expr);
            Builder.CreateStore(val, var);
            break;
        }
        case NODE_PRINT: {
            std::string var_name = node->print_var_name;
            Value* var = NamedValues[var_name];
            if (!var) {
                std::cerr << "Unknown variable name: " << var_name << "\n";
                return;
            }
            Value* val = Builder.CreateLoad(Type::getInt32Ty(TheContext), var, var_name.c_str());
            // Call printf function
            std::vector<Type*> printf_arg_types;
            printf_arg_types.push_back(Type::getInt8PtrTy(TheContext));
            FunctionType* printf_type = FunctionType::get(Type::getInt32Ty(TheContext), printf_arg_types, true);
            FunctionCallee printf_func = TheModule->getOrInsertFunction("printf", printf_type);

            Value* format_str = Builder.CreateGlobalStringPtr("%d\n");
            Builder.CreateCall(printf_func, { format_str, val });
            break;
        }
        case NODE_WHILE: {
            Function *TheFunction = Builder.GetInsertBlock()->getParent();

            BasicBlock *CondBB = BasicBlock::Create(TheContext, "while.cond", TheFunction);
            BasicBlock *BodyBB = BasicBlock::Create(TheContext, "while.body");
            BasicBlock *EndBB = BasicBlock::Create(TheContext, "while.end");

            Builder.CreateBr(CondBB);

            Builder.SetInsertPoint(CondBB);
            Value *CondV = codegen_expr(node->while_loop.condition);
            if (!CondV) return;

            // Ensure condition is i1 (boolean)
            Value *CmpV;
            if (CondV->getType() == Type::getInt1Ty(TheContext)) {
                CmpV = CondV;
            } else {
                CmpV = Builder.CreateICmpNE(CondV, ConstantInt::get(TheContext, APInt(32, 0)), "whilecond");
            }

            TheFunction->getBasicBlockList().push_back(BodyBB);
            TheFunction->getBasicBlockList().push_back(EndBB);

            Builder.CreateCondBr(CmpV, BodyBB, EndBB);

            Builder.SetInsertPoint(BodyBB);
            ASTNode* body = node->while_loop.body;
            while (body) {
                codegen_stmt(body->stmt_list.stmt);
                body = body->stmt_list.next;
            }

            Builder.CreateBr(CondBB);

            Builder.SetInsertPoint(EndBB);
            break;
        }
        default:
            std::cerr << "Unknown statement type\n";
            break;
    }
}

Value* codegen_expr(ASTNode* node) {
    switch (node->type) {
        case NODE_INT:
            return ConstantInt::get(TheContext, APInt(32, node->int_value));
        case NODE_VAR: {
            std::string var_name = node->var_name_expr;
            Value* var = NamedValues[var_name];
            if (!var) {
                std::cerr << "Unknown variable name: " << var_name << "\n";
                return nullptr;
            }
            return Builder.CreateLoad(Type::getInt32Ty(TheContext), var, var_name.c_str());
        }
        case NODE_BINOP: {
            Value* left = codegen_expr(node->binop.left);
            Value* right = codegen_expr(node->binop.right);

            if (!left || !right)
                return nullptr;

            // make sure both operands are of the same type (i32)
            if (left->getType() != Type::getInt32Ty(TheContext)) {
                left = Builder.CreateIntCast(left, Type::getInt32Ty(TheContext), true, "cast");
            }
            if (right->getType() != Type::getInt32Ty(TheContext)) {
                right = Builder.CreateIntCast(right, Type::getInt32Ty(TheContext), true, "cast");
            }

            switch (node->binop.op) {
                case '+':
                    return Builder.CreateAdd(left, right, "addtmp");
                case '<': {
                    Value* cmp = Builder.CreateICmpSLT(left, right, "cmptmp");
                    // convert i1 to i32 to be consistent
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                case '>': {
                    Value* cmp = Builder.CreateICmpSGT(left, right, "cmptmp");
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                case 'l': { // <=
                    Value* cmp = Builder.CreateICmpSLE(left, right, "cmptmp");
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                case 'g': { // >=
                    Value* cmp = Builder.CreateICmpSGE(left, right, "cmptmp");
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                case 'e': { // ==
                    Value* cmp = Builder.CreateICmpEQ(left, right, "cmptmp");
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                case 'n': { // !=
                    Value* cmp = Builder.CreateICmpNE(left, right, "cmptmp");
                    return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "zexttmp");
                }
                default:
                    std::cerr << "Unknown binary operator\n";
                    return nullptr;
            }
        }
        default:
            std::cerr << "Unknown expression type\n";
            return nullptr;
    }
}

int main() {
    extern int yyparse();
    yyparse();
    generate_code(ast_root);
    return 0;
}
