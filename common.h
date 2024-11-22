#ifndef COMMON_H
#define COMMON_H
#define DEADLOCK_CHECK_INTERVAL 1.0

extern double *shared_clock;

// Function Prototypes
void setup_shared_memory(int *clock_shmid, int *resource_shmid);
void increment_clock();
double get_simulated_time();
void cleanup_resources(int clock_shmid);
void parse_arguments(int argc, char *argv[], double *sim_duration, int *max_processes, double *log_interval, char *log_file);

#endif // COMMON_H
