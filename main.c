#include "src/csapp.h"

#define MAXLINE 64

int main(void)
{
    int n;
    rio_t rp;
    char buf[MAXLINE];

    rio_readinitb(&rp, STDIN_FILENO);
    while ((n = rio_readlineb(&rp, buf, MAXLINE)) != 0)
    {
        rio_writen(STDOUT_FILENO, buf, n);
    }

    return 0;
}
