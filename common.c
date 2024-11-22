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
    while ((opt = getopt(argc, argv, "t:n:i:l:")) != -1) {
        switch (opt) {
            case 't':
                *sim_duration = atof(optarg);
            break;
            case 'n':
                *max_processes = atoi(optarg);
            break;
            case 'i':
                *log_interval = atof(optarg);
            break;
            case 'l':
                strncpy(log_file, optarg, 255);
            log_file[255] = '\0';
            break;
            default:
                fprintf(stderr, "Usage: ./oss -t sim_duration -n max_processes -i log_interval -l log_file\n");
            exit(EXIT_FAILURE);
        }
    }
}
