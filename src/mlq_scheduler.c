/*
 * mlq_scheduler.c
 *
 * Multilevel Queue Scheduler with Aging
 *
 * Queue 0 : CRITICAL -> Priority Scheduling
 * Queue 1 : URGENT   -> Round Robin, Quantum = 2
 * Queue 2 : NORMAL   -> FCFS
 *
 * Aging:
 * NORMAL processes waiting for AGING_THRESHOLD time units
 * are promoted to the URGENT queue.
 *
 * Owner: Amulya Pratap Singh
 */

#include <stdio.h>
#include <string.h>

#include "pcb.h"
#include "blood.h"
#include "logger.h"
#include "mlq_scheduler.h"

#define MLQ_QUANTUM       2
#define AGING_THRESHOLD   6

#define QUEUE_CRITICAL    0
#define QUEUE_URGENT      1
#define QUEUE_NORMAL      2

#define MAX_SLOTS (MAX_PROCESSES * 4)

typedef struct
{
    int pid;
    int start;
    int end;
} MLQGanttSlot;


/* ---------------------------------------------------------
   Queue selection
   --------------------------------------------------------- */

/*
 * Returns the current queue of a process.
 *
 * Critical -> Queue 0
 * Urgent   -> Queue 1
 * Normal   -> Queue 2
 *
 * Normal process is promoted to Queue 1 after
 * waiting for AGING_THRESHOLD time units.
 */
static int get_queue_level(
    PCB *p,
    int waiting_time
)
{
    if (p->priority == PRIO_CRITICAL)
        return QUEUE_CRITICAL;

    if (p->priority == PRIO_URGENT)
        return QUEUE_URGENT;

    /* Aging */
    if (waiting_time >= AGING_THRESHOLD)
        return QUEUE_URGENT;

    return QUEUE_NORMAL;
}


/* ---------------------------------------------------------
   Priority comparison for Critical queue
   --------------------------------------------------------- */

static int critical_before(
    PCB *a,
    PCB *b
)
{
    if (a->arrival_time != b->arrival_time)
        return a->arrival_time < b->arrival_time;

    return a->pid < b->pid;
}


/* ---------------------------------------------------------
   Gantt chart
   --------------------------------------------------------- */

static void print_gantt(
    MLQGanttSlot *slots,
    int count
)
{
    int i;

    if (count <= 0)
        return;

    printf("\nGantt Chart:\n");

    for (i = 0; i < count; i++)
        printf("+--------");

    printf("+\n");

    for (i = 0; i < count; i++)
    {
        if (slots[i].pid == 0)
            printf("| IDLE   ");
        else
            printf("| P%-6d", slots[i].pid);
    }

    printf("|\n");

    for (i = 0; i < count; i++)
        printf("+--------");

    printf("+\n");

    for (i = 0; i < count; i++)
        printf("%-9d", slots[i].start);

    printf("%d\n", slots[count - 1].end);
}


/* ---------------------------------------------------------
   Process table
   --------------------------------------------------------- */

static void print_table(
    PCB **done,
    int count
)
{
    int i;

    double total_wt = 0;
    double total_tat = 0;
    double total_rt = 0;

    printf("\n");
    printf("PID   Patient      Group  Units  Priority   AT   BT   ST   CT   WT   TAT   RT\n");
    printf("-------------------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        PCB *p = done[i];

        int response_time =
            p->start_time - p->arrival_time;

        printf(
            "%-5d %-12s %-6s %-6d %-9s %4d %4d %4d %4d %4d %5d %4d\n",
            p->pid,
            p->patient,
            p->blood_group,
            p->units,
            priority_name(p->priority),
            p->arrival_time,
            p->burst_time,
            p->start_time,
            p->completion_time,
            p->waiting_time,
            p->turnaround_time,
            response_time
        );

        total_wt += p->waiting_time;
        total_tat += p->turnaround_time;
        total_rt += response_time;
    }

    printf("-------------------------------------------------------------------------------\n");

    printf(
        "Average Waiting Time    : %.2f\n",
        total_wt / count
    );

    printf(
        "Average Turnaround Time : %.2f\n",
        total_tat / count
    );

    printf(
        "Average Response Time   : %.2f\n",
        total_rt / count
    );
}


/* ---------------------------------------------------------
   MLQ Scheduler
   --------------------------------------------------------- */

