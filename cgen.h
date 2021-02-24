#ifndef _CGEN_H_
#define _CGEN_H_

/* funcoes para gerar o codigo intermediario de
tres enderecos percorrendo a arvore sintatica */
void genStmt(TreeNode * tree, int temp);
void genExp(TreeNode * tree, int temp);
void codeGen(TreeNode * syntaxTree);

#endif
