#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "assembgen.h"

/* arquvio para escrita do codigo intermediario */
static FILE* assemb;

/* Contadores globais para entradas das tabelas */
int nInst = 0;
int endvar = VARstart;
int ptr = 0;
int call;

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

void insertVar(tab_t* tab, char* nome, char* escopo, int tam, int end){
	int size;

	tab->nVar++;

	size = tab->nVar;

	tab->tab_var = (tab_var_t*) realloc(tab->tab_var, size * sizeof(tab_var_t));
	
	strcpy(tab->tab_var[size-1].nome, nome);
	strcpy(tab->tab_var[size-1].escopo, escopo);
	tab->tab_var[size-1].endereco = end;
	tab->tab_var[size-1].tamanho = tam;
}

int ehPonteiro(tab_t* tab, char* nome, char* escopo){
	int i;

	for(i=0; i < tab->nVar; i++){
		if(strcmp(nome, tab->tab_var[i].nome) == 0 && strcmp(escopo, tab->tab_var[i].escopo) == 0){
			if(tab->tab_var[i].endereco >= PTR0 && tab->tab_var[i].endereco <= PTR4)
				return 1;
			else
				return 0;
		}
	}

	return 0;
}

int procuraVar(tab_t* tab, char* nome, char* escopo){
	int i;

	for(i=0; i < tab->nVar; i++)
		if(strcmp(nome, tab->tab_var[i].nome) == 0 && strcmp(escopo, tab->tab_var[i].escopo) == 0)
			return 1;

	return 0;
}

