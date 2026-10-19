#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "pcb.h"
#include "blood.h"
#include "scheduler.h"
#include "logger.h"
#include "sync.h"

/* -------------------------------------------------
 * Existing process creation function
 * ------------------------------------------------- */
extern int create_process(
    const char *patient,
    const char *blood_group,
    int units,
    Priority priority
);

/* -------------------------------------------------
 * Display process table
 * ------------------------------------------------- */
static void show_processes(void)
{
    if (process_count == 0)
    {
        printf("\nNo processes available.\n");
        return;
    }

    printf("\n");
    printf("==============================================\n");
    printf("              PROCESS TABLE\n");
    printf("==============================================\n");

    printf(
        "%-6s %-15s %-8s %-6s %-12s %-12s\n",
        "PID",
        "PATIENT",
        "GROUP",
        "UNITS",
        "PRIORITY",
        "STATE"
    );

    printf("----------------------------------------------\n");

    for (int i = 0; i < process_count; i++)
    {
        PCB *p = &process_table[i];

        printf(
            "%-6d %-15s %-8s %-6d %-12s %-12s\n",
            p->pid,
            p->patient,
            p->blood_group,
            p->units,
            priority_name(p->priority),
            state_name(p->state)
        );
    }

    printf("==============================================\n");
}

/* -------------------------------------------------
 * Help
 * ------------------------------------------------- */
static void print_help(void)
{
    printf("\n");
    printf("========== RaktSetu Commands ==========\n");

    printf("help                         Show commands\n");

    printf("request <patient> <group> <units> <priority>\n");
    printf("                             Create blood request\n");

    printf("ps                           Show process table\n");
    printf("blood                        Show blood inventory\n");

    printf("schedule <fcfs|priority>     Select scheduler\n");
    printf("run                          Execute READY processes\n");

    printf("logs                         Show process state logs\n");

    printf("mutex <on|off>               Run mutex race demo\n");
    printf("lockvariable                 Run Lock Variable demo\n");
    printf("strict                       Run Strict Alternation demo\n");
    printf("peterson                     Run Peterson's algorithm\n");
    printf("bakery                       Run Lamport Bakery algorithm\n");
    printf("testandset                   Run Test-and-Set algorithm\n");
    printf("semaphore                    Run binary/counting semaphore demo\n");
    printf("monitor                      Run monitor demo\n");
    printf("boundedbuffer                Run producer-consumer demo\n");
    printf("philosophers                 Run Dining Philosophers demo\n");

    printf("exit                         Exit program\n");

    printf("========================================\n");
}

/* -------------------------------------------------
 * Parse priority
 * ------------------------------------------------- */
static Priority parse_priority(const char *text)
{
    if (strcasecmp(text, "critical") == 0)
        return PRIO_CRITICAL;

    if (strcasecmp(text, "urgent") == 0)
        return PRIO_URGENT;

    if (strcasecmp(text, "normal") == 0)
        return PRIO_NORMAL;

    return PRIO_NORMAL;
}

/* -------------------------------------------------
 * Mutex
 * ------------------------------------------------- */
static void handle_mutex(char *mode)
{
    if (strcasecmp(mode, "on") == 0)
        run_mutex_demo(1);
    else if (strcasecmp(mode, "off") == 0)
        run_mutex_demo(0);
    else
        printf("Usage: mutex <on|off>\n");
}

/* -------------------------------------------------
 * Synchronization demos
 * ------------------------------------------------- */
static void handle_lock_variable(void)
{
    run_lock_variable_demo();
}

static void handle_strict_alternation(void)
{
    run_strict_alternation_demo();
}

static void handle_peterson(void)
{
    run_peterson_demo();
}

static void handle_bakery(void)
{
    run_bakery_demo();
}

static void handle_test_and_set(void)
{
    run_test_and_set_demo();
}

static void handle_semaphore(void)
{
    run_semaphore_demo();
}

static void handle_monitor(void)
{
    run_monitor_demo();
}

static void handle_bounded_buffer(void)
{
    run_bounded_buffer_demo();
}

static void handle_philosophers(void)
{
    run_philosophers_demo();
}

/* -------------------------------------------------
 * Main Shell
 * ------------------------------------------------- */
