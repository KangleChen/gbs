#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include "list.h"

#define MYPORT 44445
#define MAX_CLIENT 10
#define TIMEOUT_IDLE 10000 //in millisec
#define READ_BUFFER_SIZE 1024
#define CLIENT_TIMEOUT 5000 //in millisec

struct connection {
    int sd;
    int timeout;
};

int connection_comparator(void *c1, void *c2) {
    struct connection *connection1 = (struct connection *) c1;
    struct connection *connection2 = (struct connection *) c2;
    return connection1->timeout - connection2->timeout;
}

int connection_equality(void *c1, void *c2) {
    struct connection *connection1 = (struct connection *) c1;
    struct connection *connection2 = (struct connection *) c2;
    return connection1->sd - connection2->sd;
}

int main(int argc, char *argv[]) {
    struct pollfd myPoll[MAX_CLIENT+1]; //we store listening listensd in myPoll[0]
    list_t *timeoutList = list_init();

    for (int i = 0; i < MAX_CLIENT + 1; i++) {
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

    //don't really need, but put it here, it doesn't change anything.
    if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    struct connection listenTimeout;
    listenTimeout.sd = listensd;
    listenTimeout.timeout = TIMEOUT_IDLE;
    list_append(timeoutList, &listenTimeout);

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
        struct connection *nextWait = list_findmin(timeoutList, connection_comparator);
        struct timeval start, end;
        gettimeofday(&start, NULL);
        int ready = poll(myPoll, MAX_CLIENT + 1, nextWait->timeout);
        gettimeofday(&end, NULL);
        long timeElapsedInMilliSec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        if (ready==0){
            timeElapsedInMilliSec = nextWait->timeout;
            if (nextWait->sd == listensd) {
                printf("Timed out after 10 secs");
                close(listensd);
                return 0;
            } else {
                //client timeout
                /* char *serverMess = "You have been closed for inactivity";
                write(nextWait->sd, serverMess, strlen(serverMess));
                 */
                close(nextWait->sd);
                list_elem *elementToRemove = list_find(timeoutList, nextWait, connection_equality);
                list_remove(timeoutList, elementToRemove);
                //update time through our list.
                for (list_elem *curr = timeoutList->first; curr; curr = curr->next) {
                    struct connection *dataAtCurr = (struct connection *) curr->data;
                    dataAtCurr->timeout -= timeElapsedInMilliSec;
                }
                //remove the client from my poll
                for (int i = 1; i < MAX_CLIENT + 1; i++) {
                    if (myPoll[i].fd != nextWait->sd)
                        continue;
                    myPoll[i].fd = -1;
                }
                continue;
            }
        }
        else if (ready==-1){
            perror("Error while polling");
            return -1;
        }
        else{
            //sanity check:
            if (timeElapsedInMilliSec >= nextWait->timeout) {
                timeElapsedInMilliSec = nextWait->timeout - 50;
            }
            //if new client connection or new input
            //update timeout of all connection
            for (list_elem *curr = timeoutList->first; curr; curr = curr->next) {
                struct connection *dataAtCurr = (struct connection *) curr->data;
                dataAtCurr->timeout -= timeElapsedInMilliSec;
            }

            //reset timeout for the listensd, because server has new activity
            list_elem *timeoutListen = list_find(timeoutList, &listenTimeout, connection_equality);
            ((struct connection *) (timeoutListen->data))->timeout = TIMEOUT_IDLE;

            if (myPoll[0].revents & POLLIN){

                int sd=accept(listensd, NULL, NULL); //don't care about the address of sender

                //update myPoll with the new connection
                int i = 1;
                for (;i<MAX_CLIENT+1; i++){
                    if (myPoll[i].fd==-1){
                        myPoll[i].fd=sd;
                        break;
                    }
                }
                if (i==MAX_CLIENT+1){
                    printf("Connection missed: Too many clients");
                } else {
                    struct connection newTimeout;
                    newTimeout.sd = sd;
                    newTimeout.timeout = CLIENT_TIMEOUT;
                    list_append(timeoutList, &newTimeout);
                }
            }
            //check for data
            for (int i=1; i< MAX_CLIENT+1; i++){
                if (myPoll[i].fd==-1)
                    continue;
                char buffer[READ_BUFFER_SIZE];
                if (myPoll[i].revents & (POLLIN | POLLERR)) {
                    int sockfd=myPoll[i].fd;
                    //reset the timeout of sockfd in the list
                    for (list_elem *curr = timeoutList->first; curr; curr = curr->next) {
                        struct connection *dataAtCurr = (struct connection *) curr->data;
                        if (dataAtCurr->sd != sockfd)
                            continue;
                        dataAtCurr->timeout = CLIENT_TIMEOUT;
                    }
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


