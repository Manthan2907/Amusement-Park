#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/visitor.h"

/* Add ride to visitor's history */
void addRideToHistory(Visitor* visitor, int ride_id) {
    if (!visitor) return;

    RideHistoryEntry* entry = (RideHistoryEntry*)malloc(sizeof(RideHistoryEntry));
    if (!entry) {
        fprintf(stderr, "Error: Memory allocation failed for ride history entry\n");
        return;
    }

    entry->ride_id = ride_id;
    entry->timestamp = time(NULL);
    entry->next = visitor->ride_history_head;
    visitor->ride_history_head = entry;
}

/* Get visitor's ride history */
RideHistoryEntry* getVisitorRideHistory(Visitor* visitor) {
    return visitor ? visitor->ride_history_head : NULL;
}

/* Check if ride was recently ridden (in last 3 rides) */
int wasRecentlyRidden(Visitor* visitor, int ride_id) {
    if (!visitor || !visitor->ride_history_head) return 0;

    RideHistoryEntry* current = visitor->ride_history_head;
    int count = 0;

    while (current && count < 3) {
        if (current->ride_id == ride_id) {
            return 1;
        }
        current = current->next;
        count++;
    }

    return 0;
}

/* Undo last ride */
int undoLastRide(Visitor* visitor) {
    if (!visitor || !visitor->ride_history_head) return 0;

    RideHistoryEntry* entry = visitor->ride_history_head;
    visitor->ride_history_head = entry->next;
    free(entry);

    if (visitor->rides_completed > 0) {
        visitor->rides_completed--;
    }

    return 1;
}

/* Clear all ride history */
void clearRideHistory(Visitor* visitor) {
    if (!visitor) return;

    while (visitor->ride_history_head) {
        RideHistoryEntry* entry = visitor->ride_history_head;
        visitor->ride_history_head = entry->next;
        free(entry);
    }
}