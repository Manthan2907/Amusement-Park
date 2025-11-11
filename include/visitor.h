#ifndef VISITOR_H
#define VISITOR_H

#include "config.h"

/* Forward declaration */
typedef struct Ride Ride;

/* Ticket Type Enum */
typedef enum {
    TICKET_NORMAL = 0,
    TICKET_PREMIUM = 1
} TicketType;

/* RideHistoryEntry structure for keeping track of rides taken */
typedef struct RideHistoryEntry {
    int ride_id;
    time_t timestamp;
    struct RideHistoryEntry *next;
} RideHistoryEntry;

/* Visitor Structure */
typedef struct Visitor {
    int id;
    char name[MAX_NAME_LENGTH];
    int current_location;            // Current ride ID or 0 for entrance
    int thrill_preference;           // 1-10 scale
    int rides_completed;             // Count of completed rides
    int total_distance_traveled;     // In meters
    float satisfaction_score;        // 0-100 scale
    TicketType ticket_type;          // Normal or Premium ticket
    int fast_passes_remaining;       // Number of fast passes available
    int entry_time;                  // Time entered park (timestamp)
    RideHistoryEntry *ride_history_head;  // Stack implementation for ride history
} Visitor;

/* Visitor Node for Doubly Linked List */
typedef struct VisitorNode {
    Visitor* visitor;
    struct VisitorNode* next;
    struct VisitorNode* prev;
} VisitorNode;

/* Visitor Group Structure (Doubly Linked List) */
typedef struct VisitorGroup {
    VisitorNode* head;
    VisitorNode* tail;
    int size;
    int group_id;
    float average_thrill_preference;
} VisitorGroup;

/* Function Prototypes */

// Visitor Management
Visitor* createVisitor(int id, const char* name, int thrill_preference);
Visitor* createVisitorWithTicket(int id, const char* name, int thrill_preference, TicketType ticket_type);
void freeVisitor(Visitor* visitor);
void displayVisitorInfo(Visitor* visitor);
void updateVisitorLocation(Visitor* visitor, int new_location);
void updateVisitorStats(Visitor* visitor, int distance, float satisfaction);
const char* getTicketTypeName(TicketType type);

// Visitor Group Operations (Doubly Linked List)
VisitorGroup* createVisitorGroup(int group_id);
void addVisitorToGroup(VisitorGroup* group, Visitor* visitor);
void removeVisitorFromGroup(VisitorGroup* group, int visitor_id);
Visitor* findVisitorInGroup(VisitorGroup* group, int visitor_id);
void displayGroup(VisitorGroup* group);
void displayGroupReverse(VisitorGroup* group);
void freeVisitorGroup(VisitorGroup* group);

// Group Movement
void moveGroupToRide(VisitorGroup* group, Ride* ride);
VisitorGroup* splitGroup(VisitorGroup* group, int count);
void mergeGroups(VisitorGroup* g1, VisitorGroup* g2);
void calculateGroupPreference(VisitorGroup* group);

// Ride History Operations
void addRideToHistory(Visitor* visitor, int ride_id);
RideHistoryEntry* getVisitorRideHistory(Visitor* visitor);
int wasRecentlyRidden(Visitor* visitor, int ride_id);
int undoLastRide(Visitor* visitor);
void clearRideHistory(Visitor* visitor);

#endif /* VISITOR_H */
