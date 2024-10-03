/**
 * tiny.c - A simple , interative HTTP/1.0 Web Server that uses the GET method only to
 * serve static content and dynamic content .
 */
#include "src/csapp.h"
#include <sys/mman.h>
#ifdef MAXLINE
#undef MAXLINE
#endif
#define MAXLINE 8192

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(const char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int listenfd, connfd;
    char hostname[512], port[16];
    socklen_t clilen;
    struct sockaddr_storage cliaddr;

    // Create a socket for listening for incoming connections
    listenfd = open_listenfd(argv[1]);
    if (listenfd < 0)
    {
        fprintf(stderr, "ERROR: open_listenfd %s\n", argv[1]);
        exit(1);
    }
    while (1)
    {
        clilen = sizeof(cliaddr);
        // Wait for a connection
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        getnameinfo((struct sockaddr *)&cliaddr, clilen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from %s:%s\n", hostname, port);
        doit(connfd);
        close(connfd);
    }
}

void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], uri[MAXLINE], method[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rp;

    rio_readinitb(&rp, fd);

    // read request line
    rio_readlineb(&rp, buf, MAXLINE);
    printf("Request line:\n");
    printf("%s\n", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET"))
    {
        client_error(fd, "Method Not Allowed", "501", "Not Implemented", "Tiny does not implement this method");
        return;
    }

    read_requesthdrs(&rp);

    // parse uri from get request

    is_static = parse_uri(uri, filename, cgiargs);

    if (stat(filename, &sbuf) < 0)
    {
        client_error(fd, "Not Found", "404", "Not Found", "Tiny could not find the requested file");
        return;
    }

    if (is_static)
    {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        {
            client_error(fd, "Forbidden", "403", "Forbidden", "Tiny does not allow read to this file");
            return;
        }
        // serve static file
        serve_static(fd, filename, sbuf.st_size);
    }
    else
    {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
        {
            client_error(fd, "Forbidden", "403", "Forbidden", "Tiny does not allow run the CGI program");
            return;
        }
        // serve dynamic content
        // serve_dynamic(fd, filename, cgiargs);
    }
}

void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXLINE];
    // build http response body
    sprintf(body, "<html><head><title>Tiny Error</title></head>");
    sprintf(body, "%s<body bgcolor="
                  "#ffffff"
                  ">\r\n",
            body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>Tiny Web Server</em>\r\n", body);

    // build http response header
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-Type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-Length: %d\r\n\r\n", strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    rio_readlineb(rp, buf, MAXLINE);

    while (strcmp(buf, "\r\n"))
    {
        rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }

    return;
}

int parse_uri(const char *uri, char *filename, char *cgiargs)
{
    char *ptr;
    if (!strstr(uri, "api"))
    {
        // static content
        strcpy(cgiargs, "");
        strcpy(filename, "./resources");
        if (uri[strlen(uri) - 1] == '/')
        {
            strcat(filename, "/index.html");
        }
        else
        {
            strcat(filename, uri);
        }
        return 1;
    }
    else
    {
        // dynamic content
        ptr = strchr(uri, '?');
        if (ptr)
        {
            strcpy(cgiargs, ptr + 1);
            *ptr = '\0';
        }
        else
        {
            strcpy(cgiargs, "");
        }
        strcpy(filename, "./resources/cgi-bin");
        strcat(filename, uri);
        return 0;
    }
}

void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, filetype[16], buf[MAXLINE];
    // get file type
    get_filetype(filename, filetype);
    // send http response header
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-Type: %s\r\n\r\n", buf, filetype);
    rio_writen(fd, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

    // send response body to client
    srcfd = open(filename, O_RDONLY, 0);
    srcp = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    close(srcfd);
    rio_writen(fd, srcp, filesize);
    munmap(srcp, filesize);
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
}

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html"))
    {
        strcpy(filetype, "text/html");
    }
    else if (strstr(filename, ".gif"))
    {
        strcpy(filetype, "image/gif");
    }
    else if (strstr(filename, ".jpg"))
    {
        strcpy(filetype, "image/jpeg");
    }
    else if (strstr(filename, ".png"))
    {
        strcpy(filetype, "image/png");
    }
    else
    {
        strcpy(filetype, "text/plain");
    }
}