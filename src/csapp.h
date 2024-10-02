#include "sys/types.h"
#include "sys/stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>

/**
 * RIO(Robust IO，健壮的IO包)
 */

// RIO无缓冲的输入输出函数

/**
 * 从文件描述符fd的当前文件位置最多传送n个字节到内存位置usrbuf
 * 遇到EOF返回0,错误返回-1,否则返回传送的字节数
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n);

/**
 * 从位置usrbuf传送n和字节到文件fd
 * 错误返回-1,否则返回传送的字节数
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

// RIO带缓冲区的输入输出函数

#define RIO_BUFSIZE 8192
typedef struct RIO_BUF
{
    int rio_fd;                   /* descriptor for this internal buffer */
    int rio_cnt;                  /* unread bytes of internal buffer */
    char *rio_bufptr;             /* next unread byte of internal buffer */
    char rio_buffer[RIO_BUFSIZE]; /* internal buffer */
} rio_t;

/**
 * init read buffer
 */
void rio_readinitb(rio_t *rp, int fd);

/**
 * 每次从rp最多读取maxlen-1字节到usrbuf
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/**
 * 每次从rp最多读n字节到usrbuf
 */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

/**
 * 打开一个客户端连接到服务器hostname:port，返回文件描述符，任何<0的值表示出错
 */
int open_clientfd(const char *hostname, const char *port);

/**
 * 打开一个服务器监听端口port，返回文件描述符，任何<0的值表示出错
 */
int open_listenfd(const char *port);