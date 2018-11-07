#include <stdio.h>


int main(int argc, char **argv) {


    printf("Oct\tDec\tHex\tChar\n");

    unsigned int count;
    for(count = 0; count <=255; count++){

        if(count <= 7){
            printf("00%o\t%d\t0%x\t%c\n", count, count, count, count);
        }
        if(count >= 8 && count < 10){
            printf("0%o\t%d\t0%x\t%c\n", count, count, count, count);
        }
        if(count >=10 && count < 16){
            printf("0%o\t%d\t0%x\t%c\n", count, count, count, count);
        }
        if(count >= 16 && count < 64){
            printf("0%o\t%d\t%x\t%c\n", count, count, count, count);
        }
        if(count >= 64 && count < 256){
            printf("%o\t%d\t%x\t%c\n", count, count, count, count);
        }}

    return 0;
}