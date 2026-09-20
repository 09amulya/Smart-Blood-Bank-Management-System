/* blood.h - Blood resource interface (DRAFT - owned by blood.c, Ananya) */
#ifndef BLOOD_H
#define BLOOD_H

int  check_blood(const char *group, int units);    /* 1 = available, 0 = not */
int  allocate_blood(const char *group, int units); /* 1 = allocated, 0 = shortage */
void release_blood(const char *group, int units);
void print_inventory(void);

#endif
