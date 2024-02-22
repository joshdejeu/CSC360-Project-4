#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

typedef int buffer_item;

#define BUFFER_SIZE 5 // Default buffer size, can be changed and buffer size will adjust
typedef struct

{
    buffer_item *buffer[BUFFER_SIZE];
    int in;
    int out;
    int items_produced;
    int items_consumed;
    int times_buffer_was_empty;
    int times_buffer_was_full;
    sem_t full;
    sem_t empty;
    pthread_mutex_t mutex;
} Buffer;

void buffer_initialize(Buffer *buffer);
bool buffer_insert_item(Buffer *buffer, buffer_item item, int consumerId);
bool buffer_remove_item(Buffer *buffer, int consumerId);

void buffer_initialize(Buffer *buffer)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer->buffer[i] = -1;
    }
    buffer->in = 0;
    buffer->out = 0;
    buffer->items_produced = 0;
    buffer->items_consumed = 0;
    buffer->times_buffer_was_empty = 0;
    buffer->times_buffer_was_full = 0;
    sem_init(&buffer->full, 0, 0);
    sem_init(&buffer->empty, 0, BUFFER_SIZE);
    pthread_mutex_init(&buffer->mutex, NULL);
};

bool buffer_insert_item(Buffer *buffer, buffer_item item, int consumerId)
{
    if (sem_wait(&buffer->empty) != 0)
    {
        return false; // attempt to check if empty slot exists
    }
    if (pthread_mutex_lock(&buffer->mutex) != 0)
    {
        sem_post(&buffer->empty);
        return false; // attempt to get writing privs
    }
    printf("Producer %d writes %d\n", consumerId, item);
    buffer->items_produced++;
    buffer->buffer[buffer->in++ % BUFFER_SIZE] = item;
    int numOfBuffersOccupied;
    sem_getvalue(&buffer->empty, &numOfBuffersOccupied);
    if (numOfBuffersOccupied == 0)
    {
        buffer->times_buffer_was_empty++;
    }
    else if (numOfBuffersOccupied == BUFFER_SIZE)
    {
        printf("All buffers full.  Producer %d waits.\n\n", consumerId);
        buffer->times_buffer_was_full++;
    }
    pthread_mutex_unlock(&buffer->mutex);
    sem_post(&buffer->full);
    return true;
}

bool buffer_remove_item(Buffer *buffer, int consumerId)
{
    if (sem_wait(&buffer->full) != 0)
    {
        return false; // attempt to check if full slot exists
    }
    if (pthread_mutex_lock(&buffer->mutex) != 0)
    {
        sem_post(&buffer->full);
        return false; // attempt to get writing privs
    }
    int numOfBuffersOccupied;
    sem_getvalue(&buffer->empty, &numOfBuffersOccupied);
    if (numOfBuffersOccupied == 0)
    {
        printf("All buffers empty.  Consumer %d waits.\n\n", consumerId);
        buffer->times_buffer_was_empty++;
    }
    else if (numOfBuffersOccupied == BUFFER_SIZE)
    {
        buffer->times_buffer_was_full++;
    }
    int numberConsumed = buffer->buffer[buffer->out++ % BUFFER_SIZE];
    buffer->items_consumed++;
    printf("Consumer %d reads %d\n", consumerId, numberConsumed);
    numberConsumed = -1;
    pthread_mutex_unlock(&buffer->mutex);
    sem_post(&buffer->empty);
    return true;
}

#endif // _BUFFER_H_DEFINED_
