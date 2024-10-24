#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_CLASS,
    NODE_METHOD,
    NODE_STMT_LIST,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_INT,
    NODE_VAR,
    NODE_BINOP,
    NODE_WHILE
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        struct {
            struct ASTNode* class_node;
        } program;

        struct {
            char* name;
            struct ASTNode* body;
        } class_decl;

        struct {
            char* name;
            struct ASTNode* body;
        } method_decl;

        struct {
            struct ASTNode* stmt;
            struct ASTNode* next;
        } stmt_list;

        char* var_name;

        struct {
            char* var_name;
            struct ASTNode* expr;
        } assign;

        char* print_var_name;

        int int_value;

        char* var_name_expr;

        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;

        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_loop;
    };
} ASTNode;

ASTNode* create_program_node(ASTNode* class_node);
ASTNode* create_class_node(char* name, ASTNode* body);
ASTNode* create_method_node(char* name, ASTNode* body);
ASTNode* append_statement(ASTNode* list, ASTNode* stmt);
ASTNode* create_var_decl_node(char* name);
ASTNode* create_assign_node(char* name, ASTNode* expr);
ASTNode* create_print_node(char* name);
ASTNode* create_int_node(int value);
ASTNode* create_var_node(char* name);
ASTNode* create_binop_node(char op, ASTNode* left, ASTNode* right);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);

#endif
