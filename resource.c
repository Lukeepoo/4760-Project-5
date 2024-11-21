#include "resource.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

ResourceDescriptor resources[MAX_RESOURCES];

void initialize_resources() {
    printf("DEBUG: Initializing resources...\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].available = rand() % MAX_INSTANCES + 1;
        printf("DEBUG: Resource %d initialized with %d instances available.\n", i, resources[i].available);
        for (int j = 0; j < MAX_PROCESSES; j++) {
            resources[i].allocated[j] = 0;
            resources[i].requested[j] = 0;
        }
    }
}

bool request_resource(int process_id, int resource_id) {
    if (resources[resource_id].available > 0) {
        resources[resource_id].available--;
        resources[resource_id].allocated[process_id]++;
        printf("DEBUG: Process %d granted resource %d.\n", process_id, resource_id);
        return true;
    }
    printf("DEBUG: Process %d request for resource %d blocked.\n", process_id, resource_id);
    return false;
}

bool detect_and_resolve_deadlock() {
    printf("DEBUG: Starting deadlock detection...\n");

    bool deadlock_detected = false;
    int deadlocked_process = -1;

    // Simulate deadlock detection by iterating over processes and checking for unresolved requests
    for (int i = 0; i < MAX_PROCESSES; i++) {
        bool has_requests = false;

        for (int j = 0; j < MAX_RESOURCES; j++) {
            if (resources[j].requested[i] > 0) {
                has_requests = true;
                break;
            }
        }

        if (has_requests) {
            deadlock_detected = true;
            deadlocked_process = i; // Assume one process is deadlocked for simplicity
            break;
        }
    }

    if (deadlock_detected) {
        printf("DEBUG: Deadlock detected. Terminating process %d to resolve.\n", deadlocked_process);
        release_all_resources(deadlocked_process); // Release all resources held by this process
    } else {
        printf("DEBUG: No deadlock detected.\n");
    }

    return deadlock_detected;
}

void release_all_resources(int process_id) {
    printf("DEBUG: Releasing resources held by process %d.\n", process_id);

    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i].allocated[process_id] > 0) {
            resources[i].available += resources[i].allocated[process_id];
            resources[i].allocated[process_id] = 0;
            printf("DEBUG: Released resource %d from process %d.\n", i, process_id);
        }
    }
}

void process_requests() {
    printf("DEBUG: Processing resource requests...\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            // Simulate processing resource requests
            if (resources[j].requested[i] > 0) {
                if (resources[j].available > 0) {
                    resources[j].allocated[i]++;
                    resources[j].available--;
                    resources[j].requested[i]--;
                    printf("DEBUG: Granted resource %d to process %d.\n", j, i);
                } else {
                    printf("DEBUG: Resource %d requested by process %d is unavailable.\n", j, i);
                }
            }
        }
    }
}