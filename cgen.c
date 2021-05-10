/* Gerador de codigo intermediario. Produz codigo de tres
enderecos a partir da arvore de analise sintatica */

#include "globals.h"
#include "symtab.h"
#include "util.h"

#define TAMBYTES 4
#define TAMPILHA 100
#define TAMFILA 100

void genStmt(TreeNode * tree, int temp);
void genExp(TreeNode * tree, int temp);
void codeGen(TreeNode * syntaxTree);
static void cGen(TreeNode * tree);

static int s = 0; // contador de temporarios
static int l = 1; // contador de labels

/* pilha para manipular expressoes com
mais de dois operandos */
static int pilha[TAMPILHA];
static int topo = -1;

/* fila para manipular labels de gotos de if */
static int F_gotoif[TAMFILA];
static int F_gotoif_f = 50;
static int F_gotoif_t = 50;

/* pilha para manipular labels de gotos de if */
static int P_gotoif[TAMPILHA];
static int P_gotoif_topo = -1;

/* pilha para manipular labels de comeco de while */
static int P_while_comeco[TAMPILHA];
static int P_while_comeco_topo = -1;

/* pilha para manipular labels de saida de while */
static int P_while_fim[TAMPILHA];
static int P_while_fim_topo = -1;

/* arquvio para escrita do codigo intermediario */
static FILE* itmc;

/* flag para finalizacao de funcoes */
static int funOpen = FALSE;
static char* funName;

