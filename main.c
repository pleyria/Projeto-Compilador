/* Arquivo principal onde sao chamadas as rotinas de
analise do codigo fonte e sintese do codigo intermediario. */

#include "globals.h"

/* definir como TRUE para ter apenas o scanner */
#define NO_PARSE FALSE
/* definir como TRUE para nao ter analise semantica */
#define NO_ANALYZE FALSE

/* definir com TRUE para nao gerar o codigo intermediario */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif
#include "assembgen.h"

int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

int main( int argc, char * argv[] )
{ TreeNode * syntaxTree;
  tab_t* tab;
  char pgm[120]; 
  if (argc != 2)
    { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cms");
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nCompilacao em C menos: %s\n",pgm);
#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"\nArvore sintatica:\n");
    printTree(syntaxTree);
  }
#if !NO_ANALYZE
  if (! Error)
  { if (TraceAnalyze) fprintf(listing,"\nConstruindo Tabela de Simbolos...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nChecando Tipos...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nChecagem de Tipos Finalizada\n");
  }
#if !NO_CODE
  if (! Error)
  { 
    codeGen(syntaxTree);
    tab = assembgen();
  }
#endif
#endif
#endif
  fclose(source);
  return 0;
}

