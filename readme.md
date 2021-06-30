# Compilador da linguagem C-
Inclui implementações:
* Analisador léxico
* Analisador semântico
* Árvore de análise sintática
* Tabela de símbolos
* Analisador semântico
* Gerador de Código intermediário de três endereços
* Gerador de Código intermediário de quádruplas
* Gerador de código assembly
* Gerador de código binário executável para a arquitetura desenvolvida no lab. Arquitetura e Organização de Computadores

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

A saída no terminal contém os tokens identificados e seus lexemas, a árvore de análise sintática formatada por tabulações, a tabela de simbolos gerada, o resultado da verificação de tipos, o código intermediário de três endereços, o código intermediário de quádruplas, o código assembly, a tabela de variáveis, a tabela de labels e o código binário executável.

São gerados também três arquivos:
* itmCode: Contém o código intermediário gerado na forma de quádruplas
* assembCode: Contém o código assembly gerado com as instruções do processador
* binCode: Contém o código binário executável correspondente ao assembly gerado na arquitura do processador

Ambos os códigos assembly e binário são feitos com base na arquitetura do processador desenvolvido anteriormente na disciplina lab. Arquitetura e Organização de Computadores.
