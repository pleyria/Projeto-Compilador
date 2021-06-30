#define TAMPALAVRA 16

// tipos de instrucoes
typedef enum{
	STAi, LDAi, ADDi, SUBi, ANDi, ORi, NOTi, Ji, JNi, JZi, INi,
	OUTi, SHRi, SHLi, HLTi, LABELi
} tipo_inst_t;

// instrucao
typedef struct{
	// campos
	char** campo;
} instrucao_t;

void bingen(tab_t* tab);