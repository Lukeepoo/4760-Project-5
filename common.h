#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>

// Time intervals for logging and deadlock checks
#define RESOURCE_LOG_INTERVAL 1.0
#define DEADLOCK_CHECK_INTERVAL 2.0

void parse_arguments(int argc, char *argv[], double *sim_duration);
void setup_shared_memory(int *clock_shmid, int *resource_shmid);
void increment_clock();
double get_simulated_time();
bool can_launch_child();
bool still_children_to_launch();
bool children_in_system();
bool should_check_deadlock();
void fork_child_process();
void cleanup_resources();
void process_requests();
void log_resource_status();
void log_deadlock_resolution();

#endif // COMMON_H
