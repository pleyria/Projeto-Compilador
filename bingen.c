#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "assembgen.h"
#include "bingen.h"

// variavel para imprimir os numeros das linhas no terminal
int linha;

/* arquivo para escrita do codigo binario */
static FILE* binc;

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
	if(!strcmp(com, "INPUT"))
		return INi;
	if(!strcmp(com, "OUTPUT"))
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

int enderecoVar(tab_t* tab, char* escopo, char* var){
	int i;

	for(i=0; i< tab->nVar; i++){
		if(strcmp(var, tab->tab_var[i].nome) == 0 && strcmp(escopo, tab->tab_var[i].escopo) == 0)
			return tab->tab_var[i].endereco;
	}
}

int enderecoLabel(tab_t* tab, char* lab){
	int i;

	for(i=0; i < tab->nLab; i++){
		if(!strcmp(tab->tab_label[i].nome, lab))
			return tab->tab_label[i].endereco;
	}
}

// decimal -> binario
void decToBin(int dec){
	int i, n;
	int bin[TAMPALAVRA];

	// impressao da linha no terminal
	printf("%d:\t", linha);
	linha++;

	n = 0;
	while(dec > 0){
		bin[n] = dec % 2;
		dec /= 2;
		n++;
	}

	for(i=0; i < TAMPALAVRA-n; i++){
		printf("0");
		fprintf(binc, "0");
	}

	for(i=n-1; i >= 0; i--){
		printf("%d", bin[i]);
		fprintf(binc, "%d", bin[i]);
	}
	printf("\n");
	fprintf(binc, "\n");
}

// trata a segunda parte das instrucoes
void endercamento(tab_t* tab, char* escopoAtual, char* com){
	int im;
	char temp[4];

	switch(com[0]){
		// sem operando explicito (11)
		case('-'):
			printf("11000000000\n");
			fprintf(binc, "11000000000\n");
			break;

		// imediato (10)
		case('&'):
			// ponteiro
			// uso do denrecamento imediato
			// obtencao do endereco do nome da variavel sem o '&'
			printf("10000000000\n");
			fprintf(binc, "10000000000\n");
			im = enderecoVar(tab, escopoAtual, com + sizeof(char));
			decToBin(im);
			break;

		case('0'):
		case('1'):
		case('2'):
		case('3'):
		case('4'):
		case('5'):
		case('6'):
		case('7'):
		case('8'):
		case('9'):
			// numero
			printf("10000000000\n");
			fprintf(binc, "10000000000\n");
			// impressao do imediato
			decToBin(atoi(com));
			break;

		// enderecamento indireto (01)
		case('['):
			printf("01000000000\n");
			fprintf(binc, "01000000000\n");
			// remove o ultimo ']'
			com[strlen(com)-1] = '\0';
			// especial
			if(com[1] == '$'){
				if(!strcmp(com + 2*sizeof(char), "stck")){ // pilha 1
					decToBin(STCK);
					break;
				}
				if(!strcmp(com + 2*sizeof(char), "stck2")){ // pilha 2
					decToBin(STCK2);
					break;
				}
				else{
					// obtem o numero do temporario
					strcpy(temp, "");
					strcat(temp, com + 3*sizeof(char));
					im = atoi(temp);
					// imprime o endereco do temporario
					decToBin(TEMPstart + im - 1);
					break;
				}
			}
			break;

		// enderecamento direto (00)
		case('$'):
			printf("00000000000\n");
			fprintf(binc, "00000000000\n");
			// temporario
			if(com[1] == 't'){
				// obtem o numero do temporario
				strcpy(temp, "");
				strcat(temp, com + 2*sizeof(char));
				im = atoi(temp);
				// imprimme o endereco do temporario
				decToBin(TEMPstart + im - 1);
				break;
			}
			// $OP1
			if(!strcmp(com + sizeof(char), "op1")){
				decToBin(OP1);
				break;
			}
			// $OP2
			if(!strcmp(com + sizeof(char), "op2")){
				decToBin(OP2);
				break;
			}
			// topo da pilha 1
			if(!strcmp(com + sizeof(char), "stck")){
				decToBin(STCK);
				break;
			}
			// topo da pilha 2
			if(!strcmp(com + sizeof(char), "stck2")){
				decToBin(STCK2);
				break;
			}
			// valor de retorno de funcao
			if(!strcmp(com + sizeof(char), "ret")){
				decToBin(RET);
				break;
			}
			// $ptr0 ... $ptr14
			else{
				// obtem o numero do ponteiro
				strcpy(temp, "");
				strcat(temp, com + 4*sizeof(char));
				im = atoi(temp);
				// imprime endereco do ponteiro
				decToBin(PTR0 + im);
				break;
			}
			break;

		// labels
		case('.'):
		case('_'):
		case('>'):
		case('<'):
			// para labels obtidas da tabela, o enderecamento eh direto
			printf("00000000000\n");
			fprintf(binc, "00000000000\n");
			// pega o endereco indicado pela label
			im = enderecoLabel(tab, com);
			decToBin(im);
			break;

		default:
			// variavel criada pelo usuario ou ponteiro (direto)
			printf("00000000000\n");
			fprintf(binc, "00000000000\n");
			im = enderecoVar(tab, escopoAtual, com);
			decToBin(im);
			break;
	}
}

