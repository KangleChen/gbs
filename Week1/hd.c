#include <stdio.h>
#include <string.h>

void hexdump(FILE *output, char *buffer, int length);

int main(int argc, char **argv) {

    if (argc <= 1) {
        printf("Puffer angeben!!\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        hexdump(stdout, argv[i], strlen(argv[i]) + 1);
        printf("\n");
    }
}
