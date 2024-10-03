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

int open_clientfd(const char *hostname, const char *port)
{
    int clientfd;
    struct addrinfo _opt_conf, *listp, *p;

    // 设置调用getaddrinfo函数的选项结构体
    memset(&_opt_conf, 0, sizeof(_opt_conf));
    _opt_conf.ai_family = AF_INET;       // 允许 IPv4
    _opt_conf.ai_socktype = SOCK_STREAM; // 使用 TCP 套接字
    _opt_conf.ai_flags = AI_NUMERICSERV; // 服务为数字端口号形式
    _opt_conf.ai_flags |= AI_ADDRCONFIG; // 仅使用可用的地址

    // 获取地址信息
    if (getaddrinfo(hostname, port, &_opt_conf, &listp) != 0)
    {
        fprintf(stderr, "getaddrinfo error\n");
        return -1;
    }

    // 遍历所有地址并尝试连接
    for (p = listp; p != NULL; p = p->ai_next)
    {
        // 创建套接字
        clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (clientfd < 0)
        {
            continue; // 如果创建套接字失败，继续尝试下一个地址
        }

        // 连接到服务器
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            // 打印连接的主机IP地址和端口号
            char host[128], serv[16];
            getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
            printf("Connected to %s:%s\n", host, serv);
            break; // 连接成功
        }

        close(clientfd); // 连接失败，关闭套接字，继续尝试下一个地址
    }

    freeaddrinfo(listp); // 释放地址信息
    if (p == NULL)
    {
        return -1; // 如果没有成功连接的地址
    }

    return clientfd; // 返回连接的套接字文件描述符
}

int open_listenfd(const char *port)
{
    int listenfd, optval = 1;
    struct addrinfo _opt_conf, *listp, *p;

    // 设置调用getaddrinfo函数的选项结构体
    memset(&_opt_conf, 0, sizeof(_opt_conf));
    _opt_conf.ai_family = AF_INET;                   // 允许 IPv4
    _opt_conf.ai_socktype = SOCK_STREAM;             // 使用 TCP 套接字
    _opt_conf.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // 获取到的socket会被服务器应作监听socket 监听所有地址
    _opt_conf.ai_flags |= AI_NUMERICSERV;            // 服务为数字端口号形式

    // 获取地址信息
    if (getaddrinfo(NULL, port, &_opt_conf, &listp) != 0)
    {
        fprintf(stderr, "getaddrinfo error\n");
        return -1;
    }

    // 遍历可用地址，找到可进行bind的端口
    for (p = listp; p != NULL; p = p->ai_next)
    {
        // 创建套接字
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenfd < 0)
        {
            continue; // 如果创建套接字失败，继续尝试下一个地址
        }

        // 设置 SO_REUSEADDR 选项
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        // 绑定到本地地址
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            // 打印绑定的主机IP地址和端口号
            char host[128], serv[16];
            getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
            printf("Listening on %s:%s\n", host, serv);
            break; // 绑定成功
        }

        close(listenfd); // 绑定失败，关闭套接字，继续尝试下一个地址
    }

    freeaddrinfo(listp); // 释放地址信息
    if (p == NULL)
    {
        return -1; // 如果没有成功绑定地址
    }

    // 开始监听
    if (listen(listenfd, 1024) == 0)
    {
        return listenfd; // 监听成功
    }

    close(listenfd); // 监听失败，关闭套接字
    return -1;
}