#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ride_manager.h"
#include "../include/file_io.h"

/* Create a new ride */
Ride* createRide(int id, const char* name, int capacity, int thrill_level, int base_wait_time) {
    Ride* ride = (Ride*)malloc(sizeof(Ride));
    if (!ride) {
        fprintf(stderr, "Error: Memory allocation failed for ride\n");
        return NULL;
    }
    
    ride->id = id;
    strncpy(ride->name, name, MAX_NAME_LENGTH - 1);
    ride->name[MAX_NAME_LENGTH - 1] = '\0';
    ride->capacity = capacity;
    ride->thrill_level = thrill_level;
    ride->current_wait_time = base_wait_time;
    ride->distance_from_entrance = 0;
    ride->total_visitors_served = 0;
    ride->is_operational = 1;
    
    return ride;
}

/* Free ride memory */
void freeRide(Ride* ride) {
    if (ride) {
        free(ride);
    }
}

/* Display ride information */
void displayRideInfo(Ride* ride) {
    if (!ride) return;
    
    printf("\n--- Ride Information ---\n");
    printf("ID: %d\n", ride->id);
    printf("Name: %s\n", ride->name);
    printf("Capacity: %d people\n", ride->capacity);
    printf("Thrill Level: %d/10\n", ride->thrill_level);
    printf("Current Wait Time: %d minutes\n", ride->current_wait_time);
    printf("Status: %s\n", ride->is_operational ? "Operational" : "Closed");
    printf("Total Visitors Served: %d\n", ride->total_visitors_served);
    printf("------------------------\n");
}

/* Create ride list */
RideList* createRideList() {
    RideList* list = (RideList*)malloc(sizeof(RideList));
    if (!list) {
        fprintf(stderr, "Error: Memory allocation failed for ride list\n");
        return NULL;
    }
    
    list->head = NULL;
    list->count = 0;
    return list;
}

/* Add ride to list */
void addRideToList(RideList* list, Ride* ride) {
    if (!list || !ride) return;
    
    RideNode* node = (RideNode*)malloc(sizeof(RideNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for ride node\n");
        return;
    }
    
    node->ride = ride;
    node->next = list->head;
    list->head = node;
    list->count++;
}

/* Find ride by ID */
Ride* findRideById(RideList* list, int ride_id) {
    if (!list) return NULL;
    
    RideNode* current = list->head;
    while (current) {
        if (current->ride->id == ride_id) {
            return current->ride;
        }
        current = current->next;
    }
    
    return NULL;
}

/* Remove ride from list */
void removeRideFromList(RideList* list, int ride_id) {
    if (!list || !list->head) return;
    
    RideNode* current = list->head;
    RideNode* prev = NULL;
    
    while (current) {
        if (current->ride->id == ride_id) {
            if (prev) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }
            
            freeRide(current->ride);
            free(current);
            list->count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

/* Display all rides */
void displayAllRides(RideList* list) {
    if (!list || !list->head) {
        printf("No rides available.\n");
        return;
    }
    
    printf("\n========== ALL RIDES ==========\n");
    RideNode* current = list->head;
    while (current) {
        printf("\n[%d] %s\n", current->ride->id, current->ride->name);
        printf("    Thrill: %d/10 | Wait: %d min | Capacity: %d\n",
               current->ride->thrill_level,
               current->ride->current_wait_time,
               current->ride->capacity);
        printf("    Status: %s | Served: %d visitors\n",
               current->ride->is_operational ? "OPEN" : "CLOSED",
               current->ride->total_visitors_served);
        current = current->next;
    }
    printf("===============================\n");
}

/* Free ride list */
void freeRideList(RideList* list) {
    if (!list) return;
    
    RideNode* current = list->head;
    while (current) {
        RideNode* next = current->next;
        freeRide(current->ride);
        free(current);
        current = next;
    }
    
    free(list);
}

/* Update ride wait time based on queue size */
void updateRideWaitTime(Ride* ride, int queue_size) {
    if (!ride) return;
    
    if (queue_size == 0) {
        ride->current_wait_time = 0;
        return;
    }
    // Calculate wait time: (queue_size / capacity) * ride_duration + boarding_time
    int cycles = (queue_size + ride->capacity - 1) / ride->capacity;
    ride->current_wait_time = cycles * (BASE_RIDE_DURATION + BOARDING_TIME);
}

/* Mark ride as closed */
void markRideClosed(Ride* ride) {
    if (ride) {
        ride->is_operational = 0;
    }
}

/* Mark ride as open */
void markRideOpen(Ride* ride) {
    if (ride) {
        ride->is_operational = 1;
    }
}

/* Increment visitor count */
void incrementVisitorCount(Ride* ride, int count) {
    if (ride) {
        ride->total_visitors_served += count;
    }
}

/* Initialize park system */
int initializeParkSystem(RideList** rides) {
    *rides = createRideList();
    if (!*rides) {
        return 0;
    }
    
    // Load rides from file
    int loaded = loadRidesFromFile(RIDES_FILE, *rides);
    if (loaded > 0) {
        printf("Successfully loaded %d rides from file.\n", loaded);
    } else {
        printf("No rides loaded from file. Starting with empty park.\n");
    }
    
    return 1;
}

/* Shutdown park system */
void shutdownParkSystem(RideList* rides) {
    if (rides) {
        // Save rides to file before shutdown
        saveRidesToFile(RIDES_FILE, rides);
        freeRideList(rides);
    }
}