void genBin(instrucao_t* inst, int nInst, tab_t* tab){
	int i;
	int label = 0;
	char escopoAtual[MAXLAB];

	linha = 0;

	for(i=0; i<nInst; i++){
		label = 0;

		// impressao da instrucao assembly correspondente no terminal
		printf("\t%s %s\n", inst[i].campo[0], inst[i].campo[1]);

		// impressao do numero da linha no terminal
		if(tipoInst(inst[i].campo[0]) != LABELi){
			printf("%d:\t", linha);
			linha++;
		}

		switch(tipoInst(inst[i].campo[0])){
			// Store Accumulator
			case(STAi):
				// 5 bits especificando a instrucao
				printf("00001");
				fprintf(binc, "00001");
				break;

			// Load Accumulator
			case(LDAi):
				// 5 bits especificando a instrucao
				printf("00010");
				fprintf(binc, "00010");
				break;

			// Adition
			case(ADDi):
				// 5 bits especificando a instrucao
				printf("00011");
				fprintf(binc, "00011");
				break;

			// Subtraction
			case(SUBi):
				// 5 bits especificando a instrucao
				printf("00100");
				fprintf(binc, "00100");
				break;

			// AND bitwise
			case(ANDi):
				// 5 bits especificando a instrucao
				printf("00101");
				fprintf(binc, "00101");
				break;

			// OR bitwise
			case(ORi):
				// 5 bits especificando a instrucao
				printf("00110");
				fprintf(binc, "00110");
				break;

			// NOT bitwise
			case(NOTi):
				// 5 bits especificando a instrucao
				printf("00111");
				fprintf(binc, "00111");
				break;

			// Jump
			case(Ji):
				// 5 bits especificando a instrucao
				printf("01000");
				fprintf(binc, "01000");
				break;

			// Jump if Negative
			case(JNi):
				// 5 bits especificando a instrucao
				printf("01001");
				fprintf(binc, "01001");
				break;

			// Jump if Zero
			case(JZi):
				// 5 bits especificando a instrucao
				printf("01010");
				fprintf(binc, "01010");
				break;

			// INput
			case(INi):
				// 5 bits especificando a instrucao
				printf("01011");
				fprintf(binc, "01011");
				break;

			// OUTput
			case(OUTi):
				// 5 bits especificando a instrucao
				printf("01100");
				fprintf(binc, "01100");
				break;

			// SHift Right
			case(SHRi):
				// 5 bits especificando a instrucao
				printf("01101");
				fprintf(binc, "01101");
				break;

			// SHift Left
			case(SHLi):
				// 5 bits especificando a instrucao
				printf("01110");
				fprintf(binc, "01110");
				break;

			// HaLT
			case(HLTi):
				// 5 bits especificando a instrucao
				printf("01111");
				fprintf(binc, "01111");
				break;

			// ignora labels do codigo assembly
			case(LABELi):
				label = 1;
				// se o label for de inicio de funcao
				// salva o nome do escopo atual
				if(inst[i].campo[0][0] == '>')
					strcpy(escopoAtual, inst[i].campo[0] + sizeof(char));
				break;
		}
		if(!label)
			endercamento(tab, escopoAtual, inst[i].campo[1]);
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

	binc = fopen("binCode", "w");
	if(binc == NULL){
		printf("\nErro ao escrever no binc do codigo binario!\n");
		return;
	}

	printf("Codigo binario:\n");
	// produz o codigo binario
	genBin(inst, nInst, tab);

	fclose(binc);

	for(i=0; i<nInst; i++){
		for(j=0; j<2; j++)
			free(inst[i].campo[j]);
		free(inst[i].campo);
	}
	free(inst);
}