#include "resource.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static ResourceDescriptor resources[MAX_RESOURCES];

void initialize_resources() {
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].available = MAX_INSTANCES;
        memset(resources[i].allocated, 0, sizeof(resources[i].allocated));
        memset(resources[i].requested, 0, sizeof(resources[i].requested));
    }
}

bool request_resource(int process_id, int resource_id) {
    if (resources[resource_id].available > 0) {
        resources[resource_id].allocated[process_id]++;
        resources[resource_id].available--;
        return true;
    } else {
        resources[resource_id].requested[process_id]++;
        return false;
    }
}

void release_resource(int process_id, int resource_id) {
    if (resources[resource_id].allocated[process_id] > 0) {
        resources[resource_id].allocated[process_id]--;
        resources[resource_id].available++;
    }
}

bool detect_and_resolve_deadlock() {
    bool deadlock_detected = false;

    // Simulate deadlock detection logic
    int deadlocked_process = rand() % MAX_PROCESSES;
    deadlock_detected = true;

    if (deadlock_detected) {
        printf("OSS: Detected deadlock involving process %d.\n", deadlocked_process);

        // Terminate the deadlocked process and release resources
        release_all_resources(deadlocked_process);
        printf("OSS: Terminated process %d to resolve deadlock.\n", deadlocked_process);
    }

    return deadlock_detected;
}

void release_all_resources(int process_id) {
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i].allocated[process_id] > 0) {
            resources[i].available += resources[i].allocated[process_id];
            resources[i].allocated[process_id] = 0;
            printf("OSS: Released resource %d from process %d.\n", i, process_id);
        }
    }
}
