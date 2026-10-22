#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

static int lock = 0;
static int blood_stock = 100;


/* -------------------------------------------------
 * Test-and-Set
 * ------------------------------------------------- */

static int test_and_set(int *target)
{
    int old_value = *target;

    *target = 1;

    return old_value;
}


/* -------------------------------------------------
 * Acquire Lock
 * ------------------------------------------------- */

static void tas_lock(void)
{
    while (test_and_set(&lock)) {
        /* Busy waiting */
    }
}


/* -------------------------------------------------
 * Release Lock
 * ------------------------------------------------- */

static void tas_unlock(void)
{
    lock = 0;
}


/* -------------------------------------------------
 * Thread Function
 * ------------------------------------------------- */

static void *blood_thread(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        tas_lock();

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

        tas_unlock();
    }

    return NULL;
}


/* -------------------------------------------------
 * Test-and-Set Demonstration
 * ------------------------------------------------- */

void run_test_and_set_demo(void)
{
    pthread_t request_thread;
    pthread_t donation_thread;

    int request_id = 0;
    int donation_id = 1;

    lock = 0;
    blood_stock = 100;


    printf("\n");
    printf("====================================\n");
    printf("       TEST-AND-SET LOCK DEMO\n");
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