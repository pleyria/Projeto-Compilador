default:
	flex cms.l
	bison -d cms.y
	gcc -g *.c -o cms
