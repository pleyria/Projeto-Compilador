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

digito			[0-9]
numero			{digito}+
letra			[a-zA-Z]
identificador	{letra}+
newline     	\n
whitespace		[ \t]+

%%

"else"			{print_token("palavra-chave else"); return ELSE;}
"if"			{print_token("palavra-chave if"); return IF;}
"int"			{print_token("palavra-chave int"); return INT;}
"return"		{print_token("palavra-chave return"); return RETURN;}
"void"			{print_token("palavra-chave void"); return VOID;}
"while"			{print_token("palavra-chave while"); return WHILE;}

"+"				{print_token("simbolo especial +"); return MAIS;}
"-"				{print_token("simbolo especial -"); return MENOS;}
"*"				{print_token("simbolo especial *"); return ASTERISCO;}
"/"				{print_token("simbolo especial /"); return BARRA;}
"<"				{print_token("simbolo especial <"); return MENOR;}
"<="			{print_token("simbolo especial <="); return MENORIGUAL;}
">"				{print_token("simbolo especial >"); return MAIOR;}
">="			{print_token("simbolo especial >="); return MAIORIGUAL;}
"=="			{print_token("simbolo especial =="); return IGUALIGUAL;}
"!="			{print_token("simbolo especial !="); return DIFERENTE;}
"="				{print_token("simbolo especial ="); return IGUAL;}
";"				{print_token("simbolo especial ;"); return PONTOVIRGULA;}
","				{print_token("simbolo especial ,"); return VIRGULA;}
"("				{print_token("simbolo especial ("); return ABREPAR;}
")"				{print_token("simbolo especial )"); return FECHAPAR;}
"["				{print_token("simbolo especial ["); return ABRECOL;}
"]"				{print_token("simbolo especial ]"); return FECHACOL;}
"{"				{print_token("simbolo especial {"); return ABRECHA;}
"}"				{print_token("simbolo especial }"); return FECHACHA;}
"/*"			{print_token("simbolo especial /*"); return ABRECOM;}
"*/"			{print_token("simbolo especial */"); return FECHACOM;}

{numero}		{print_token("numero"); return NUM;}
{identificador}	{print_token("identificador"); return ID;}

{newline}		{lineno ++;}
{whitespace}    {}



%%

void print_token(char* token_type){
	printf("token: %s\n valor: %s\n linha: %d\n", token_type, yytext, lineno);
}
