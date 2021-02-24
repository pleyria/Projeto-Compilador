# Compilador da linguagem C-
Inclui implementações:
* Analisador léxico
* Analisador semântico
* Árvore de análise sintática
* Tabela de símbolos
* Analisador semântico
* Gerador de Código intermediário de três endereços


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

Isso gera o executavel cms que aceita como entrada um arquivo de código fonte. Para executar:

```
./cms "arquivo do codigo fonte"
```

A saída no terminal contém os tokens identificados e seus lexemas, a árvore de análise sintática formatada por tabulações, a tabela de simbolos gerada, o resultado da verificação de tipos e, por fim, o código de três endereços produzido.
