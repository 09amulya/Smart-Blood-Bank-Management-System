/* pcb.h - Shared PCB definitions (DRAFT - finalize with the team on Day 1)
 * RaktSetu Mini OS Prototype - Group 2
 */
#ifndef PCB_H
#define PCB_H

#define MAX_PROCESSES 32

typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED } ProcessState;

/* Lower value = higher priority */
typedef enum { PRIO_CRITICAL = 1, PRIO_URGENT = 2, PRIO_NORMAL = 3 } Priority;

typedef struct {
    int pid;
    char patient[32];
    char blood_group[4];      /* "A+", "O-", "AB+" ... */
    int units;
    Priority priority;
    ProcessState state;

    int arrival_time;
    int burst_time;           /* burst = units * 3 */
    int remaining_time;

    /* filled by the scheduler */
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
} PCB;

/* Process table - owned by process.c (Aashini) */
extern PCB process_table[MAX_PROCESSES];
extern int process_count;

/* State transitions - owned by states.c (Amulya) */
void set_state(PCB *p, ProcessState new_state);

static inline const char *state_name(ProcessState s) {
    switch (s) {
        case NEW:        return "NEW";
        case READY:      return "READY";
        case RUNNING:    return "RUNNING";
        case WAITING:    return "WAITING";
        case TERMINATED: return "TERMINATED";
    }
    return "?";
}

static inline const char *priority_name(Priority p) {
    switch (p) {
        case PRIO_CRITICAL: return "CRITICAL";
        case PRIO_URGENT:   return "URGENT";
        case PRIO_NORMAL:   return "NORMAL";
    }
    return "?";
}

#endif
