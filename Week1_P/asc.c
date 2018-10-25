#include <stdio.h>

int main(int argc, char **argv) {
    printf("Oct\tDec\tHex\tChar\n");
    for (unsigned char ascii = 0; ascii < 128; ascii++) {
        printf("%03o\t%u\t%02x\t%c\n", ascii, ascii, ascii, ascii);
    }
}
