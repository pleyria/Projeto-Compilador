/* Codgio Flex para o analisador lexico */

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

extern FILE* yyin;
extern FILE* yyout;

int lineno = 1;

void print_token(char* token_type);
//void yyerror();

%}

digit       [0-9]
letter      [a-zA-Z]
newline     \n
whitespace  [ \t]+

%%

"else"			{print_token("palavra-chave else"); return ELSE;}
"if"			{print_token("palavra-chave if"); return IF;}
"int"			{print_token("palavra-chave int"); return INT;}
"return"		{print_token("palavra-chave return"); return RETURN;}
"void"			{print_token("palavra-chave void"); return VOID;}
"while"			{print_token("palavra-chave while"); return WHILE;}

{newline}		{lineno ++;}
{whitespace}    {}

.				{return OUTRO;}

%%

void print_token(char* token_type){
	printf("token: %s\n valor: %s\n linha: %d\n", token_type, yytext, lineno);
}
