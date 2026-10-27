#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int blood_stock = 100;

pthread_mutex_t stock_mutex = PTHREAD_MUTEX_INITIALIZER;

void *request_thread(void *arg)
{
    int use_mutex = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        if (use_mutex)
            pthread_mutex_lock(&stock_mutex);

        int current = blood_stock;

        usleep(1000);

        blood_stock = current - 10;

        printf("[REQUEST] Blood stock: %d\n", blood_stock);

        if (use_mutex)
            pthread_mutex_unlock(&stock_mutex);
    }

    return NULL;
}

void *donation_thread(void *arg)
{
    int use_mutex = *(int *)arg;

    for (int i = 0; i < 5; i++) {

        if (use_mutex)
            pthread_mutex_lock(&stock_mutex);

        int current = blood_stock;

        usleep(1000);

        blood_stock = current + 20;

        printf("[DONATION] Blood stock: %d\n", blood_stock);

        if (use_mutex)
            pthread_mutex_unlock(&stock_mutex);
    }

    return NULL;
}

void run_mutex_demo(int use_mutex)
{
    pthread_t request;
    pthread_t donation;

    blood_stock = 100;

    printf("\n====================================\n");
    printf("     RAKTSETU SYNCHRONIZATION\n");
    printf("====================================\n");

    if (use_mutex)
        printf("Mode: MUTEX ON\n");
    else
        printf("Mode: MUTEX OFF\n");

    printf("Initial O- blood stock: %d\n\n", blood_stock);

    pthread_create(&request, NULL, request_thread, &use_mutex);
    pthread_create(&donation, NULL, donation_thread, &use_mutex);

    pthread_join(request, NULL);
    pthread_join(donation, NULL);

    printf("\nFinal O- blood stock: %d\n", blood_stock);
    printf("====================================\n");
}