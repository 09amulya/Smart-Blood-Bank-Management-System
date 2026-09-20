/* scheduler.h - CPU scheduler interface
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef enum { SCHED_FCFS, SCHED_PRIORITY } SchedAlgo;

void        scheduler_set(SchedAlgo algo);
SchedAlgo   scheduler_get(void);
const char *scheduler_name(SchedAlgo algo);

/* Parse "fcfs" / "priority" (case-insensitive). Returns 0 on success, -1 if unknown. */
int scheduler_parse(const char *text, SchedAlgo *out);

/* Execute all READY processes with the selected algorithm.
 *   commit = 1 : real run - allocates blood, changes process states
 *   commit = 0 : preview  - works on copies, nothing is changed,
 *                           resource check skipped (for comparing algorithms)
 * Returns number of processes executed. */
int scheduler_run(int commit);

#endif
