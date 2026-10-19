#include <stdio.h>
#include <pthread.h>

#include "sync.h"


/* -------------------------------------------------
 * Custom Semaphore Structure
 * ------------------------------------------------- */

typedef struct
{
    int value;

    pthread_mutex_t mutex;
    pthread_cond_t condition;

} Semaphore;


/* -------------------------------------------------
 * Semaphore Initialize
 * ------------------------------------------------- */

static void semaphore_init(Semaphore *sem, int value)
{
    sem->value = value;

    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->condition, NULL);
}


/* -------------------------------------------------
 * Semaphore Wait / P
 * ------------------------------------------------- */

static void semaphore_wait(Semaphore *sem)
{
    pthread_mutex_lock(&sem->mutex);

    while (sem->value == 0) {
        pthread_cond_wait(
            &sem->condition,
            &sem->mutex
        );
    }

    sem->value--;

    pthread_mutex_unlock(&sem->mutex);
}


/* -------------------------------------------------
 * Semaphore Signal / V
 * ------------------------------------------------- */

static void semaphore_signal(Semaphore *sem)
{
    pthread_mutex_lock(&sem->mutex);

    sem->value++;

    pthread_cond_signal(
        &sem->condition
    );

    pthread_mutex_unlock(&sem->mutex);
}


/* -------------------------------------------------
 * Semaphore Destroy
 * ------------------------------------------------- */

static void semaphore_destroy(Semaphore *sem)
{
    pthread_mutex_destroy(&sem->mutex);

    pthread_cond_destroy(&sem->condition);
}


/* -------------------------------------------------
 * Shared Blood Stock
 * ------------------------------------------------- */

static int blood_stock = 100;


/* -------------------------------------------------
 * Binary Semaphore Demo
 *
 * Initial value = 1
 *
 * Only one thread can enter the
 * critical section at a time.
 * ------------------------------------------------- */

static Semaphore binary_sem;


static void *binary_request_thread(void *arg)
{
    (void)arg;

    for (int i = 0; i < 5; i++) {

        semaphore_wait(&binary_sem);

        /*
         * Critical Section
         */

        blood_stock -= 10;

        printf(
            "[BINARY] REQUEST  -> Blood stock: %d\n",
            blood_stock
        );

        semaphore_signal(&binary_sem);
    }

    return NULL;
}


static void *binary_donation_thread(void *arg)
{
    (void)arg;

    for (int i = 0; i < 5; i++) {

        semaphore_wait(&binary_sem);

        /*
         * Critical Section
         */

        blood_stock += 20;

        printf(
            "[BINARY] DONATION -> Blood stock: %d\n",
            blood_stock
        );

        semaphore_signal(&binary_sem);
    }

    return NULL;
}


static void run_binary_semaphore_demo(void)
{
    pthread_t request_thread;
    pthread_t donation_thread;

    blood_stock = 100;

    semaphore_init(
        &binary_sem,
        1
    );

    printf("\n");
    printf("------------------------------------\n");
    printf("       BINARY SEMAPHORE DEMO\n");
    printf("------------------------------------\n");

    printf(
        "Initial O- blood stock: %d\n\n",
        blood_stock
    );


    pthread_create(
        &request_thread,
        NULL,
        binary_request_thread,
        NULL
    );

    pthread_create(
        &donation_thread,
        NULL,
        binary_donation_thread,
        NULL
    );


    pthread_join(
        request_thread,
        NULL
    );

    pthread_join(
        donation_thread,
        NULL
    );


    printf(
        "\nFinal O- blood stock: %d\n",
        blood_stock
    );

    printf("------------------------------------\n");


    semaphore_destroy(&binary_sem);
}


/* -------------------------------------------------
 * Counting Semaphore Demo
 *
 * Initial value = 2
 *
 * At most two threads can access
 * the protected resource simultaneously.
 * ------------------------------------------------- */

static Semaphore counting_sem;

static int active_requests = 0;


static void *counting_thread(void *arg)
{
    int id = *(int *)arg;

    semaphore_wait(&counting_sem);

    active_requests++;

    printf(
        "[COUNTING] Thread %d entered resource. "
        "Active threads: %d\n",
        id,
        active_requests
    );


    /*
     * Simulated resource usage
     */

    for (volatile long i = 0; i < 5000000; i++);


    active_requests--;

    printf(
        "[COUNTING] Thread %d leaving resource. "
        "Active threads: %d\n",
        id,
        active_requests
    );


    semaphore_signal(&counting_sem);

    return NULL;
}


static void run_counting_semaphore_demo(void)
{
    pthread_t threads[4];

    int ids[4] = {
        1, 2, 3, 4
    };


    active_requests = 0;

    semaphore_init(
        &counting_sem,
        2
    );


    printf("\n");
    printf("------------------------------------\n");
    printf("      COUNTING SEMAPHORE DEMO\n");
    printf("------------------------------------\n");

    printf(
        "Semaphore capacity: 2\n"
    );

    printf(
        "Four threads will share the resource.\n\n"
    );


    for (int i = 0; i < 4; i++) {

        pthread_create(
            &threads[i],
            NULL,
            counting_thread,
            &ids[i]
        );
    }


    for (int i = 0; i < 4; i++) {

        pthread_join(
            threads[i],
            NULL
        );
    }


    printf("\nCounting semaphore demo completed.\n");

    printf("------------------------------------\n");


    semaphore_destroy(&counting_sem);
}


/* -------------------------------------------------
 * Semaphore Demonstration
 * ------------------------------------------------- */

void run_semaphore_demo(void)
{
    printf("\n");
    printf("====================================\n");
    printf("       SEMAPHORE DEMONSTRATION\n");
    printf("====================================\n");

    run_binary_semaphore_demo();

    run_counting_semaphore_demo();

    printf("\n");
    printf("====================================\n");
    printf("       SEMAPHORE DEMO COMPLETE\n");
    printf("====================================\n");
}