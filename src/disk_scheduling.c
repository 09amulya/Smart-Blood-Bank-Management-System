
#include <stdio.h>
#include <stdlib.h>
#include "disk_scheduling.h"

#define REQUEST_COUNT 8
#define DISK_SIZE 200

static const int requests[REQUEST_COUNT] =
    {98, 183, 37, 122, 14, 124, 65, 67};

static const int initial_head = 53;

/* Print order and calculate total head movement */
static void print_order(const char *name,
                        int order[],
                        int count)
{
    int head = initial_head;
    int movement = 0;

    printf("\n--- %s ---\n", name);
    printf("Initial head position: %d\n", head);
    printf("Seek sequence: %d", head);

    for (int i = 0; i < count; i++)
    {
        movement += abs(order[i] - head);
        head = order[i];
        printf(" -> %d", head);
    }

    printf("\nTotal head movement: %d cylinders\n", movement);
}

static void print_requests(void)
{
    printf("Requests: ");

    for (int i = 0; i < REQUEST_COUNT; i++)
        printf("%d ", requests[i]);

    printf("\n");
}

static void sort_requests(int arr[])
{
    for (int i = 0; i < REQUEST_COUNT - 1; i++)
    {
        for (int j = 0; j < REQUEST_COUNT - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/* 1. First Come First Served */
void fcfs_disk_demo(void)
{
    int order[REQUEST_COUNT];

    for (int i = 0; i < REQUEST_COUNT; i++)
        order[i] = requests[i];

    print_requests();
    print_order("FCFS DISK SCHEDULING", order, REQUEST_COUNT);
}

/* 2. Shortest Seek Time First */
void sstf_disk_demo(void)
{
    int pending[REQUEST_COUNT];
    int order[REQUEST_COUNT];
    int head = initial_head;

    for (int i = 0; i < REQUEST_COUNT; i++)
        pending[i] = requests[i];

    for (int i = 0; i < REQUEST_COUNT; i++)
    {
        int nearest = -1;
        int minimum_distance = DISK_SIZE;

        for (int j = 0; j < REQUEST_COUNT; j++)
        {
            if (pending[j] != -1)
            {
                int distance = abs(pending[j] - head);

                if (distance < minimum_distance)
                {
                    minimum_distance = distance;
                    nearest = j;
                }
            }
        }

        order[i] = pending[nearest];
        head = pending[nearest];
        pending[nearest] = -1;
    }

    print_order("SSTF DISK SCHEDULING", order, REQUEST_COUNT);
}

/* 3. SCAN: initially move towards higher cylinders */
void scan_disk_demo(void)
{
    int sorted[REQUEST_COUNT];
    int order[REQUEST_COUNT + 1];
    int count = 0;

    for (int i = 0; i < REQUEST_COUNT; i++)
        sorted[i] = requests[i];

    sort_requests(sorted);

    for (int i = 0; i < REQUEST_COUNT; i++)
        if (sorted[i] >= initial_head)
            order[count++] = sorted[i];

    /* Reach the disk's upper end before reversing */
    order[count++] = DISK_SIZE - 1;

    for (int i = REQUEST_COUNT - 1; i >= 0; i--)
        if (sorted[i] < initial_head)
            order[count++] = sorted[i];

    print_order("SCAN DISK SCHEDULING", order, count);
}

/* 4. C-SCAN: move upward, jump to zero, continue upward */
void cscan_disk_demo(void)
{
    int sorted[REQUEST_COUNT];
    int order[REQUEST_COUNT + 2];
    int count = 0;

    for (int i = 0; i < REQUEST_COUNT; i++)
        sorted[i] = requests[i];

    sort_requests(sorted);

    for (int i = 0; i < REQUEST_COUNT; i++)
        if (sorted[i] >= initial_head)
            order[count++] = sorted[i];

    order[count++] = DISK_SIZE - 1;
    order[count++] = 0;

    for (int i = 0; i < REQUEST_COUNT; i++)
        if (sorted[i] < initial_head)
            order[count++] = sorted[i];

    print_order("C-SCAN DISK SCHEDULING", order, count);
}

/* 5. LOOK: reverse at the last request, not disk end */
void look_disk_demo(void)
{
    int sorted[REQUEST_COUNT];
    int order[REQUEST_COUNT];
    int count = 0;

    for (int i = 0; i < REQUEST_COUNT; i++)
        sorted[i] = requests[i];

    sort_requests(sorted);

    for (int i = 0; i < REQUEST_COUNT; i++)
        if (sorted[i] >= initial_head)
            order[count++] = sorted[i];

    for (int i = REQUEST_COUNT - 1; i >= 0; i--)
        if (sorted[i] < initial_head)
            order[count++] = sorted[i];

    print_order("LOOK DISK SCHEDULING", order, count);
}
