#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/priority_queue.h"

/* Create priority queue */
PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        fprintf(stderr, "Error: Memory allocation failed for priority queue\n");
        return NULL;
    }
    
    pq->size = 0;
    for (int i = 0; i < MAX_RIDES; i++) {
        pq->nodes[i] = NULL;
    }
    
    return pq;
}

/* Swap two nodes */
void swap(PriorityQueueNode** a, PriorityQueueNode** b) {
    PriorityQueueNode* temp = *a;
    *a = *b;
    *b = temp;
}

/* Heapify up */
void heapifyUp(PriorityQueue* pq, int index) {
    if (index <= 0 || !pq) return;
    
    int parent = (index - 1) / 2;
    
    if (pq->nodes[index]->priority_score > pq->nodes[parent]->priority_score) {
        swap(&pq->nodes[index], &pq->nodes[parent]);
        heapifyUp(pq, parent);
    }
}

/* Heapify down */
void heapifyDown(PriorityQueue* pq, int index) {
    if (!pq || index >= pq->size) return;
    
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    if (left < pq->size && pq->nodes[left]->priority_score > pq->nodes[largest]->priority_score) {
        largest = left;
    }
    
    if (right < pq->size && pq->nodes[right]->priority_score > pq->nodes[largest]->priority_score) {
        largest = right;
    }
    
    if (largest != index) {
        swap(&pq->nodes[index], &pq->nodes[largest]);
        heapifyDown(pq, largest);
    }
}

/* Insert with priority */
void insertWithPriority(PriorityQueue* pq, Ride* ride, float priority) {
    if (!pq || !ride || pq->size >= MAX_RIDES) return;
    
    PriorityQueueNode* node = (PriorityQueueNode*)malloc(sizeof(PriorityQueueNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
    
    node->ride = ride;
    node->priority_score = priority;
    
    pq->nodes[pq->size] = node;
    heapifyUp(pq, pq->size);
    pq->size++;
}

/* Extract max priority ride */
Ride* extractMax(PriorityQueue* pq) {
    if (isPriorityQueueEmpty(pq)) return NULL;
    
    Ride* ride = pq->nodes[0]->ride;
    free(pq->nodes[0]);
    
    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;
    
    if (pq->size > 0) {
        heapifyDown(pq, 0);
    }
    
    return ride;
}

/* Peek at max */
Ride* peekMax(PriorityQueue* pq) {
    return isPriorityQueueEmpty(pq) ? NULL : pq->nodes[0]->ride;
}

/* Check if empty */
int isPriorityQueueEmpty(PriorityQueue* pq) {
    return (pq == NULL || pq->size == 0);
}

/* Free priority queue */
void freePriorityQueue(PriorityQueue* pq) {
    if (!pq) return;
    
    for (int i = 0; i < pq->size; i++) {
        if (pq->nodes[i]) free(pq->nodes[i]);
    }
    free(pq);
}


/* Calculate priority score */
float calculatePriority(Visitor* visitor, Ride* ride, int distance) {
    if (!visitor || !ride) return 0.0f;
    
    float thrill_match = calculateThrillMatch(visitor->thrill_preference, ride->thrill_level);
    float wait_time = (float)ride->current_wait_time;
    float dist = (float)distance / 100.0f;
    
    return (thrill_match * THRILL_MATCH_WEIGHT) 
           - (wait_time * WAIT_TIME_WEIGHT / 10.0f)
           - (dist * DISTANCE_WEIGHT);
}

/* Build priority queue for visitor */
PriorityQueue* buildPriorityQueue(Visitor* visitor, RideList* rides, int current_location) {
    if (!visitor || !rides) return NULL;
    
    PriorityQueue* pq = createPriorityQueue();
    if (!pq) return NULL;
    
    RideNode* current = rides->head;
    
    while (current) {
        Ride* ride = current->ride;
        if (ride->is_operational) {
            int distance = abs(ride->id - current_location) * 100;
            float priority = calculatePriority(visitor, ride, distance);
            insertWithPriority(pq, ride, priority);
        }
        current = current->next;
    }
    
    return pq;
}

/* Suggest next ride */
void suggestNextRide(Visitor* visitor, RideList* rides, int current_location, int top_n) {
    if (!visitor || !rides) return;
    
    PriorityQueue* pq = buildPriorityQueue(visitor, rides, current_location);
    if (!pq) return;
    
    printf("\n========== PRIORITY QUEUE RIDE SUGGESTIONS ==========\n");
    printf("Visitor: %s | Thrill Preference: %d/10\n", visitor->name, visitor->thrill_preference);
    printf("Current Location: Ride %d\n\n", current_location);
    printf("** Using MAX-HEAP Priority Queue **\n");
    printf("Priority Formula: (Thrill Match x 40) - (Wait Time x 3) - (Distance x 20)\n\n");
    
    int count = 0;
    while (!isPriorityQueueEmpty(pq) && count < top_n) {
        Ride* ride = extractMax(pq);
        if (ride) {
            float thrill_match = calculateThrillMatch(visitor->thrill_preference, ride->thrill_level);
            int distance = abs(current_location - ride->id);
            float priority = calculatePriority(visitor, ride, distance);
            
            printf("-------------------------------\n");
            printf("%d. %s\n", ++count, ride->name);
            printf("   Thrill Level: %d/10 (Match: %.1f/10)\n", ride->thrill_level, thrill_match);
            printf("   Wait Time: %d min | Distance: %d units\n", ride->current_wait_time, distance);
            printf("   PRIORITY SCORE: %.2f\n", priority);
            if (ride->current_occupancy > 0) {
                printf("   Queue: %d people waiting\n", ride->current_occupancy);
            }
        }
    }
    printf("===============================================\n");
    freePriorityQueue(pq);
}


/* Display top rides */
void displayTopRides(PriorityQueue* pq, int count) {
    if (isPriorityQueueEmpty(pq)) {
        printf("No rides available.\n");
        return;
    }
    
    printf("\n--- Top %d Rides ---\n", count);
    int displayed = 0;
    
    while (!isPriorityQueueEmpty(pq) && displayed < count) {
        Ride* ride = extractMax(pq);
        if (ride) {
            printf("%d. %s\n", ++displayed, ride->name);
        }
    }
}
