#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/visitor.h"
#include "../include/ride_manager.h"

/* Create a new visitor */
Visitor* createVisitor(int id, const char* name, int thrill_preference) {
    return createVisitorWithTicket(id, name, thrill_preference, TICKET_NORMAL);
}

/* Create a new visitor with ticket type */
Visitor* createVisitorWithTicket(int id, const char* name, int thrill_preference, TicketType ticket_type) {
    Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
    if (!visitor) {
        fprintf(stderr, "Error: Memory allocation failed for visitor\n");
        return NULL;
    }
    
    visitor->id = id;
    strncpy(visitor->name, name, MAX_NAME_LENGTH - 1);
    visitor->name[MAX_NAME_LENGTH - 1] = '\0';
    visitor->current_location = 0;  // Start at entrance
    visitor->thrill_preference = thrill_preference;
    visitor->rides_completed = 0;
    visitor->total_distance_traveled = 0;
    visitor->satisfaction_score = 50.0f;  // Start at neutral
    visitor->ticket_type = ticket_type;
    visitor->fast_passes_remaining = (ticket_type == TICKET_PREMIUM) ? 3 : 0;
    visitor->entry_time = (int)time(NULL);
    visitor->ride_history_head = NULL;  // Initialize empty ride history
    
    return visitor;
}

/* Get ticket type name */
const char* getTicketTypeName(TicketType type) {
    return (type == TICKET_PREMIUM) ? "Premium" : "Normal";
}

/* Free visitor memory */
void freeVisitor(Visitor* visitor) {
    if (visitor) {
        // Free ride history
        while (visitor->ride_history_head) {
            RideHistoryEntry* entry = visitor->ride_history_head;
            visitor->ride_history_head = entry->next;
            free(entry);
        }
        free(visitor);
    }
}

/* Display visitor information */
void displayVisitorInfo(Visitor* visitor) {
    if (!visitor) return;
    
    printf("\n--- Visitor Information ---\n");
    printf("ID: %d\n", visitor->id);
    printf("Name: %s\n", visitor->name);
    printf("Ticket Type: %s\n", getTicketTypeName(visitor->ticket_type));
    printf("Fast-Passes Remaining: %d\n", visitor->fast_passes_remaining);
    printf("Thrill Preference: %d/10\n", visitor->thrill_preference);
    printf("Current Location: Ride #%d\n", visitor->current_location);
    printf("Rides Completed: %d\n", visitor->rides_completed);
    printf("Distance Traveled: %d meters\n", visitor->total_distance_traveled);
    printf("Satisfaction Score: %.1f/100\n", visitor->satisfaction_score);
    printf("---------------------------\n");
}

/* Update visitor location */
void updateVisitorLocation(Visitor* visitor, int new_location) {
    if (visitor) {
        visitor->current_location = new_location;
    }
}

/* Update visitor statistics */
void updateVisitorStats(Visitor* visitor, int distance, float satisfaction) {
    if (!visitor) return;
    
    visitor->total_distance_traveled += distance;
    visitor->satisfaction_score = (visitor->satisfaction_score + satisfaction) / 2.0f;
    visitor->rides_completed++;
}

/* Create visitor group */
VisitorGroup* createVisitorGroup(int group_id) {
    VisitorGroup* group = (VisitorGroup*)malloc(sizeof(VisitorGroup));
    if (!group) {
        fprintf(stderr, "Error: Memory allocation failed for visitor group\n");
        return NULL;
    }
    
    group->head = NULL;
    group->tail = NULL;
    group->size = 0;
    group->group_id = group_id;
    group->average_thrill_preference = 0.0f;
    
    return group;
}

/* Add visitor to group (doubly linked list) */
void addVisitorToGroup(VisitorGroup* group, Visitor* visitor) {
    if (!group || !visitor) return;
    
    VisitorNode* node = (VisitorNode*)malloc(sizeof(VisitorNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for visitor node\n");
        return;
    }
    
    node->visitor = visitor;
    node->next = NULL;
    node->prev = group->tail;
    
    if (group->tail) {
        group->tail->next = node;
    } else {
        group->head = node;
    }
    
    group->tail = node;
    group->size++;
    
    calculateGroupPreference(group);
}

/* Remove visitor from group */
void removeVisitorFromGroup(VisitorGroup* group, int visitor_id) {
    if (!group || !group->head) return;
    
    VisitorNode* current = group->head;
    
    while (current) {
        if (current->visitor->id == visitor_id) {
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                group->head = current->next;
            }
            
            if (current->next) {
                current->next->prev = current->prev;
            } else {
                group->tail = current->prev;
            }
            
            freeVisitor(current->visitor);
            free(current);
            group->size--;
            
            calculateGroupPreference(group);
            return;
        }
        current = current->next;
    }
}

