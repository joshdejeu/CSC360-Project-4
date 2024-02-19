#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

typedef int buffer_item;

typedef struct
{
    buffer_item *buffer;
    int buffer_size;
    int in;
    int out;
    sem_t full;
    sem_t empty;
    pthread_mutex_t mutex;
} Buffer;

void buffer_initialize(Buffer *buffer, int *buffer_size);
bool buffer_insert_item(Buffer *buffer, buffer_item item);
bool buffer_remove_item(Buffer *buffer);

void buffer_initialize(Buffer *buffer, int *buffer_size)
{
    // Allocate memory to the buffer
    buffer->buffer = malloc(*buffer_size * sizeof(buffer_item));
    buffer->buffer_size = *buffer_size;
    for (int i = 0; i < *buffer_size; i++)
    {
        buffer->buffer[i] = -1;
    }
    buffer->in = 0;
    buffer->out = 0;
    sem_init(&buffer->full, 0, 0);
    sem_init(&buffer->empty, 0, *buffer_size);
    pthread_mutex_init(&buffer->mutex, NULL);
};

bool buffer_insert_item(Buffer *buffer, buffer_item item)
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
    // sem_wait(&buffer->empty);           // check if buffer slots are available
    // pthread_mutex_lock(&buffer->mutex); // check if writing privilages are available
    buffer->buffer[buffer->in++ % buffer->buffer_size] = item;
    pthread_mutex_unlock(&buffer->mutex);
    sem_post(&buffer->full);
    return true;
}

bool buffer_remove_item(Buffer *buffer)
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
    buffer->buffer[buffer->out++ % buffer->buffer_size] = -1;
    pthread_mutex_unlock(&buffer->mutex);
    sem_post(&buffer->empty);
    return true;
}

#endif // _BUFFER_H_DEFINED_
