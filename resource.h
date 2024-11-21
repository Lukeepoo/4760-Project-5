#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdbool.h>

#define MAX_RESOURCES 10
#define MAX_INSTANCES 20
#define MAX_PROCESSES 18
#define MAX_CHILDREN 18 // Moved here for global accessibility

typedef struct {
    int allocated[MAX_PROCESSES];
    int requested[MAX_PROCESSES];
    int available;
} ResourceDescriptor;

extern ResourceDescriptor resources[MAX_RESOURCES]; // Add this line

void initialize_resources();
bool request_resource(int process_id, int resource_id);
void release_resource(int process_id, int resource_id);
bool detect_and_resolve_deadlock();
void release_all_resources(int process_id);
void process_requests(); // Ensure process_requests is declared

#endif // RESOURCE_H
