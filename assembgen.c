#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "assembgen.h"

#define MAX_LINHA 200
#define MAX_CAMPO 50

/* Contadores globais para entradas das tabelas */
int nInst = 0;

comando_t tipoQuad(char* com){
	if(!strcmp(com, "ASSIGN"))
		return ASSIGNq;
	if(!strcmp(com, "ADD"))
		return ADDq;
	if(!strcmp(com, "SUB"))
		return SUBq;
	if(!strcmp(com, "MULT"))
		return MULTq;
	if(!strcmp(com, "DIV"))
		return DIVq;
	if(!strcmp(com, "GTE"))
		return GTEq;
	if(!strcmp(com, "GT"))
		return GTq;
	if(!strcmp(com, "LTE"))
		return LTEq;
	if(!strcmp(com, "LT"))
		return LTq;
	if(!strcmp(com, "EQ"))
		return EQq;
	if(!strcmp(com, "NE"))
		return NEq;
	if(!strcmp(com, "IF"))
		return IFq;
	if(!strcmp(com, "GOTO"))
		return GOTOq;
	if(!strcmp(com, "LABEL"))
		return LABELq;
	if(!strcmp(com, "LOAD"))
		return LOADq;
	if(!strcmp(com, "STORE"))
		return STOREq;
	if(!strcmp(com, "ALLOC"))
		return ALLOCq;
	if(!strcmp(com, "PARAM"))
		return PARAMq;
	if(!strcmp(com, "ARG"))
		return ARGq;
	if(!strcmp(com, "FUN"))
		return FUNq;
	if(!strcmp(com, "CALL"))
		return CALLq;
	if(!strcmp(com, "RETURN"))
		return RETURNq;
	if(!strcmp(com, "END"))
		return ENDq;
	else
		return HLTq;
}

// conta o numero de linhas em um arquivo
int contaLinhas(char* arquivo){
	int lines = 1;
	char ch;
	char* restoLinha;
	FILE* itmc;

	itmc = fopen(arquivo, "r");
	if(itmc == NULL)
		exit(EXIT_FAILURE);

	while(!feof(itmc)){
	  ch = fgetc(itmc);
	  if(ch == '\n')
	    lines++;
	}

	fclose(itmc);

	return lines;
}

// le quadruplas do arquivo de codigo intermediario
void readQuad(quadrupla_t* quad, int nQuad, char* arquivo){
	char linha[MAX_LINHA];
	char* restoLinha, *camp;
	int i, j;
	FILE* itmc;

	itmc = fopen(arquivo, "r");
	if(itmc == NULL)
		exit(EXIT_FAILURE);

	for(i=0; i<nQuad; i++){
		if(fgets(linha, MAX_LINHA, itmc) == NULL)
			return;
		// remove o newline da string lida
		strtok(linha, "\n");

		quad[i].campo = (char**) malloc(4*sizeof(char*));
		for(j=0; j<4; j++)
			quad[i].campo[j] = (char*) malloc(MAX_CAMPO*sizeof(char));

		restoLinha = linha;
		j = 0;
		while((camp = strtok_r(restoLinha, "(), ", &restoLinha))){
			strcpy(quad[i].campo[j], camp);
			j++;
		}
	}

	fclose(itmc);
}

void printQuad(quadrupla_t* quad, int nQuad){
	int i;

	for(i=0; i<nQuad; i++)
		printf("\t(%s, %s, %s, %s)\n", quad[i].campo[0], quad[i].campo[1], quad[i].campo[2], quad[i].campo[3]);
}

/* funcoes para tabela de labels */
void insertLabel(tab_t* tab, char* nome, tipoLabel tipo){
	int size;

	tab->nLab++;

	size = tab->nLab;

	tab->tab_label = (tab_label_t*) realloc(tab->tab_label, size * sizeof(tab_label_t));

	strcpy(tab->tab_label[size-1].nome, nome);
	tab->tab_label[size-1].endereco = nInst;
	tab->tab_label[size-1].tipo = tipo;
}

void insertVar(tab_t* tab, char* nome, char* escopo, int call, int tam){
	int size;

	tab->nVar++;

	size = tab->nVar;

	tab->tab_var = (tab_var_t*) realloc(tab->tab_var, size * sizeof(tab_var_t));
	
	strcpy(tab->tab_var[size-1].nome, nome);
	strcpy(tab->tab_var[size-1].escopo, escopo);
	tab->tab_var[size-1].endereco = tab->endvar;
	tab->tab_var[size-1].tamanho = tam;
	tab->tab_var[size-1].call = call;

	tab->endvar -= tam;
}

