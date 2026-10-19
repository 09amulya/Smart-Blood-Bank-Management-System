#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"


/* -------------------------------------------------
 * Monitor Structure
 * ------------------------------------------------- */

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;

} Monitor;


/* -------------------------------------------------
 * Monitor Functions
 * ------------------------------------------------- */

static void monitor_init(Monitor *monitor)
{
    pthread_mutex_init(
        &monitor->mutex,
        NULL
    );

    pthread_cond_init(
        &monitor->condition,
        NULL
    );
}


static void monitor_enter(Monitor *monitor)
{
    pthread_mutex_lock(
        &monitor->mutex
    );
}


static void monitor_leave(Monitor *monitor)
{
    pthread_mutex_unlock(
        &monitor->mutex
    );
}


static void monitor_wait(Monitor *monitor)
{
    pthread_cond_wait(
        &monitor->condition,
        &monitor->mutex
    );
}


static void monitor_signal(Monitor *monitor)
{
    pthread_cond_signal(
        &monitor->condition
    );
}


static void monitor_destroy(Monitor *monitor)
{
    pthread_mutex_destroy(
        &monitor->mutex
    );

    pthread_cond_destroy(
        &monitor->condition
    );
}


/* -------------------------------------------------
 * RaktSetu Monitor
 * ------------------------------------------------- */

static Monitor blood_monitor;

static int blood_stock = 0;

static const int REQUIRED_UNITS = 50;


/* -------------------------------------------------
 * Donation Thread
 * ------------------------------------------------- */

static void *donation_thread(void *arg)
{
    (void)arg;

    sleep(1);

    monitor_enter(&blood_monitor);

    printf(
        "[DONATION] Blood donation received.\n"
    );

    blood_stock += 60;

    printf(
        "[DONATION] Blood stock increased to: %d\n",
        blood_stock
    );

    /*
     * Notify the waiting request.
     */

    monitor_signal(&blood_monitor);

    monitor_leave(&blood_monitor);

    return NULL;
}


/* -------------------------------------------------
 * Blood Request Thread
 * ------------------------------------------------- */

static void *request_thread(void *arg)
{
    (void)arg;

    monitor_enter(&blood_monitor);

    printf(
        "[REQUEST] Request needs %d units.\n",
        REQUIRED_UNITS
    );

    /*
     * The request cannot proceed until
     * enough blood is available.
     */

    while (blood_stock < REQUIRED_UNITS) {

        printf(
            "[REQUEST] Insufficient blood.\n"
        );

        printf(
            "[REQUEST] Process entering WAITING state...\n"
        );

        monitor_wait(&blood_monitor);

        printf(
            "[REQUEST] Process awakened by donation.\n"
        );
    }


    /*
     * Critical section
     */

    blood_stock -= REQUIRED_UNITS;

    printf(
        "[REQUEST] Blood allocated successfully.\n"
    );

    printf(
        "[REQUEST] Remaining blood stock: %d\n",
        blood_stock
    );


    monitor_leave(&blood_monitor);

    return NULL;
}


/* -------------------------------------------------
 * Monitor Demo
 * ------------------------------------------------- */

void run_monitor_demo(void)
{
    pthread_t request;
    pthread_t donation;

    blood_stock = 0;

    monitor_init(
        &blood_monitor
    );


    printf("\n");
    printf("====================================\n");
    printf("          MONITOR DEMO\n");
    printf("====================================\n");

    printf(
        "Initial O- blood stock: %d\n\n",
        blood_stock
    );


    /*
     * Start request first so that it
     * enters the WAITING state.
     */

    pthread_create(
        &request,
        NULL,
        request_thread,
        NULL
    );


    pthread_create(
        &donation,
        NULL,
        donation_thread,
        NULL
    );


    pthread_join(
        request,
        NULL
    );

    pthread_join(
        donation,
        NULL
    );


    printf(
        "\nFinal O- blood stock: %d\n",
        blood_stock
    );

    printf("====================================\n");


    monitor_destroy(
        &blood_monitor
    );
}