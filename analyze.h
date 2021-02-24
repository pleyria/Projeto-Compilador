#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* constroi a tabela de simbolos percorrendo a
arvore sintatica em pre ordem */
void buildSymtab(TreeNode *);

/* raliza a verificacao de tipos percorrendo a
arvore sintatica em pos ordem */
void typeCheck(TreeNode *);

#endif
