# Compilador da linguagem C-
inclui implementa��es:
* Analisador l�xico
* Analisador sem�ntico
* �rvore de an�lise sint�tica
* Tabela de s�mbolos
* Analisador sem�ntico


No prompt:

 flex cms.l
 bison -d cms.y
 gcc -c *.c
 gcc -o cms *.o -ly -lfl

Execut�vel gerado � c-

Executar: ./cms
