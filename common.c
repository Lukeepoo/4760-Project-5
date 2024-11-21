#include "common.h"
#include "resource.h"
#include <signal.h>
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// Parameters with defaults
int max_processes = 10;        // Default for -n
float sim_duration = 5.0;      // Default for -s
int launch_interval = 100;     // Default for -i
char log_file[256] = "log.txt"; // Default for -f

static double *shared_clock;

void setup_shared_memory(int *clock_shmid, int *resource_shmid) {
    *clock_shmid = shmget(IPC_PRIVATE, sizeof(double), IPC_CREAT | 0666);
    if (*clock_shmid == -1) {
        perror("Failed to allocate shared memory for clock");
        exit(1);
    }

    shared_clock = (double *)shmat(*clock_shmid, NULL, 0);
    if (shared_clock == (void *)-1) {
        perror("Failed to attach shared memory for clock");
        exit(1);
    }

    shared_clock[0] = 0.0; // Initialize the clock
    printf("DEBUG: Shared clock initialized: %f\n", shared_clock[0]);
}

void increment_clock() {
    if (shared_clock) {
        shared_clock[0] += 0.01; // Increment the clock by 10ms
        printf("DEBUG: Clock incremented to: %f\n", shared_clock[0]);
    } else {
        fprintf(stderr, "Error: Shared clock not initialized!\n");
        exit(1);
    }
}

double get_simulated_time() {
    if (shared_clock) {
        return shared_clock[0];
    } else {
        fprintf(stderr, "Error: Shared clock not initialized!\n");
        exit(1);
    }
}

void cleanup_resources(int clock_shmid) {
    printf("Cleaning up resources...\n");

    for (int i = 0; i < MAX_CHILDREN; i++) {
        kill(0, SIGTERM);
    }
    if (shared_clock != NULL) {
        shmdt(shared_clock);
        printf("DEBUG: Shared clock memory detached.\n");
    }
    shmctl(clock_shmid, IPC_RMID, NULL);
    printf("DEBUG: Shared clock memory removed.\n");
}

void parse_arguments(int argc, char *argv[], double *sim_duration, int *max_processes, double *log_interval, char *log_file) {
    // Set default values explicitly before parsing
    *sim_duration = 10.0; // Default simulation duration
    *max_processes = 5;   // Default max processes
    *log_interval = 1.0;  // Default log interval
    strncpy(log_file, "output.log", 255);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            *sim_duration = atof(argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            *max_processes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            *log_interval = atof(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strncpy(log_file, argv[++i], 255);
            log_file[255] = '\0';
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            exit(1);
        }
    }

    // Debug output to verify parsing
    printf("DEBUG: Parsed arguments: sim_duration=%.2f, max_processes=%d, log_interval=%.2f, log_file=%s\n",
           *sim_duration, *max_processes, *log_interval, log_file);
}
