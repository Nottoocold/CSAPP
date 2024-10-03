#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

void int_to_hex(int num, char *hex)
{
    sprintf(hex, "%02X", num);
}

int dd2hex(const char *dd)
{
    size_t len = strlen(dd);
    const char *p = dd;
    int nums[4], i = 0;
    const char *dot = strchr(dd, '.');
    int num = 0;
    while (dot != NULL)
    {
        while (p != dot)
        {
            num = num * 10 + (*p - '0');
            p++;
        }
        nums[i++] = num;
        num = 0;
        p = dot + 1;
        dot = strchr(p, '.');
    }
    num = 0;
    while (*p != '\0')
    {
        num = num * 10 + (*p - '0');
        p++;
    }
    nums[i++] = num;
    char hex[14];
    hex[0] = '0';
    hex[1] = 'x';
    for (i = 0; i < 4; i++)
    {
        int_to_hex(nums[i], hex + 2 + i * 2);
    }
    printf("%s\n", hex);
    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <hex_string>, e.g. 205.188.160.121\n", argv[0]);
        return 1;
    }
    dd2hex(argv[1]);
    return 0;
}
