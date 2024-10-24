#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* create_program_node(ASTNode* class_node) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->program.class_node = class_node;
    return node;
}

ASTNode* create_class_node(char* name, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_CLASS;
    node->class_decl.name = strdup(name);
    node->class_decl.body = body;
    return node;
}

ASTNode* create_method_node(char* name, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_METHOD;
    node->method_decl.name = strdup(name);
    node->method_decl.body = body;
    return node;
}

ASTNode* append_statement(ASTNode* list, ASTNode* stmt) {
    if (!list) {
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = NODE_STMT_LIST;
        node->stmt_list.stmt = stmt;
        node->stmt_list.next = NULL;
        return node;
    } else {
        ASTNode* current = list;
        while (current->stmt_list.next) {
            current = current->stmt_list.next;
        }
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = NODE_STMT_LIST;
        node->stmt_list.stmt = stmt;
        node->stmt_list.next = NULL;
        current->stmt_list.next = node;
        return list;
    }
}

ASTNode* create_var_decl_node(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VAR_DECL;
    node->var_name = strdup(name);
    return node;
}

ASTNode* create_assign_node(char* name, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGN;
    node->assign.var_name = strdup(name);
    node->assign.expr = expr;
    return node;
}

ASTNode* create_print_node(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->print_var_name = strdup(name);
    return node;
}

ASTNode* create_int_node(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_INT;
    node->int_value = value;
    return node;
}

ASTNode* create_var_node(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VAR;
    node->var_name_expr = strdup(name);
    return node;
}

ASTNode* create_binop_node(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->binop.op = op;
    node->binop.left = left;
    node->binop.right = right;
    return node;
}

ASTNode* create_while_node(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->while_loop.condition = condition;
    node->while_loop.body = body;
    return node;
}
