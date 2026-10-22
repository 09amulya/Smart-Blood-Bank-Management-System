#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

/*
 * Peterson's Algorithm
 *
 * flag[i] = 1  -> thread i wants to enter critical section
 * turn      -> gives the other thread priority
 */

static int flag[2] = {0, 0};
static int turn = 0;

static int blood_stock = 100;


/* -------------------------------------------------
 * Peterson lock
 * ------------------------------------------------- */

static void peterson_lock(int self)
{
    int other = 1 - self;

    flag[self] = 1;
    turn = other;

    while (flag[other] && turn == other) {
        /* Busy waiting */
    }
}


/* -------------------------------------------------
 * Peterson unlock
 * ------------------------------------------------- */

static void peterson_unlock(int self)
{
    flag[self] = 0;
}


/* -------------------------------------------------
 * Thread function
 * ------------------------------------------------- */

static void *blood_thread(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        peterson_lock(id);

        /*
         * Critical Section
         */
        int current = blood_stock;

        usleep(1000);

        if (id == 0)
            blood_stock = current - 10;
        else
            blood_stock = current + 20;

        if (id == 0)
            printf("[REQUEST]  Blood stock: %d\n",
                   blood_stock);
        else
            printf("[DONATION] Blood stock: %d\n",
                   blood_stock);

        peterson_unlock(id);
    }

    return NULL;
}


/* -------------------------------------------------
 * Peterson demonstration
 * ------------------------------------------------- */

void run_peterson_demo(void)
{
    pthread_t request_thread;
    pthread_t donation_thread;

    int request_id = 0;
    int donation_id = 1;

    flag[0] = 0;
    flag[1] = 0;
    turn = 0;

    blood_stock = 100;

    printf("\n");
    printf("====================================\n");
    printf("     PETERSON'S ALGORITHM DEMO\n");
    printf("====================================\n");

    printf("Initial O- blood stock: %d\n\n",
           blood_stock);

    pthread_create(
        &request_thread,
        NULL,
        blood_thread,
        &request_id
    );

    pthread_create(
        &donation_thread,
        NULL,
        blood_thread,
        &donation_id
    );

    pthread_join(request_thread, NULL);
    pthread_join(donation_thread, NULL);

    printf("\nFinal O- blood stock: %d\n",
           blood_stock);

    printf("====================================\n");
}