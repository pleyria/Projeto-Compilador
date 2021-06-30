# Compilador da linguagem C-
Inclui implementa��es:
* Analisador l�xico
* Analisador sem�ntico
* �rvore de an�lise sint�tica
* Tabela de s�mbolos
* Analisador sem�ntico
* Gerador de C�digo intermedi�rio de tr�s endere�os
* Gerador de C�digo intermedi�rio de qu�druplas
* Gerador de c�digo assembly
* Gerador de c�digo bin�rio execut�vel para a arquitetura desenvolvida no lab. Arquitetura e Organiza��o de Computadores

Para compilar:

```
make
```

ou:

```
flex cms.l
bison -d cms.y
gcc -g *.c -o cms
```

Isso gera o executavel cms que aceita como entrada um arquivo de c�digo fonte. Para executar:

```
./cms "arquivo do codigo fonte"
```

A sa�da no terminal cont�m os tokens identificados e seus lexemas, a �rvore de an�lise sint�tica formatada por tabula��es, a tabela de simbolos gerada, o resultado da verifica��o de tipos, o c�digo intermedi�rio de tr�s endere�os, o c�digo intermedi�rio de qu�druplas, o c�digo assembly, a tabela de vari�veis, a tabela de labels e o c�digo bin�rio execut�vel.

S�o gerados tamb�m tr�s arquivos:
* itmCode: Cont�m o c�digo intermedi�rio gerado na forma de qu�druplas
* assembCode: Cont�m o c�digo assembly gerado com as instru��es do processador
* binCode: Cont�m o c�digo bin�rio execut�vel correspondente ao assembly gerado na arquitura do processador

Ambos os c�digos assembly e bin�rio s�o feitos com base na arquitetura do processador desenvolvido anteriormente na disciplina lab. Arquitetura e Organiza��o de Computadores.
