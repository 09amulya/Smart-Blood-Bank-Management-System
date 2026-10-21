
#include <stdio.h>
#include "file_allocation.h"

#define DISK_BLOCKS 20

/* 1. Contiguous Allocation */
void contiguous_allocation_demo(void)
{
    int start_block = 4;
    int file_size = 5;

    printf("\n--- CONTIGUOUS FILE ALLOCATION ---\n");
    printf("Start block: %d\n", start_block);
    printf("File size: %d blocks\n", file_size);
    printf("Allocated blocks: ");

    if (start_block < 0 ||
        file_size <= 0 ||
        start_block + file_size > DISK_BLOCKS)
    {
        printf("Invalid allocation request.\n");
        return;
    }

    for (int i = 0; i < file_size; i++)
        printf("%d ", start_block + i);

    printf("\n");
    printf("All file blocks are stored consecutively.\n");
}

/* 2. Linked Allocation */
void linked_allocation_demo(void)
{
    int next_block[DISK_BLOCKS];

    for (int i = 0; i < DISK_BLOCKS; i++)
        next_block[i] = -1;

    /* Example file: 2 -> 7 -> 4 -> 12 */
    next_block[2] = 7;
    next_block[7] = 4;
    next_block[4] = 12;
    next_block[12] = -1;

    int current = 2;

    printf("\n--- LINKED FILE ALLOCATION ---\n");
    printf("File blocks: ");

    while (current != -1)
    {
        printf("%d", current);
        current = next_block[current];

        if (current != -1)
            printf(" -> ");
    }

    printf(" -> NULL\n");
    printf("Each block points to the next file block.\n");
}

/* 3. Indexed Allocation */
void indexed_allocation_demo(void)
{
    int index_block = 5;
    int data_blocks[] = {2, 8, 11, 15};
    int count = sizeof(data_blocks) / sizeof(data_blocks[0]);

    printf("\n--- INDEXED FILE ALLOCATION ---\n");
    printf("Index block: %d\n", index_block);
    printf("Data blocks stored in index block: ");

    for (int i = 0; i < count; i++)
        printf("%d ", data_blocks[i]);

    printf("\n");
    printf("The index block stores pointers to all data blocks.\n");
}
