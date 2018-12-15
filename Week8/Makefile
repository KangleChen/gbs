all: server

SOURCE=list.c list.h server.c hexdump.c

server: server.c list.c list.h hexdump.c
	gcc -g -Wall server.c list.c hexdump.c -o $@

submit: $(SOURCE)
	tar czvf blatt08.tgz $(SOURCE)
