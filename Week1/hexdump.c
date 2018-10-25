#include <stdio.h>
#include <ctype.h>

//This does not close the file
void hexdump(FILE *output, char *buffer, int length)
{
    //Print the output line by line
    int offset = 0;
    while (offset < length)
    {
        //print the offset
        fprintf(output, "%06X : ", offset);
        //Hex dump part
        for (int i = 0; i < 16; i++)
        {
            int pos = offset + i;
            if (pos < length)
            {
                //%x expects an int, but we give in a char, so in debian it will be casted to
                //int but because of sign extension, the result is wrong (output 0xff...), we need to cast it like this
                fprintf(output, "%02x ", (unsigned)(unsigned char)buffer[pos]);
            }
            else
            {
                fprintf(output, "   ");
            }
        }
        //ASCII part
        fprintf(output, "  "); //2 more Leerzeich for total 3
        for (int i = 0; i < 16; i++)
        {
            int pos = offset + i;
            if (pos < length)
            {
                if (isprint(buffer[pos]))
                {
                    fprintf(output, "%c", buffer[pos]);
                }
                else
                {
                    fprintf(output, ".");
                }
            }
            else
            {
                fprintf(output, " ");
            }
        }
        fprintf(output, "\n");
        offset += 16;
    }
}