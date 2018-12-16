#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <arpa/inet.h>
#include "list.h"
#include <sys/socket.h>
#include <errno.h>

extern void hexdump(int outputFD, char *buffer, int length);

struct connection {
    int sd;
    struct sockaddr_in sin;
    time_t time;
    int serverNum;
};

void fillPollFdArray(list_t *list, struct pollfd fdArray[]) {
    struct list_elem *curr = list->first;
    for (int i = 0; curr != NULL; i++) {
        fdArray[i].fd = ((struct connection *) curr->data)->sd;
        fdArray[i].events = 0;
        curr = curr->next;
    }
}

int cmpConn(const void *conP, const void *fdP) {
    return ((struct connection *) conP)->sd - *(int *) fdP;
}

void setupServer(list_t *list, int port, struct pollfd pollfds[], struct connection *sConP) {
    int server_fd, ret;
    struct sockaddr_in server;


    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd < 0) {
        perror("Could not create socket in main()\n");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    ret = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
    if (ret < 0) {
        perror("Could not bind socket in main()\n");
        exit(-1);
    }

    ret = listen(server_fd, 16);
    if (ret < 0) {
        perror("Could not listen on socket in main()\n");
        exit(-1);
    }

    sConP->sd = server_fd;
    sConP->sin = server;
    sConP->time = time(NULL);
    sConP->serverNum = list_length(list);
    list_append(list, sConP);
    fillPollFdArray(list, pollfds);
    pollfds[list_length(list) - 1].events = POLLIN;
}

int echo(int fd, char buf[], int bufLen) {

    int n = recv(fd, buf, bufLen, MSG_DONTWAIT);

    //printf("client %d n:%d\n", j, n);

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return -2;
    } else if (n <= 0) {
        return n;
    }

    return send(fd, buf, n, MSG_DONTWAIT);
}

int hexdumpEcho(int fd, char buf[], int bufLen) {
    int n = recv(fd, buf, bufLen, MSG_DONTWAIT);

    //printf("client %d n:%d\n", j, n);

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return -2;
    } else if (n <= 0) {
        return n;
    }

    hexdump(fd, buf, n);
    return n;
}

int main(int argc, char *argv[], char *envp[]) {


    int client_fd, ret;
    struct sockaddr_in client;
    char buf[1024];
    list_t *conList = list_init();
    struct pollfd pollfds[12];
    int serverCount = 2;
    struct connection *conP, *sConP[serverCount];

    sConP[0] = malloc(sizeof(struct connection));
    setupServer(conList, 44445, pollfds, sConP[0]);

    sConP[1] = malloc(sizeof(struct connection));
    setupServer(conList, 44446, pollfds, sConP[1]);

    int updateTime[2];

    for (;;) {
        ret = poll(pollfds, list_length(conList), 500);

        if (ret < 0) {
            perror("Error polling in main()\n");
            exit(-1);
        }

        for (int k = 0; k < serverCount; ++k) {
            if (difftime(time(NULL), sConP[k]->time) >= 10.0) {
                while (conList->first != NULL) {
                    struct list_elem *cC = conList->last;
                    close(((struct connection *) cC->data)->sd);
                    free(cC->data);
                    list_remove(conList, cC);
                }
                exit(0);
            }

            printf("ServerRevent: %X\n", pollfds[0].revents);
            if (pollfds[k].revents == POLLIN) {
                socklen_t clientLen = sizeof(client);
                client_fd = accept(pollfds[k].fd, (struct sockaddr *) &client, &clientLen);

                conP = malloc(sizeof(struct connection));
                conP->sd = client_fd;
                conP->sin = client;
                conP->time = time(NULL);
                conP->serverNum = k;
                list_append(conList, conP);

                fillPollFdArray(conList, pollfds);

                updateTime[0] = updateTime[1] = 1;

                char addrBuf[16];
                printf("Client %s:%d connected\n", inet_ntop(AF_INET, &client.sin_addr, addrBuf, 16),
                       ntohs(client.sin_port));
            }
        }


        for (int j = serverCount; j < list_length(conList); ++j) {
            struct list_elem *leP = list_find(conList, &pollfds[j].fd, cmpConn);
            conP = (struct connection *) leP->data;
            if (difftime(time(NULL), conP->time) >= 5.0) {
                close(pollfds[j].fd);
                list_remove(conList, leP);
                continue;
            }

            printf("Client%dRevents: %X\n", j, pollfds[j].revents);
            if (pollfds[j].revents == POLLIN) {
                for (;;) {
                    if (conP->serverNum == 0) {
                        ret = echo(pollfds[j].fd, buf, 1024);
                    } else if (conP->serverNum == 1) {
                        ret = hexdumpEcho(pollfds[j].fd, buf, 1024);
                    } else {
                        // Ahhh don't know how to handle the input, just close the connection to client
                        ret = -1;
                    }
                    if (ret == -2) {
                        break;
                    } else if (ret <= 0) {
                        // DEBUG [
                        char addrBuf[16];
                        printf("Client %s:%d disconnected\n", inet_ntop(AF_INET, &conP->sin.sin_addr, addrBuf, 16),
                               ntohs(conP->sin.sin_port));
                        // ] DEBUG
                        close(pollfds[j].fd);
                        list_remove(conList, leP);
                        break;
                    }
                }
                conP->time = time(NULL);
                //updateTime[conP->serverNum] = 1;
                updateTime[0] = updateTime[1] = 1;
            }
        }

        fillPollFdArray(conList, pollfds);
        for (int i = 0; i < list_length(conList); ++i) {
            pollfds[i].events = POLLIN;
        }

        for (int l = 0; l < serverCount; ++l) {
            if (updateTime[l]) {
                sConP[l]->time = time(NULL);
                updateTime[l] = 0;
            }
        }

    }

}
