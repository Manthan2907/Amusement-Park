#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/ride_simulator.h"
#include "../include/utils.h"
#include "../include/visitor.h"
#include "../include/ride_manager.h"

/* Default ride durations */
static const RideDuration default_durations[] = {
    {0, "Roller Coaster", 5, 24},
    {0, "Ferris Wheel", 10, 30},
    {0, "Carousel", 7, 20},
    {0, "Water Rapids", 8, 16},
    {0, "Drop Tower", 3, 12},
    {0, "Haunted House", 12, 15},
    {0, "Bumper Cars", 6, 20},
    {0, "Swing Ride", 5, 16},
    {0, "Train Ride", 15, 40},
    {0, "Virtual Reality", 8, 10}
};

static time_t last_update = 0;
static const int UPDATE_INTERVAL = 30; // Update every 30 seconds

void initializeRideSimulator(void) {
    last_update = time(NULL);
}

int calculateRideDuration(const char* ride_name) {
    for (size_t i = 0; i < sizeof(default_durations) / sizeof(default_durations[0]); i++) {
        if (strstr(ride_name, default_durations[i].name) != NULL) {
            return default_durations[i].duration;
        }
    }
    return 5; // Default duration
}

void simulateRideCompletion(Ride* ride, DualQueue* queue) {
    if (!ride || !queue) return;
    
    int total_served = 0;
    
    // Process FastPass queue with priority (4:1 ratio)
    for (int i = 0; i < ride->capacity && total_served < ride->capacity; i++) {
        if (i % 5 == 0 && !isEmpty(queue->fastpass_queue)) {
            // Serve FastPass queue
            Visitor* v = dequeue(queue->fastpass_queue);
            if (v) {
                updateVisitorStats(v, ride->id, calculateSatisfactionScore(v, ride));
                total_served++;
            }
        } else if (!isEmpty(queue->regular_queue)) {
            // Serve regular queue
            Visitor* v = dequeue(queue->regular_queue);
            if (v) {
                updateVisitorStats(v, ride->id, calculateSatisfactionScore(v, ride));
                total_served++;
            }
        }
    }
    
    // Update wait times
    int cycles_needed = (queue->regular_queue->size + queue->fastpass_queue->size) / ride->capacity;
    if ((queue->regular_queue->size + queue->fastpass_queue->size) % ride->capacity > 0) cycles_needed++;
    
    ride->current_wait_time = cycles_needed * calculateRideDuration(ride->name);
}

void updateRideStatus(RideList* rides, DualQueue** queues) {
    time_t current_time = time(NULL);
    if (current_time - last_update < UPDATE_INTERVAL) return;
    
    RideNode* node = rides->head;
    while (node) {
        if (node->ride->is_operational) {
            DualQueue* queue = queues[node->ride->id];
            if (queue && (queue->regular_queue->size > 0 || queue->fastpass_queue->size > 0)) {
                simulateRideCompletion(node->ride, queue);
            }
        }
        node = node->next;
    }
    
    last_update = current_time;
}