#ifndef RIDE_SIMULATOR_H
#define RIDE_SIMULATOR_H

#include "config.h"
#include "ride_manager.h"
#include "queue_manager.h"

/* Ride Duration Constants */
typedef struct RideDuration {
    int ride_id;
    char name[MAX_NAME_LENGTH];
    int duration;  // in minutes
    int capacity;
} RideDuration;

/* Simulation Functions */
void initializeRideSimulator(void);
void updateRideStatus(RideList* rides, DualQueue** queues);
void simulateRideCompletion(Ride* ride, DualQueue* queue);
int calculateRideDuration(const char* ride_name);
void updateQueueWaitTimes(RideList* rides, DualQueue** queues);

#endif /* RIDE_SIMULATOR_H */