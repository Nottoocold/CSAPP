#include "src/csapp.h"
#include "sys/select.h"

void echo(int connfd);
void command();

int main(int argc, const char *argv[])
{
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_storage cliaddr; // client address information, enough space for any address
    char client_host[MAXLINE], client_port[MAXLINE];
    fd_set read_set, ready_set;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    FD_ZERO(&read_set);              // clear the set
    FD_SET(STDIN_FILENO, &read_set); // add stdin to the set
    FD_SET(listenfd, &read_set);     // add listenfd to the set

    while (1)
    {
        ready_set = read_set; // copy read_set to ready_set
        // wait for input on stdin or the listenfd
        if (select(FD_SETSIZE, &ready_set, NULL, NULL, NULL) == -1)
        {
            perror("select error");
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &ready_set))
        {
            command();
        }
        if (FD_ISSET(listenfd, &ready_set))
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            getnameinfo((struct sockaddr *)&cliaddr, clilen, client_host, MAXLINE, client_port, MAXLINE, 0);
            printf("server: got connection from %s:%s\n", client_host, client_port);
            echo(connfd);
            close(connfd);
            printf("server: connection closed (%s:%s)\n", client_host, client_port);
        }
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

void command()
{
    char cmd[MAXLINE];
    if (fgets(cmd, MAXLINE, stdin) == NULL)
    {
        printf("server: read command failed\n");
        exit(1);
    }
    printf("server: received command: %s", cmd);
}