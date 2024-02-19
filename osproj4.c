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

void *producer(void *param);
void *consumer(void *param);
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num);
double randomFloat()
{
    return (double)rand() / (double)RAND_MAX * 2.5;
}

pthread_mutex_t print_buffer_priv;
int buffer_size = 5; // Default buffer size

// sim sleep, max pc sleep, p thread, c thread, yn
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Must provide args\n");
        printf("\tsimulation time \n\tmax thread sleep time\n\tp thread count\n\tc thread count\n\tyes/no to show buffer state\n");
        exit(-1);
    }

    buffer_size = atoi(argv[1]);

    pthread_mutex_init(&print_buffer_priv, NULL);

    srand(time(NULL));
    Buffer myBuffer;
    myBuffer.buffer_size = buffer_size; // used in initializer & (in++/out++) % buffer_size
    buffer_initialize(&myBuffer);       // semaphores + mutex

    pthread_t tid_p[number_of_producers];
    pthread_t tid_c[number_of_consumers];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_create(&tid_p[i], &attr, producer, (void *)&myBuffer);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_create(&tid_c[i], &attr, consumer, (void *)&myBuffer);
    }

    // terminate all threads after N time

    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_join(tid_p[i], NULL);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_join(tid_c[i], NULL);
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
        bool succesOrFail = buffer_insert_item(buffer, randNum);

        pthread_mutex_lock(&print_buffer_priv); // temp lock printing privs
        printf("╭────────────────╮\n");
        printf("│ ");
        succesOrFail ? printf("\033[32mSUCCESS INSERT") : printf("\033[31mINSERT FAILURE");
        printf("\033[0m │\n");
        printBuffer(buffer, true, buffer->in, randNum);
        pthread_mutex_unlock(&print_buffer_priv); // unlock printing privs
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
        bool succesOrFail = buffer_remove_item(buffer);

        pthread_mutex_lock(&print_buffer_priv); // temp lock printing privs
        printf("╭────────────────╮\n");
        printf("│ ");
        succesOrFail ? printf("\033[32mSUCCESS REMOVE") : printf("\033[31mREMOVAL FAILED");
        printf("\033[0m │\n");
        printBuffer(buffer, false, buffer->out, item);
        pthread_mutex_unlock(&print_buffer_priv); // unlock printing privs
    }
    pthread_exit(0);
}

// @Brief prints the state of the buffer and if produced / consumed event
// porc: true for produced - false for consumed
// num: number either prodcued or consumed
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num)
{

    printf("\033[0m");
    indexOfEvent = (indexOfEvent - 1) % buffer_size;
    const char *eventContent = porc ? "W" : "R";
    char slotContents[buffer_size][3];
    for (int i = 0; i < buffer_size; i++)
    { // empty slots
        slotContents[i][0] = '\0';
    }
    if (indexOfEvent >= 0 && indexOfEvent < buffer_size)
    {
        snprintf(slotContents[indexOfEvent], sizeof(slotContents[indexOfEvent]), "%s", eventContent);
    }

    // correcting top border length
    printf("├────────────────┴");
    if (buffer_size > 3)
    {
        for (int i = 0; i < buffer_size - 4; i++)
        {
            printf("─────");
        }
        printf("───╮\n");
    }
    else
    {
        printf("╮\n");
    }

    printf("│");
    // Buffer items
    for (int i = 0; i < buffer_size; i++)
    {
        printf(" %-4d", buffer->buffer[i]);
    }
    printf("│\n│");
    // Buffer item platform
    for (int i = 0; i < buffer_size; i++)
    { // empty slots
        printf(" --- ");
    }
    printf("│\n│");
    // RW event under platform
    for (int i = 0; i < buffer_size; i++)
    { // empty slots
        printf("   %-2s", slotContents[i]);
    }
    printf("│\n");

    porc ? printf("│ \033[44m") : printf("│ \033[41m"); // choose produce or consume color
    porc ? printf(" Produced:") : printf(" Consumed:");
    printf(" %-3d\033[0m", num); // reset colors
    for (int i = 0; i < buffer_size - 3; i++)
    {
        printf("     ");
    }
    printf("│\n");

    // correcting bottom border length
    printf("╰─────────────────");
    for (int i = 0; i < buffer_size - 4; i++)
    {
        printf("─────");
    }
    printf("───╯\n\n");
}
