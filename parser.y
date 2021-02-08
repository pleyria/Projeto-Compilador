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
%token MAIS MENOS ASTERISCO BARRA
%token MENOR MENORIGUAL MAIOR MAIORIGUAL IGUALIGUAL DIFERENTE IGUAL
%token PONTOVIRGULA VIRGULA
%token ABREPAR FECHAPAR ABRECOL FECHACOL ABRECHA FECHACHA ABRECOM FECHACOM
%token ID NUM

%start programa

%%
/*
programa :
	declaracao_lista;

declaracao_lista : 
	declaracao_lista declaracao | 
	declaracao;

declaracao : 
	var_declaracao | 
	fun_declaracao;

var_declaracao : 
	tipo_especificador ID PONTOVIRGULA |
	tipo_especificador ID ABRECOL NUM FECHACOL PONTOVIRGULA;

tipo_especificador : 
	INT |
	VOID;

fun_declaracao : 
	tipo_especificador ID ABREPAR params FECHAPAR composto_decl;

params : 
	param_lista |
	VOID;

param_lista : 
	param_lista VIRGULA param | 
	param;

param :
	tipo_especificador ID | 
	tipo_especificador ID ABRECOL FECHACOL;

composto_decl : 
	ABRECHA local_declaracoes statement_lista FECHACHA;

local_declaracoes : 
	local_declaracoes var_declaracao | ;

statement_lista :
	statement_lista statement | ;

statement : 
	expressao_decl | 
	composto_decl |
	selecao_decl |
	iteracao_decl |
	retorno_decl;

expressao_decl :
	expressao PONTOVIRGULA | 
	PONTOVIRGULA;

selecao_decl : 
	IF ABREPAR expressao FECHAPAR statement | 
	IF ABREPAR expressao FECHAPAR statement ELSE statement;

iteracao_decl : 
	WHILE ABREPAR expressao FECHAPAR statement;

retorno_decl : 
	RETURN PONTOVIRGULA | 
	RETURN expressao;

expressao : 
	var IGUAL expressao |
	simples_expressao;

var : 
	ID |
	ID ABRECOL expressao ABRECOL;

simples_expressao:
	soma_expressao relacional soma_expressao | 
	soma_expressao;

relacional :
	MENORIGUAL |
	MENOR |
	MAIOR |
	MAIORIGUAL |
	IGUALIGUAL |
	DIFERENTE;

soma_expressao : 
	soma_expressao soma termo |
	termo;

soma : 
	MAIS |
	MENOS;

termo : 
	termo mult fator |
	fator;

mult : 
	ASTERISCO |
	BARRA;

fator : 
	ABREPAR expressao FECHAPAR |
	var |
	ativacao |
	NUM;

ativacao :
	ID ABREPAR args FECHAPAR;

args :
	arg_lista | ;

arg_lista : 
	arg_lista VIRGULA expressao |
	expressao;
*/
programa : 
	VOID INT ID ABREPAR VOID FECHAPAR PONTOVIRGULA;

%%

void yyerror(){
	fprintf(stderr, "Erro na linha %d\n", lineno);
}

int main(int arcg, char* argv[]) {
	int flag;
	yyin = fopen(argv[1], "r");
	yyparse();
	fclose(yyin);

	return 0;
}