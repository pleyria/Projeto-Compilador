# Compilador da linguagem C-
Inclui implementa��es:
* Analisador l�xico
* Analisador sem�ntico
* �rvore de an�lise sint�tica
* Tabela de s�mbolos
* Analisador sem�ntico
* Gerador de C�digo intermedi�rio de tr�s endere�os


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

A sa�da no terminal cont�m os tokens identificados e seus lexemas, a �rvore de an�lise sint�tica formatada por tabula��es, a tabela de simbolos gerada, o resultado da verifica��o de tipos e, por fim, o c�digo de tr�s endere�os produzido.
