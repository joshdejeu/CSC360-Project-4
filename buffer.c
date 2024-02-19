// #include "buffer.h"
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdbool.h>

// void buffer_initialize(Buffer *buffer);
// bool buffer_insert_item(Buffer *buffer, buffer_item item);
// bool buffer_remove_item(Buffer *buffer, buffer_item *item);

// // sem_t full;
// // sem_t empty;
// // pthread_mutex_t mutex;
// // sem_init(&empty, 0, 5);
// // sem_init(&full, 0, 0);
// // pthread_mutex_init(&mutex, NULL);

// void buffer_initialize(Buffer *buffer)
// {
//     for (int i = 0; i < BUFFER_SIZE; i++)
//     {
//         buffer->buffer[i] = -1;
//     }
//     buffer->in = 0;
//     buffer->out = 0;
//     sem_init(&buffer->full, 0, 0);
//     sem_init(&buffer->empty, 0, BUFFER_SIZE);
//     pthread_mutex_init(&buffer->mutex, NULL);
// };

// bool buffer_insert_item(Buffer *buffer, buffer_item item)
// {
//     sem_wait(&buffer->empty);           // check if buffer slots are available
//     pthread_mutex_lock(&buffer->mutex); // check if writing privilages are available
//     const int RANDOM_NUM = rand() % 100;
//     buffer->in++ % BUFFER_SIZE;
//     buffer->buffer[buffer->in] = item;
//     pthread_mutex_unlock(&buffer->mutex);
//     sem_post(&buffer->full);
// }
