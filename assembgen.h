#define MAX_LINHA 200
#define MAX_CAMPO 50

#define MAXLAB 100
#define MAXVAR 100
#define MAXTEMP 50

#define OP1 43670	// operador 1
#define OP2 43671	// operador 2

#define STCK 43672	// topo da pilha
#define STCK2 43673	// topo da pilha 2

#define RET 43674	// retorno

#define PTR0 43675	// ponteiro 0
#define PTR1 43676	// ponteiro 1
#define PTR2 43677	// ponteiro 2
#define PTR3 43678	// ponteiro 3
#define PTR4 43679	// ponteiro 4

#define STACK2start 21845
#define STACKstart 43669
#define TEMPstart 43690
#define VARstart 65535

// tipos de quadruplas
typedef enum{
	ASSIGNq, ADDq, SUBq, MULTq, DIVq, GTEq, GTq, LTEq, LTq, EQq, NEq, IFq,
	GOTOq, LABELq, LOADq, STOREq, ALLOCq, PARAMq, ARGq, FUNq, CALLq,
	RETURNq, ENDq, HLTq
} comando_t;

// quadrupla
typedef struct {
	// campos
	char** campo;
} quadrupla_t;

// tipos de labels
typedef enum{
	funcao, label
} tipoLabel;

// tabela de labels
typedef struct{
	char nome[MAXLAB];
	int endereco;
	tipoLabel tipo;
} tab_label_t;

// tabela de variaveis
typedef struct{
	char nome[MAXVAR];
	char escopo[MAXVAR];
	int endereco;
	int tamanho;
} tab_var_t;

// tabela geral
typedef struct{
	int nLab, nVar;
	tab_label_t* tab_label;
	tab_var_t* tab_var;
} tab_t;

tab_t* assembgen(void);

int contaLinhas(char* arquivo);