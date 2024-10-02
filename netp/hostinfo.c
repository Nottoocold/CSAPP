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
    struct addrinfo *p, *listp, hints;
    char buf[24];
    int rc, flags;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <domain name or IP address>\n", argv[0]);
        return 1;
    }
    /* get a list of address information for the given domain name or IP address */
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;   /* we want canonical name */
    hints.ai_family = AF_INET;       /* we want IPv4 addresses */
    hints.ai_socktype = SOCK_STREAM; /* we want TCP sockets */
    rc = getaddrinfo(argv[1], NULL, &hints, &listp);
    if (rc != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return 1;
    }
    /* print the address information for each address */
    for (p = listp; p != NULL; p = p->ai_next)
    {
        printf("Family: %d, Socket type: %d, Protocol: %d\n", p->ai_family, p->ai_socktype, p->ai_protocol);
        printf("Canonical name: %s\n", p->ai_canonname ? p->ai_canonname : "(none)");
        printf("Address length: %d\n", p->ai_addrlen);
        printf("Address: ");
        switch (p->ai_family)
        {
        case AF_INET:
            struct sockaddr_in *addr4 = (struct sockaddr_in *)p->ai_addr;
            printf("%s\n", inet_ntop(p->ai_family, &addr4->sin_addr, buf, sizeof(buf)));
            break;
        case AF_INET6:
            struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)p->ai_addr;
            printf("%s\n", inet_ntop(p->ai_family, &addr6->sin6_addr, buf, sizeof(buf)));
            break;
        default:
            printf("(unknown)\n");
            break;
        }
    }
    /* free the address information list */
    freeaddrinfo(listp);
    return 0;
}