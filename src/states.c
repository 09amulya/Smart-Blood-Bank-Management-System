#include <stdio.h>

#include "pcb.h"


void set_state(PCB *p, ProcessState new_state)
{
    if (p == NULL)
        return;


    ProcessState old_state = p->state;


    switch (old_state)
    {
        case NEW:

            if (new_state != READY)
                return;

            break;


        case READY:

            if (new_state != RUNNING &&
                new_state != WAITING)
                return;

            break;


        case RUNNING:

            if (new_state != WAITING &&
                new_state != TERMINATED)
                return;

            break;


        case WAITING:

            if (new_state != READY &&
                new_state != TERMINATED)
                return;

            break;


        case TERMINATED:

            return;
    }


    p->state = new_state;


    printf(
        "[STATE] P%d: %s -> %s\n",
        p->pid,
        state_name(old_state),
        state_name(new_state)
    );
}