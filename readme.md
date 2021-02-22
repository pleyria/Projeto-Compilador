# Compilador da linguagem C-
inclui implementa��es:
* Analisador l�xico
* Analisador sem�ntico
* �rvore de an�lise sint�tica
* Tabela de s�mbolos
* Analisador sem�ntico


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

Isso gera o executavel cms que aceita como entrada um arquivo de c�digo fonte. Para executar:

'''
./cms "arquivo do codigo fonte"
'''
