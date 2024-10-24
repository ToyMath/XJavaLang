%{
#include "parser.tab.h"
%}

%%

"public"           { return PUBLIC; }
"class"            { return CLASS; }
"static"           { return STATIC; }
"void"             { return VOID; }
"main"             { return MAIN; }
"String"           { return STRING; }
"int"              { return INT; }
"System"           { return SYSTEM; }
"out"              { return OUT; }
"println"          { return PRINTLN; }
"while"            { return WHILE; }

[0-9]+             { yylval.intval = atoi(yytext); return INTEGER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.strval = strdup(yytext); return IDENTIFIER; }

"("                { return LPAREN; }
")"                { return RPAREN; }
"{"                { return LBRACE; }
"}"                { return RBRACE; }
";"                { return SEMICOLON; }
"="                { return ASSIGN; }
"+"                { return PLUS; }
","                { return COMMA; }
"."              { return DOT; }
"["              { return LBRACKET; }
"]"              { return RBRACKET; }
"<"                { return LT; }
">"                { return GT; }
"<="               { return LE; }
">="               { return GE; }
"=="               { return EQ; }
"!="               { return NE; }



[ \t\n]+           { /* skip whitespace */ }
.                  { printf("Unknown character: %s\n", yytext); }

%%

int yywrap() { return 1; }
