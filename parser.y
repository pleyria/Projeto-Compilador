%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE* yyin;

extern int lineno;
extern int yylex();

void yyerror();
	
%}

%token ELSE IF INT RETURN VOID WHILE OUTRO

%start programa

%%

programa : 
	ELSE | IF | INT | RETURN | VOID | WHILE |
	OUTRO {yyerror();}


%%

void yyerror(){
	fprintf(stderr, "Erro na linha %d\n", lineno);
}

int main(int arcg, char* argv[]) {
	int flag;
	yyin = fopen(argv[1], "r");
	flag = yyparse();
	fclose(yyin);

	return flag;
}