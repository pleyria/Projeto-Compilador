default:
	flex scanner.l
	bison -d parser.y
	gcc -o teste parser.tab.c lex.yy.c -lfl