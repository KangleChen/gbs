#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void hexdump(int sd, char *buffer, int length) {
    char hex[50];
    char str[20];
    hex[0] = str[0] = '\0';
    for (int i = 0; i < length; i++) {
        sprintf(hex + strlen(hex), "%02x ", (unsigned char) buffer[i]);
        char c = buffer[i];
        if (c < 32 || c > 126) {
            c = '.';
        }
        sprintf(str + strlen(str), "%c", c);
        if ((i + 1) % 16 == 0 || i + 1 == length) {
            char *mString = malloc(1024);
            sprintf(mString, "%06X : %-48s  %s\n", i - ((i) % 16), hex, str);
            send(sd, mString, strlen(mString), MSG_DONTWAIT);
            hex[0] = '\0';
            str[0] = '\0';
        }
    }
}
