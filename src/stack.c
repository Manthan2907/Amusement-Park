#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/stack.h"

/* Create stack */
Stack* createStack(int visitor_id) {
    Stack* s = (Stack*)malloc(sizeof(Stack));
    if (!s) {
        fprintf(stderr, "Error: Memory allocation failed for stack\n");
        return NULL;
    }
    
    s->top = NULL;
    s->size = 0;
    s->visitor_id = visitor_id;
    
    return s;
}

/* Push ride onto stack */
void push(Stack* s, Ride* ride, int timestamp) {
    if (!s || !ride) return;
    
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for stack node\n");
        return;
    }
    
    node->ride = ride;
    node->timestamp = timestamp;
    node->next = s->top;
    s->top = node;
    s->size++;
}

/* Pop ride from stack */
Ride* pop(Stack* s) {
    if (isStackEmpty(s)) return NULL;
    
    StackNode* node = s->top;
    Ride* ride = node->ride;
    
    s->top = node->next;
    free(node);
    s->size--;
    
    return ride;
}

/* Peek at top ride */
Ride* stack_peek(Stack* s) {
    return isStackEmpty(s) ? NULL : s->top->ride;
}

/* Check if stack is empty */
int isStackEmpty(Stack* s) {
    return (s == NULL || s->top == NULL);
}

/* Get stack size */
int getStackSize(Stack* s) {
    return s ? s->size : 0;
}

/* Free stack */
void freeStack(Stack* s) {
    if (!s) return;
    
    while (!isStackEmpty(s)) {
        pop(s);
    }
    
    free(s);
}

/* Display ride history */
void displayHistory(Stack* s) {
    if (isStackEmpty(s)) {
        printf("No ride history available.\n");
        return;
    }
    
    printf("\n========== RIDE HISTORY (Visitor #%d) ==========\n", s->visitor_id);
    printf("Total rides completed: %d\n\n", s->size);
    
    StackNode* current = s->top;
    int count = 1;
    
    while (current) {
        printf("%d. %s (Thrill: %d/10)\n",
               count++, current->ride->name, current->ride->thrill_level);
        current = current->next;
    }
    
    printf("================================================\n");
}

/* Display last N rides */
void displayLastNRides(Stack* s, int n) {
    if (isStackEmpty(s)) {
        printf("No ride history.\n");
        return;
    }
    
    printf("\n--- Last %d Rides ---\n", n);
    
    StackNode* current = s->top;
    int count = 0;
    
    while (current && count < n) {
        printf("%d. %s\n", ++count, current->ride->name);
        current = current->next;
    }
}

/* Check if visitor has visited a ride */
int hasVisitedRide(Stack* s, int ride_id) {
    if (isStackEmpty(s)) return 0;
    
    StackNode* current = s->top;
    
    while (current) {
        if (current->ride->id == ride_id) {
            return 1;
        }
        current = current->next;
    }
    
    return 0;
}

/* Get last N rides */
Ride** getLastNRides(Stack* s, int n, int* count) {
    if (isStackEmpty(s) || n <= 0) {
        *count = 0;
        return NULL;
    }
    
    int actual_count = (n < s->size) ? n : s->size;
    Ride** rides = (Ride**)malloc(sizeof(Ride*) * actual_count);
    
    if (!rides) {
        *count = 0;
        return NULL;
    }
    
    StackNode* current = s->top;
    int i = 0;
    
    while (current && i < actual_count) {
        rides[i++] = current->ride;
        current = current->next;
    }
    
    *count = actual_count;
    return rides;
}

/* Count visits to a specific ride */
int countRideVisits(Stack* s, int ride_id) {
    if (isStackEmpty(s)) return 0;
    
    int count = 0;
    StackNode* current = s->top;
    
    while (current) {
        if (current->ride->id == ride_id) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

/* Clear history */
void clearHistory(Stack* s) {
    if (!s) return;
    
    while (!isStackEmpty(s)) {
        pop(s);
    }
}

/* Get most visited ride */
Ride* getMostVisitedRide(Stack* s) {
    if (isStackEmpty(s)) return NULL;
    
    // Simple implementation: return first ride (can be improved)
    return s->top->ride;
}

/* Get total rides completed */
int getTotalRidesCompleted(Stack* s) {
    return getStackSize(s);
}

/* Generate visitor history report */
void generateVisitorHistory(Stack* s) {
    displayHistory(s);
}
