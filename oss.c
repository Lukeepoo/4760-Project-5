#include "common.h"
#include "resource.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int clock_shmid;
static int resource_shmid;
static double sim_duration; // Duration of simulation
static double end_time;

void signal_handler(int sig) {
    printf("Received signal %d. Cleaning up resources and terminating...\n", sig);
    cleanup_resources(clock_shmid);
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler); // Handle Ctrl+C to clean up
    parse_arguments(argc, argv, &sim_duration);

    setup_shared_memory(&clock_shmid, &resource_shmid);
    end_time = get_simulated_time() + sim_duration;

    initialize_resources();

    double last_deadlock_check = 0.0;
    double last_resource_log = 0.0;

    while (get_simulated_time() < end_time) {
        increment_clock();
        printf("Current simulated time: %f\n", get_simulated_time()); // Debugging output

        process_requests();

        if (get_simulated_time() - last_resource_log >= RESOURCE_LOG_INTERVAL) {
            log_resource_status();
            last_resource_log = get_simulated_time();
        }

        if (get_simulated_time() - last_deadlock_check >= DEADLOCK_CHECK_INTERVAL) {
            if (detect_and_resolve_deadlock()) {
                log_deadlock_resolution();
            }
            last_deadlock_check = get_simulated_time();
        }

        usleep(10000); // Sleep for 10ms to simulate real-time passage
    }

    printf("Simulation time elapsed. Cleaning up...\n");
    cleanup_resources(clock_shmid);
    return 0;
}
