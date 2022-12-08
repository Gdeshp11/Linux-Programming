/* Implementation of ringbuffer using array*/

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 5

typedef struct rbuf
{
    int *buf;
    int head;
    int tail;
    int num_entries;
    void (*init)(struct rbuf *self);
    void (*exit)(struct rbuf *self);
} ringbuf_t;

void init_ringbuf(ringbuf_t *rbuf)
{
    rbuf->buf = malloc(sizeof(int) * BUF_SIZE);
    rbuf->head = 0;
    rbuf->tail = 0;
    rbuf->num_entries = 0;
}

void exit_ringbuf(ringbuf_t *rbuf)
{
    free(rbuf->buf);
}

void enqueue(int num, ringbuf_t *rbuf)
{
    rbuf->buf[rbuf->head] = num;
    rbuf->head = (rbuf->head + 1) % BUF_SIZE;
    rbuf->num_entries++;
}

int dequeue(ringbuf_t *rbuf)
{
    int ret = -1;
    if (rbuf->num_entries == 0)
    {
        printf("\nRing Buffer is empty..");
        return ret;
    }
    ret = rbuf->buf[rbuf->tail];
    rbuf->buf[rbuf->tail] = 0;
    rbuf->tail = (rbuf->tail + 1) % BUF_SIZE;
    rbuf->num_entries--;
    return ret;
}

void print_ringbuf(ringbuf_t *rbuf)
{
    if (rbuf->num_entries == 0)
    {
        printf("\nringbuf is empty..");
        return;
    }
    printf("\nRingBuf data:");
    for (int i = 0; i < BUF_SIZE; i++)
    {
        printf("\t%d", rbuf->buf[i]);
    }
}

int main()
{
    ringbuf_t *ringbuf = malloc(sizeof(ringbuf_t));
    ringbuf->init = init_ringbuf;
    ringbuf->exit = exit_ringbuf;
    ringbuf->init(ringbuf);
    enqueue(1, ringbuf);
    enqueue(2, ringbuf);
    enqueue(3, ringbuf);
    enqueue(4, ringbuf);
    enqueue(5, ringbuf);
    enqueue(6, ringbuf);
    print_ringbuf(ringbuf);
    dequeue(ringbuf);
    dequeue(ringbuf);
    dequeue(ringbuf);
    dequeue(ringbuf);
    // dequeue(ringbuf);
    print_ringbuf(ringbuf);
    ringbuf->exit(ringbuf);
    free(ringbuf);
    return 0;
}
