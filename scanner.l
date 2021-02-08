%{
#include "token.h"
%}

LETRA	[a-zA-Z]
DIGITO	[0-9]

%%

"else"		{printf("Palavra-chave: else\n");}
"if"		{printf("Palavra-chave: if\n");}
"int"		{printf("Palavra-chave: int\n");}
"return"	{printf("palavra-chave: return\n");}
"void"		{printf("palavra-chave: void\n");}
"while"		{printf("palavra-chave: while\n");}

"+"		{printf("Simbolo especial: +\n");}
"-"		{printf("Simbolo especial: -\n");}
"*"		{printf("Simbolo especial: *\n");}
"/"		{printf("Simbolo especial: /\n");}
"<"		{printf("Simbolo especial: <\n");}
"<="		{printf("Simbolo especial: <=\n");}
">"		{printf("Simbolo especial: >\n");}
">="		{printf("Simbolo especial: >=\n");}
"=="		{printf("Simbolo especial: ==\n");}
"!="		{printf("Simbolo especial: !=\n");}
"="		{printf("Simbolo especial: =\n");}
";"		{printf("Simbolo especial: ;\n");}
","		{printf("Simbolo especial: ,\n");}
"("		{printf("Simbolo especial: (\n");}
")"		{printf("Simbolo especial: )\n");}
"["		{printf("Simbolo especial: [\n");}
"]"		{printf("Simbolo especial: ]\n");}
"{"		{printf("Simbolo especial: {\n");}
"}"		{printf("Simbolo especial: }\n");}
"/*"		{printf("Simbolo especial: /*\n");}
"*/"		{printf("Simbolo especial: */\n");}
{LETRA}+	{printf("ID: %s\n", yytext);}
{DIGITO}+	{printf("NUM: %s\n", yytext);}
%%

int main(int argc, char *argv[]){
	FILE *f_in;

	if (argc == 2){
		if(f_in = fopen(argv[1], "r")) yyin = f_in;
		else perror(argv[0]);
	}
	else yyin = stdin;

	yylex();
	return 0;
}
