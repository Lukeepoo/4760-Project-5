#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

FILE *log_fp = NULL;

double *shared_clock = NULL;

void increment_clock() {
    if (shared_clock) {
        shared_clock[0] += 0.001; // Increment by 1 millisecond
        printf("DEBUG: Clock incremented to %.6f\n", shared_clock[0]);
    } else {
        printf("ERROR: Shared clock not initialized!\n");
    }
}

double get_simulated_time() {
    if (shared_clock) {
        printf("DEBUG: Simulated time: %.6f\n", shared_clock[0]);
        return shared_clock[0];
    }
    printf("ERROR: Shared clock not initialized!\n");
    return 0.0;
}

void cleanup_resources(int clock_shmid) {
    if (shared_clock) {
        shmdt(shared_clock);
    }
    shmctl(clock_shmid, IPC_RMID, NULL);
    printf("DEBUG: Cleaned up shared memory resources.\n");
}

void parse_arguments(int argc, char *argv[], double *sim_duration, int *max_processes, double *log_interval, char *log_file) {
    int opt;
    while ((opt = getopt(argc, argv, "ht:n:i:l:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: ./oss [OPTIONS]\n");
                printf("Options:\n");
                printf("  -h           Display this help message and exit.\n");
                printf("  -t SECONDS   Set the total simulation time (default: 10.0 seconds).\n");
                printf("  -n NUMBER    Set the maximum number of processes (default: 10, max: 18).\n");
                printf("  -i SECONDS   Set the logging interval in seconds (default: 1.0).\n");
                printf("  -l FILE      Set the name of the log file (default: output.log).\n");
                exit(EXIT_SUCCESS);
            case 't':
                *sim_duration = atof(optarg);
                break;
            case 'n':
                *max_processes = atoi(optarg);
                if (*max_processes > 18) {
                    fprintf(stderr, "Error: Maximum number of processes cannot exceed 18.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                *log_interval = atof(optarg);
                break;
            case 'l':
                strncpy(log_file, optarg, 255);
                log_file[255] = '\0';
                break;
            default:
                fprintf(stderr, "Invalid option. Use -h for help.\n");
                exit(EXIT_FAILURE);
        }
    }
}
