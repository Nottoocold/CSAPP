#include "src/csapp.h"
#include "sys/select.h"

typedef struct pooled_conn
{                                  /* represents a pool of connected descriptors */
    int maxfd;                     /* highest fd in the pool */
    fd_set read_set;               /* set of all active read fds */
    fd_set ready_set;              /* subset of read_set that is ready for read */
    int nready;                    /* number of ready descriptors from select */
    int maxi;                      /* index of highest descriptor in the pool */
    int client_fds[FD_SETSIZE];    /* pool of connected descriptors */
    rio_t client_rios[FD_SETSIZE]; /* rio structures for each descriptor */
} pool;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);

int byte_cnt = 0;

int main(int argc, const char *argv[])
{
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_storage cliaddr; // client address information, enough space for any address
    char client_host[MAXLINE], client_port[MAXLINE];
    static pool pool;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    init_pool(listenfd, &pool);

    while (1)
    {
        // waiting for listening/connected descriptor(s) to become ready for read
        pool.ready_set = pool.read_set;
        pool.nready = select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

        // if listening descriptor is ready for read, accept a new connection
        if (FD_ISSET(listenfd, &pool.ready_set))
        {
            clilen = sizeof(struct sockaddr_storage);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            getnameinfo((struct sockaddr *)&cliaddr, clilen, client_host, MAXLINE, client_port, MAXLINE, 0);
            printf("server: got connection from %s:%s\n", client_host, client_port);
            add_client(connfd, &pool);
        }

        // echo any textlines received from ready descriptors
        check_clients(&pool);
    }

    return 0;
}

void init_pool(int listenfd, pool *p)
{
    int i;
    // initially, all fds are -1
    p->maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
    {
        p->client_fds[i] = -1;
    }
    // initially, only the listenfd is in the pool
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p)
{
    int i;
    p->nready--;
    // find an empty slot in the pool
    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (p->client_fds[i] < 0)
        {
            // found an empty slot
            p->client_fds[i] = connfd;
            rio_readinitb(&p->client_rios[i], connfd);

            // add the new descriptor to the read_set
            FD_SET(connfd, &p->read_set);

            // update the maxfd and maxi
            if (connfd > p->maxfd)
            {
                p->maxfd = connfd;
            }
            if (i > p->maxi)
            {
                p->maxi = i;
            }
            break;
        }
    }
    if (i == FD_SETSIZE)
    {
        printf("server: too many clients, rejecting connection\n");
    }
}

void check_clients(pool *p)
{
    int i, connfd, n;
    char buffer[MAXLINE];
    rio_t rio;

    // check for new connections
    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++)
    {
        connfd = p->client_fds[i];
        rio = p->client_rios[i];

        // if the descriptor is ready for read, echo a textline from it
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set)))
        {
            p->nready--;
            if ((n = rio_readlineb(&rio, buffer, MAXLINE)) != 0)
            {
                byte_cnt += n;
                printf("server: received %d (%d total) bytes on fd %d\n", n, byte_cnt, connfd);
                rio_writen(connfd, buffer, n);
            }
            else
            {
                // EOF on this descriptor, remove it from the pool
                close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->client_fds[i] = -1;
                printf("server: client %d closed\n", connfd);
            }
        }
    }
}