int main(void)
{
    char input[256];

    printf("\n");
    printf("====================================\n");
    printf("       RAKTSETU BLOOD BANK OS\n");
    printf("====================================\n");
    printf("Type 'help' to see available commands.\n");

    while (1)
    {
        printf("\nraktsetu> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        /* -------------------------------------------------
         * Help
         * ------------------------------------------------- */
        if (strcmp(input, "help") == 0)
        {
            print_help();
        }

        /* -------------------------------------------------
         * Exit
         * ------------------------------------------------- */
        else if (strcmp(input, "exit") == 0)
        {
            printf("Exiting RaktSetu...\n");
            break;
        }

        /* -------------------------------------------------
         * Process table
         * ------------------------------------------------- */
        else if (strcmp(input, "ps") == 0)
        {
            show_processes();
        }

        /* -------------------------------------------------
         * Blood inventory
         * ------------------------------------------------- */
        else if (strcmp(input, "blood") == 0)
        {
            print_inventory();
        }

        /* -------------------------------------------------
         * Logs
         * ------------------------------------------------- */
        else if (strcmp(input, "logs") == 0)
        {
            show_logs();
        }

        /* -------------------------------------------------
         * Mutex
         * ------------------------------------------------- */
        else if (strncmp(input, "mutex ", 6) == 0)
        {
            handle_mutex(input + 6);
        }

        /* -------------------------------------------------
         * Lock Variable
         * ------------------------------------------------- */
        else if (strcmp(input, "lockvariable") == 0)
        {
            handle_lock_variable();
        }

        /* -------------------------------------------------
         * Strict Alternation
         * ------------------------------------------------- */
        else if (strcmp(input, "strict") == 0)
        {
            handle_strict_alternation();
        }

        /* -------------------------------------------------
         * Peterson
         * ------------------------------------------------- */
        else if (strcmp(input, "peterson") == 0)
        {
            handle_peterson();
        }

        /* -------------------------------------------------
         * Lamport Bakery
         * ------------------------------------------------- */
        else if (strcmp(input, "bakery") == 0)
        {
            handle_bakery();
        }

        /* -------------------------------------------------
         * Test-and-Set
         * ------------------------------------------------- */
        else if (strcmp(input, "testandset") == 0)
        {
            handle_test_and_set();
        }

        /* -------------------------------------------------
         * Semaphore
         * ------------------------------------------------- */
        else if (strcmp(input, "semaphore") == 0)
        {
            handle_semaphore();
        }

        /* -------------------------------------------------
         * Monitor
         * ------------------------------------------------- */
        else if (strcmp(input, "monitor") == 0)
        {
            handle_monitor();
        }

        /* -------------------------------------------------
         * Bounded Buffer
         * ------------------------------------------------- */
        else if (strcmp(input, "boundedbuffer") == 0)
        {
            handle_bounded_buffer();
        }

        /* -------------------------------------------------
         * Dining Philosophers
         * ------------------------------------------------- */
        else if (strcmp(input, "philosophers") == 0)
        {
            handle_philosophers();
        }

        /* -------------------------------------------------
         * Create process
         * ------------------------------------------------- */
        else if (strncmp(input, "request ", 8) == 0)
        {
            char patient[32];
            char group[4];
            char priority_text[20];
            int units;

            int result = sscanf(
                input + 8,
                "%31s %3s %d %19s",
                patient,
                group,
                &units,
                priority_text
            );

            if (result != 4)
            {
                printf(
                    "Usage: request <patient> <group> <units> <priority>\n"
                );

                printf(
                    "Priority: critical | urgent | normal\n"
                );

                continue;
            }

            Priority priority = parse_priority(priority_text);

            int pid = create_process(
                patient,
                group,
                units,
                priority
            );

            if (pid >= 0)
                printf(
                    "Process created successfully. PID: %d\n",
                    pid
                );
            else
                printf("Failed to create process.\n");
        }

        /* -------------------------------------------------
         * Scheduler selection
         * ------------------------------------------------- */
        else if (strncmp(input, "schedule ", 9) == 0)
        {
            SchedAlgo algo;

            if (scheduler_parse(input + 9, &algo) == 0)
            {
                scheduler_set(algo);

                printf(
                    "Scheduler set to %s.\n",
                    scheduler_name(algo)
                );
            }
            else
            {
                printf(
                    "Usage: schedule <fcfs|priority>\n"
                );
            }
        }

        /* -------------------------------------------------
         * Run scheduler
         * ------------------------------------------------- */
        else if (strcmp(input, "run") == 0)
        {
            int executed = scheduler_run(1);

            printf(
                "Scheduler executed %d process(es).\n",
                executed
            );
        }

        /* -------------------------------------------------
         * Unknown command
         * ------------------------------------------------- */
        else
        {
            printf("Unknown command: %s\n", input);
            printf("Type 'help' to see available commands.\n");
        }
    }

    return 0;
}