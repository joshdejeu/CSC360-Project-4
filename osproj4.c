#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include "buffer.h"
#include <time.h> //for srand seed
#include <string.h>

void *producer(void *param);
void *consumer(void *param);
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num);

pthread_mutex_t print_buffer_priv;
int simulation_time = 5;         // Default simulation time
int max_thread_sleep = 5;        // Default max thread sleep time
int number_of_producers = 1;     // Default producer thread count
int number_of_consumers = 1;     // Default consumer thread count
bool showBufferState = false;    // Default show buffer state
void printStats(Buffer *buffer); // Shows stats after simulation ends

// args
// [1] - simulation time
// [2] - max thread sleep time
// [3] - number of producer threads
// [4] - number of consumer threads
// [5] - yes/no to show buffer after every event
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Must provide args\n");
        printf("\tsimulation time \n\tmax thread sleep time\n\tp thread count\n\tc thread count\n\tyes/no to show buffer state\n");
        exit(-1);
    }

    // get cmd line args
    simulation_time = atoi(argv[1]);
    max_thread_sleep = atoi(argv[2]);
    number_of_producers = atoi(argv[3]);
    number_of_consumers = atoi(argv[4]);
    showBufferState = strcmp(argv[5], "yes") == 0;

    pthread_mutex_init(&print_buffer_priv, NULL);

    srand(time(NULL));            // seed random time
    Buffer myBuffer;              // initialize a buffer
    buffer_initialize(&myBuffer); // semaphores + mutex

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
    sleep(simulation_time);
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_cancel(tid_p[i]);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_cancel(tid_c[i]);
    }

    // join all threads back together
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_join(tid_p[i], NULL);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_join(tid_c[i], NULL);
    }

    printStats(&myBuffer);
    exit(0);
}

void *producer(void *myArg)
{
    Buffer *buffer = (Buffer *)myArg; // cast myArg to Buffer*
    pthread_t threadId = pthread_self();

    buffer_item randNum;
    while (1)
    {
        int random_number = rand() % (max_thread_sleep + 1);
        sleep(random_number);
        randNum = rand() % 100;
        bool succesOrFail = buffer_insert_item(buffer, randNum, threadId);

        pthread_mutex_lock(&print_buffer_priv); // temp lock printing privs
        // succesOrFail ? printf("\033[32mSUCCESS INSERT") : printf("\033[31mINSERT FAILURE");
        // printf("\033[0m \n");
        showBufferState ? printBuffer(buffer, true, buffer->in, randNum) : printf("\n");
        pthread_mutex_unlock(&print_buffer_priv); // unlock printing privs
    }
    pthread_exit(0);
}

void *consumer(void *myArg)
{
    Buffer *buffer = (Buffer *)myArg; // cast myArg to Buffer*
    pthread_t threadId = pthread_self();
    while (1)
    {
        int random_number = rand() % (max_thread_sleep + 1);
        sleep(random_number);
        buffer_item item = buffer->buffer[buffer->out]; // no priviledges req to read
        bool succesOrFail = buffer_remove_item(buffer, threadId);

        pthread_mutex_lock(&print_buffer_priv); // temp lock printing privs
        // succesOrFail ? printf("\033[32mSUCCESS REMOVE") : printf("\033[31mREMOVAL FAILED");
        // printf("\033[0m \n");
        showBufferState ? printBuffer(buffer, false, buffer->out, item) : printf("\n");
        pthread_mutex_unlock(&print_buffer_priv); // unlock printing privs
    }
    pthread_exit(0);
}

// @Brief prints the state of the buffer and if produced / consumed event
// porc: true for produced - false for consumed
// num: number either prodcued or consumed
void printBuffer(Buffer *buffer, bool porc, int indexOfEvent, int num)
{

    int numOfBuffersOccupied;
    sem_getvalue(&buffer->empty, &numOfBuffersOccupied);
    printf("(buffers occupied: %d)\n", (BUFFER_SIZE - numOfBuffersOccupied));

    indexOfEvent = (indexOfEvent - 1) % BUFFER_SIZE;
    const char *eventContent = porc ? "W" : "R";
    char slotContents[BUFFER_SIZE][3];
    for (int i = 0; i < BUFFER_SIZE; i++)
    { // empty slots
        slotContents[i][0] = '\0';
    }
    if (indexOfEvent >= 0 && indexOfEvent < BUFFER_SIZE)
    {
        snprintf(slotContents[indexOfEvent], sizeof(slotContents[indexOfEvent]), "%s", eventContent);
    }

    printf("buffers: ");
    // Buffer items
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %-4d", buffer->buffer[i]);
    }
    printf("\n\t");
    // Buffer item platform
    for (int i = 0; i < BUFFER_SIZE; i++)
    { // empty slots
        printf("  ---");
    }
    printf("\n\t");
    // RW event under platform
    for (int i = 0; i < BUFFER_SIZE; i++)
    { // empty slots
        printf("   %-2s", slotContents[i]);
    }
    printf("\n\n");
}

void printStats(Buffer *buffer)
{
    int threadCounter = 1;
    int num_of_buffers_occupied;
    sem_getvalue(&buffer->empty, &num_of_buffers_occupied);
    printf("\n");
    printf("PRODUCER / CONSUMER SIMULATION COMPLETE\n");
    printf("=======================================\n");
    printf("Simulation Time:                        %d\n", simulation_time);
    printf("Maximum Thread Sleep Time:              %d\n", max_thread_sleep);
    printf("Number of Producer Threads:             %d\n", number_of_producers);
    printf("Number of Consumer Threads:             %d\n", number_of_consumers);
    printf("Size of Buffer:                         %d\n", BUFFER_SIZE);
    printf("\n");
    printf("Total Number of Items Produced:         %d\n", buffer->items_produced);
    for (threadCounter; threadCounter < number_of_producers + 1; threadCounter++)
    {
        printf("    Thread %d:                           %d\n", threadCounter, 0);
    }
    printf("\n");
    printf("Total Number of Items Consumed:         %d\n", buffer->items_consumed);
    for (threadCounter; threadCounter < (number_of_producers + number_of_consumers + 1); threadCounter++)
    {
        printf("    Thread %d:                           %d\n", threadCounter, 0);
    }
    //   Thread 3:				22
    //   Thread 4:				26
    printf("\n");
    printf("Number Of Items Remaining in Buffer:    %d\n", num_of_buffers_occupied);
    printf("Number Of Times Buffer Was Full:        %d\n", buffer->times_buffer_was_full);
    printf("Number Of Times Buffer Was Empty:       %d\n", buffer->times_buffer_was_empty);
    printf("\n");
}