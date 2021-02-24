#include "globals.h"
#include "symtab.h"

#define TAMBYTES 4

void genStmt(TreeNode * tree, int temp);
void genExp(TreeNode * tree, int temp);
void codeGen(TreeNode * syntaxTree);
static void cGen(TreeNode * tree);

static int s = 0;
static int l = 1;

void genStmt (TreeNode * tree, int temp){
  int h, nParam, i;
  int * params;
  TreeNode * param;
  switch(tree->kind.stmt){
    case (assignK):
      if (tree->child[1]->nodekind == statementK)
        genStmt(tree->child[1], temp);
      else
        genExp(tree->child[1], temp);
      printf("\t%s = t%d\n", tree->child[0]->attr.name, temp+s-1);
      break;

    case (functionK):
      printf("\n%s:\n", tree->attr.name);
      cGen(tree->child[1]);
      break;

    case (ifK):
      genExp(tree->child[0], temp+s);
      printf("\tif t%d goto L%d\n", temp+s-1, l);
      l++;
      cGen(tree->child[2]);
      printf("\tgoto L%d\n", l);
      l++;
      printf("L%d:", l-2);
      cGen(tree->child[1]);
      printf("L%d:", l-1);
      break;

    case (whileK):
      switch(tree->child[0]->attr.op){
        case (LT):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d > t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (EQ):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d != t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (GT):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d < t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (LTE):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d > t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (GTE):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d < t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (NE):
          genExp(tree->child[0]->child[0], temp+s); 
          genExp(tree->child[0]->child[1], temp+s);
          printf("\tt%d = t%d == t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        default:
          break;
      }
      printf("L%d:\tif t%d goto ", l, temp+s-1);
      l++;
      printf("L%d\n", l);
      l++;
      cGen(tree->child[1]);
      printf("\tgoto L%d\n", l-2);
      printf("L%d:", l-1);
      break;

    case (returnK):
      if (tree->child[0]->nodekind == statementK)
        genStmt(tree->child[0], temp+s);
      else
        genExp(tree->child[0], temp+s);
      printf("\treturn t%d\n", temp+s-1);
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
          genExp(param, temp);
        params[i] = s;
        i++;
        param = param->sibling;
      }
      for (i=0; i < nParam; i ++)
        printf("\tparam t%d\n", params[i]);
      free(params);
      printf("\tt%d = call %s, %d\n", temp+s, tree->attr.name, nParam);
      s++;
      break;

    default:
      break;
  }
}

void genExp ( TreeNode * tree, int temp){
  int h, n;
  switch(tree->kind.exp){
    case (typeK):
      cGen(tree->child[0]);
      break;

    case (constantK):
      printf("\tt%d = %d\n", temp, tree->attr.val);
      s++;
      break;

    case (idK):
      printf("\tt%d = %s\n", temp, tree->attr.name);
      s++;
      break;

    case (operationK):
      switch (tree->attr.op){
        case (TIMES):
          genExp(tree->child[0], temp+s);
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d * t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (OVER):
          genExp(tree->child[0], temp+s);
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d / t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (PLUS):
          genExp(tree->child[0], temp+s);
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d + t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (MINUS):
          genExp(tree->child[0], temp+s);
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d - t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (LT):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d < t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
        case (EQ):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d == t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (GT):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d > t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (LTE):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d <= t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (GTE):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d >= t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        case (NE):
          genExp(tree->child[0], temp+s); 
          genExp(tree->child[1], temp+s);
          printf("\tt%d = t%d != t%d\n", temp+s, temp+s-2, temp+s-1);
          s++;
          break;
        default:
          break;
      }
      break;

    case vectorK:
      n = TAMBYTES;
      genExp(tree->child[0], temp+s);
      printf("\tt%d = t%d * %d\n", temp+s, temp+s-1, n);
      s++;
      printf("\tt%d = %s[t%d]\n", temp+s, tree->attr.name, temp+s-1);
      s++;
      break;

    default:
      break;
  }
}

static void cGen( TreeNode * tree)
{ s = 0;
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
    //printf("\n\nirmao\n\n");
    cGen(tree->sibling);
  }
}

void codeGen(TreeNode * syntaxTree)
{  
   printf("\nCodigo de tres enderecos:\n");
   cGen(syntaxTree);
}
