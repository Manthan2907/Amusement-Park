#ifndef RIDE_MANAGER_H
#define RIDE_MANAGER_H

#include "config.h"

/* Ride Structure */
typedef struct Ride {
    int id;
    char name[MAX_NAME_LENGTH];
    int capacity;                    // Number of people per ride cycle
    int current_wait_time;           // In minutes
    int thrill_level;                // 1-10 scale
    int distance_from_entrance;      // In meters
    int total_visitors_served;       // Statistics
    int is_operational;              // 1 = open, 0 = closed
} Ride;

/* Ride Node for Linked List */
typedef struct RideNode {
    Ride* ride;
    struct RideNode* next;
} RideNode;

/* Ride List Structure */
typedef struct RideList {
    RideNode* head;
    int count;
} RideList;

/* Function Prototypes */

// Ride Management
Ride* createRide(int id, const char* name, int capacity, int thrill_level, int base_wait_time);
void freeRide(Ride* ride);
void displayRideInfo(Ride* ride);

// Ride List Operations
RideList* createRideList();
void addRideToList(RideList* list, Ride* ride);
Ride* findRideById(RideList* list, int ride_id);
void removeRideFromList(RideList* list, int ride_id);
void displayAllRides(RideList* list);
void freeRideList(RideList* list);

// Ride Operations
void updateRideWaitTime(Ride* ride, int queue_size);
void markRideClosed(Ride* ride);
void markRideOpen(Ride* ride);
void incrementVisitorCount(Ride* ride, int count);

// Park System Management
int initializeParkSystem(RideList** rides);
void shutdownParkSystem(RideList* rides);

#endif /* RIDE_MANAGER_H */
