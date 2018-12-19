all: mem

SOURCE=list.c list.h memalloc.c memalloc.h mem.c

mem: $(SOURCE)
	gcc -g -Wall memalloc.c list.c mem.c -o $@

submit: $(SOURCE)
	tar czvf blatt09.tgz $(SOURCE)
