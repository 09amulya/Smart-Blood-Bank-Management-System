#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

#define BUFFER_SIZE 5
#define PRODUCE_COUNT 10


/* -------------------------------------------------
 * Semaphore
 * ------------------------------------------------- */

typedef struct
{
    int value;

    pthread_mutex_t mutex;
    pthread_cond_t condition;

} BufferSemaphore;


static void semaphore_init(BufferSemaphore *sem, int value)
{
    sem->value = value;

    pthread_mutex_init(
        &sem->mutex,
        NULL
    );

    pthread_cond_init(
        &sem->condition,
        NULL
    );
}


static void semaphore_wait(BufferSemaphore *sem)
{
    pthread_mutex_lock(
        &sem->mutex
    );

    while (sem->value == 0) {

        pthread_cond_wait(
            &sem->condition,
            &sem->mutex
        );
    }

    sem->value--;

    pthread_mutex_unlock(
        &sem->mutex
    );
}


static void semaphore_signal(BufferSemaphore *sem)
{
    pthread_mutex_lock(
        &sem->mutex
    );

    sem->value++;

    pthread_cond_signal(
        &sem->condition
    );

    pthread_mutex_unlock(
        &sem->mutex
    );
}


static void semaphore_destroy(BufferSemaphore *sem)
{
    pthread_mutex_destroy(
        &sem->mutex
    );

    pthread_cond_destroy(
        &sem->condition
    );
}


/* -------------------------------------------------
 * Bounded Buffer
 * ------------------------------------------------- */

static int buffer[BUFFER_SIZE];

static int in = 0;
static int out = 0;


/*
 * empty = number of empty slots
 *
 * Initially all slots are empty.
 */

static BufferSemaphore empty;


/*
 * full = number of filled slots
 *
 * Initially no items are available.
 */

static BufferSemaphore full;


/*
 * Mutex protects the actual buffer.
 */

static pthread_mutex_t buffer_mutex =
    PTHREAD_MUTEX_INITIALIZER;


/* -------------------------------------------------
 * Producer
 * ------------------------------------------------- */

static void *producer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= PRODUCE_COUNT; i++) {

        /*
         * Wait for an empty slot.
         */

        semaphore_wait(&empty);


        /*
         * Enter critical section.
         */

        pthread_mutex_lock(
            &buffer_mutex
        );


        buffer[in] = i;

        printf(
            "[PRODUCER] Added blood unit %d at slot %d\n",
            i,
            in
        );


        in = (in + 1) % BUFFER_SIZE;


        /*
         * Leave critical section.
         */

        pthread_mutex_unlock(
            &buffer_mutex
        );


        /*
         * One more item is available.
         */

        semaphore_signal(&full);


        usleep(100000);
    }

    return NULL;
}


/* -------------------------------------------------
 * Consumer
 * ------------------------------------------------- */

static void *consumer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= PRODUCE_COUNT; i++) {

        /*
         * Wait until at least one item exists.
         */

        semaphore_wait(&full);


        /*
         * Enter critical section.
         */

        pthread_mutex_lock(
            &buffer_mutex
        );


        int blood_unit = buffer[out];

        printf(
            "[CONSUMER] Allocated blood unit %d from slot %d\n",
            blood_unit,
            out
        );


        out = (out + 1) % BUFFER_SIZE;


        /*
         * Leave critical section.
         */

        pthread_mutex_unlock(
            &buffer_mutex
        );


        /*
         * One buffer slot became empty.
         */

        semaphore_signal(&empty);


        usleep(150000);
    }

    return NULL;
}


/* -------------------------------------------------
 * Bounded Buffer Demo
 * ------------------------------------------------- */

void run_bounded_buffer_demo(void)
{
    pthread_t producer_thread;
    pthread_t consumer_thread;


    /*
     * Reset buffer.
     */

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    in = 0;
    out = 0;


    /*
     * Initially:
     *
     * empty = BUFFER_SIZE
     * full  = 0
     */

    semaphore_init(
        &empty,
        BUFFER_SIZE
    );

    semaphore_init(
        &full,
        0
    );


    printf("\n");
    printf("====================================\n");
    printf("       BOUNDED BUFFER DEMO\n");
    printf("====================================\n");

    printf(
        "Buffer capacity: %d\n",
        BUFFER_SIZE
    );

    printf(
        "Items to produce: %d\n\n",
        PRODUCE_COUNT
    );


    pthread_create(
        &producer_thread,
        NULL,
        producer,
        NULL
    );


    pthread_create(
        &consumer_thread,
        NULL,
        consumer,
        NULL
    );


    pthread_join(
        producer_thread,
        NULL
    );

    pthread_join(
        consumer_thread,
        NULL
    );


    printf("\n");
    printf("Producer-Consumer execution completed.\n");
    printf("====================================\n");


    semaphore_destroy(&empty);

    semaphore_destroy(&full);

    pthread_mutex_destroy(&buffer_mutex);
}