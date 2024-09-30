#include "../src/csapp.h"

int main(int argc, char **argv)
{
    struct stat stat_t;
    char *type, *readok;
    stat(argv[1], &stat_t); // get file metadata

    if (S_ISREG(stat_t.st_mode))
    {
        type = "regular";
    }
    else if (S_ISDIR(stat_t.st_mode))
    {
        type = "directory";
    }
    else
    {
        type = "other";
    }

    if ((stat_t.st_mode & S_IRUSR))
    {
        readok = "yes";
    }
    else
    {
        readok = "no";
    }

    printf("type:%s, read:%s\n", type, readok);

    return 0;
}