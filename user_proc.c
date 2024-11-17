#include "resource.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define TERMINATE_PROBABILITY 0.05
#define REQUEST_PROBABILITY 0.8

void send_termination_message(int process_id) {
    printf("Process %d: Termination requested.\n", process_id);
    // Simulate sending a termination message to oss
}

void send_resource_request(int process_id, int resource_id) {
    printf("Process %d: Requested resource %d.\n", process_id, resource_id);
    // Simulate sending a message to oss about requesting a resource
}

void send_resource_release(int process_id, int resource_id) {
    printf("Process %d: Released resource %d.\n", process_id, resource_id);
    // Simulate sending a message to oss about releasing a resource
}

int main(int argc, char *argv[]) {
    int process_id = getpid();
    srand(process_id); // Seed randomness with process ID

    while (1) {
        // Simulate process action
        double action_chance = (double)rand() / RAND_MAX;

        if (action_chance < TERMINATE_PROBABILITY) {
            send_termination_message(process_id);
            break;
        } else if (action_chance < REQUEST_PROBABILITY) {
            int resource_id = rand() % MAX_RESOURCES;
            send_resource_request(process_id, resource_id);
        } else {
            int resource_id = rand() % MAX_RESOURCES;
            send_resource_release(process_id, resource_id);
        }

        // Simulate processing time
        usleep((rand() % 100 + 50) * 1000);
    }

    return 0;
}
