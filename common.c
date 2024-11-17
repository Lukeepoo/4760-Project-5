#include "common.h"
#include <signal.h>
#include "resource.h"
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

extern double sim_duration;

static double *shared_clock;
static ResourceDescriptor resources[MAX_RESOURCES];

void setup_shared_memory(int *clock_shmid, int *resource_shmid) {
    // Create shared memory for the clock
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
    printf("Shared clock initialized: %f\n", shared_clock[0]);
}

void increment_clock() {
    if (shared_clock) {
        shared_clock[0] += 0.01; // Increment the clock by 10ms
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

    printf("Terminating all child processes...\n");
    for (int i = 0; i < MAX_CHILDREN; i++) {
        kill(0, SIGTERM);
    }

    if (shared_clock != NULL) {
        shmdt(shared_clock);
        printf("Shared clock memory detached.\n");
    }

    shmctl(clock_shmid, IPC_RMID, NULL);
    printf("Shared clock memory removed.\n");
}

void parse_arguments(int argc, char *argv[], double *sim_duration) {
    *sim_duration = 5.0; // Default duration
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: ./oss [-h help] [-s duration in seconds]\n");
            exit(0);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            *sim_duration = atof(argv[++i]);
            if (*sim_duration <= 0) {
                fprintf(stderr, "Error: Invalid simulation duration '%s'\n", argv[i]);
                exit(1);
            }
            printf("Simulation duration set to %f seconds.\n", *sim_duration);
        }
    }
}

void fork_child_process() {
    if (fork() == 0) {
        execl("./user_proc", "./user_proc", NULL);
        perror("Failed to exec user_proc");
        exit(1);
    }
}

void process_requests() {
    printf("Processing resource requests...\n");

    for (int process_id = 0; process_id < MAX_PROCESSES; process_id++) {
        int resource_id = rand() % MAX_RESOURCES;

        if (resources[resource_id].available > 0) {
            resources[resource_id].allocated[process_id]++;
            resources[resource_id].available--;
            printf("OSS: Granted resource %d to process %d.\n", resource_id, process_id);
        } else {
            resources[resource_id].requested[process_id]++;
            printf("OSS: Blocked resource %d request by process %d.\n", resource_id, process_id);
        }
    }
}

void log_resource_status() {
    printf("Logging resource status...\n");

    for (int i = 0; i < MAX_RESOURCES; i++) {
        printf("Resource %d: Available = %d\n", i, resources[i].available);

        for (int j = 0; j < MAX_PROCESSES; j++) {
            if (resources[i].allocated[j] > 0) {
                printf("  Allocated to process %d: %d instances\n", j, resources[i].allocated[j]);
            }
        }
    }
}

bool detect_deadlock() {
    for (int i = 0; i < MAX_RESOURCES; i++) {
        int total_requested = 0;
        for (int j = 0; j < MAX_PROCESSES; j++) {
            total_requested += resources[i].requested[j];
        }
        if (total_requested > resources[i].available) {
            return true;
        }
    }
    return false;
}

void resolve_deadlock() {
    int terminated_process = rand() % MAX_PROCESSES;

    printf("OSS: Detected deadlock involving process %d. Terminating to resolve deadlock.\n", terminated_process);

    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].available += resources[i].allocated[terminated_process];
        resources[i].allocated[terminated_process] = 0;
        resources[i].requested[terminated_process] = 0;
    }
}

void log_deadlock_resolution() {
    printf("Logging deadlock resolution...\n");
}
