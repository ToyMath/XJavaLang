%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

void yyerror(const char *s);
int yylex(void);

ASTNode* ast_root;
%}

%union {
    int intval;
    char* strval;
    ASTNode* node;
}

%code requires {
    typedef struct ASTNode ASTNode;
    typedef struct ASTNodeList ASTNodeList;
}


%token <strval> IDENTIFIER
%token <intval> INTEGER

%token PUBLIC CLASS STATIC VOID MAIN STRING INT
%token SYSTEM OUT PRINTLN WHILE

%token LPAREN RPAREN LBRACE RBRACE SEMICOLON ASSIGN PLUS COMMA DOT LBRACKET RBRACKET
%token LT GT LE GE EQ NE

%type <node> program class_decl class_body method_decl stmt_list stmt expr

%left LT GT LE GE EQ NE
%left PLUS

%start program

%%

program:
    class_decl
    {
        ast_root = create_program_node($1);
    }
    ;

class_decl:
    PUBLIC CLASS IDENTIFIER LBRACE class_body RBRACE
    {
        $$ = create_class_node($3, $5);
    }
    ;

class_body:
    method_decl
    {
        $$ = $1;
    }
    ;

method_decl:
    PUBLIC STATIC VOID MAIN LPAREN STRING LBRACKET RBRACKET IDENTIFIER RPAREN LBRACE stmt_list RBRACE
    {
        $$ = create_method_node("main", $12);
    }
    ;

stmt_list:
    /* empty */
    {
        $$ = NULL;
    }
    | stmt_list stmt
    {
        $$ = append_statement($1, $2);
    }
    ;

stmt:
    INT IDENTIFIER SEMICOLON
    {
        $$ = create_var_decl_node($2);
    }
    | IDENTIFIER ASSIGN expr SEMICOLON
    {
        $$ = create_assign_node($1, $3);
    }
    | SYSTEM DOT OUT DOT PRINTLN LPAREN IDENTIFIER RPAREN SEMICOLON
    {
        $$ = create_print_node($7);
    }
    | WHILE LPAREN expr RPAREN LBRACE stmt_list RBRACE
    {
        $$ = create_while_node($3, $6);
    }
    ;

expr:
    INTEGER
    {
        $$ = create_int_node($1);
    }
    | IDENTIFIER
    {
        $$ = create_var_node($1);
    }
    | expr PLUS expr
    {
        $$ = create_binop_node('+', $1, $3);
    }
    | expr LT expr
    {
        $$ = create_binop_node('<', $1, $3);
    }
    | expr GT expr
    {
        $$ = create_binop_node('>', $1, $3);
    }
    | expr LE expr
    {
        $$ = create_binop_node('l', $1, $3);  // 'l' for less than or equal
    }
    | expr GE expr
    {
        $$ = create_binop_node('g', $1, $3);  // 'g' for greater than or equal
    }
    | expr EQ expr
    {
        $$ = create_binop_node('e', $1, $3);  // 'e' for equals
    }
    | expr NE expr
    {
        $$ = create_binop_node('n', $1, $3);  // 'n' for not equals
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