int mlq_scheduler_run(int commit)
{
    PCB copies[MAX_PROCESSES];

    PCB *processes[MAX_PROCESSES];
    PCB *done[MAX_PROCESSES];

    int waiting_time[MAX_PROCESSES] = {0};
    int started[MAX_PROCESSES] = {0};
    int finished[MAX_PROCESSES] = {0};
    int aged[MAX_PROCESSES] = {0};

    MLQGanttSlot slots[MAX_SLOTS];

    int n = 0;
    int done_count = 0;
    int slot_count = 0;

    int clock = 0;
    int i;

    printf("\n");
    printf("====================================================\n");
    printf("        MULTILEVEL QUEUE SCHEDULER\n");
    printf("====================================================\n");

    printf("Q0 : CRITICAL -> Priority Scheduling\n");
    printf("Q1 : URGENT   -> Round Robin (Quantum = %d)\n",
           MLQ_QUANTUM);
    printf("Q2 : NORMAL   -> FCFS\n");

    printf(
        "Aging Threshold : %d time units\n",
        AGING_THRESHOLD
    );

    printf("====================================================\n");


    /* -----------------------------------------------------
       Collect READY processes
       ----------------------------------------------------- */

    for (i = 0; i < process_count; i++)
    {
        if (process_table[i].state != READY)
            continue;

        if (commit)
        {
            processes[n] = &process_table[i];
        }
        else
        {
            copies[n] = process_table[i];
            processes[n] = &copies[n];
        }

        /*
         * remaining_time is important for Round Robin.
         */
        processes[n]->remaining_time =
            processes[n]->burst_time;

        n++;
    }


    if (n == 0)
    {
        printf("No READY processes to schedule.\n");
        return 0;
    }


    /*
     * Start clock at earliest arrival.
     */
    clock = processes[0]->arrival_time;

    for (i = 1; i < n; i++)
    {
        if (processes[i]->arrival_time < clock)
            clock = processes[i]->arrival_time;
    }


    /* -----------------------------------------------------
       Main scheduling loop
       ----------------------------------------------------- */

    while (done_count < n)
    {
        int selected = -1;
        int selected_queue = 99;

        /*
         * -------------------------------------------------
         * STEP 1: Update waiting time
         * -------------------------------------------------
         *
         * A process that has arrived and is not running
         * accumulates waiting time.
         */

        for (i = 0; i < n; i++)
        {
            if (finished[i])
                continue;

            if (processes[i]->arrival_time <= clock)
            {
                /*
                 * If process has started previously,
                 * its waiting time is calculated using
                 * actual scheduler waiting.
                 *
                 * We don't increment it here for the
                 * currently running process because the
                 * process selected below hasn't run yet.
                 */
            }
        }


        /*
         * -------------------------------------------------
         * STEP 2: Check aging
         * -------------------------------------------------
         */

        for (i = 0; i < n; i++)
        {
            if (finished[i])
                continue;

            if (processes[i]->priority != PRIO_NORMAL)
                continue;

            if (processes[i]->arrival_time > clock)
                continue;

            /*
             * Approximate accumulated waiting:
             *
             * elapsed time since arrival minus CPU time used.
             */
            int calculated_wait =
                (clock - processes[i]->arrival_time)
                -
                (processes[i]->burst_time -
                 processes[i]->remaining_time);

            if (calculated_wait >= AGING_THRESHOLD)
            {
                waiting_time[i] = calculated_wait;

                if (!aged[i])
                {
                    printf(
                        "[AGING] P%d NORMAL -> URGENT "
                        "(waiting time = %d)\n",
                        processes[i]->pid,
                        calculated_wait
                    );

                    aged[i] = 1;
                }
            }
            else
            {
                waiting_time[i] = calculated_wait;

                if (waiting_time[i] < 0)
                    waiting_time[i] = 0;
            }
        }


        /*
         * -------------------------------------------------
         * STEP 3: Select highest priority queue
         * -------------------------------------------------
         */

        for (i = 0; i < n; i++)
        {
            if (finished[i])
                continue;

            if (processes[i]->arrival_time > clock)
                continue;

            int q =
                get_queue_level(
                    processes[i],
                    waiting_time[i]
                );

            /*
             * Lower queue number = higher priority.
             */
            if (selected == -1 ||
                q < selected_queue)
            {
                selected = i;
                selected_queue = q;
            }
            else if (q == selected_queue)
            {
                /*
                 * Critical queue:
                 * Priority/FCFS style.
                 */
                if (q == QUEUE_CRITICAL)
                {
                    if (critical_before(
                            processes[i],
                            processes[selected]))
                    {
                        selected = i;
                    }
                }

                /*
                 * Normal queue:
                 * FCFS.
                 */
                else if (q == QUEUE_NORMAL)
                {
                    if (processes[i]->arrival_time <
                        processes[selected]->arrival_time)
                    {
                        selected = i;
                    }
                }

                /*
                 * Urgent queue:
                 * Round Robin.
                 *
                 * Since the scheduler scans from the beginning,
                 * the first ready process gets the next quantum.
                 */
            }
        }


        /*
         * -------------------------------------------------
         * STEP 4: No process available
         * -------------------------------------------------
         */

        if (selected == -1)
        {
            int next_arrival = -1;

            for (i = 0; i < n; i++)
            {
                if (finished[i])
                    continue;

                if (next_arrival == -1 ||
                    processes[i]->arrival_time < next_arrival)
                {
                    next_arrival =
                        processes[i]->arrival_time;
                }
            }

            if (next_arrival > clock)
            {
                if (slot_count < MAX_SLOTS)
                {
                    slots[slot_count].pid = 0;
                    slots[slot_count].start = clock;
                    slots[slot_count].end = next_arrival;
                    slot_count++;
                }

                clock = next_arrival;
            }

            continue;
        }


        PCB *p = processes[selected];


        /*
         * -------------------------------------------------
         * STEP 5: First execution
         * -------------------------------------------------
         */

        if (!started[selected])
        {
            started[selected] = 1;

            p->start_time = clock;

            if (commit)
            {
                log_state_change(
                    p->pid,
                    state_name(p->state),
                    "RUNNING"
                );

                set_state(p, RUNNING);


                /*
                 * Allocate blood only once.
                 */
                if (!allocate_blood(
                        p->blood_group,
                        p->units))
                {
                    log_state_change(
                        p->pid,
                        state_name(p->state),
                        "WAITING"
                    );

                    set_state(p, WAITING);

                    printf(
                        "        Reason: "
                        "RESOURCE_UNAVAILABLE "
                        "(%s x %d units)\n",
                        p->blood_group,
                        p->units
                    );

                    finished[selected] = 1;
                    done_count++;

                    continue;
                }

                printf(
                    "        %d unit(s) of %s allocated "
                    "to P%d\n",
                    p->units,
                    p->blood_group,
                    p->pid
                );
            }
        }


        /*
         * -------------------------------------------------
         * STEP 6: Decide execution time
         * -------------------------------------------------
         */

        int execution_time;

        if (selected_queue == QUEUE_URGENT)
        {
            /*
             * Round Robin quantum.
             */
            execution_time = MLQ_QUANTUM;

            if (p->remaining_time < execution_time)
                execution_time = p->remaining_time;
        }
        else
        {
            /*
             * Critical and Normal:
             * run until completion.
             */
            execution_time = p->remaining_time;
        }


        /*
         * -------------------------------------------------
         * STEP 7: Execute process
         * -------------------------------------------------
         */

        int start = clock;

        clock += execution_time;

        p->remaining_time -= execution_time;


        /*
         * Add Gantt entry.
         */
        if (slot_count < MAX_SLOTS)
        {
            slots[slot_count].pid = p->pid;
            slots[slot_count].start = start;
            slots[slot_count].end = clock;

            slot_count++;
        }


        /*
         * -------------------------------------------------
         * STEP 8: Process finished
         * -------------------------------------------------
         */

        if (p->remaining_time <= 0)
        {
            p->remaining_time = 0;

            p->completion_time = clock;

            p->turnaround_time =
                p->completion_time -
                p->arrival_time;

            /*
             * Waiting time =
             * Turnaround - Burst
             */
            p->waiting_time =
                p->turnaround_time -
                p->burst_time;

            if (p->waiting_time < 0)
                p->waiting_time = 0;


            finished[selected] = 1;

            done[done_count] = p;
            done_count++;


            if (commit)
            {
                log_state_change(
                    p->pid,
                    state_name(p->state),
                    "TERMINATED"
                );

                set_state(
                    p,
                    TERMINATED
                );
            }

            printf(
                "[MLQ] P%d completed at time %d\n",
                p->pid,
                clock
            );
        }
        else
        {
            /*
             * Process was preempted because its
             * Round Robin quantum expired.
             */

            printf(
                "[MLQ] P%d quantum expired "
                "(remaining = %d)\n",
                p->pid,
                p->remaining_time
            );


            /*
             * Return process to READY.
             */
            if (commit)
            {
                if (p->state == RUNNING)
                {
                    set_state(
                        p,
                        READY
                    );
                }
            }
        }


        /*
         * -------------------------------------------------
         * STEP 9: Update waiting times
         * -------------------------------------------------
         *
         * Waiting time for unfinished ready processes
         * is derived at the next scheduling decision.
         */
    }


    /*
     * -----------------------------------------------------
     * Output
     * -----------------------------------------------------
     */

    if (done_count > 0)
    {
        printf("\nExecution Order: ");

        for (i = 0; i < done_count; i++)
        {
            printf(
                "P%d%s",
                done[i]->pid,
                i < done_count - 1
                    ? " -> "
                    : "\n"
            );
        }

        print_gantt(
            slots,
            slot_count
        );

        print_table(
            done,
            done_count
        );
    }


    printf("\n");
    printf("====================================================\n");
    printf(
        "MLQ Scheduler completed %d process(es).\n",
        done_count
    );
    printf("====================================================\n");

    return done_count;
}