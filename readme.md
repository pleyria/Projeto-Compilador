# Compilador da linguagem C-
inclui implementações:
* Analisador léxico
* Analisador semântico
* Árvore de análise sintática
* Tabela de símbolos
* Analisador semântico


Para compilar:

'''
make
'''

ou:

'''
flex cms.l
bison -d cms.y
gcc -g *.c -o cms
'''

Isso gera o executavel cms que aceita como entrada um arquivo de código fonte. Para executar:

'''
./cms "arquivo do codigo fonte"
'''
