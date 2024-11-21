#include "common.h"
#include "resource.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

static int clock_shmid;
static double end_time = 0.0;
static int max_processes = MAX_CHILDREN;
static double log_interval = 1.0;
static char log_file[256] = "output.log";
static FILE *log_fp = NULL;

void signal_handler(int sig) {
    printf("DEBUG: Received signal %d. Cleaning up resources and terminating...\n", sig);
    if (log_fp) fclose(log_fp);
    cleanup_resources(clock_shmid);
    exit(EXIT_SUCCESS);
}

void log_message(const char *format, ...) {
    if (!log_fp) return;
    va_list args;
    va_start(args, format);
    vfprintf(log_fp, format, args);
    va_end(args);
    fflush(log_fp); // Ensure logs are immediately written to the file
}

void log_resource_status() {
    if (!log_fp) return;

    fprintf(log_fp, "DEBUG: Logging resource status...\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        fprintf(log_fp, "Resource %d available: %d\n", i, resources[i].available);
        for (int j = 0; j < MAX_PROCESSES; j++) {
            if (resources[i].allocated[j] > 0) {
                fprintf(log_fp, "  Allocated to process %d: %d\n", j, resources[i].allocated[j]);
            }
        }
    }
    fflush(log_fp); // Ensure logs are written to the file immediately
}


void log_deadlock_resolution() {
    if (!log_fp) return;
    fprintf(log_fp, "Deadlock resolved and logged.\n");
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    parse_arguments(argc, argv, &end_time, &max_processes, &log_interval, log_file);

    printf("DEBUG: Simulation setup: duration=%.2f, max_processes=%d, log_interval=%.2f, log_file=%s\n",
           end_time, max_processes, log_interval, log_file);

    log_fp = fopen(log_file, "w");
    if (!log_fp) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    setup_shared_memory(&clock_shmid, NULL);
    initialize_resources();

    double last_deadlock_check = 0.0;
    double last_resource_log = 0.0;

    while (get_simulated_time() < end_time) {
        increment_clock();
        process_requests();

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
    }

    cleanup_resources(clock_shmid);

    if (log_fp) fclose(log_fp);

    return 0;
}
