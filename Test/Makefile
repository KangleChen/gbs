all: mysh

SOURCE=list.c list.h parser.c wildcard.c wildcard.h mysh.c io.h io.c

mysh: mysh.c list.c list.h parser.c wildcard.c io.h io.c
	gcc -g -Wall mysh.c list.c parser.c wildcard.c io.c -o $@

submit: $(SOURCE)
	tar czvf blatt12.tgz $(SOURCE)
