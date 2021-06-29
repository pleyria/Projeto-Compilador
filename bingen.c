#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "assembgen.h"
#include "bingen.h"

tipo_inst_t tipoInst(char* com){
	if(!strcmp(com, "STA"))
		return STAi;
	if(!strcmp(com, "LDA"))
		return LDAi;
	if(!strcmp(com, "ADD"))
		return ADDi;
	if(!strcmp(com, "SUB"))
		return SUBi;
	if(!strcmp(com, "AND"))
		return ANDi;
	if(!strcmp(com, "OR"))
		return ORi;
	if(!strcmp(com, "NOT"))
		return NOTi;
	if(!strcmp(com, "J"))
		return Ji;
	if(!strcmp(com, "JN"))
		return JNi;
	if(!strcmp(com, "JZ"))
		return JZi;
	if(!strcmp(com, "IN"))
		return INi;
	if(!strcmp(com, "OUT"))
		return OUTi;
	if(!strcmp(com, "SHR"))
		return SHRi;
	if(!strcmp(com, "SHL"))
		return SHLi;
	if(!strcmp(com, "HLT"))
		return HLTi;
	else
		return LABELi;
}

// le instrucoes do arquivo de codigo assembly
void readInst(instrucao_t* inst, int nInst, char* arquivo){
	char linha[MAX_LINHA];
	char* restoLinha, *camp;
	int i, j;
	FILE* assemb;

	assemb = fopen(arquivo, "r");
	if(assemb == NULL)
		exit(EXIT_FAILURE);

	for(i=0; i<nInst; i++){
		if(fgets(linha, MAX_LINHA, assemb) == NULL)
			return;
		// remove o newline da string lida
		strtok(linha, "\n");

		inst[i].campo = (char**) malloc(2 * sizeof(char*));
		for(j=0; j<2; j++)
			inst[i].campo[j] = (char*) malloc(MAX_CAMPO * sizeof(char));

		restoLinha = linha;
		j = 0;
		while((camp = strtok_r(restoLinha, " ", &restoLinha))){
			strcpy(inst[i].campo[j], camp);
			j++;
		}
		// coloca "-" no segundo campo da isntrucao se ela nao tiver operando
		if(j == 1)
			strcpy(inst[i].campo[1], "-");
	}

	fclose(assemb);
}

void genBin(instrucao_t* inst, int nInst, tab_t* tab){
	int i;

	for(i=0; i<nInst; i++){
		switch(tipoInst(inst[i].campo[0])){
			// Store Accumulator
			case(STAi):
				// 5 bits especificando a instrucao
				printf("00001\n");
				break;

			// Load Accumulator
			case(LDAi):
				// 5 bits especificando a instrucao
				printf("00010\n");
				break;

			// Adition
			case(ADDi):
				// 5 bits especificando a instrucao
				printf("00011\n");
				break;

			// Subtraction
			case(SUBi):
				// 5 bits especificando a instrucao
				printf("00100\n");
				break;

			// AND bitwise
			case(ANDi):
				// 5 bits especificando a instrucao
				printf("00101\n");
				break;

			// OR bitwise
			case(ORi):
				// 5 bits especificando a instrucao
				printf("00110\n");
				break;

			// NOT bitwise
			case(NOTi):
				// 5 bits especificando a instrucao
				printf("00111\n");
				break;

			// Jump
			case(Ji):
				// 5 bits especificando a instrucao
				printf("01000\n");
				break;

			// Jump if Negative
			case(JNi):
				// 5 bits especificando a instrucao
				printf("01001\n");
				break;

			// Jump if Zero
			case(JZi):
				// 5 bits especificando a instrucao
				printf("01010\n");
				break;

			// INput
			case(INi):
				// 5 bits especificando a instrucao
				printf("01011\n");
				break;

			// OUTput
			case(OUTi):
				// 5 bits especificando a instrucao
				printf("01100\n");
				break;

			// SHift Right
			case(SHRi):
				// 5 bits especificando a instrucao
				printf("01101\n");
				break;

			// SHift Left
			case(SHLi):
				// 5 bits especificando a instrucao
				printf("01110\n");
				break;

			// HaLT
			case(HLTi):
				// 5 bits especificando a instrucao
				printf("01111\n");
				break;

			// ignora labels do codigo assembly
			default:
				break;
		}
	}
}

void bingen(tab_t* tab){
	instrucao_t* inst;
	int i, j, nInst;

	// pega o numero de instrucoes
	nInst = contaLinhas("assembCode");
	nInst--;

	// aloca um vetor de instrucoes no tamanho certo
	inst = (instrucao_t*) malloc(nInst * sizeof(instrucao_t));

	// le as instrucoes no codigo assembly
	readInst(inst, nInst, "assembCode");

	printf("Codigo binario:\n");
	// produz o codigo binario
	genBin(inst, nInst, tab);

	for(i=0; i<nInst; i++){
		for(j=0; j<2; j++)
			free(inst[i].campo[j]);
		free(inst[i].campo);
	}
	free(inst);
}