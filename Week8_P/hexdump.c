#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void hexdump(int outputFD, char *buffer, int length) {
    char hex[50];
    char str[20];
    char res[80];
    hex[0] = str[0] = '\0';
    for (int i = 0; i < length; i++) {
        sprintf(hex + strlen(hex), "%02x ", (unsigned char) buffer[i]);
        char c = buffer[i];
        if (c < 32 || c > 126) {
            c = '.';
        }
        sprintf(str + strlen(str), "%c", c);
        if ((i + 1) % 16 == 0 || i + 1 == length) {
            sprintf(res, "%06X : %-48s  %s\n", i - ((i) % 16), hex, str);
            send(outputFD, res, strlen(res), MSG_DONTWAIT);
            hex[0] = '\0';
            str[0] = '\0';
        }
    }
}