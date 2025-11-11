/* Dual Queue Implementation for Fast-Pass System */
#include <stdio.h>
#include <stdlib.h>
#include "../include/queue_manager.h"
#include "../include/visitor.h"

/* Create dual queue system */
DualQueue* createDualQueue(int ride_id, int merge_ratio) {
    DualQueue* dq = (DualQueue*)malloc(sizeof(DualQueue));
    if (!dq) {
        fprintf(stderr, "Error: Memory allocation failed for dual queue\n");
        return NULL;
    }
    
    dq->regular_queue = createQueue(ride_id);
    dq->fastpass_queue = createQueue(ride_id);
    dq->ride_id = ride_id;
    dq->merge_ratio = merge_ratio;  // Default 4:1 (4 regular, 1 fast-pass)
    
    if (!dq->regular_queue || !dq->fastpass_queue) {
        if (dq->regular_queue) freeQueue(dq->regular_queue);
        if (dq->fastpass_queue) freeQueue(dq->fastpass_queue);
        free(dq);
        return NULL;
    }
    
    return dq;
}

/* Enqueue visitor to appropriate queue */
void enqueueDual(DualQueue* dq, Visitor* visitor) {
    if (!dq || !visitor) return;
    
    // Premium ticket holders with fast-passes go to fast-pass queue
    if (visitor->ticket_type == TICKET_PREMIUM && visitor->fast_passes_remaining > 0) {
        enqueue(dq->fastpass_queue, visitor);
        visitor->fast_passes_remaining--;
        printf("[Fast-Pass] %s added to priority queue (Passes left: %d)\n", 
               visitor->name, visitor->fast_passes_remaining);
    } else {
        enqueue(dq->regular_queue, visitor);
        printf("[Regular] %s added to standard queue\n", visitor->name);
    }
}

/* Dequeue visitor using merge ratio */
Visitor* dequeueDual(DualQueue* dq) {
    if (!dq) return NULL;
    
    static int regular_count = 0;
    
    // Check if fast-pass queue has visitors
    if (!isEmpty(dq->fastpass_queue)) {
        // Dequeue from fast-pass every merge_ratio regular visitors
        if (regular_count >= dq->merge_ratio || isEmpty(dq->regular_queue)) {
            regular_count = 0;
            return dequeue(dq->fastpass_queue);
        }
    }
    
    // Dequeue from regular queue
    if (!isEmpty(dq->regular_queue)) {
        regular_count++;
        return dequeue(dq->regular_queue);
    }
    
    // If regular is empty but fast-pass has visitors
    if (!isEmpty(dq->fastpass_queue)) {
        regular_count = 0;
        return dequeue(dq->fastpass_queue);
    }
    
    return NULL;
}

/* Display dual queue status */
void displayDualQueueStatus(DualQueue* dq, const char* ride_name) {
    if (!dq) return;
    
    printf("\n=== %s Queue Status ===\n", ride_name);
    printf("Regular Queue: %d visitors\n", dq->regular_queue->size);
    printf("Fast-Pass Queue: %d visitors\n", dq->fastpass_queue->size);
    printf("Total: %d visitors\n", getTotalQueueSize(dq));
    printf("Merge Ratio: %d:1 (Regular:Fast-Pass)\n", dq->merge_ratio);
    printf("========================\n");
}

/* Free dual queue */
void freeDualQueue(DualQueue* dq) {
    if (!dq) return;
    
    if (dq->regular_queue) freeQueue(dq->regular_queue);
    if (dq->fastpass_queue) freeQueue(dq->fastpass_queue);
    free(dq);
}

/* Get total queue size */
int getTotalQueueSize(DualQueue* dq) {
    if (!dq) return 0;
    return dq->regular_queue->size + dq->fastpass_queue->size;
}
