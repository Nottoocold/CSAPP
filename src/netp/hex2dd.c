#include <stdio.h>
#include <string.h>
#include <assert.h>

void is_valid_len(const char *addr, const size_t len)
{
    assert(addr != NULL);
    assert(strlen(addr) == len);
}

int hexchar2decimal(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else
        return -1;
}

int hex2dd(const char *src)
{
    const char *p = src;
    while (*p == '0' && *(p + 1) == 'x')
        p += 2;
    is_valid_len(p, 8);
    int res[4];
    int i = 0;
    while (*p != '\0')
    {
        int d = hexchar2decimal(*p);
        if (d == -1)
            return -1;
        p++;
        int dd = d << 4;
        d = hexchar2decimal(*p);
        if (d == -1)
            return -1;
        p++;
        dd |= d;
        res[i] = dd;
        i++;
    }
    printf("%d.%d.%d.%d\n", res[0], res[1], res[2], res[3]);
    return 0;
}

int main(int args, const char **argv)
{
    if (args < 2)
    {
        printf("Usage: %s <hex_addr>, e.g. 0x7f000001\n", argv[0]);
        return -1;
    }
    is_valid_len(argv[1], 10);
    hex2dd(argv[1]);
    return 0;
}