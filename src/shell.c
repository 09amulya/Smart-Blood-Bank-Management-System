#include "sync.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "pcb.h"
#include "blood.h"
#include "scheduler.h"
#include "logger.h"


extern int create_process(const char *patient,
                          const char *blood_group,
                          int units,
                          Priority priority);


/* -------------------------------------------------
 * Parse Priority
 * ------------------------------------------------- */

static Priority parse_priority(const char *text)
{
    if (strcasecmp(text, "critical") == 0)
        return PRIO_CRITICAL;

    if (strcasecmp(text, "urgent") == 0)
        return PRIO_URGENT;

    return PRIO_NORMAL;
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
 * Process Table
 * ------------------------------------------------- */

static void print_process_table(void)
{
    int i;

    printf("\n================ PROCESS TABLE ================\n");

    if (process_count == 0) {
        printf("No processes created.\n");
        return;
    }

    printf("%-5s %-15s %-6s %-6s %-10s %-12s\n",
           "PID",
           "PATIENT",
           "GROUP",
           "UNITS",
           "PRIORITY",
           "STATE");

    printf("-------------------------------------------------------------\n");

    for (i = 0; i < process_count; i++) {

        PCB *p = &process_table[i];

        printf("%-5d %-15s %-6s %-6d %-10s %-12s\n",
               p->pid,
               p->patient,
               p->blood_group,
               p->units,
               priority_name(p->priority),
               state_name(p->state));
    }

    printf("=============================================================\n");
}


/* -------------------------------------------------
 * Request Handler
 * ------------------------------------------------- */

static void handle_request(char *args)
{
    char patient[32];
    char group[4];
    char priority_text[16];

    int units;

    if (sscanf(args,
               "%31s %3s %d %15s",
               patient,
               group,
               &units,
               priority_text) != 4) {

        printf("Usage:\n");
        printf("request <patient> <group> <units> <priority>\n");

        printf("Example:\n");
        printf("request Rahul O- 2 critical\n");

        return;
    }

    if (units <= 0) {
        printf("Units must be greater than 0.\n");
        return;
    }

    Priority priority = parse_priority(priority_text);

    int pid = create_process(
        patient,
        group,
        units,
        priority
    );

    if (pid < 0) {
        printf("Failed to create process.\n");
        return;
    }

    printf("\nBlood request created successfully.\n");

    printf("PID      : %d\n", pid);
    printf("Patient  : %s\n", patient);
    printf("Group    : %s\n", group);
    printf("Units    : %d\n", units);
    printf("Priority : %s\n",
           priority_name(priority));
}


/* -------------------------------------------------
 * Scheduler Handler
 * ------------------------------------------------- */

static void handle_schedule(char *args)
{
    SchedAlgo algorithm;

    if (scheduler_parse(args, &algorithm) != 0) {

        printf("Unknown scheduling algorithm.\n");

        printf("Available algorithms:\n");
        printf("  fcfs\n");
        printf("  priority\n");

        return;
    }

    scheduler_set(algorithm);

    printf("Selected scheduler: %s\n",
           scheduler_name(algorithm));
}


/* -------------------------------------------------
 * Run Scheduler
 * ------------------------------------------------- */

static void handle_run(void)
{
    printf("\nStarting scheduler...\n");

    int executed = scheduler_run(1);

    printf("\nScheduler finished.\n");

    printf("Processes executed: %d\n",
           executed);
}


/* -------------------------------------------------
 * Logs
 * ------------------------------------------------- */

static void handle_logs(void)
{
    show_logs();
}


/* -------------------------------------------------
 * Mutex Demo
 * ------------------------------------------------- */

static void handle_mutex(const char *mode)
{
    if (strcmp(mode, "on") == 0) {

        run_mutex_demo(1);

    } else if (strcmp(mode, "off") == 0) {

        run_mutex_demo(0);

    } else {

        printf("Usage: mutex <on|off>\n");
    }
}


/* -------------------------------------------------
 * Peterson Demo
 * ------------------------------------------------- */

static void handle_peterson(void)
{
    run_peterson_demo();
}


/* -------------------------------------------------
 * Bakery Demo
 * ------------------------------------------------- */

static void handle_bakery(void)
{
    run_bakery_demo();
}


/* -------------------------------------------------
 * Test-and-Set Demo
 * ------------------------------------------------- */

static void handle_test_and_set(void)
{
    run_test_and_set_demo();
}


/* -------------------------------------------------
 * Semaphore Demo
 * ------------------------------------------------- */

static void handle_semaphore(void)
{
    run_semaphore_demo();
}


/* -------------------------------------------------
 * Monitor Demo
 * ------------------------------------------------- */

static void handle_monitor(void)
{
    run_monitor_demo();
}


/* -------------------------------------------------
 * Bounded Buffer Demo
 * ------------------------------------------------- */

static void handle_bounded_buffer(void)
{
    run_bounded_buffer_demo();
}


/* -------------------------------------------------
 * Dining Philosophers Demo
 * ------------------------------------------------- */

static void handle_philosophers(void)
{
    run_philosophers_demo();
}


/* -------------------------------------------------
 * Main
 * ------------------------------------------------- */

int main(void)
{
    char input[256];

    printf("\n");
    printf("========================================\n");
    printf("          RAKTSETU PROTOTYPE\n");
    printf("       Smart Blood Bank OS Model\n");
    printf("========================================\n");

    printf("\nType 'help' to see available commands.\n");

    while (1) {

        printf("\nraktsetu> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;


        /* HELP */
        if (strcmp(input, "help") == 0) {

            print_help();
        }


        /* PROCESS TABLE */
        else if (strcmp(input, "ps") == 0) {

            print_process_table();
        }


        /* BLOOD INVENTORY */
        else if (strcmp(input, "blood") == 0) {

            printf("\n========== BLOOD INVENTORY ==========\n");

            print_inventory();

            printf("=====================================\n");
        }


        /* RUN SCHEDULER */
        else if (strcmp(input, "run") == 0) {

            handle_run();
        }


        /* LOGS */
        else if (strcmp(input, "logs") == 0) {

            handle_logs();
        }


        /* MUTEX */
        else if (strncmp(input, "mutex ", 6) == 0) {

            handle_mutex(input + 6);
        }


        /* PETERSON */
        else if (strcmp(input, "peterson") == 0) {

            handle_peterson();
        }


        /* BAKERY */
        else if (strcmp(input, "bakery") == 0) {

            handle_bakery();
        }


        /* TEST-AND-SET */
        else if (strcmp(input, "testandset") == 0) {

            handle_test_and_set();
        }


        /* SEMAPHORE */
        else if (strcmp(input, "semaphore") == 0) {

            handle_semaphore();
        }


        /* MONITOR */
        else if (strcmp(input, "monitor") == 0) {

            handle_monitor();
        }


        /* BOUNDED BUFFER */
        else if (strcmp(input, "boundedbuffer") == 0) {

            handle_bounded_buffer();
        }


        /* DINING PHILOSOPHERS */
        else if (strcmp(input, "philosophers") == 0) {

            handle_philosophers();
        }


        /* EXIT */
        else if (strcmp(input, "exit") == 0) {

            printf("Shutting down RaktSetu prototype...\n");

            break;
        }


        /* REQUEST */
        else if (strncmp(input, "request ", 8) == 0) {

            handle_request(input + 8);
        }


        /* SCHEDULER */
        else if (strncmp(input, "schedule ", 9) == 0) {

            handle_schedule(input + 9);
        }


        /* UNKNOWN COMMAND */
        else {

            printf("Unknown command: %s\n", input);

            printf("Type 'help' for available commands.\n");
        }
    }

    return 0;
}