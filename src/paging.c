
#include <stdio.h>
#include "paging.h"

/*
 * RaktSetu Mini OS
 * Unit 4: Memory Management
 *
 * Demonstrations:
 * 1. Paging
 * 2. Segmentation
 * 3. Segmented Paging
 */

#define PAGE_SIZE 100
#define MAX_SEGMENTS 3
#define MAX_PAGES 4

/* -------------------- 1. PAGING -------------------- */

void paging_demo(void)
{
    int page_table[] = {3, 1, 4, 0};
    int number_of_pages = 4;
    int logical_address = 250;

    int page_number = logical_address / PAGE_SIZE;
    int offset = logical_address % PAGE_SIZE;

    printf("\n========== PAGING DEMO ==========\n");
    printf("Page size: %d bytes\n", PAGE_SIZE);

    printf("\nPage Table:\n");
    printf("Page Number\tFrame Number\n");

    for (int i = 0; i < number_of_pages; i++)
    {
        printf("%d\t\t%d\n", i, page_table[i]);
    }

    printf("\nLogical Address: %d\n", logical_address);
    printf("Page Number: %d\n", page_number);
    printf("Offset: %d\n", offset);

    if (page_number >= 0 && page_number < number_of_pages)
    {
        int frame_number = page_table[page_number];
        int physical_address =
            frame_number * PAGE_SIZE + offset;

        printf("Frame Number: %d\n", frame_number);
        printf("Physical Address: %d\n", physical_address);
    }
    else
    {
        printf("Error: Invalid logical address.\n");
    }

    printf("=================================\n");
}

/* ---------------- 2. SEGMENTATION ---------------- */

void segmentation_demo(void)
{
    int base[MAX_SEGMENTS] = {1000, 2500, 4000};
    int limit[MAX_SEGMENTS] = {500, 300, 700};

    int segment_number = 1;
    int offset = 200;

    printf("\n======= SEGMENTATION DEMO =======\n");

    printf("\nSegment Table:\n");
    printf("Segment\tBase\tLimit\n");

    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        printf("%d\t%d\t%d\n",
               i, base[i], limit[i]);
    }

    printf("\nSegment Number: %d\n", segment_number);
    printf("Offset: %d\n", offset);

    if (segment_number >= 0 &&
        segment_number < MAX_SEGMENTS)
    {
        if (offset >= 0 && offset < limit[segment_number])
        {
            int physical_address =
                base[segment_number] + offset;

            printf("Physical Address: %d\n",
                   physical_address);
        }
        else
        {
            printf("Error: Offset exceeds segment limit.\n");
        }
    }
    else
    {
        printf("Error: Invalid segment number.\n");
    }

    printf("=================================\n");
}

/* --------------- 3. SEGMENTED PAGING --------------- */

void segmented_paging_demo(void)
{
    /*
     * Each segment has its own page table.
     * segment_page_count tells how many pages
     * are valid in each segment.
     */
    int page_table[MAX_SEGMENTS][MAX_PAGES] =
    {
        {2, 5, 1, -1},
        {4, 0, -1, -1},
        {3, 6, 7, 2}
    };

    int segment_page_count[MAX_SEGMENTS] = {3, 2, 4};

    int segment_number = 0;
    int logical_offset = 150;

    int page_number = logical_offset / PAGE_SIZE;
    int offset = logical_offset % PAGE_SIZE;

    printf("\n====== SEGMENTED PAGING DEMO ======\n");
    printf("Page size: %d bytes\n", PAGE_SIZE);
    printf("Segment Number: %d\n", segment_number);
    printf("Logical Offset: %d\n", logical_offset);
    printf("Page Number: %d\n", page_number);
    printf("Offset within page: %d\n", offset);

    if (segment_number >= 0 &&
        segment_number < MAX_SEGMENTS)
    {
        if (logical_offset >= 0 &&
            page_number < segment_page_count[segment_number])
        {
            int frame_number =
                page_table[segment_number][page_number];

            if (frame_number >= 0)
            {
                int physical_address =
                    frame_number * PAGE_SIZE + offset;

                printf("Frame Number: %d\n", frame_number);
                printf("Physical Address: %d\n",
                       physical_address);
            }
            else
            {
                printf("Error: Page is not mapped to a frame.\n");
            }
        }
        else
        {
            printf("Error: Offset exceeds segment size.\n");
        }
    }
    else
    {
        printf("Error: Invalid segment number.\n");
    }

    printf("===================================\n");
}
