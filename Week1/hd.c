#include <stdio.h>
#include <string.h>
extern void hexdump(FILE *output, char *buffer, int length);
int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        hexdump(stdout, argv[i], strlen(argv[i]) + 1);
        printf("\n");
    }
}