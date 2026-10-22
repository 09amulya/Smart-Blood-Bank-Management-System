#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

static int lock = 0;
static int blood_stock = 100;


/* -------------------------------------------------
 * Lock Variable
 * ------------------------------------------------- */

static void lock_variable_acquire(void)
{
    while (lock == 1) {
        /* Busy waiting */
    }

    lock = 1;
}


static void lock_variable_release(void)
{
    lock = 0;
}


/* -------------------------------------------------
 * Thread
 * ------------------------------------------------- */

static void *blood_thread(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        lock_variable_acquire();

        int current = blood_stock;

        usleep(1000);

        if (id == 0) {
            blood_stock = current - 10;
            printf("[REQUEST]  Blood stock: %d\n", blood_stock);
        }
        else {
            blood_stock = current + 20;
            printf("[DONATION] Blood stock: %d\n", blood_stock);
        }

        lock_variable_release();
    }

    return NULL;
}


/* -------------------------------------------------
 * Demo
 * ------------------------------------------------- */

void run_lock_variable_demo(void)
{
    pthread_t request_thread;
    pthread_t donation_thread;

    int request_id = 0;
    int donation_id = 1;

    lock = 0;
    blood_stock = 100;

    printf("\n");
    printf("====================================\n");
    printf("       LOCK VARIABLE DEMO\n");
    printf("====================================\n");

    printf("Initial O- blood stock: %d\n\n", blood_stock);

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

    printf("\nFinal O- blood stock: %d\n", blood_stock);

    printf("====================================\n");
}