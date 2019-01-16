all: mysh

SOURCE=list.c list.h parser.c wildcard.c wildcard.h mysh.c

mysh: mysh.c list.c list.h parser.c wildcard.c
	gcc -g -Wall mysh.c list.c parser.c wildcard.c -o $@

submit: $(SOURCE)
	tar czvf blatt11.tgz $(SOURCE)
