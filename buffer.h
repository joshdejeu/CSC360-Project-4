#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_
#define BUFFER_SIZE 5

typedef int buffer_item;

typedef struct
{
    buffer_item buffer[BUFFER_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    pthread_mutex_t mutex;
} Buffer;

void buffer_initialize(Buffer *buffer);
bool buffer_insert_item(Buffer *buffer, buffer_item item);
bool buffer_remove_item(Buffer *buffer);

void buffer_initialize(Buffer *buffer)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer->buffer[i] = -1;
    }
    buffer->in = 0;
    buffer->out = 0;
    sem_init(&buffer->full, 0, 0);
    sem_init(&buffer->empty, 0, BUFFER_SIZE);
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
    buffer->buffer[buffer->in++ % BUFFER_SIZE] = item;
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
        sem_post(&buffer->empty);
        return false; // attempt to get writing privs
    }
    buffer->buffer[buffer->out++ % BUFFER_SIZE] = -1;
    pthread_mutex_unlock(&buffer->mutex);
    sem_post(&buffer->empty);
    return true;
}

#endif // _BUFFER_H_DEFINED_
