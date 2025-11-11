#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "config.h"
#include "ride_manager.h"
#include "visitor.h"

/* Priority Queue Node Structure */
typedef struct PriorityQueueNode {
    Ride* ride;
    float priority_score;
} PriorityQueueNode;

/* Priority Queue Structure (Max-Heap) */
typedef struct PriorityQueue {
    PriorityQueueNode* nodes[MAX_RIDES];
    int size;
} PriorityQueue;

/* Function Prototypes */

// Priority Queue Operations
PriorityQueue* createPriorityQueue();
void insertWithPriority(PriorityQueue* pq, Ride* ride, float priority);
Ride* extractMax(PriorityQueue* pq);
Ride* peekMax(PriorityQueue* pq);
int isPriorityQueueEmpty(PriorityQueue* pq);
void freePriorityQueue(PriorityQueue* pq);

// Heap Operations
void heapifyUp(PriorityQueue* pq, int index);
void heapifyDown(PriorityQueue* pq, int index);
void swap(PriorityQueueNode** a, PriorityQueueNode** b);

// Priority Calculation
float calculatePriority(Visitor* visitor, Ride* ride, int distance);
float calculateThrillMatch(int preference, int ride_level);

// Ride Suggestion System
PriorityQueue* buildPriorityQueue(Visitor* visitor, RideList* rides, int current_location);
void suggestNextRide(Visitor* visitor, RideList* rides, int current_location, int top_n);
void displayTopRides(PriorityQueue* pq, int count);

#endif /* PRIORITY_QUEUE_H */
