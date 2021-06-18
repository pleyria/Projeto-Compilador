#define MAXLAB 100
#define MAXVAR 100

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
	int nLab, nVar, endvar;
	tab_label_t* tab_label;
	tab_var_t* tab_var;
} tab_t;

tab_t* assembgen(void);