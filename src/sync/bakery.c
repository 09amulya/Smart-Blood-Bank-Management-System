#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

/*
 * Lamport Bakery Algorithm
 *
 * Each thread takes a number like a customer
 * taking a token at a bakery.
 *
 * The thread with the smallest number enters
 * the critical section first.
 */

#define NUM_THREADS 2

static int choosing[NUM_THREADS] = {0, 0};
static int number[NUM_THREADS] = {0, 0};

static int blood_stock = 100;


/* -------------------------------------------------
 * Bakery Lock
 * ------------------------------------------------- */

static void bakery_lock(int self)
{
    int max_number = 0;

    choosing[self] = 1;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (number[i] > max_number)
            max_number = number[i];
    }

    number[self] = max_number + 1;

    choosing[self] = 0;


    for (int i = 0; i < NUM_THREADS; i++) {

        if (i == self)
            continue;

        /*
         * Wait while the other thread is
         * choosing its ticket.
         */
        while (choosing[i]) {
            /* Busy waiting */
        }

        /*
         * Wait if the other thread has
         * a smaller ticket.
         *
         * If tickets are equal, smaller
         * thread ID gets priority.
         */
        while (number[i] != 0 &&
               (number[i] < number[self] ||
               (number[i] == number[self] && i < self))) {
            /* Busy waiting */
        }
    }
}


/* -------------------------------------------------
 * Bakery Unlock
 * ------------------------------------------------- */

static void bakery_unlock(int self)
{
    number[self] = 0;
}


/* -------------------------------------------------
 * Thread Function
 * ------------------------------------------------- */

static void *blood_thread(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        bakery_lock(id);

        /*
         * Critical Section
         */

        int current = blood_stock;

        usleep(1000);

        if (id == 0)
            blood_stock = current - 10;
        else
            blood_stock = current + 20;


        if (id == 0) {

            printf("[REQUEST]  Blood stock: %d\n",
                   blood_stock);

        } else {

            printf("[DONATION] Blood stock: %d\n",
                   blood_stock);
        }

        bakery_unlock(id);
    }

    return NULL;
}


/* -------------------------------------------------
 * Bakery Demonstration
 * ------------------------------------------------- */

void run_bakery_demo(void)
{
    pthread_t request_thread;
    pthread_t donation_thread;

    int request_id = 0;
    int donation_id = 1;


    choosing[0] = 0;
    choosing[1] = 0;

    number[0] = 0;
    number[1] = 0;

    blood_stock = 100;


    printf("\n");
    printf("====================================\n");
    printf("      LAMPORT BAKERY ALGORITHM\n");
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


    pthread_join(
        request_thread,
        NULL
    );

    pthread_join(
        donation_thread,
        NULL
    );


    printf("\nFinal O- blood stock: %d\n",
           blood_stock);

    printf("====================================\n");
}