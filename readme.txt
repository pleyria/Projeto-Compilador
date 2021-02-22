# Compilador da linguagem C-
inclui implementações:
* Analisador léxico
* Analisador semântico
* Árvore de análise sintática
* Tabela de símbolos
* Analisador semântico


No prompt:

 flex cms.l
 bison -d cms.y
 gcc -c *.c
 gcc -o cms *.o -ly -lfl

Executável gerado é c-

Executar: ./cms
