#include "src/csapp.h"

void echo(int connfd);

int main(int argc, const char *argv[])
{
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_storage cliaddr; // client address information, enough space for any address
    char client_host[MAXLINE], client_port[MAXLINE];
    char buffer[1024];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    while (1)
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        getnameinfo((struct sockaddr *)&cliaddr, clilen, client_host, MAXLINE, client_port, MAXLINE, 0);
        printf("server: got connection from %s:%s\n", client_host, client_port);
        echo(connfd);
        close(connfd);
        printf("server: connection closed (%s:%s)\n", client_host, client_port);
    }

    return 0;
}

void echo(int connfd)
{
    size_t n;
    char buffer[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio, connfd);
    while ((n = rio_readlineb(&rio, buffer, MAXLINE)) != 0)
    {
        printf("server received %d bytes: %s", (int)n, buffer);
        rio_writen(connfd, buffer, n);
    }
}