/* Find visitor in group */
Visitor* findVisitorInGroup(VisitorGroup* group, int visitor_id) {
    if (!group) return NULL;
    
    VisitorNode* current = group->head;
    while (current) {
        if (current->visitor->id == visitor_id) {
            return current->visitor;
        }
        current = current->next;
    }
    
    return NULL;
}

/* Display group (forward) */
void displayGroup(VisitorGroup* group) {
    if (!group || !group->head) {
        printf("Group is empty.\n");
        return;
    }
    
    printf("\n========== GROUP #%d ==========\n", group->group_id);
    printf("Size: %d visitors\n", group->size);
    printf("Average Thrill Preference: %.1f/10\n", group->average_thrill_preference);
    printf("\nMembers:\n");
    
    VisitorNode* current = group->head;
    while (current) {
        printf("  [%d] %s (Thrill: %d/10, Completed: %d rides)\n",
               current->visitor->id,
               current->visitor->name,
               current->visitor->thrill_preference,
               current->visitor->rides_completed);
        current = current->next;
    }
    printf("================================\n");
}

/* Display group (reverse) */
void displayGroupReverse(VisitorGroup* group) {
    if (!group || !group->tail) {
        printf("Group is empty.\n");
        return;
    }
    
    printf("\n========== GROUP #%d (Reverse) ==========\n", group->group_id);
    
    VisitorNode* current = group->tail;
    while (current) {
        printf("  [%d] %s\n", current->visitor->id, current->visitor->name);
        current = current->prev;
    }
    printf("=========================================\n");
}

/* Free visitor group */
void freeVisitorGroup(VisitorGroup* group) {
    if (!group) return;
    
    VisitorNode* current = group->head;
    while (current) {
        VisitorNode* next = current->next;
        freeVisitor(current->visitor);
        free(current);
        current = next;
    }
    
    free(group);
}

/* Move group to ride */
void moveGroupToRide(VisitorGroup* group, Ride* ride) {
    if (!group || !ride) return;
    
    VisitorNode* current = group->head;
    while (current) {
        updateVisitorLocation(current->visitor, ride->id);
        current = current->next;
    }
}

/* Split group into two */
VisitorGroup* splitGroup(VisitorGroup* group, int count) {
    if (!group || count >= group->size || count <= 0) {
        return NULL;
    }
    
    VisitorGroup* newGroup = createVisitorGroup(group->group_id + 1000);
    if (!newGroup) return NULL;
    
    VisitorNode* current = group->head;
    int i = 0;
    
    // Move to split point
    while (current && i < count) {
        current = current->next;
        i++;
    }
    
    if (!current) return newGroup;
    
    // Split the list
    newGroup->head = current;
    newGroup->tail = group->tail;
    
    if (current->prev) {
        current->prev->next = NULL;
    }
    group->tail = current->prev;
    current->prev = NULL;
    
    // Update sizes
    newGroup->size = group->size - count;
    group->size = count;
    
    calculateGroupPreference(group);
    calculateGroupPreference(newGroup);
    
    return newGroup;
}

/* Merge two groups */
void mergeGroups(VisitorGroup* g1, VisitorGroup* g2) {
    if (!g1 || !g2 || !g2->head) return;
    
    if (g1->tail) {
        g1->tail->next = g2->head;
        g2->head->prev = g1->tail;
        g1->tail = g2->tail;
    } else {
        g1->head = g2->head;
        g1->tail = g2->tail;
    }
    
    g1->size += g2->size;
    
    // Clear g2 without freeing visitors
    g2->head = NULL;
    g2->tail = NULL;
    g2->size = 0;
    
    calculateGroupPreference(g1);
}

/* Calculate group's average thrill preference */
void calculateGroupPreference(VisitorGroup* group) {
    if (!group || group->size == 0) {
        if (group) group->average_thrill_preference = 0.0f;
        return;
    }
    
    float sum = 0.0f;
    VisitorNode* current = group->head;
    
    while (current) {
        sum += current->visitor->thrill_preference;
        current = current->next;
    }
    
    group->average_thrill_preference = sum / group->size;
}
