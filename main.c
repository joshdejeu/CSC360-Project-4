#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include "buffer.h"
#include <time.h> //for srand seed

#define number_of_producers 1
#define number_of_consumers 1

#define BUFFER_SIZE 5

buffer_item myBuffer[BUFFER_SIZE];

void *producer(void *param);
void *consumer(void *param);
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num);
double randomFloat()
{
    return (double)rand() / (double)RAND_MAX * 2.5;
}
int main(int argc, char *argv[])
{
    srand(time(NULL));
    Buffer myBuffer;
    buffer_initialize(&myBuffer); // semaphores + mutex

    pthread_t tid[number_of_producers];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_create(&tid[i], &attr, producer, (void *)&myBuffer);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_create(&tid[i], &attr, consumer, (void *)&myBuffer);
    }
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_join(tid[i], NULL);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_join(tid[i], NULL);
    }
    sleep(5);
    exit(0);
}

void *producer(void *bufferArg)
{
    Buffer *buffer = (Buffer *)bufferArg; // cast bufferArg to Buffer*
    buffer_item randNum;
    while (1)
    {
        sleep(randomFloat());
        randNum = rand() % 100;
        buffer_insert_item((Buffer *)bufferArg, randNum) ? printf("\033[32mSUCCESS INSERT\n") : printf("\033[31mFAILURE TO INSERT\n");
        printBuffer(buffer, true, buffer->in, randNum);
    }
    pthread_exit(0);
}
void *consumer(void *bufferArg)
{
    Buffer *buffer = (Buffer *)bufferArg; // cast bufferArg to Buffer*
    while (1)
    {
        sleep(randomFloat());
        buffer_item item = buffer->buffer[buffer->out]; // no priviledges req to read
        buffer_remove_item((Buffer *)bufferArg) ? printf("\033[32mSUCCESS REMOVE\n") : printf("\033[31mFAILURE TO REMOVE\n");
        printBuffer(buffer, false, buffer->out, item);
    }
    pthread_exit(0);
}

// @Brief prints the state of the buffer and if produced / consumed event
// porc: true for produced - false for consumed
// num: number either prodcued or consumed
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num)
{
    printf("\033[0m");
    indexOfEvent = (indexOfEvent - 1) % BUFFER_SIZE;
    const char *eventContent = porc ? "W" : "R";
    char slotContents[5][3]; // 5 slots of chars
    for (int i = 0; i < 5; i++)
    { // empty slots
        slotContents[i][0] = '\0';
    }
    if (indexOfEvent >= 0 && indexOfEvent < 5)
    {
        snprintf(slotContents[indexOfEvent], sizeof(slotContents[indexOfEvent]), "%s", eventContent);
    }

    printf("┌─────────────────────────┐\n");
    printf("│");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %-4d", buffer->buffer[i]);
    }
    printf("│\n");
    printf("│ ---  ---  ---  ---  --- │\n");

    printf("│ %-2s   %-2s   %-2s   %-2s   %-2s  │\n", slotContents[0], slotContents[1], slotContents[2], slotContents[3], slotContents[4]);

    porc ? printf("│ \033[44m") : printf("│ \033[41m"); // choose produce or consume color
    porc ? printf(" Produced:") : printf(" Consumed:");
    printf(" %-3d\033[0m          │\n", num); // reset colors
    printf("└─────────────────────────┘\n\n");
}
