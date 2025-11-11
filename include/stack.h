#ifndef STACK_H
#define STACK_H

#include "config.h"
#include "ride_manager.h"

/* Stack Node Structure */
typedef struct StackNode {
    Ride* ride;
    int timestamp;  // For tracking when ride was completed
    struct StackNode* next;
} StackNode;

/* Stack Structure */
typedef struct Stack {
    StackNode* top;
    int size;
    int visitor_id;  // Associated visitor ID
} Stack;

/* Function Prototypes */

// Basic Stack Operations
Stack* createStack(int visitor_id);
void push(Stack* s, Ride* ride, int timestamp);
Ride* pop(Stack* s);
Ride* stack_peek(Stack* s);
int isStackEmpty(Stack* s);
int getStackSize(Stack* s);
void freeStack(Stack* s);

// Display Operations
void displayHistory(Stack* s);
void displayLastNRides(Stack* s, int n);

// History Query Operations
int hasVisitedRide(Stack* s, int ride_id);
Ride** getLastNRides(Stack* s, int n, int* count);
int countRideVisits(Stack* s, int ride_id);
void clearHistory(Stack* s);

// Statistics
Ride* getMostVisitedRide(Stack* s);
int getTotalRidesCompleted(Stack* s);
void generateVisitorHistory(Stack* s);

#endif /* STACK_H */
