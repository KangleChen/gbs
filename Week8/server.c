#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>

#define MYPORT 44445
#define MAX_CLIENT 10
#define TIMEOUT_IDLE 10000
#define READ_BUFFER_SIZE 1024

/*
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};
 */

int main(int argc, char *argv[]) {
    struct pollfd myPoll[MAX_CLIENT+1]; //we store listening listensd in myPoll[0]
    for (int i=0; i< MAX_CLIENT+1; i++){
        //first there is no client, initialize to -1
        myPoll[i].fd=-1;
        myPoll[i].events= 0 | POLLIN;
    }
    //building the listening socket
    if ((myPoll[0].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return -1;
    }
    int listensd=myPoll[0].fd;

    struct sockaddr_in sa;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(MYPORT);

    if (bind(listensd, (struct sockaddr *) &sa, sizeof(sa))) {
        //Bind the socket to the adress and port stored in sa
        perror("Bind failed");
        return -1; //exit(-1);
    }
    if (listen(listensd, 10) == -1) {
        perror("Listen failed");
        return -1; //exit(-1);
    }
    while (1) {
        int ready=poll(myPoll, MAX_CLIENT+1, TIMEOUT_IDLE);
        if (ready==0){
            printf("Timed out after 10 secs");
            close(listensd);
            return 0;
        }
        else if (ready==-1){
            perror("Error while polling");
            return -1;
        }
        else{
            //if new client connection
            if (myPoll[0].revents & POLLIN){
                int sd=accept(listensd, NULL, NULL); //don't care about the address of sender
                //update myPoll with the new connection
                int i=1;
                for (;i<MAX_CLIENT+1; i++){
                    if (myPoll[i].fd==-1){
                        myPoll[i].fd=sd;
                        break;
                    }
                }
                if (i==MAX_CLIENT+1){
                    printf("Connection missed: Too many clients");
                }
            }
            //check for data
            for (int i=1; i< MAX_CLIENT+1; i++){
                if (myPoll[i].fd==-1)
                    continue;
                char buffer[READ_BUFFER_SIZE];
                if (myPoll[i].revents & POLLIN){
                    int sockfd=myPoll[i].fd;
                    int bytesread=read(sockfd, buffer, READ_BUFFER_SIZE);
                    switch (bytesread){
                        case -1:
                            perror("Cannot read from socket");
                            return -1;
                        case 0:
                            //connection closed by client
                            close(sockfd);
                            myPoll[i].fd=-1;
                            break;
                        default:
                            if(write(sockfd, buffer, bytesread) <=0){
                                perror("Error while writing");
                                return -1;
                            }
                    }
                }
            }
        }
    }
}


