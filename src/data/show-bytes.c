#include <stdio.h>

typedef unsigned char *byte_ptr;

void show_byte(byte_ptr ptr, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++)
    {
        printf(" %.2x", ptr[i]);
    }
    printf("\n");
}

void show_int(int val)
{
    show_byte((byte_ptr)&val, sizeof(int));
}

void show_float(float val)
{
    show_byte((byte_ptr)&val, sizeof(float));
}

void show_pointer(void *p)
{
    show_byte((byte_ptr)&p, sizeof(void *));
}

int main(int argc, const char *argv[])
{
    int a = 12345;
    float f = (float)a;
    int *p = &a;
    show_int(a);
    show_float(f);
    show_pointer(p);
    return 0;
}