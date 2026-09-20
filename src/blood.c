#include <stdio.h>
#include <string.h>
#include "blood.h"

typedef struct {
    const char *group;
    int units;
} BloodStock;

static BloodStock inventory[] = {
    {"A+", 10},
    {"A-", 10},
    {"B+", 10},
    {"B-", 10},
    {"AB+", 10},
    {"AB-", 10},
    {"O+", 10},
    {"O-", 5}
};

#define INVENTORY_SIZE (sizeof(inventory) / sizeof(inventory[0]))

static int find_group(const char *group)
{
    int i;

    if (group == NULL)
        return -1;

    for (i = 0; i < INVENTORY_SIZE; i++) {
        if (strcmp(inventory[i].group, group) == 0)
            return i;
    }

    return -1;
}

int check_blood(const char *group, int units)
{
    int index = find_group(group);

    if (index < 0 || units <= 0)
        return 0;

    return inventory[index].units >= units;
}

int allocate_blood(const char *group, int units)
{
    int index = find_group(group);

    if (index < 0 || units <= 0)
        return 0;

    if (inventory[index].units < units)
        return 0;

    inventory[index].units -= units;
    return 1;
}

void release_blood(const char *group, int units)
{
    int index = find_group(group);

    if (index < 0 || units <= 0)
        return;

    inventory[index].units += units;
}

void print_inventory(void)
{
    int i;

    printf("%-10s %-10s\n", "GROUP", "UNITS");
    printf("--------------------\n");

    for (i = 0; i < INVENTORY_SIZE; i++) {
        printf("%-10s %-10d\n",
               inventory[i].group,
               inventory[i].units);
    }
}