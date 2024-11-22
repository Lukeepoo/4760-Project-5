#include "common.h"
#include "resource.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>

static int clock_shmid;
static int resource_shmid;
static double end_time = 10.0;
static int max_processes = 10;
static double log_interval = 1.0;
static char log_file[256] = "output.log";

void signal_handler(int sig) {
    fprintf(stderr, "DEBUG: Received signal %d. Cleaning up resources and terminating...\n", sig);
    if (log_fp) fclose(log_fp);
    cleanup_resources(clock_shmid);
    shmctl(resource_shmid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void setup_shared_memory(int *clock_shmid, int *resource_shmid) {
    key_t shm_key = ftok("oss", 1);
    *resource_shmid = shmget(shm_key, sizeof(ResourceDescriptor) * MAX_RESOURCES, IPC_CREAT | 0666);
    if (*resource_shmid == -1) {
        perror("Error creating shared memory for resources");
        exit(1);
    }

    ResourceDescriptor *resources = shmat(*resource_shmid, NULL, 0);
    if (resources == (void *)-1) {
        perror("Error attaching shared memory for resources");
        exit(1);
    }

    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].available = rand() % MAX_INSTANCES + 1;
        for (int j = 0; j < MAX_PROCESSES; j++) {
            resources[i].allocated[j] = 0;
            resources[i].requested[j] = 0;
        }
    }

    *clock_shmid = shmget(IPC_PRIVATE, sizeof(double), IPC_CREAT | 0666);
    if (*clock_shmid == -1) {
        perror("Error creating shared memory for clock");
        exit(1);
    }

    shared_clock = shmat(*clock_shmid, NULL, 0); // Use global shared_clock
    if (shared_clock == (void *)-1) {
        perror("Error attaching shared memory for clock");
        exit(1);
    }

    shared_clock[0] = 0.0; // Initialize the clock
    printf("DEBUG: Shared clock initialized to %f\n", shared_clock[0]);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    parse_arguments(argc, argv, &end_time, &max_processes, &log_interval, log_file);

    printf("DEBUG: Simulation setup: duration=%.2f, max_processes=%d, log_interval=%.2f, log_file=%s\n",
           end_time, max_processes, log_interval, log_file);

    log_fp = fopen(log_file, "w");
    if (!log_fp) {
        perror("Error opening log file");
        exit(1);
    }
    fprintf(stderr, "Log file %s opened successfully.\n", log_file);

    setup_shared_memory(&clock_shmid, &resource_shmid);

    double last_deadlock_check = 0.0;
    double last_resource_log = 0.0;
    int no_progress_cycles = 0;
    const int max_no_progress_cycles = 10;

    while (get_simulated_time() < end_time) {
        printf("DEBUG: Entering simulation loop at time %.6f\n", get_simulated_time());
        increment_clock();
        process_requests(&no_progress_cycles); // Pass the variable as a pointer

        if (get_simulated_time() - last_resource_log >= log_interval) {
            log_resource_status();
            last_resource_log = get_simulated_time();
        }

        if (get_simulated_time() - last_deadlock_check >= DEADLOCK_CHECK_INTERVAL) {
            if (detect_and_resolve_deadlock()) {
                log_deadlock_resolution();
            }
            last_deadlock_check = get_simulated_time();
        }

        // Check termination condition
        printf("DEBUG: Progress cycle: %d\n", no_progress_cycles);
        if (no_progress_cycles >= max_no_progress_cycles) {
            printf("DEBUG: Termination condition met (no progress for %d cycles).\n", max_no_progress_cycles);
            break;
        }
    }

    cleanup_resources(clock_shmid);
    if (log_fp) fclose(log_fp);

    shmctl(resource_shmid, IPC_RMID, NULL);
    return 0;
}
