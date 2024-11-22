#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdbool.h>
#include <stdio.h> // Include to recognize FILE type

#define MAX_RESOURCES 20
#define MAX_INSTANCES 20
#define MAX_PROCESSES 18

typedef struct {
    int allocated[MAX_PROCESSES];
    int requested[MAX_PROCESSES];
    int available;
} ResourceDescriptor;

extern ResourceDescriptor resources[MAX_RESOURCES];
extern FILE *log_fp; // Declaration for shared logging file pointer

// Function Prototypes
void initialize_resources();
bool request_resource(int process_id, int resource_id);
void release_resource(int process_id, int resource_id);
bool detect_and_resolve_deadlock();
void release_all_resources(int process_id);
void process_requests(int *no_progress_cycles); // Updated prototype
void log_resource_status();
void log_deadlock_resolution();

#endif // RESOURCE_H
