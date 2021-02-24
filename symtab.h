#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* insere linhas na tabela hash */
void st_insert( char * name, int lineno, int loc, char* scope, char* typeID, char* typeData);

/* retorna o local de uma variavel na memoria */
int st_lookup (char * name, char* scope );

char* st_lookup_type(char* name, char* scope);

/* imprime o conteudo da tabela de simbolos */
void printSymTab(FILE * listing);

#endif
