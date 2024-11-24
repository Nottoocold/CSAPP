#include "sbuf.h"

void sbuf_init(sbuf_t *buf, int size)
{
    buf->buf = calloc(size, sizeof(int)); // allocate the buffer
    buf->len = size;                      // set the buffer length
    buf->head = buf->tail = 0;            // initialize the head and tail pointers
    sem_init(&buf->mutex, 0, 1);          // binary semaphore for locking
    sem_init(&buf->slots, 0, size);       // initialize, buf has size slots available
    sem_init(&buf->items, 0, 0);          // initialize, buf has zero items available
}

void sbuf_destroy(sbuf_t *buf)
{
    free(buf->buf);
    sem_destroy(&buf->mutex);
    sem_destroy(&buf->slots);
    sem_destroy(&buf->items);
}

void sbuf_insert(sbuf_t *buf, int item)
{
    sem_wait(&buf->slots);                       // wait for a slot to become available
    sem_wait(&buf->mutex);                       // lock the buffer
    buf->buf[(++buf->tail) % (buf->len)] = item; // insert the item into the buffer
    sem_post(&buf->mutex);                       // unlock the buffer
    sem_post(&buf->items);                       // signal that an item has been added to the buffer
}

int sbuf_remove(sbuf_t *buf)
{
    int item;
    sem_wait(&buf->items);                       // wait for an item to become available
    sem_wait(&buf->mutex);                       // lock the buffer
    item = buf->buf[(++buf->head) % (buf->len)]; // remove the item from the buffer
    sem_post(&buf->mutex);                       // unlock the buffer
    sem_post(&buf->slots);                       // signal that a slot is now available
    return item;
}
