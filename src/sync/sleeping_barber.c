#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "sync.h"

#define CHAIRS 3
#define CUSTOMERS 6

static sem_t customers;
static sem_t barber;
static pthread_mutex_t mutex;

static int waiting_customers = 0;

/* -------------------------------------------------
 * Barber
 * ------------------------------------------------- */
static void *barber_thread(void *arg)
{
    (void)arg;

    for (int i = 0; i < CUSTOMERS; i++)
    {
        /*
         * Sleep until a customer arrives.
         */
        sem_wait(&customers);

        /*
         * Access waiting customer count safely.
         */
        pthread_mutex_lock(&mutex);

        waiting_customers--;

        printf(
            "[BARBER] Calling customer. Waiting customers: %d\n",
            waiting_customers
        );

        /*
         * Signal that the barber is ready.
         */
        sem_post(&barber);

        pthread_mutex_unlock(&mutex);

        /*
         * Simulate haircut.
         */
        printf("[BARBER] Cutting hair...\n");
        usleep(150000);

        printf("[BARBER] Haircut completed.\n\n");
    }

    return NULL;
}

/* -------------------------------------------------
 * Customer
 * ------------------------------------------------- */
static void *customer_thread(void *arg)
{
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);

    /*
     * Check whether a waiting chair is available.
     */
    if (waiting_customers < CHAIRS)
    {
        waiting_customers++;

        printf(
            "[CUSTOMER %d] Waiting. Customers in queue: %d\n",
            id,
            waiting_customers
        );

        /*
         * Notify the barber that a customer is waiting.
         */
        sem_post(&customers);

        pthread_mutex_unlock(&mutex);

        /*
         * Wait until barber is ready.
         */
        sem_wait(&barber);

        printf(
            "[CUSTOMER %d] Getting haircut.\n",
            id
        );
    }
    else
    {
        /*
         * No waiting chair available.
         */
        printf(
            "[CUSTOMER %d] No chair available. Leaving.\n",
            id
        );

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* -------------------------------------------------
 * Sleeping Barber Demo
 * ------------------------------------------------- */
void run_sleeping_barber_demo(void)
{
    pthread_t barber_thread_id;
    pthread_t customer_threads[CUSTOMERS];

    int customer_ids[CUSTOMERS];

    waiting_customers = 0;

    sem_init(&customers, 0, 0);
    sem_init(&barber, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    printf("\n");
    printf("====================================\n");
    printf("       SLEEPING BARBER DEMO\n");
    printf("====================================\n");

    printf("Waiting chairs: %d\n", CHAIRS);
    printf("Customers: %d\n\n", CUSTOMERS);

    /*
     * Start barber.
     */
    pthread_create(
        &barber_thread_id,
        NULL,
        barber_thread,
        NULL
    );

    /*
     * Create customers.
     */
    for (int i = 0; i < CUSTOMERS; i++)
    {
        customer_ids[i] = i + 1;

        pthread_create(
            &customer_threads[i],
            NULL,
            customer_thread,
            &customer_ids[i]
        );

        /*
         * Small delay so that the queue behavior
         * can be observed clearly.
         */
        usleep(50000);
    }

    /*
     * Wait for all customers.
     */
    for (int i = 0; i < CUSTOMERS; i++)
    {
        pthread_join(
            customer_threads[i],
            NULL
        );
    }

    /*
     * Wait for barber to finish all accepted customers.
     *
     * The barber exits after processing CUSTOMERS
     * semaphore signals, but customers who leave
     * do not generate a signal.
     *
     * Therefore we need to cancel the barber after
     * all customers have been handled.
     */
    pthread_cancel(barber_thread_id);

    pthread_join(
        barber_thread_id,
        NULL
    );

    sem_destroy(&customers);
    sem_destroy(&barber);

    pthread_mutex_destroy(&mutex);

    printf("\n");
    printf("Sleeping Barber synchronization completed.\n");

    printf("====================================\n");
}