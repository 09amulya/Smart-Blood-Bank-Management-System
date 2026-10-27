#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sync.h"

#define PHILOSOPHERS 5
#define EATING_ROUNDS 3


static pthread_mutex_t forks[PHILOSOPHERS];

static void think(int id)
{
    printf("[PHILOSOPHER %d] Thinking...\n", id);

    usleep(100000);
}


static void eat(int id)
{
    printf("[PHILOSOPHER %d] Eating...\n", id);

    usleep(150000);
}


/*
 * Deadlock avoidance:
 *
 * Always acquire the lower-numbered fork first.
 *
 * Example:
 * Philosopher 3 needs forks 3 and 4
 * -> takes fork 3 first
 * -> then fork 4
 *
 * Philosopher 4 needs forks 4 and 0
 * -> takes fork 0 first
 * -> then fork 4
 *
 * Therefore, circular wait is avoided.
 */

static void *philosopher(void *arg)
{
    int id = *(int *)arg;

    int left = id;
    int right = (id + 1) % PHILOSOPHERS;

    int first;
    int second;


    if (left < right) {
        first = left;
        second = right;
    } else {
        first = right;
        second = left;
    }


    for (int round = 0; round < EATING_ROUNDS; round++) {

        think(id);


        printf(
            "[PHILOSOPHER %d] Wants forks %d and %d\n",
            id,
            left,
            right
        );


        pthread_mutex_lock(&forks[first]);

        printf(
            "[PHILOSOPHER %d] Picked fork %d\n",
            id,
            first
        );


        pthread_mutex_lock(&forks[second]);

        printf(
            "[PHILOSOPHER %d] Picked fork %d\n",
            id,
            second
        );


        eat(id);


        pthread_mutex_unlock(&forks[second]);

        pthread_mutex_unlock(&forks[first]);


        printf(
            "[PHILOSOPHER %d] Released forks %d and %d\n",
            id,
            left,
            right
        );
    }


    printf(
        "[PHILOSOPHER %d] Finished dining.\n",
        id
    );

    return NULL;
}


void run_philosophers_demo(void)
{
    pthread_t threads[PHILOSOPHERS];

    int ids[PHILOSOPHERS];


    for (int i = 0; i < PHILOSOPHERS; i++) {

        pthread_mutex_init(
            &forks[i],
            NULL
        );

        ids[i] = i;
    }


    printf("\n");
    printf("====================================\n");
    printf("       DINING PHILOSOPHERS\n");
    printf("====================================\n");

    printf(
        "Philosophers: %d\n",
        PHILOSOPHERS
    );

    printf(
        "Eating rounds: %d\n",
        EATING_ROUNDS
    );

    printf(
        "Deadlock prevention: Resource Ordering\n\n"
    );


    for (int i = 0; i < PHILOSOPHERS; i++) {

        pthread_create(
            &threads[i],
            NULL,
            philosopher,
            &ids[i]
        );
    }


    for (int i = 0; i < PHILOSOPHERS; i++) {

        pthread_join(
            threads[i],
            NULL
        );
    }


    for (int i = 0; i < PHILOSOPHERS; i++) {

        pthread_mutex_destroy(
            &forks[i]
        );
    }


    printf("\n");
    printf("All philosophers finished dining.\n");
    printf("Deadlock avoided successfully.\n");
    printf("====================================\n");
}