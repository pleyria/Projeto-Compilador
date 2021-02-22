#include "globals.h"
#include "symtab.h"

void genStmt(TreeNode * tree, int temp);
void genExp(TreeNode * tree, int temp);
void codeGen(TreeNode * syntaxTree);
static void cGen(TreeNode * tree);

int high(TreeNode * tree){
    int altE,altD;
    if(tree==NULL) return 0;
    altE=high(tree->child[0])+1;
    altD=high(tree->child[1])+1;
    if(altE>altD) return altE;
    return altD;
}

void genStmt (TreeNode * tree, int temp){
  int h;
  switch(tree->kind.stmt){
    case assignK:
      genExp(tree->child[1], temp);

      h = high(tree);

      printf("%s = t%d\n", tree->child[0]->attr.name, temp+h);
      break;

    case functionK:
      printf("%s: \n", tree->attr.name);
      cGen(tree->child[1]);
      break;

    case variableK:
      break;

    case whileK:
      break;

    case callK:
      break;

    default:
      break;
  }
}

void genExp ( TreeNode * tree, int temp){
  int h;
  switch(tree->kind.exp){
    case typeK:
      cGen(tree->child[0]);
      break;

    case constantK:
      printf("t%d = %d\n", temp, tree->attr.val);
      break;

    case idK:
      break;

    case operationK:
      switch (tree->attr.op){
        case (TIMES):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp+1);
          printf("t%d = t%d * t%d\n", temp+2, temp, temp+1);
          break;
        case (OVER):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp+1);
          printf("t%d = t%d / t%d\n", temp+2, temp, temp+1);
          break;
        case (PLUS):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp+1);
          h = high(tree)-1;
          printf("t%d = t%d + t%d\n", temp+h, temp, temp+1);
          break;
        case (MINUS):
          genExp(tree->child[0], temp);
          genExp(tree->child[1], temp+1);
          h = high(tree)-1;
          printf("t%d = t%d - t%d\n", temp+h, temp, temp+1);
          break;
        default:
          break;
      }

    default:
      break;
  }
}

static void cGen( TreeNode * tree)
{ if (tree != NULL)
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


/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree)
{  
   printf("\nThree address code:\n\n");
   cGen(syntaxTree);
}