void printVar_tab(tab_t* tab){
	int i;

	printf("  NOME      endereco      escopo      tamanho  \n");
	printf("--------  ------------  ----------  -----------\n");
	for(i=0; i < tab->nVar; i++){
		printf("%8s ", tab->tab_var[i].nome);
		printf(" %12d ", tab->tab_var[i].endereco);
		printf(" %10s ", tab->tab_var[i].escopo);
		printf(" %11d\n", tab->tab_var[i].tamanho);
	}
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

int buscaTemp(int tempv[], int t){
	int i;

	i = 0;

	while(tempv[i] != -1){
		if(tempv[i] == t)
			return 1;
		i++;
	}

	return 0;
}

/* Salva as variaveis dentro do escopo de uma funcao
 * antes de executar uma chamada recursiva */
void salvaEstado(tab_t* tab, quadrupla_t* quad, int nQuad, char* funcao, int tempv[]){
	int i, j, k, t;
	char temp[4];
	char escopoAtual[MAXLAB];

	// salva variaveis
	for(i=0; i < tab->nVar; i++){
		if(!strcmp(funcao, tab->tab_var[i].escopo) && !ehPonteiro(tab, tab->tab_var[i].nome, funcao)){
			// carrega variavel no acumulador
			printf("LDA %s\n", tab->tab_var[i].nome);
			fprintf(assemb, "LDA %s\n", tab->tab_var[i].nome); nInst += 2;
			// armazena acumulador na pilha
			printf("STA [$stck]\n");
			fprintf(assemb, "STA [$stck]\n"); nInst += 2; // enderecamento indireto
			// carrega o ponteiro de pilha no acumulador
			printf("LDA $stck\n");
			fprintf(assemb, "LDA $stck\n"); nInst += 2;
			// soma 1 no acumulador
			printf("ADD 1\n");
			fprintf(assemb, "ADD 1\n"); nInst += 2;
			// armazena acumulador no ponteiro de pilha
			printf("STA $stck\n");
			fprintf(assemb, "STA $stck\n"); nInst += 2;
		}
	}

	// salva temporarios
	k = 0;
	strcpy(escopoAtual, "");
	// percorre todas as quadruplas para preencher tempv
	for(i=0; i < nQuad; i++){
		// marca o escopo atual
		if(tipoQuad(quad[i].campo[0]) == FUNq)
			strcpy(escopoAtual, quad[i].campo[2]);
		// se o escopo for o da funcao passada
		if(!strcmp(escopoAtual, funcao)){
			// percorre os campos da quadrupla
			for(j=1; j<=3; j++){
				// se for um temporario
				if(quad[i].campo[j][0] == '$'){
					// obtem numero do temporario
					strcpy(temp, "");
					strcat(temp, quad[i].campo[j] + 2*sizeof(char));
					t = atoi(temp);
					// adiciona o temporario em tempv, se ele ja nao estiver la
					if(!buscaTemp(tempv, t)){
						tempv[k++] = t;
						// marca o fim de tempv com -1
						tempv[k] = -1;
					}
				}
			}
		}
	}
	// empilha os temporarios de tempv
	i=0;
	while(tempv[i] != -1){
		// carrega temporario no acumulador
		printf("LDA $t%d\n", tempv[i]);
		fprintf(assemb, "LDA $t%d\n", tempv[i]); nInst += 2;
		// armazena acumulador na pilha
		printf("STA [$stck]\n");
		fprintf(assemb, "STA [$stck]\n"); nInst += 2; // enderecamento indireto
		// carrega o ponteiro de pilha no acumulador
		printf("LDA $stck\n");
		fprintf(assemb, "LDA $stck\n"); nInst += 2;
		// soma 1 no acumulador
		printf("ADD 1\n");
		fprintf(assemb, "ADD 1\n"); nInst += 2;
		// armazena acumulador no ponteiro de pilha
		printf("STA $stck\n");
		fprintf(assemb, "STA $stck\n"); nInst += 2;
		i++;
	}

}

/* Carrega um estado salvo de uma funcao */
void carregaEstado(tab_t* tab, char* funcao, int tempv[]){
	int i;

	// carrega temporarios
	// coloca i na posicao do ultimo temporario de tempv
	i = 0;
	while(tempv[i] != -1)
		i++;
	i--;

	// percorre os temporarios em ordem invera para desempilhar
	while(i >= 0){
		// carrega ponteiro de pilha no acumulador
		printf("LDA $stck\n");
		fprintf(assemb, "LDA $stck\n"); nInst += 2;
		// subtrai um do acumulador
		printf("SUB 1\n");
		fprintf(assemb, "SUB 1\n"); nInst += 2;
		// carrega acumulador no ponteiro de pilha
		printf("STA $stck\n");
		fprintf(assemb, "STA $stck\n"); nInst += 2;
		// carrega o topo da pilha no acumulador
		printf("LDA [$stck]\n");
		fprintf(assemb, "LDA [$stck]\n"); nInst += 2; // enderecamento indireto
		// armazena o acumulador no temporario
		printf("STA $t%d\n", tempv[i]);
		fprintf(assemb, "STA $t%d\n", tempv[i]); nInst += 2;

		i--;
	}

	// carrega variaveis
	for(i = tab->nVar-1; i >= 0; i--){
		if(!strcmp(funcao, tab->tab_var[i].escopo) && !ehPonteiro(tab, tab->tab_var[i].nome, funcao)){
			// carrega ponteiro de pilha no acumulador
			printf("LDA $stck\n");
			fprintf(assemb, "LDA $stck\n"); nInst += 2;
			// subtrai um do acumulador
			printf("SUB 1\n");
			fprintf(assemb, "SUB 1\n"); nInst += 2;
			// carrega acumulador no ponteiro de pilha
			printf("STA $stck\n");
			fprintf(assemb, "STA $stck\n"); nInst += 2;
			// carrega o topo da pilha no acumulador
			printf("LDA [$stck]\n");
			fprintf(assemb, "LDA [$stck]\n"); nInst += 2; // enderecamento indireto
			// armazena o acumulador na variavel
			printf("STA %s\n", tab->tab_var[i].nome);
			fprintf(assemb, "STA %s\n", tab->tab_var[i].nome); nInst += 2;
		}
	}
}

void genAsemb(quadrupla_t* quad, int nQuad, tab_t* tab){
	int i, j, tam, count_op = 0;
	int startParam = 0, call = 0;
	int tempv[MAXTEMP];
	char labelAux[MAXLAB], nOp[MAXLAB], funcat[MAXLAB];
	char escopoAtual[MAXLAB];

	tempv[0] = -1;

	for(i=0; i<nQuad; i++){
		switch(tipoQuad(quad[i].campo[0])){
			// (ASSIGN, $t, im, -)
			// atribui imediato no temporario
			case(ASSIGNq):
				// carrega im no acumualdor
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// armazena conteudo do acumulador no temporario
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				startParam = 0;
				break;

			// (ADD, $t3, $t1, $t2)
			// $t3 = $t1 + $t2
			case(ADDq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// soma conteudo do acumulador com $t2
				printf("ADD %s\n", quad[i].campo[3]);
				fprintf(assemb, "ADD %s\n", quad[i].campo[3]); nInst += 2;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				startParam = 0;
				break;

			// (SUB, $t3, $t1, $t2)
			// $t3 = $t1 - $t2
			case(SUBq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai conteudo do acumulador com $t2
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				startParam = 0;
				break;

			// (MULT, $t3, $t1, $t2)
			// $t3 = $t1 * $t2
			case(MULTq):
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// armazena conteudo do acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// armazena o conteudo do acumulador em $op1
				printf("STA $op1\n");
				fprintf(assemb, "STA $op1\n"); nInst += 2;
				// carrega $t2 no acumulador
				printf("LDA %s\n", quad[i].campo[3]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[3]); nInst += 2;
				// armazena o conteudo do acumulador em $op2
				printf("STA $op2\n");
				fprintf(assemb, "STA $op2\n"); nInst += 2;
				// label de inicio
				printf(".start%d\n", count_op);
				fprintf(assemb, ".start%d\n", count_op); nInst += 2;
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".start");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega $op2 no acumulador
				printf("LDA $op2\n"); 
				fprintf(assemb, "LDA $op2\n"); nInst += 2;
				// desvio para label de fim se acumulador for zero
				printf("JZ .end%d\n", count_op);
				fprintf(assemb, "JZ .end%d\n", count_op); nInst += 2;
				// AND bit a bit com acumulador e 1
				printf("AND 1\n");
				fprintf(assemb, "AND 1\n"); nInst += 2;
				// subtrai 1 do acumulador
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// desvio para label de skip se acumulador for negativo
				printf("JN .skip%d\n", count_op);
				fprintf(assemb, "JN .skip%d\n", count_op); nInst += 2;
				// carrega $t3 no acumulador
				printf("LDA %s\n", quad[i].campo[1]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				// soma $op1 ao acumulador
				printf("ADD $op1\n");
				fprintf(assemb, "ADD $op1\n"); nInst += 2;
				// label de skip
				printf(".skip%d\n", count_op);
				fprintf(assemb, ".skip%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".skip");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega $op1 no acumulador
				printf("LDA $op1\n");
				fprintf(assemb, "LDA $op1\n"); nInst += 2;
				// desloca acumulador para esquerda
				printf("SHL\n");
				fprintf(assemb, "SHL\n"); nInst ++;
				// armazena acumulador em $op1
				printf("STA $op1\n");
				fprintf(assemb, "STA $op1\n"); nInst += 2;
				// carrega $op2 no acumulador
				printf("LDA $op2\n");
				fprintf(assemb, "LDA $op2\n"); nInst += 2;
				// desloca acumulador para direita
				printf("SHR\n");
				fprintf(assemb, "SHR\n"); nInst ++;
				// armazena acumulador em $op2
				printf("STA $op2\n");
				fprintf(assemb, "STA $op2\n"); nInst += 2;
				// desvio para o inicio
				printf("J .start%d\n", count_op);
				fprintf(assemb, "J .start%d\n", count_op); nInst += 2;
				// label de fim
				printf(".end%d\n", count_op);
				fprintf(assemb, ".end%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".end");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				count_op++;
				startParam = 0;
				break;

			// (DIV, $t3, $t1, $t2)
			// $t3 = $t1 / $t2
			case(DIVq):
				/* inicializacao do quociente */
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// armazena em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;

				/* obtencao do sinal de $t1 em $op1 (0 = +, 1 = -) */
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// desvio se for negativo
				printf("JN .1stnegative%d\n", count_op);
				fprintf(assemb, "JN .1stnegative%d\n", count_op); nInst += 2;
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// desvio para label positivo
				printf("J .1stpositive%d\n", count_op);
				fprintf(assemb, "J .1stpositive%d\n", count_op); nInst += 2;
				// label de negativo
				printf(".1stnegative%d\n", count_op);
				fprintf(assemb, ".1stnegative%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".1stnegative");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				/* pega o modulo de $t1 */
				// inverte os bits de $t1
				printf("NOT\n");
				fprintf(assemb, "NOT\n"); nInst++;
				// adiciona 1
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena em $t1
				printf("STA %s\n", quad[i].campo[2]);
				fprintf(assemb, "STA %s\n", quad[i].campo[2]); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// label de positivo
				printf(".1stpositive%d\n", count_op);
				fprintf(assemb, ".1stpositive%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".1stpositive");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena em $op1
				printf("STA $op1\n");
				fprintf(assemb, "STA $op1\n"); nInst += 2;


				/* obtencao do sinal de $t2 em $op2 (0 = +, 1 = -) */
				// carrega $t2 no acumulador
				printf("LDA %s\n", quad[i].campo[3]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[3]); nInst += 2;
				// desvio se for negativo
				printf("JN .2ndnegative%d\n", count_op);
				fprintf(assemb, "JN .2ndnegative%d\n", count_op); nInst += 2;
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// desvio para label positivo
				printf("J .2ndpositive%d\n", count_op);
				fprintf(assemb, "J .2ndpositive%d\n", count_op); nInst += 2;
				// label de negativo
				printf(".2ndnegative%d\n", count_op);
				fprintf(assemb, ".2ndnegative%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".2ndnegative");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				/* pega o modulo de $t2 */
				// inverte os bits de $t1
				printf("NOT\n");
				fprintf(assemb, "NOT\n"); nInst++;
				// adiciona 1
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena em $t2
				printf("STA %s\n", quad[i].campo[3]);
				fprintf(assemb, "STA %s\n", quad[i].campo[3]); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// label de positivo
				printf(".2ndpositive%d\n", count_op);
				fprintf(assemb, ".2ndpositive%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".2ndpositive");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena em $op1
				printf("STA $op2\n");
				fprintf(assemb, "STA $op2\n"); nInst += 2;

				/* verifica se os sinais de $t1 e $t2 sao iguais e armazena em $op1 (0 =, 1 !=)*/
				// $op2 ja esta no acumulador
				// subtrai $op1
				printf("SUB $op1\n");
				fprintf(assemb, "SUB $op1\n"); nInst += 2;
				// desvio se for zero
				printf("JZ .igual%d\n", count_op);
				fprintf(assemb, "JZ .igual%d\n", count_op); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// desvio para label de diferente
				printf("J .dif%d\n", count_op);
				fprintf(assemb, "J .dif%d\n", count_op); nInst += 2;
				// label de igual
				printf(".igual%d\n", count_op);
				fprintf(assemb, ".igual%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".igual");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// label de diferente
				printf(".dif%d\n", count_op);
				fprintf(assemb, ".dif%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".dif");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena em $op1
				printf("STA $op2\n");
				fprintf(assemb, "STA $op2\n"); nInst += 2;

				/* realiza o algoritmo de divisao com os valores absolutos de $t1 e $t2 */
				// label de inicio
				printf(".start%d\n", count_op);
				fprintf(assemb, ".start%d\n", count_op); nInst += 2;
				sprintf(nOp, "%d", count_op);
				strcpy(labelAux, ".start");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega $t1
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// desvio para label de fim se for negativo
				printf("JN .end\n");
				fprintf(assemb, "JN .end\n"); nInst += 2;
				// armazena em $t1
				printf("STA %s\n", quad[i].campo[2]);
				fprintf(assemb, "STA %s\n", quad[i].campo[2]); nInst += 2;
				// carrega $t3 no acumulador
				printf("LDA %s\n", quad[i].campo[1]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				// adiciona 1
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				// desvio para label de inicio
				printf("J .start%d\n", count_op);
				fprintf(assemb, "J .start%d\n", count_op); nInst += 2;
				// label de fim
				printf(".end%d\n", count_op);
				fprintf(assemb, ".end%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".end");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);

				/* inverte o sinal do quociente se $t1 e $t2 tiverem sinais opostos */
				// carrega $op1 no acumulador
				printf("LDA $op1\n");
				fprintf(assemb, "LDA $op1\n"); nInst += 2;
				// desvio para label de skip se for zero (sinais iguais)
				printf("JZ .skip%d\n", count_op);
				fprintf(assemb, "J .skip%d\n", count_op); nInst += 2;
				// carrega $t3
				printf("LDA %s\n", quad[i].campo[1]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				// inverte os bits de $t3
				printf("NOT\n");
				fprintf(assemb, "NOT\n"); nInst++;
				// adiciona 1
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;

				// label de skip
				printf(".skip%d\n", count_op);
				fprintf(assemb, ".skip%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".skip");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				count_op++;
				startParam = 0;
				break;

			// (GTE, $t3, $t1, $t2)
			// $t3 = $t1 >= $t2
			case(GTEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// desvio para label de falso
				printf("JN .false%d\n", count_op);
				fprintf(assemb, "JN .false%d\n", count_op); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// desvio para label de verdadeiro
				printf("J .true%d\n", count_op);
				fprintf(assemb, "J .true%d\n", count_op); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (GTE, $t3, $t1, $t2)
			// $t3 = $t1 > $t2
			case(GTq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// subtrai 1 do acumulador
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// desvio para label de falso
				printf("JN .false%d\n", count_op);
				fprintf(assemb, "JN .false%d\n", count_op); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// desvio para label de verdadeiro
				printf("J .true%d\n", count_op);
				fprintf(assemb, "J .true%d\n", count_op); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (LTE, $t3, $t1, $t2)
			// $t3 = $t1 <= $t2
			case(LTEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// subtrai 1 do acumulador
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// desvio para label de verdadeiro
				printf("JN .true%d\n", count_op);
				fprintf(assemb, "JN .true%d\n", count_op); nInst += 2;
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// desvio para label de falso
				printf("J .false%d\n", count_op);
				fprintf(assemb, "J .false%d\n", count_op); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (LT, $t3, $t1, $t2)
			// $t3 = $t1 < $t2
			case(LTq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// desvio para label de verdadeiro
				printf("JN .true%d\n", count_op);
				fprintf(assemb, "JN .true%d\n", count_op); nInst += 2;
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// desvio para label de falso
				printf("J .false%d\n", count_op);
				fprintf(assemb, "J .false%d\n", count_op); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (EQ, $t3, $t1, $t2)
			// $t3 = $t1 == $t2
			case(EQq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// desvio para label de verdadeiro
				printf("JZ .true%d\n", count_op);
				fprintf(assemb, "JZ .true%d\n", count_op); nInst += 2;
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// desvio para label de falso
				printf("J .false%d\n", count_op);
				fprintf(assemb, "J .false%d\n", count_op); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (NEQ, $t3, $t1, $t2)
			// $t3 = $t1 != $t2
			case(NEq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[2]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
				// subtrai $t2 do acumulador
				printf("SUB %s\n", quad[i].campo[3]);
				fprintf(assemb, "SUB %s\n", quad[i].campo[3]); nInst += 2;
				// desvio para label de falso
				printf("JZ .false%d\n", count_op);
				fprintf(assemb, "JZ .false%d\n", count_op); nInst += 2;
				// carrega 1 no acumulador
				printf("LDA 1\n");
				fprintf(assemb, "LDA 1\n"); nInst += 2;
				// desvio para label de verdadeiro
				printf("J .true%d\n", count_op);
				fprintf(assemb, "J .true%d\n", count_op); nInst += 2;
				// label de falso
				printf(".false%d\n", count_op);
				fprintf(assemb, ".false%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".false");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// carrega 0 no acumulador
				printf("LDA 0\n");
				fprintf(assemb, "LDA 0\n"); nInst += 2;
				// label de verdadeiro
				printf(".true%d\n", count_op);
				fprintf(assemb, ".true%d\n", count_op);
				sprintf(nOp,"%d", count_op);
				strcpy(labelAux, ".true");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, label);
				// armazena acumulador em $t3
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				count_op++;
				startParam = 0;
				break;

			// (IF, $t1, label, -)
			// if($t1) goto label
			case(IFq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[1]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				// subtrai 1 do acumulador
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// desvio (se $t1 for 1, $t1-1=0 e o desvio eh feito)
				printf("JZ %s\n", quad[i].campo[2]);
				fprintf(assemb, "JZ %s\n", quad[i].campo[2]); nInst += 2;
				startParam = 0;
				break;

			// (GOTO, label, -, -)
			// goto label
			case(GOTOq):
				// desvio para label
				printf("J %s\n", quad[i].campo[1]);
				fprintf(assemb, "J %s\n", quad[i].campo[1]); nInst += 2;
				startParam = 0;
				break;

			// (LABEL, label, -, -)
			// label
			case(LABELq):
				// label
				printf("_%s\n", quad[i].campo[1]);
				fprintf(assemb, "_%s\n", quad[i].campo[1]);
				strcpy(labelAux, "_");
				strcat(labelAux, quad[i].campo[1]);
				insertLabel(tab, labelAux, label);
				startParam = 0;
				break;

			// (LOAD, $t1, var, -) ou (LOAD, $t1, var, $t2)
			// $t1 = var ou $t1 = var[$t2]
			case(LOADq):
				if(!strcmp(quad[i].campo[3], "-")){ // simples
					// carrega var no acumulador
					printf("LDA %s\n", quad[i].campo[2]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
					// armazena acumulador em $t1
					printf("STA %s\n", quad[i].campo[1]);
					fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				}
				else{ // vetor
					// carrega $t2 no acumulador
					printf("LDA %s\n", quad[i].campo[3]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[3]); nInst += 2;
					// verifica se a variavel eh ponteiro
					if(ehPonteiro(tab, quad[i].campo[2], escopoAtual)){
						printf("ADD [%s]\n", quad[i].campo[2]);
						fprintf(assemb, "ADD [%s]\n", quad[i].campo[2]); nInst += 2;
					}
					else{
						// soma o endereco de var ao acumulador
						printf("ADD &%s\n", quad[i].campo[1]);
						fprintf(assemb, "ADD &%s\n", quad[i].campo[1]); nInst += 2; // puxa o endereco da tabela
					}
					// armazena acumulador em $t2
					printf("STA %s\n", quad[i].campo[3]);
					fprintf(assemb, "STA %s\n", quad[i].campo[3]); nInst += 2;
					// carrega o endereco apontado por $t2
					printf("LDA [%s]\n", quad[i].campo[3]);
					fprintf(assemb, "LDA [%s]\n", quad[i].campo[3]); nInst += 2; // enderecamento indireto
					// armazena acumulador em $t1
					printf("STA %s\n", quad[i].campo[1]);
					fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				}
				startParam = 0;
				break;

			// (STORE, var, $t1, -) ou (STORE, var, $t1, $t2)
			// var = $t1 ou var[$t2] = $t1
			case(STOREq):
				if(!strcmp(quad[i].campo[3], "-")){ // simples
					// carrega $t1 no acumulador
					printf("LDA %s\n", quad[i].campo[2]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
					// armazena acumulador em var
					printf("STA %s\n", quad[i].campo[1]);
					fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				}
				else{ // vetor
					// carrega $t2 no acumulador
					printf("LDA %s\n", quad[i].campo[3]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[3]); nInst += 2;
					// verifica se a variavel eh ponteiro
					if(ehPonteiro(tab, quad[i].campo[1], escopoAtual)){
						// soma o endereco apontado pelo ponteiro ao acumulador
						printf("ADD [%s]\n", quad[i].campo[1]);
						fprintf(assemb, "ADD [%s]\n", quad[i].campo[1]); nInst += 2;
					}
					else{
						// soma o endereco de var ao acumulador
						printf("ADD &%s\n", quad[i].campo[1]);
						fprintf(assemb, "ADD &%s\n", quad[i].campo[1]); nInst += 2; // puxa o endereco da tabela
					}
					// armazena acumulador em $t2
					printf("STA %s\n", quad[i].campo[3]);
					fprintf(assemb, "STA %s\n", quad[i].campo[3]); nInst += 2;
					// carrega $t1 no acumulador
					printf("LDA %s\n", quad[i].campo[2]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[2]); nInst += 2;
					// armazena $t1 no endereco apontado por $t2
					printf("STA [%s]\n", quad[i].campo[3]);
					fprintf(assemb, "STA [%s]\n", quad[i].campo[3]); nInst += 2; // enderecamento indireto
				}
				startParam = 0;
				break;

			// (ALLOC, var, escopo, tam)
			// cria a variavel var no seu escopo com tamanho tam
			case(ALLOCq):
				// adiciona a variavel na tabela
				tam = strtoumax(quad[i].campo[3], NULL, 10);
				insertVar(tab, quad[i].campo[1], quad[i].campo[2], tam, endvar);
				endvar -= tam;
				startParam = 0;
				break;

			// (PARAM, $t1, int, escopo) ou (PARAM, var, vet, escopo)
			// empilha o conteudo de $t1 nos parametros ou
			// empilha 0 se for vetor (passado por referencia)
			// escopo eh de onde os parametros saem
			case(PARAMq):
				if(!startParam){
					j = i + 1;
					while(tipoQuad(quad[j].campo[0]) != CALLq)
						j++;
					// verifica se a chamada eh recursiva
					if(!strcmp(quad[j].campo[2], escopoAtual)){
						// empilha o conteudo das variaveis e temporarios locais
						salvaEstado(tab, quad, nQuad, escopoAtual, tempv);
					}
					startParam = 1;
				}
				if(!strcmp(quad[i].campo[2], "int")){ // int
					// carrega $t1 no acumulador
					printf("LDA %s\n", quad[i].campo[1]);
					fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				}
				else { // vetor
					// carrega endereco no acumulador
					if(ehPonteiro(tab, quad[i].campo[1], quad[i].campo[3])){
						printf("LDA [%s]\n", quad[i].campo[1]);
						fprintf(assemb, "LDA [%s]\n", quad[i].campo[1]); nInst += 2;
					}
					else{
						printf("LDA &%s\n", quad[i].campo[1]);
						fprintf(assemb, "LDA &%s\n", quad[i].campo[1]); nInst += 2;
					}
				}
				// armazena acumulador na pilha
				printf("STA [$stck]\n");
				fprintf(assemb, "STA [$stck]\n"); nInst += 2; // enderecamento indireto
				// carrega o ponteiro de pilha no acumulador
				printf("LDA $stck\n");
				fprintf(assemb, "LDA $stck\n"); nInst += 2;
				// soma 1 no acumulador
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena acumulador no ponteiro de pilha
				printf("STA $stck\n");
				fprintf(assemb, "STA $stck\n"); nInst += 2;
				break;

			// (ARG, var, escopo, int) ou (ARG, var, escopo, vet)
			case(ARGq):
				// adiciona variavel no escopo da funcao se nao existir
				// ja pode ter sido alocada no caso de uma chamada recursiva
				if(!procuraVar(tab, quad[i].campo[1], quad[i].campo[2])){
					// se for vetor insere com tamanho zero na tabela
					// tamanho zero significa que eh um ponteiro
					if(!strcmp(quad[i].campo[3], "vet")){ // vetor
						insertVar(tab, quad[i].campo[1], quad[i].campo[2], 1, PTR0 + ptr);
						ptr++;
					}
					else{
						insertVar(tab, quad[i].campo[1], quad[i].campo[2], 1, endvar);
						endvar -= 1;
					}
				}
				// desempilha um parametro
				// carrega ponteiro de pilha no acumulador
				printf("LDA $stck\n");
				fprintf(assemb, "LDA $stck\n"); nInst += 2;
				// subtrai um do acumulador
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// carrega acumulador no ponteiro de pilha
				printf("STA $stck\n");
				fprintf(assemb, "STA $stck\n"); nInst += 2;
				// carrega o topo da pilha no acumulador
				printf("LDA [$stck]\n");
				fprintf(assemb, "LDA [$stck]\n"); nInst += 2; // enderecamento indireto
				// se for variavel
				if(!strcmp(quad[i].campo[3], "int")){
					// armazena pilha na variavel
					printf("STA %s\n", quad[i].campo[1]);
					fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;
				}
				else { // se for vetor
					// atualiza endereco do ponteiro
					// endereco do vetor ta no acumulador
					printf("STA $ptr%d\n", ptr-1);
					fprintf(assemb, "STA $ptr%d\n", ptr-1); nInst += 2;
				}
				startParam = 0;
				break;

			// (FUNC, tipo, nome, -)
			// cria o label da funcao
			case(FUNq):
				// label da funcao			
				printf(">%s\n", quad[i].campo[2]);
				fprintf(assemb, ">%s\n", quad[i].campo[2]);
				strcpy(labelAux, ">");
				strcat(labelAux, quad[i].campo[2]);
				insertLabel(tab, labelAux, funcao);
				strcpy(escopoAtual, quad[i].campo[2]);
				startParam = 0;
				break;

			// (CALL, $t1, func, nParam)
			// $t1 = func(nParam)
			case(CALLq):
				// caso especial para input
				if(!strcmp(quad[i].campo[2], "input")){
					printf("INPUT %s\n", quad[i].campo[1]);
					fprintf(assemb, "INPUT %s\n", quad[i].campo[1]); nInst += 2;
					break;
				}
				// caso especial para output
				if(!strcmp(quad[i].campo[2], "output")){
					// carrega topo da pilha
					printf("LDA $stck\n");
					fprintf(assemb, "LDA $stck\n"); nInst += 2;
					// diminui topo da pilha
					printf("SUB 1\n");
					fprintf(assemb, "SUB 1\n"); nInst += 2;
					// armazena topo da pilha
					printf("STA $stck\n");
					fprintf(assemb, "STA $stck\n");
					// obtem numero do temporario do parametro
					strcpy(funcat, "");
					strcat(funcat, quad[i].campo[1] + 2*sizeof(char));
					// chama a funcao output ja no temporario do parametro
					printf("OUTPUT $t%d\n", atoi(funcat)-1);
					fprintf(assemb, "OUTPUT $t%d\n", atoi(funcat)-1); nInst += 2;
					break;
				}
				// funcoes definidas pelo usuario
				// empilha endereco de retorno
				// carrega endereco de retorno no acumulador
				printf("LDA %d\n", nInst+6);
				fprintf(assemb, "LDA %d\n", nInst+6); nInst += 2;
				// armazena no topo da pilha 2
				printf("STA [$stck2]\n");
				fprintf(assemb, "STA [$stck2]\n"); nInst += 2;
				// carrega ponteiro de topo da pilha 2
				printf("LDA $stck2\n");
				fprintf(assemb, "LDA $stck2\n"); nInst += 2;
				// aumenta o topo da pilha 2 (pilha 2 cresce para baixo)
				printf("SUB 1\n");
				fprintf(assemb, "SUB 1\n"); nInst += 2;
				// armazena ponteiro de pilha 2
				printf("STA $stck2\n");
				fprintf(assemb, "STA $stck2\n"); nInst += 2;

				strcpy(funcat, ">");
				strcat(funcat, quad[i].campo[2]);
				// desvio para label da funcao
				printf("J %s\n", funcat);
				fprintf(assemb, "J %s\n", funcat); nInst += 2;

				// insere label de retorno da chamada
				printf("<call%d\n", call);
				fprintf(assemb, "<call%d\n", call); call++;
				sprintf(nOp,"%d", call-1);
				strcpy(labelAux, "<call");
				strcat(labelAux, nOp);
				insertLabel(tab, labelAux, funcao);
				
				// precisa restaurar o estado das variaveis se acabou de sair de uma chamada recursiva
				if(!strcmp(quad[i].campo[2], escopoAtual))
					carregaEstado(tab, quad[i].campo[2], tempv);

				// carrega $ret
				printf("LDA $ret\n");
				fprintf(assemb, "LDA $ret\n"); nInst += 2;
				// armazena em $t1
				printf("STA %s\n", quad[i].campo[1]);
				fprintf(assemb, "STA %s\n", quad[i].campo[1]); nInst += 2;

				startParam = 0;
				break;

			// (RETURN, $t1, -, -)
			// $ret = $t1
			case(RETURNq):
				// carrega $t1 no acumulador
				printf("LDA %s\n", quad[i].campo[1]);
				fprintf(assemb, "LDA %s\n", quad[i].campo[1]); nInst += 2;
				// armazena acumulador em $ret
				printf("STA $ret\n");
				fprintf(assemb, "STA $ret\n"); nInst += 2;

				// execucao igual ao do end
				// desempilha o endereco de retorno
				// carrega ponteiro de pilha 2
				printf("LDA $stck2\n");
				fprintf(assemb, "LDA $stck2\n"); nInst += 2;
				// diminui o tamanho da pilha 2 (pilha 2 decresce para cima)
				printf("ADD 1\n");
				fprintf(assemb, "ADD 1\n"); nInst += 2;
				// armazena ponteiro de pilha 2
				printf("STA $stck2\n");
				fprintf(assemb, "STA $stck2\n"); nInst += 2;
				// faz o desvio para o endereco indicado no topo da pilha
				printf("J [$stck2]\n");
				fprintf(assemb, "J [$stck2]\n"); nInst += 2; // enderecamento direto
				
				startParam=0;
				break;

			// (END, func, -, -)
			// fim da funcao func
			// essa parte eh executada so para funcoes void onde nao tem return
			case(ENDq):
				// nao precisa de desvio se for o fim da funcao main
				if(strcmp(quad[i].campo[1], "main") != 0){
					// desempilha o endereco de retorno
					// carrega ponteiro de pilha 2
					printf("LDA $stck2\n");
					fprintf(assemb, "LDA $stck2\n"); nInst += 2;
					// diminui o tamanho da pilha 2 (pilha 2 decresce para cima)
					printf("ADD 1\n");
					fprintf(assemb, "ADD 1\n"); nInst += 2;
					// armazena ponteiro de pilha 2
					printf("STA $stck2\n");
					fprintf(assemb, "STA $stck2\n"); nInst += 2;
					// faz o desvio para o endereco indicado no topo da pilha
					printf("J [$stck2]\n");
					fprintf(assemb, "J [$stck2]\n"); nInst += 2; // enderecamento direto
				}
				startParam = 0;
				break;

			// (HLT, -, -, -)
			// fim do programa
			case(HLTq):
				printf("HLT\n");
				fprintf(assemb, "HLT\n"); nInst++;
				startParam = 0;
				break;

			// ???
			default:
				startParam = 0;
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

	// estou usando itmCodeTeste para testar as quadruplas
	// voltar para itmCode na versao final

	// pega o numero de quadruplas
	nQuad = contaLinhas("itmCode");
	nQuad--;

	// aloca um vetor de quaruplas no tamanho certo
	quad = (quadrupla_t*) malloc(nQuad * sizeof(quadrupla_t));

	// le as quadruplas do codigo intermediario
	readQuad(quad, nQuad, "itmCode");

	printf("Quadruplas:\n");
	printQuad(quad, nQuad);
	printf("\n");

	assemb = fopen("assembCode", "w");
	if(assemb == NULL){
		printf("\nErro ao escrever no assemb do codigo intermediario!\n");
		return NULL;
	}

	printf("Codigo Assembly:\n");
	// inicializa ponteiros de pilha
	printf("LDA %d\n", STACKstart);
	fprintf(assemb, "LDA %d\n", STACKstart);
	printf("STA $stck\n");
	fprintf(assemb, "STA $stck\n");
	printf("LDA %d\n", STACK2start);
	fprintf(assemb, "LDA %d\n", STACK2start);
	printf("STA $stck2\n");
	fprintf(assemb, "STA $stck2\n");
	// comeca a execucao na funcao main
	printf("J >main\n"); nInst += 2;
	fprintf(assemb, "J >main\n");
	genAsemb(quad, nQuad, tab);

	fclose(assemb);

	printf("\nTabela de labels:\n");
	printLabel_tab(tab);

	printf("\nTabela de variaveis:\n");
	printVar_tab(tab);
	printf("\n");

	for(i=0; i<nQuad; i++){
		for(j=0; j<4; j++)
			free(quad[i].campo[j]);
		free(quad[i].campo);
	}
	free(quad);

	return tab;
}