
#include <stdio.h>
#include <string.h>
#include "memory.h"

#define MAX_BLOCKS 10
#define MAX_REQUESTS 10

typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} FitAlgorithm;

static const char *fit_name(FitAlgorithm algorithm)
{
    switch (algorithm) {
        case FIRST_FIT: return "First Fit";
        case BEST_FIT:  return "Best Fit";
        case WORST_FIT: return "Worst Fit";
        default:        return "Unknown";
    }
}

/* Variable partitions: each allocation can split a free block. */
static void run_variable_fit(
    FitAlgorithm algorithm,
    const int initial_blocks[],
    int block_count,
    const int requests[],
    int request_count)
{
    int holes[MAX_BLOCKS];
    int allocation[MAX_REQUESTS];
    int i, j;

    int total_free = 0;
    int largest_hole = 0;

    memcpy(holes, initial_blocks,
           (size_t)block_count * sizeof(int));

    for (i = 0; i < request_count; i++)
        allocation[i] = -1;

    printf("\n--- %s: Variable Partitions ---\n",
           fit_name(algorithm));

    for (i = 0; i < request_count; i++) {
        int chosen = -1;

        for (j = 0; j < block_count; j++) {
            if (holes[j] < requests[i])
                continue;

            if (algorithm == FIRST_FIT) {
                chosen = j;
                break;
            }

            if (chosen == -1 ||
                (algorithm == BEST_FIT &&
                 holes[j] < holes[chosen]) ||
                (algorithm == WORST_FIT &&
                 holes[j] > holes[chosen])) {
                chosen = j;
            }
        }

        if (chosen != -1) {
            allocation[i] = chosen;
            holes[chosen] -= requests[i];
        }
    }

    printf("%-10s %-12s %-12s\n",
           "Request", "Size", "Allocation");

    for (i = 0; i < request_count; i++) {
        if (allocation[i] == -1) {
            printf("R%-9d %-12d Not allocated\n",
                   i + 1, requests[i]);
        } else {
            printf("R%-9d %-12d Block %d\n",
                   i + 1, requests[i],
                   allocation[i] + 1);
        }
    }

    printf("\nRemaining free blocks:\n");

    for (i = 0; i < block_count; i++) {
        printf("Block %d: %d units\n", i + 1, holes[i]);
        total_free += holes[i];

        if (holes[i] > largest_hole)
            largest_hole = holes[i];
    }

    printf("Total free memory: %d units\n", total_free);
    printf("Largest free block: %d units\n", largest_hole);
}

/* Fixed partitions: one request occupies one entire partition. */
void memory_fixed_demo(void)
{
    int partitions[] = {100, 500, 200, 300, 600};
    int requests[] = {212, 417, 112, 426};

    int used[5] = {0};
    int i, j;
    int internal_fragmentation = 0;

    printf("\n--- Fixed Partition Allocation ---\n");

    printf("%-10s %-12s %-12s %-12s\n",
           "Request", "Size", "Partition", "Unused");

    for (i = 0; i < 4; i++) {
        int chosen = -1;

        for (j = 0; j < 5; j++) {
            if (!used[j] && partitions[j] >= requests[i]) {
                chosen = j;
                break;
            }
        }

        if (chosen == -1) {
            printf("R%-9d %-12d Not allocated\n",
                   i + 1, requests[i]);
        } else {
            int unused = partitions[chosen] - requests[i];

            used[chosen] = 1;
            internal_fragmentation += unused;

            printf("R%-9d %-12d P%-11d %-12d\n",
                   i + 1, requests[i], chosen + 1, unused);
        }
    }

    printf("Total internal fragmentation: %d units\n",
           internal_fragmentation);
}

void memory_fit_demo(void)
{
    int blocks[] = {100, 500, 200, 300, 600};
    int requests[] = {212, 417, 112, 426};

    run_variable_fit(FIRST_FIT, blocks, 5, requests, 4);
    run_variable_fit(BEST_FIT, blocks, 5, requests, 4);
    run_variable_fit(WORST_FIT, blocks, 5, requests, 4);
}
