#include <stdio.h>
#include <string.h>

void hexdump(FILE *output, char *buffer, int length){
        char hex[50];
	char str[20];
	hex[0] = str[0] = '\0';
	for(int i=0;i<length;i++){
		sprintf(hex + strlen(hex), "%02x ", (unsigned char)buffer[i]);
		char c = buffer[i];
		if(c<32 || c>126){
			c = '.';
		}
		sprintf(str + strlen(str), "%c", c);
		if((i+1)%16==0||i+1==length){
			fprintf(output, "%06X : %-48s  %s\n", i-((i)%16), hex, str);
			hex[0] = '\0';
			str[0] = '\0';
		}
	}
}

void hexdumpTest(int argc, char** argv){
        char a[] = "Das ist ein Test-String.";
        char b[] = "12345abc";
        char c[] = "+-#,!$\"%&/()=?";
        char d[] = "Ein langer Text, \nzum testen der richtigen \nFunktionalitaet der hexdump-funktion!";

        hexdump(stdout, a, strlen(a));
        printf("\n");
        hexdump(stdout, b, strlen(b));
        printf("\n");
        hexdump(stdout, c, strlen(c));
        printf("\n");
        hexdump(stdout, d, strlen(d));
        printf("\n");
}
