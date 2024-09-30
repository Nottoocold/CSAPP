#include "csapp.h"

// RIO带缓冲区的输入输出函数实现

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    size_t nread;
    char *bufp = usrbuf;

    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            return -1; /* ERROR */
        }
        else if (nread == 0)
        {
            break; /* EOF */
        }
        nleft -= nread;
        bufp += nread;
    }

    return n - nleft;
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    size_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            return -1;
        }
        nleft -= nwritten;
        bufp += nwritten;
    }

    return n;
}

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buffer;
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;
    while (rp->rio_cnt <= 0) /* 缓冲区为空，调用read函数填充缓冲区 */
    {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buffer, sizeof(rp->rio_buffer));
        if (rp->rio_cnt < 0)
        {
            return -1; /* error */
        }
        else if (rp->rio_cnt == 0)
        {
            return 0; /* EOF */
        }
        else
        {
            rp->rio_bufptr = rp->rio_buffer; /* reset internal buffer ptr */
        }
    }

    /* copy min(n, rp->rio_cnt) bytes from internal buffer to user buffer */
    cnt = n;
    if (rp->rio_cnt < n)
    {
        cnt = rp->rio_cnt;
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int n, rc;
    char c, *bufp = usrbuf;
    for (n = 1; n < maxlen; n++)
    {
        if ((rc = rio_read(rp, &c, 1)) == 1)
        {
            *bufp = c;
            bufp++;
            if (c == '\n')
            {
                n++;
                break;
            }
        }
        else if (rc == 0)
        {
            if (n == 1)
            {
                return 0;
            }
            else
            {
                break;
            }
        }
        else
        {
            return -1;
        }
    }
    *bufp = 0; // 最后一位设置为NULL
    return n - 1;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0)
    {
        if ((nread = rio_read(rp, bufp, nleft)) < 0)
        {
            return -1; // error
        }
        else if (nread == 0)
        {
            return 0; // EOF
        }
        else
        {
            nleft -= nread;
            bufp += nread;
        }
    }
    return n - nleft;
}