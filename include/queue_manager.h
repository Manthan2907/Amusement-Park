#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "config.h"
#include "visitor.h"

/* Queue Node Structure */
typedef struct QueueNode {
    Visitor* visitor;
    struct QueueNode* next;
} QueueNode;

/* Queue Structure */
typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;
    int ride_id;  // Associated ride ID
} Queue;

/* Dual Queue System (Regular + Fast-Pass) */
typedef struct DualQueue {
    Queue* regular_queue;
    Queue* fastpass_queue;
    int ride_id;
    int merge_ratio;  // e.g., 4:1 (4 regular, 1 fast-pass)
} DualQueue;

/* Function Prototypes */

// Basic Queue Operations
Queue* createQueue(int ride_id);
int isEmpty(Queue* q);
int isFull(Queue* q);
void enqueue(Queue* q, Visitor* visitor);
Visitor* dequeue(Queue* q);
Visitor* queue_peek(Queue* q);
int getQueueSize(Queue* q);
void freeQueue(Queue* q);

// Display Operations
void displayQueue(Queue* q);
void displayQueueStatus(Queue* q, const char* ride_name);

// Advanced Queue Operations
int estimateWaitTime(Queue* q, int ride_capacity);
void mergeQueues(Queue* q1, Queue* q2);
int countVisitorsInQueue(Queue* q);
void clearQueue(Queue* q);

// Queue Statistics
float getAverageWaitTime(Queue* q, int ride_capacity);
int getPositionInQueue(Queue* q, int visitor_id);

// Dual Queue Operations
DualQueue* createDualQueue(int ride_id, int merge_ratio);
void enqueueDual(DualQueue* dq, Visitor* visitor);
Visitor* dequeueDual(DualQueue* dq);
void displayDualQueueStatus(DualQueue* dq, const char* ride_name);
void freeDualQueue(DualQueue* dq);
int getTotalQueueSize(DualQueue* dq);

#endif /* QUEUE_MANAGER_H */
