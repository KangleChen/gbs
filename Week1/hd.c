#include <stdio.h>
#include <string.h>
extern void hexdump(FILE *output, char *buffer, int length);
int main(int argc, char **argv)
{
    //argc counts the program itself as one of the arguments, damn
    if (argc > 2)
    {
        //we have more than one argument
        for (int i = 1; i < argc - 1; i++) //argc-1 because we do not want a \n in the end
        {
            hexdump(stdout, argv[i], strlen(argv[i]) + 1); //strlen does not count \0 byte at the end
            printf("\n");
        }
    }
    else if (argc == 2)
    {
        hexdump(stdout, argv[1], strlen(argv[1]) + 1);
    }
}