#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void toUpperCase(char *string)
{
    int i = 0;
    while (string[i] != 0)
    {
        if (string[i] >= 97 && string[i] <= 122)
        {
            string[i] -= 32;
        }
        i++;
    }
}

int main()
{
    char *input = (char *)malloc(200);
    read(0, input, 200);
    //remove \n
    int length = 0;
    while (input[length] != '\n')
    {
        length++;
    }
    input[length] = 0;
    //look for the first non character
    int i = 0;
    while (input[i] != 0)
    {
        if ((input[i] >= 97 && input[i] <= 122) || (input[i] >= 65 && input[i] <= 90))
        {
            i++;
            continue;
        }
        break;
    }

    char *copy = (char *)malloc(i + 1); //one more byte for \0
    for (int ii = 0; ii < i; ii++)
    {
        copy[ii] = input[ii];
    }
    copy[i] = 0; //set the end of the string
    toUpperCase(copy);

    //ROT13, i is still the length of copy
    for (int ii = 0; ii < i; ii++)
    {
        char posInAlphabet = copy[ii] - 65;
        copy[ii] = (posInAlphabet + 13) % 26 + 65;
    }

    printf("Hallo:  %s  -- %s", input, copy);
}