/* geracao de codigo de declaracoes */
void genStmt (TreeNode * tree, int temp){
  int h, nParam, i, v, n;
  int * params;
  TreeNode * param;
  switch(tree->kind.stmt){
    case (assignK):
      if (tree->child[0]->kind.exp == idK) {
        if (tree->child[1]->nodekind == statementK)
          genStmt(tree->child[1], temp);
        else
          genExp(tree->child[1], temp);
        printf("\t%s = t%d\n", tree->child[0]->attr.name, temp+s-1);
        fprintf(itmc, "(STORE, %s, $t%d, )\n", tree->child[0]->attr.name, temp+s-1);
      }
      else if (tree->child[0]->kind.exp == vectorK) {
        genExp(tree->child[0]->child[0], temp);
        v = s;
        if (tree->child[1]->nodekind == statementK)
          genStmt(tree->child[1], temp);
        else
          genExp(tree->child[1], temp);
        printf("\t%s[t%d] = t%d\n", tree->child[0]->attr.name, v, temp+s-1);
        fprintf(itmc, "(STORE, %s, $t%d, $t%d)\n", tree->child[0]->attr.name, temp+s-1, v);
      }

      break;

    case (functionK):
      // quadrupla de termino de funcao
      if (!funOpen){
        funName = copyString(tree->attr.name);
        funOpen = !funOpen;
      }
      else{
        fprintf(itmc, "(END, %s, , )\n", funName);
        funName = copyString(tree->attr.name);
      }
      printf("\n%s:\n", tree->attr.name);
      // quadrupla de declaracao de funcao
      switch (tree->type){
        case(integerK):
          fprintf(itmc, "(FUN, int, %s, )\n", tree->attr.name);
          break;
        default:
          fprintf(itmc, "(FUN, void, %s, )\n", tree->attr.name);
          break;
      }
      // declaracao de parametros
      param = tree->child[0];
      while (param != NULL){
        if (param->type == integerK)
          fprintf(itmc, "(ARG, %s, %s, )\n", param->child[0]->attr.name, param->attr.scope);
        param = param->sibling;
      }
      cGen(tree->child[1]);
      break;

    case (ifK):
      genExp(tree->child[0], temp);
      printf("\tif t%d goto L%d\n", temp+s-1, l);
      fprintf(itmc, "(IF, $t%d, L%d, )\n", temp+s-1, l);
      F_gotoif[F_gotoif_t++] = l;
      l++;
      cGen(tree->child[2]);
      printf("\tgoto L%d\n", l);
      fprintf(itmc, "(GOTO, L%d, , )\n", l);
      P_gotoif[++P_gotoif_topo] = l;
      l++;
      printf("L%d:", F_gotoif[F_gotoif_f++]);
      fprintf(itmc, "(LABEL, L%d, , )\n", F_gotoif[F_gotoif_f-1]);
      cGen(tree->child[1]);
      printf("L%d:", P_gotoif[P_gotoif_topo--]);
      fprintf(itmc, "(LABEL, L%d, , )\n", P_gotoif[P_gotoif_topo+1]);
      break;

    case (whileK):
      printf("L%d:", l);
      fprintf(itmc, "(LABEL, L%d, , )\n", l);
      P_while_comeco[++P_while_comeco_topo] = l;
      l++;
      switch(tree->child[0]->attr.op){
        case (LT):
          genExp(tree->child[0]->child[0], temp);
          v = s; // guarda o lado esquerdo da compracao
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d >= t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(GTE, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;
        case (EQ):
          genExp(tree->child[0]->child[0], temp);
          v = s; // guarda o lado esquerdo da compracao 
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d != t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(NE, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;
        case (GT):
          genExp(tree->child[0]->child[0], temp);
          v = s; // guarda o lado esquerdo da compracao
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d < t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(LTE, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;
        case (LTE):
          genExp(tree->child[0]->child[0], temp);
          v = s; // guarda o lado esquerdo da compracao
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d > t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(GT, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;
        case (GTE):
          genExp(tree->child[0]->child[0], temp);
          v = s; // guarda o lado esquerdo da compracao
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d < t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(LT, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;
        case (NE):
          genExp(tree->child[0]->child[0], temp); 
          v = s; // guarda o lado esquerdo da compracao
          genExp(tree->child[0]->child[1], temp);
          printf("\tt%d = t%d == t%d\n", temp+s, v, temp+s-1);
          fprintf(itmc, "(EQ, $t%d, $t%d, $t%d)\n", temp+s, v, temp+s-1);
          s++;
          break;

        default:
          break;
      }
      printf("\tif t%d goto L%d\n", temp+s-1, l);
      fprintf(itmc, "(IF, $t%d, L%d, )\n", temp+s-1, l);
      P_while_fim[++P_while_fim_topo] = l;
      l++;
      cGen(tree->child[1]);
      printf("\tgoto L%d\n", P_while_comeco[P_while_comeco_topo--]);
      fprintf(itmc, "(GOTO, L%d, , )\n", P_while_comeco[P_while_comeco_topo+1]);
      printf("L%d:", P_while_fim[P_while_fim_topo--]);
      fprintf(itmc, "(LABEL, L%d, , )\n", P_while_fim[P_while_fim_topo+1]);
      break;

    case (returnK):
      if (tree->child[0]->nodekind == statementK)
        genStmt(tree->child[0], temp);
      else
        genExp(tree->child[0], temp);
      printf("\treturn t%d\n", temp+s-1);
      fprintf(itmc, "(RETURN, $t%d, , )\n", temp+s-1);
      break;

    case (callK):
      param = tree->child[0];
      nParam = 0;
      while (param != NULL){
        nParam++;
        param = param->sibling;
      }
      i = 0;
      params = (int*) malloc(nParam * sizeof(int));
      param = tree->child[0];
      while (param != NULL){
        if(param->nodekind == statementK)
          genStmt(param, temp);
        else
          if(param->kind.exp == constantK || param->kind.exp == idK)
            genExp(param, temp);
          else
            genExp(param, temp);
        params[i] = s;
        i++;
        param = param->sibling;
      }
      for (i=0; i < nParam; i ++){
        printf("\tparam t%d\n", params[i]);
        fprintf(itmc, "(PARAM, $t%d, , )\n", params[i]);
      }
      free(params);
      printf("\tt%d = call %s, %d\n", temp+s, tree->attr.name, nParam);
      fprintf(itmc, "(CALL, $t%d, %s, %d)\n", temp+s, tree->attr.name, nParam);
      s++;
      break;

    default:
      break;
  }
}

/* gerador de codigo para expressoes */
void genExp ( TreeNode * tree, int temp){
  int h, n, ni;
  switch(tree->kind.exp){
    case (typeK):
      if (tree->child[0]->kind.stmt == variableK){ // alocacao
        if (tree->child[0]->attr.len == 0) // variaveis simples
          fprintf(itmc, "(ALLOC, %s, %s, 1)\n", tree->child[0]->attr.name, tree->child[0]->attr.scope);
        else // vetores
          fprintf(itmc, "(ALLOC, %s, %s, %d)\n", tree->child[0]->attr.name, tree->child[0]->attr.scope, tree->child[0]->attr.len);
      }
      cGen(tree->child[0]);
      break;

    case (constantK):
      printf("\tt%d = %d\n", temp+s, tree->attr.val);
      fprintf(itmc, "(ASSIGN, $t%d, %d, )\n", temp+s, tree->attr.val);
      s++;
      break;

    case (idK):
      printf("\tt%d = %s\n", temp+s, tree->attr.name);
      fprintf(itmc, "(LOAD, $t%d, %s, )\n", temp+s, tree->attr.name);
      s++;
      break;

    case (operationK):
      switch (tree->attr.op){
        case (TIMES):
          if(tree->child[0]->kind.exp == constantK || tree->child[0]->kind.exp == idK){
            genExp(tree->child[0], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[0], temp);
          if(tree->child[1]->kind.exp == constantK || tree->child[1]->kind.exp == idK){
            genExp(tree->child[1], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[1], temp);
          printf("\tt%d = t%d * t%d\n", temp+s, pilha[topo-1], pilha[topo]);
          fprintf(itmc, "(MULT, $t%d, $t%d, $t%d)\n", temp+s, pilha[topo-1], pilha[topo]);
          topo -= 2;
          s++;
          pilha[++topo] = temp+s-1;
          break;

        case (OVER):
          if(tree->child[0]->kind.exp == constantK || tree->child[0]->kind.exp == idK){
            genExp(tree->child[0], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[0], temp);
          if(tree->child[1]->kind.exp == constantK || tree->child[1]->kind.exp == idK){
            genExp(tree->child[1], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[1], temp);
          printf("\tt%d = t%d / t%d\n", temp+s, pilha[topo-1], pilha[topo]);
          fprintf(itmc, "(DIV, $t%d, $t%d, $t%d)\n", temp+s, pilha[topo-1], pilha[topo]);
          topo -= 2;
          s++;
          pilha[++topo] = temp+s-1;
          break;

        case (PLUS):
          if(tree->child[0]->kind.exp == constantK || tree->child[0]->kind.exp == idK){
            genExp(tree->child[0], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[0], temp);
          if(tree->child[1]->kind.exp == constantK || tree->child[1]->kind.exp == idK){
            genExp(tree->child[1], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[1], temp);
          printf("\tt%d = t%d + t%d\n", temp+s, pilha[topo-1], pilha[topo]);
          fprintf(itmc, "(ADD, $t%d, $t%d, $t%d)\n", temp+s, pilha[topo-1], pilha[topo]);
          topo -=2;
          s++;
          pilha[++topo] = temp+s-1;
          break;

        case (MINUS):
          if(tree->child[0]->kind.exp == constantK || tree->child[0]->kind.exp == idK){
            genExp(tree->child[0], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[0], temp);
          if(tree->child[1]->kind.exp == constantK || tree->child[1]->kind.exp == idK){
            genExp(tree->child[1], temp);
            pilha[++topo] = temp+s-1;
          }
          else
            genExp(tree->child[1], temp);
          printf("\tt%d = t%d - t%d\n", temp+s, pilha[topo-1], pilha[topo]);
          fprintf(itmc, "(SUB, $t%d, $t%d, $t%d)\n", temp+s, pilha[topo-1], pilha[topo]);
          topo -= 2;
          s++;
          pilha[++topo] = temp+s-1;
          break;

        case (LT):
          genExp(tree->child[0], temp); 
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d < t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(LT, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        case (EQ):
          genExp(tree->child[0], temp); 
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d == t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(EQ, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        case (GT):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d > t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(GT, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        case (LTE):
          genExp(tree->child[0], temp); 
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d <= t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(LTE, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        case (GTE):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d >= t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(GTE, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        case (NE):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp);
          printf("\tt%d = t%d != t%d\n", temp+s, temp+s-2, temp+s-1);
          fprintf(itmc, "(NE, $t%d, $t%d, $t%d)\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;

        default:
          break;
      }
      break;

    case vectorK:
      genExp(tree->child[0], temp);
      // s++;
      printf("\tt%d = %s[t%d]\n", temp+s, tree->attr.name, temp+s-1);
      fprintf(itmc, "(LOAD, $t%d, %s, $t%d)\n", temp+s, tree->attr.name, temp+s-1);
      pilha[++topo] = temp+s;
      s++;
      break;

    default:
      break;
  }
}

/* gerador de codigo a partir da raiz da 
arvore de analise sintatica */
static void cGen( TreeNode * tree)
{ //s = 0;
  topo = -1;
  if (tree != NULL)
  { switch (tree->nodekind) {
      case statementK:
        genStmt(tree, 1);
        break;
      case expressionK:
        genExp(tree, 1);
        break;
      default:
        break;
    }
    cGen(tree->sibling);
  }
}

/* impressao do codigo intermediario */
void codeGen(TreeNode * syntaxTree)
{  
   itmc = fopen("itmCode", "w");
   if(itmc == NULL)
    printf("\nErro ao escrever no itmc do codigo intermediario!\n");
   printf("\nCodigo de tres enderecos:\n");
   cGen(syntaxTree);
   printf("\n");
   fprintf(itmc, "(END, %s, , )\n", funName);
   fprintf(itmc, "(HLT, , , )\n");
   free(funName);
   fclose(itmc);
}
