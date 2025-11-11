#include <stdio.h>
#include <stdlib.h>
#include "../include/queue_manager.h"

/* Create a new queue */
Queue* createQueue(int ride_id) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        fprintf(stderr, "Error: Memory allocation failed for queue\n");
        return NULL;
    }
    
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    q->ride_id = ride_id;
    
    return q;
}

/* Check if queue is empty */
int isEmpty(Queue* q) {
    return (q == NULL || q->size == 0);
}

/* Check if queue is full */
int isFull(Queue* q) {
    return (q != NULL && q->size >= MAX_QUEUE_SIZE);
}

/* Enqueue a visitor */
void enqueue(Queue* q, Visitor* visitor) {
    if (!q || !visitor) return;
    
    if (isFull(q)) {
        printf("Queue is full! Cannot add visitor.\n");
        return;
    }
    
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for queue node\n");
        return;
    }
    
    node->visitor = visitor;
    node->next = NULL;
    
    if (q->rear) {
        q->rear->next = node;
    } else {
        q->front = node;
    }
    
    q->rear = node;
    q->size++;
}

/* Dequeue a visitor */
Visitor* dequeue(Queue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    
    QueueNode* node = q->front;
    Visitor* visitor = node->visitor;
    
    q->front = node->next;
    if (!q->front) {
        q->rear = NULL;
    }
    
    free(node);
    q->size--;
    
    return visitor;
}

/* Peek at front visitor */
Visitor* queue_peek(Queue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    
    return q->front->visitor;
}

/* Get queue size */
int getQueueSize(Queue* q) {
    return isEmpty(q) ? 0 : q->size;
}

/* Free queue */
void freeQueue(Queue* q) {
    if (!q) return;
    
    while (!isEmpty(q)) {
        dequeue(q);
    }
    
    free(q);
}

/* Display queue */
void displayQueue(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty.\n");
        return;
    }
    
    printf("\n--- Queue for Ride #%d ---\n", q->ride_id);
    printf("Total visitors waiting: %d\n\n", q->size);
    
    QueueNode* current = q->front;
    int position = 1;
    
    while (current) {
        printf("%d. [%d] %s (Thrill Pref: %d/10)\n",
               position++,
               current->visitor->id,
               current->visitor->name,
               current->visitor->thrill_preference);
        current = current->next;
    }
    
    printf("-------------------------\n");
}

/* Display queue status */
void displayQueueStatus(Queue* q, const char* ride_name) {
    if (!q) return;
    
    int wait_time = estimateWaitTime(q, 10);  // Assume capacity of 10
    
    printf("\n[%s] Queue Status:\n", ride_name);
    printf("  Visitors waiting: %d\n", q->size);
    printf("  Estimated wait: %d minutes\n", wait_time);
}

/* Estimate wait time */
int estimateWaitTime(Queue* q, int ride_capacity) {
    if (isEmpty(q) || ride_capacity <= 0) {
        return 0;
    }
    
    int cycles = (q->size + ride_capacity - 1) / ride_capacity;
    return cycles * (BASE_RIDE_DURATION + BOARDING_TIME);
}

/* Merge two queues */
void mergeQueues(Queue* q1, Queue* q2) {
    if (!q1 || !q2 || isEmpty(q2)) return;
    
    if (isEmpty(q1)) {
        q1->front = q2->front;
        q1->rear = q2->rear;
        q1->size = q2->size;
    } else {
        q1->rear->next = q2->front;
        q1->rear = q2->rear;
        q1->size += q2->size;
    }
    
    // Clear q2 without freeing visitors
    q2->front = NULL;
    q2->rear = NULL;
    q2->size = 0;
}

/* Count visitors in queue */
int countVisitorsInQueue(Queue* q) {
    return getQueueSize(q);
}

/* Clear queue */
void clearQueue(Queue* q) {
    if (!q) return;
    
    while (!isEmpty(q)) {
        dequeue(q);
        // Don't free visitor here - they might be used elsewhere
    }
}

/* Get average wait time */
float getAverageWaitTime(Queue* q, int ride_capacity) {
    if (isEmpty(q) || ride_capacity <= 0) {
        return 0.0f;
    }
    
    return (float)estimateWaitTime(q, ride_capacity);
}

/* Get position in queue */
int getPositionInQueue(Queue* q, int visitor_id) {
    if (isEmpty(q)) {
        return -1;
    }
    
    QueueNode* current = q->front;
    int position = 1;
    
    while (current) {
        if (current->visitor->id == visitor_id) {
            return position;
        }
        position++;
        current = current->next;
    }
    
    return -1;  // Not found
}
