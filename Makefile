default:
	bison -d parser.y
	flex scanner.l
	gcc -o compilador parser.tab.c lex.yy.c -lfl