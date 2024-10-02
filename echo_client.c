#include "src/csapp.h"

int main(int argc, const char *argv[])
{
    int client_fd, n;
    char *host, *port, buf[MAXLINE];
    rio_t rio;
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    host = argv[1];
    port = argv[2];
    client_fd = open_clientfd(host, port);

    rio_readinitb(&rio, client_fd);

    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        rio_writen(client_fd, buf, strlen(buf));
        rio_readlineb(&rio, buf, MAXLINE);
        fputs(buf, stdout);
    }
    close(client_fd);
    return 0;
}