#include "resource.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>

#define TERMINATE_PROBABILITY 0.05
#define REQUEST_PROBABILITY 0.8

void send_resource_request(int process_id, int resource_id) {
    printf("Process %d: Requested resource %d.\n", process_id, resource_id);
    resources[resource_id].requested[process_id]++;
}

void send_resource_release(int process_id, int resource_id) {
    printf("Process %d: Released resource %d.\n", process_id, resource_id);
    if (resources[resource_id].allocated[process_id] > 0) {
        resources[resource_id].allocated[process_id]--;
        resources[resource_id].available++;
    }
}

int main(int argc, char *argv[]) {
    int process_id = getpid();
    srandom((unsigned)process_id); // Seed randomness with process ID (fix narrowing conversion warning)

    key_t shm_key = ftok("oss", 1);
    int shmid = shmget(shm_key, sizeof(ResourceDescriptor) * MAX_RESOURCES, 0666);
    if (shmid == -1) {
        perror("Error attaching to shared memory");
        exit(1);
    }

    ResourceDescriptor *resources = (ResourceDescriptor *)shmat(shmid, NULL, 0);
    if (resources == (void *)-1) {
        perror("Error attaching shared memory");
        exit(1);
    }

    while (1) {
        double action_chance = (double)random() / RAND_MAX;

        if (action_chance < TERMINATE_PROBABILITY) {
            printf("Process %d: Terminating.\n", process_id);
            break; // Exit the loop to allow the program to reach return 0;
        }
        if (action_chance < (TERMINATE_PROBABILITY + REQUEST_PROBABILITY)) {
            int resource_id = random() % MAX_RESOURCES;
            send_resource_request(process_id, resource_id);
        } else {
            int resource_id = random() % MAX_RESOURCES;
            send_resource_release(process_id, resource_id);
        }

        usleep((random() % 100 + 50) * 1000); // Wait a random time
    }

    printf("Process %d: Detaching shared memory and exiting.\n", process_id);
    shmdt((void *)resources); // Detach shared memory (fixed argument to shmdt)
    return 0; // Now reachable
}
