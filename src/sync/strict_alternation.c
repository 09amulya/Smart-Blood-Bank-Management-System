#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

#define NUM_THREADS 2
#define ROUNDS 5

static int turn = 0;
static int blood_stock = 100;

static void strict_lock(int id)
{
    while (turn != id)
    {
        /* Busy waiting */
    }
}

static void strict_unlock(int id)
{
    turn = 1 - id;
}

static void *blood_thread(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ROUNDS; i++)
    {
        strict_lock(id);

        int current = blood_stock;

        usleep(1000);

        if (id == 0)
        {
            blood_stock = current - 10;

            printf(
                "[REQUEST - T%d]  Blood stock: %d\n",
                id,
                blood_stock
            );
        }
        else
        {
            blood_stock = current + 20;

            printf(
                "[DONATION - T%d] Blood stock: %d\n",
                id,
                blood_stock
            );
        }

        strict_unlock(id);
    }

    return NULL;
}

void run_strict_alternation_demo(void)
{
    pthread_t threads[NUM_THREADS];

    int ids[NUM_THREADS] = {0, 1};

    turn = 0;
    blood_stock = 100;

    printf("\n");
    printf("====================================\n");
    printf("       STRICT ALTERNATION DEMO\n");
    printf("====================================\n");

    printf("Initial O- blood stock: %d\n", blood_stock);
    printf("Threads: 2\n");
    printf("Rounds per thread: %d\n", ROUNDS);
    printf("Synchronization: Strict Alternation\n\n");

    pthread_create(
        &threads[0],
        NULL,
        blood_thread,
        &ids[0]
    );

    pthread_create(
        &threads[1],
        NULL,
        blood_thread,
        &ids[1]
    );

    pthread_join(
        threads[0],
        NULL
    );

    pthread_join(
        threads[1],
        NULL
    );

    printf("\n");
    printf("Final O- blood stock: %d\n", blood_stock);

    printf("Strict alternation execution completed.\n");

    printf("====================================\n");
}