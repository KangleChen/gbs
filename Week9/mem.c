
#include <unistd.h>
#include <stdlib.h>
#include "memalloc.h"
#include "list.h"

int main(int argc, char *argv[]){
    enum algorithm {FIRST_FIT, WORST_FIT};
    enum algorithm myAlgo;
    int option;
    int option_argument;
    while ((option=getopt(argc, argv, "m:a:f:1w"))!=-1){
        switch (option){
            case 'm':
                option_argument=atoi(optarg);
                //TODO
                break;
            case 'a':
                option_argument=atoi(optarg);
                //TODO
                break;
            case 'f':
                option_argument=atoi(optarg);
                //TODO
                break;
            case '1':
                myAlgo=FIRST_FIT;
                break;
            case 'w':
                myAlgo=WORST_FIT;
                break;
            default:
                abort();

        }
    }
}

