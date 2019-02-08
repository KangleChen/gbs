#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char in[128];
    while (1) {
        fgets(in, 128, stdin);

        if (strncmp(in, "exit\n", 10) == 0) {
            return 0;
        }
        system(in);
    }
}
