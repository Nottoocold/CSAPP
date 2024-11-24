#include "semaphore.h"
#include <stdlib.h>
#include <stdio.h>

// 定义一个缓冲区结构体，用于实现生产者-消费者模型
typedef struct sbuf_t
{
    int *buf;    /* Pointer to the buffer */
    int len;     /* Maximum number of slots in the buffer */
    int head;    /* buf[(head+1)%max] is the first item in the buffer */
    int tail;    /* buf[tail%max] is the last item in the buffer */
    sem_t mutex; /* Protects access to buf*/
    sem_t slots; /* Number of available slots in the buffer */
    sem_t items; /* Number of items in the buffer */
} sbuf_t;

// 初始化缓冲区
// size: 缓冲区的最大长度
void sbuf_init(sbuf_t *sbuf, int size);

// 销毁缓冲区
void sbuf_destroy(sbuf_t *sbuf);

// 生产者函数，向缓冲区中添加一个元素
// item: 要添加的元素
void sbuf_insert(sbuf_t *sbuf, int item);

// 消费者函数，从缓冲区中取出一个元素
// 返回取出的元素
int sbuf_remove(sbuf_t *sbuf);

// 缓冲区是否为空
int sbuf_empty(sbuf_t *sbuf);
