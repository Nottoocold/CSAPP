#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

/* struct addrinfo
{
    int ai_flags;             // Hints argument flags
    int ai_family;            // first argument to socket function
    int ai_socktype;          // second argument to socket function
    int ai_protocol;          // third argument to socket function
    char *ai_canonname;       // canonical name for service location
    size_t ai_addrlen;        // length of socket address structure
    struct sockaddr *ai_addr; // ptr to socket address structure
    struct addrinfo *ai_next; // ptr to next item in linked list
}; */

int main(int argc, char *argv[])
{
    struct addrinfo *p, *listp, _opt_conf;
    char buf[32], host[256], port[16];
    int rc, flags;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <domain name or IP address>\n", argv[0]);
        return 1;
    }

    /* get a list of address information for the given domain name or IP address */
    memset(&_opt_conf, 0, sizeof(_opt_conf));
    _opt_conf.ai_family = AF_INET;       /* we want IPv4 addresses */
    _opt_conf.ai_socktype = SOCK_STREAM; /* we want TCP sockets */

    for (int i = 1; i < argc; i++)
    {
        rc = getaddrinfo(argv[i], NULL, &_opt_conf, &listp);
        if (rc != 0)
        {
            fprintf(stderr, "getaddrinfo(%s): %s\n", argv[i], gai_strerror(rc));
            continue;
        }
        printf("======%s======\n", argv[i]);
        /* print the address information for each address */
        for (p = listp; p != NULL; p = p->ai_next)
        {

            struct sockaddr_in *addr4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(p->ai_family, &addr4->sin_addr, buf, sizeof(buf));
            printf("%s\n", buf);
            /* getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
            printf("%s\n", host); */
        }
        /* free the address information list */
        freeaddrinfo(listp);
        printf("======%s======\n", argv[i]);
    }

    return 0;
}