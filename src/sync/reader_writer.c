#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

#define NUM_READERS 3
#define NUM_WRITERS 2
#define ROUNDS 3

static int blood_stock = 100;

/* Number of readers currently accessing the resource */
static int active_readers = 0;

/* Synchronization objects */
static pthread_mutex_t reader_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------------
 * Reader
 * ------------------------------------------------- */
static void *reader(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ROUNDS; i++)
    {
        /* Protect active_readers */
        pthread_mutex_lock(&reader_mutex);

        active_readers++;

        /*
         * First reader locks the shared resource.
         * Multiple readers can read simultaneously.
         */
        if (active_readers == 1)
        {
            pthread_mutex_lock(&resource_mutex);
        }

        pthread_mutex_unlock(&reader_mutex);

        /* Critical section - reading */
        printf(
            "[READER %d] Reading O- blood stock: %d\n",
            id,
            blood_stock
        );

        usleep(100000);

        /* Reader leaves */
        pthread_mutex_lock(&reader_mutex);

        active_readers--;

        /*
         * Last reader releases the shared resource.
         */
        if (active_readers == 0)
        {
            pthread_mutex_unlock(&resource_mutex);
        }

        pthread_mutex_unlock(&reader_mutex);

        usleep(50000);
    }

    return NULL;
}

/* -------------------------------------------------
 * Writer
 * ------------------------------------------------- */
static void *writer(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ROUNDS; i++)
    {
        /*
         * Writer requires exclusive access.
         */
        pthread_mutex_lock(&resource_mutex);

        int old_stock = blood_stock;

        /*
         * Simulate blood donation.
         */
        blood_stock += 20;

        printf(
            "[WRITER %d] Updated O- blood stock: %d -> %d\n",
            id,
            old_stock,
            blood_stock
        );

        usleep(100000);

        pthread_mutex_unlock(&resource_mutex);

        usleep(50000);
    }

    return NULL;
}

/* -------------------------------------------------
 * Reader-Writer Demo
 * ------------------------------------------------- */
void run_reader_writer_demo(void)
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    int reader_ids[NUM_READERS];
    int writer_ids[NUM_WRITERS];

    active_readers = 0;
    blood_stock = 100;

    printf("\n");
    printf("====================================\n");
    printf("       READER-WRITER DEMO\n");
    printf("====================================\n");

    printf("Initial O- blood stock: %d\n", blood_stock);
    printf("Readers: %d\n", NUM_READERS);
    printf("Writers: %d\n", NUM_WRITERS);
    printf("Rounds per thread: %d\n\n", ROUNDS);

    /* Create readers */
    for (int i = 0; i < NUM_READERS; i++)
    {
        reader_ids[i] = i + 1;

        pthread_create(
            &readers[i],
            NULL,
            reader,
            &reader_ids[i]
        );
    }

    /* Create writers */
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        writer_ids[i] = i + 1;

        pthread_create(
            &writers[i],
            NULL,
            writer,
            &writer_ids[i]
        );
    }

    /* Wait for readers */
    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(
            readers[i],
            NULL
        );
    }

    /* Wait for writers */
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(
            writers[i],
            NULL
        );
    }

    printf("\n");
    printf("Final O- blood stock: %d\n", blood_stock);
    printf("Reader-Writer synchronization completed.\n");

    printf("====================================\n");
}