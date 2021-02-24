/* Analisador semantico que preenche a tabela de simbolos
e faz a verificacao de tipos */

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Erro de tipo em %s na linha %d: %s\n",t->attr.name, t->lineno,message);
  Error = TRUE;
}

/* contador de locais de memoria */
static int location = 0;

/* percorre a arvora em pre ordem para constriuir a 
tabela de simbolos e depois percorre em pos ordem 
para fazer a verificacao de tipos */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ 

	if (t != NULL)
 	{ 
		preProc(t);
    	{ 
	  		int i;
	        for (i=0; i < MAXCHILDREN; i++)
        		traverse(t->child[i],preProc,postProc);
    	}
    	postProc(t);
    	traverse(t->sibling,preProc,postProc);
  	}
}

/* nao realiza nenhuma acao para os casos
de apenas pre ordem ou apenas em pos ordem */
static void nullProc(TreeNode * t)
{ 
	if (t==NULL) 
		return;
  	else 
		return;
}

/* insere identificadores da arvore
na tabela de simbolos */
static void insertNode( TreeNode * t)
{ 
	
	switch (t->nodekind)
    { 

		case statementK:
      	switch (t->kind.stmt)
        { 		
			case variableK:
          		if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
            		st_insert(t->attr.name,t->lineno,location++, t->attr.scope, "variavel", "inteiro");
          		else
            		typeError(t,"Erro 4: Declaracao invalida. Ja declarado.");	
            break;
			case functionK:
				if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
				{
					if(t->type == integerK)
                        st_insert(t->attr.name,t->lineno,location++, t->attr.scope,"funcao", "inteiro");
            		else
                        st_insert(t->attr.name,t->lineno,location++, t->attr.scope,"funcao", "void");
            	}
          		else
            		typeError(t,"Erro 4: Declaracao invalida. Ja declarado.");	
			break;
			case callK:
				if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
            		typeError(t,"Erro 5: Chamada invalida. Nao foi declarada.");	
          		else
            		st_insert(t->attr.name,t->lineno,location++, t->attr.scope, "chamada", "-");

			case returnK:
          	break;
        	default:
          break;
      }
      break;
      case expressionK:
      switch (t->kind.exp)
      { 
		  case idK:
			if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
				 typeError(t,"Erro 1: Nao foi declarada");
			else
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "variavel", "inteiro");	
	      break;
	      case vectorK:
	      	if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
				 typeError(t,"Error 1: Nao foi declarada");
			else
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "vetor", "inteiro");	
		  break;
		  case vectorIdK:
		  	if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
				 typeError(t,"Error 1: Nao foi declarada");
			else
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "indice de vetor", "inteiro");	
		  case typeK:
          break;
        default:
        break;
      }
      break;
    default:
      break;
  }
}

/* constroi a tabela de simbolos */
void buildSymtab(TreeNode * syntaxTree)
{ 
   
  traverse(syntaxTree,insertNode,nullProc); 
  if(st_lookup("main", "global") == -1)
   {
   		printf("main nao foi declarada");
   		Error = TRUE; 
   }
	
  if (TraceAnalyze)
  { 
    fprintf(listing,"\nTabela de Simbolos:\n\n");
    printSymTab(listing);
  }
}

static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { 
   case expressionK:
      switch (t->kind.exp)
      { 
        case operationK:
        break;
        default:
        break;
      }
      break;
    case statementK:
      switch (t->kind.stmt)
      { 
        case ifK:
          if (t->child[0]->type == integerK && t->child[1]->type == integerK)
           typeError(t->child[0],"teste do if nao e booleano");
        break;
        case assignK:
          if (t->child[0]->type == voidK || t->child[1]->type == voidK)
            typeError(t->child[0],"atribuicao de valor nao-inteiro");
		  else if(t->child[1]->kind.stmt == callK)
		  { 
		    if(strcmp(st_lookup_type(t->child[1]->attr.name, "global"), "void") == 0)
				typeError(t->child[1],"atribuicao de return void");
			}
        break;
        default:
        break;
      }
    break;
    default:
    break;
  }
}

void typeCheck(TreeNode * syntaxTree)
{ 
    traverse(syntaxTree,nullProc,checkNode);
}