void printVar_tab(tab_t* tab){
	int i;

	for(i=0; i < tab->nVar; i++)
		printf("%s\n", tab->tab_var[i].nome);
}

void printLabel_tab(tab_t* tab){
	int i;

	printf("    NOME         endereco      tipo  \n");
	printf("------------  -------------  --------\n");
	for(i=0; i < tab->nLab; i ++){
		printf("%12s ", tab->tab_label[i].nome);
		printf(" %13d ", tab->tab_label[i].endereco);
		switch(tab->tab_label[i].tipo){
			case(label):
				printf(" label\n");
				break;
			case(funcao):
				printf(" funcao\n");
		}
	}
}

void genAsemb(quadrupla_t* quad, int nQuad, tab_t* tab){
	int i, tam, count_op = 0;
	char labelAux[MAXLAB], nOp[MAXLAB];

	for(i=0; i<nQuad; i++){
		switch(tipoQuad(quad[i].campo[0])){
			// (ASSIGN, $t, im, -)
			// atribui imediato no temporario
			case(ASSIGNq):
				// carrega im no acumualdor
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// armazena conteudo do acumulador no temporario
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				break;

			// (ADD, $t3, $t1, $t2)
			// $t3 = $t1 + $t2
			case(ADDq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// soma conteudo do acumulador com $t2
				printf("ADD %s\n", quad[i].campo[3]); nInst++;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				break;

			// (SUB, $t3, $t1, $t2)
			// $t3 = $t1 - $t2
			case(SUBq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai conteudo do acumulador com $t2
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				break;

			// (MULT, $t3, $t1, $t2)
			// $t3 = $t1 * $t2
			case(MULTq):
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// armazena o conteudo do acumulador em $op1
				printf("STA $op1\n"); nInst++;
				// carrega $t2 no acumulador
				printf("LDA %s\n", quad[i].campo[3]); nInst++;
				// armazena o conteudo do acumulador em $op2
				printf("STA $op2\n"); nInst++;
				// label de inicio
				printf("_.start%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.start");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega $op2 no acumulador
				printf("LDA $op2\n"); nInst++;
				// desvio para label de fim se acumulador for zero
				printf("JZ _.end%d\n", count_op); nInst++;
				// AND bit a bit com acumulador e 1
				printf("AND 1\n"); nInst++;
				// subtrai 1 do acumulador
				printf("SUB 1\n"); nInst++;
				// desvio para label de skip se acumulador for negativo
				printf("JN _.skip%d\n", count_op); nInst++;
				// carrega $t3 no acumulador
				printf("LDA %s\n", quad[i].campo[1]); nInst++;
				// soma $op1 ao acumulador
				printf("ADD $op1\n"); nInst++;
				// label de skip
				printf("_.skip%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.skip");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega $op1 no acumulador
				printf("LDA $op1\n"); nInst++;
				// desloca acumulador para esquerda
				printf("SHL\n"); nInst++;
				// armazena acumulador em $op1
				printf("STA $op1\n"); nInst++;
				// carrega $op2 no acumulador
				printf("LDA $op2\n"); nInst++;
				// desloca acumulador para direita
				printf("SHR\n"); nInst++;
				// armazena acumulador em $op2
				printf("STA $op2\n"); nInst++;
				// desvio para o inicio
				printf("J _.start%d\n", count_op); nInst++;
				// label de fim
				printf("_.end%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.end");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				count_op++;
				break;

			// (DIV, $t3, $t1, $t2)
			// $t3 = $t1 / $t2
			case(DIVq):

				break;

			// (GTE, $t3, $t1, $t2)
			// $t3 = $t1 >= $t2
			case(GTEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// desvio para label de falso
				printf("JN _.false%d\n", count_op); nInst++;
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// desvio para label de verdadeiro
				printf("J _.true%d\n", count_op); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (GTE, $t3, $t1, $t2)
			// $t3 = $t1 > $t2
			case(GTq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// subtrai 1 do acumulador
				printf("SUB 1\n"); nInst++;
				// desvio para label de falso
				printf("JN _.false%d\n", count_op); nInst++;
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// desvio para label de verdadeiro
				printf("J _.true%d\n", count_op); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (LTE, $t3, $t1, $t2)
			// $t3 = $t1 <= $t2
			case(LTEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// subtrai 1 do acumulador
				printf("SUB 1\n"); nInst++;
				// desvio para label de verdadeiro
				printf("JN _.true%d\n", count_op); nInst++;
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// desvio para label de falso
				printf("J _.false%d\n", count_op); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (LT, $t3, $t1, $t2)
			// $t3 = $t1 < $t2
			case(LTq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// desvio para label de verdadeiro
				printf("JN _.true%d\n", count_op); nInst++;
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// desvio para label de falso
				printf("J _.false%d\n", count_op); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (EQ, $t3, $t1, $t2)
			// $t3 = $t1 == $t2
			case(EQq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// desvio para label de verdadeiro
				printf("JZ _.true%d\n", count_op); nInst++;
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// desvio para label de falso
				printf("J _.false%d\n", count_op); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (NEQ, $t3, $t1, $t2)
			// $t3 = $t1 != $t2
			case(NEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]); nInst++;
				// desvio para label de falso
				printf("JZ _.false%d\n", count_op); nInst++;
				// carrega 1 no acumulador
				printf("LDA 1\n"); nInst++;
				// desvio para label de verdadeiro
				printf("J _.true%d\n", count_op); nInst++;
				// label de falso
				printf("_.false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n"); nInst++;
				// label de verdadeiro
				printf("_.true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, "_.true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				count_op++;
				break;

			// (IF, $t1, label, -)
			// if($t1) goto label
			case(IFq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[1]); nInst++;
				// subtrai 1 do acumulador
				printf("SUB 1\n"); nInst++;
				// desvio (se $t1 for 1, $t1-1=0 e o desvio eh feito)
				printf("JZ %s\n", quad[i].campo[2]); nInst++;
				break;

			// (GOTO, label, -, -)
			// goto label
			case(GOTOq):
				// desvio para label
				printf("J %s\n", quad[i].campo[1]); nInst++;
				break;

			// (LABEL, label, -, -)
			// label
			case(LABELq):
				// label
				printf("_%s\n", quad[i].campo[1]);
				insertLabel(tab, quad[i].campo[1], label);
				break;

			// (LOAD, $t1, var, -) ou (LOAD, $t1, var, pos)
			// $t1 = var ou $t1 = var[pos]
			case(LOADq):
				if(!strcmp(quad[i].campo[3], "-")) // simples
					// carrega var no acumulador
					printf("LDA %s\n", quad[i].campo[2]);
				else // vetor
					// carrga var[pos] no acumulador
					printf("LDA %s[%s]\n", quad[i].campo[2], quad[i].campo[3]);
				nInst++;
				// armazena acumulador em $t1
				printf("STA %s\n", quad[i].campo[1]); nInst++;
				break;

			// (STORE, var, $t1, -) ou (STORE, var, %t1, pos)
			// var = $t1 ou var[pos] = $t1
			case(STOREq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]); nInst++;
				if(!strcmp(quad[i].campo[3], "-")) // simples
					// armazena acumulador em var
					printf("STA %s\n", quad[i].campo[1]);
				else // vetor
					// armazena acumulador em var[pos]
					printf("STA %s[%s]\n", quad[i].campo[1], quad[i].campo[3]);
				nInst++;
				break;

			// (ALLOC, var, escopo, tam)
			// cria a variavel var no seu escopo com tamanho tam
			case(ALLOCq):
				// adiciona a variavel na tabela
				tam = strtoumax(quad[i].campo[3], NULL, 10);
				if(!strcmp(quad[i].campo[2], "main"))
					insertVar(tab, quad[i].campo[1], quad[i].campo[2], 0, tam);
				break;

			default:
				break;
		}
	}
}

tab_t* assembgen(void){
	quadrupla_t* quad;
	int i, j, nQuad;
	tab_t* tab;

	// tabela
	tab = (tab_t*) malloc(sizeof(tab_t));
	tab->nLab = 0;
	tab->nVar = 0;
	tab->tab_var = NULL;
	tab->tab_label = NULL;
	tab->endvar = VARstart;

	// estou usando itmCodeTeste para testar as quadruplas
	// voltar para itmCode na versao final

	// pega o numero de quadruplas
	nQuad = contaLinhas("itmCodeTeste");
	nQuad--;

	// aloca um vetor de quaruplas no tamanho certo
	quad = (quadrupla_t*) malloc(nQuad*sizeof(quadrupla_t));

	// le as quadruplas do codigo intermediario
	readQuad(quad, nQuad, "itmCodeTeste");

	printf("Quadruplas:\n");
	printQuad(quad, nQuad);
	printf("\n");

	printf("Codigo Assembly:\n");
	genAsemb(quad, nQuad, tab);

	printf("\nTabela de labels:\n");
	printLabel_tab(tab);

	printf("\nTabela de variaveis:\n");
	printVar_tab(tab);

	for(i=0; i<nQuad; i++){
		for(j=0; j<4; j++)
			free(quad[i].campo[j]);
		free(quad[i].campo);
	}
	free(quad);

	return tab;
}