#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// IP Socket address structure 16 bytes
struct sockaddr_in
{
    uint16_t sin_family;     // Address family, AF_INET
    uint16_t sin_port;       // Port number in network byte order
    struct in_addr sin_addr; // IP address in network byte order
    uint8_t sin_zero[8];     // Pad to size of `struct sockaddr'
};

// generic socket address structure (for connect, bind, and accept)
struct sockaddr
{
    uint16_t sa_family; // address family 2 bytes
    char sa_data[14];   // 14 bytes of protocol-specific address information
};

struct addrinfo
{
    int ai_flags;             // Hints argument flags
    int ai_family;            // first argument to socket function
    int ai_socktype;          // second argument to socket function
    int ai_protocol;          // third argument to socket function
    char *ai_canonname;       // canonical name for service location
    size_t ai_addrlen;        // length of socket address structure
    struct sockaddr *ai_addr; // ptr to socket address structure
    struct addrinfo *ai_next; // ptr to next item in linked list
};
