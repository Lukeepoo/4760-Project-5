#include "resource.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

ResourceDescriptor resources[MAX_RESOURCES];

void initialize_resources() {
    printf("DEBUG: Initializing resources...\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].available = rand() % MAX_INSTANCES + 1; // Random available instances
        for (int j = 0; j < MAX_PROCESSES; j++) {
            resources[i].allocated[j] = 0;
            resources[i].requested[j] = (rand() % 2) ? (rand() % MAX_INSTANCES + 1) : 0; // Random requests or none
        }
    }
}

void process_requests(int *no_progress_cycles) {
    bool progress_made = false;

    fprintf(log_fp, "Processing resource requests...\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            fprintf(log_fp, "Process %d requests Resource %d: %d units.\n", i, j, resources[j].requested[i]);
            if (resources[j].requested[i] > 0) {
                if (resources[j].available > 0) {
                    resources[j].allocated[i]++;
                    resources[j].available--;
                    resources[j].requested[i]--;
                    progress_made = true;
                    fprintf(log_fp, "Granted resource %d to process %d.\n", j, i);
                } else {
                    fprintf(log_fp, "Resource %d requested by process %d is unavailable.\n", j, i);
                }
            }
        }
    }

    if (!progress_made) {
        fprintf(log_fp, "No progress made during this cycle.\n");
    } else {
        fprintf(log_fp, "Progress made during this cycle.\n");
        *no_progress_cycles = 0; // Reset when progress is made
    }
    fflush(log_fp);
}

void log_resource_status() {
    fprintf(log_fp, "Logging resource status...\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        fprintf(log_fp, "Resource %d available: %d\n", i, resources[i].available);
        for (int j = 0; j < MAX_PROCESSES; j++) {
            if (resources[i].allocated[j] > 0) {
                fprintf(log_fp, "  Allocated to process %d: %d\n", j, resources[i].allocated[j]);
            }
        }
    }
    fflush(log_fp);
}

void log_deadlock_resolution() {
    fprintf(log_fp, "Deadlock resolution logged.\n");
    fflush(log_fp);
}

bool detect_and_resolve_deadlock() {
    fprintf(log_fp, "DEBUG: Checking for deadlock...\n");

    bool deadlock_detected = false;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        bool process_deadlocked = true;

        for (int j = 0; j < MAX_RESOURCES; j++) {
            if (resources[j].requested[i] == 0 || resources[j].available > 0) {
                process_deadlocked = false;
                break;
            }
        }

        if (process_deadlocked) {
            fprintf(log_fp, "DEBUG: Deadlock detected for process %d.\n", i);
            deadlock_detected = true;

            for (int j = 0; j < MAX_RESOURCES; j++) {
                resources[j].available += resources[j].allocated[i];
                resources[j].allocated[i] = 0;
            }

            fprintf(log_fp, "DEBUG: Released resources for process %d.\n", i);
        }
    }

    fflush(log_fp);
    return deadlock_detected;
}
