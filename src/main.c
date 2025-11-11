#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "../include/config.h"
#include "../include/ride_manager.h"
#include "../include/visitor.h"
#include "../include/queue_manager.h"
#include "../include/priority_queue.h"
#include "../include/graph.h"
#include "../include/stack.h"
#include "../include/bst.h"
#include "../include/file_io.h"
#include "../include/utils.h"
#include "../include/web_server.h"
#include <time.h>

/* Global data structures */
RideList* park_rides = NULL;
Graph* park_map = NULL;
BST* wait_time_bst = NULL;
VisitorGroup* visitor_groups[100];
DualQueue* ride_queues[MAX_RIDES];
Stack* visitor_histories[MAX_VISITORS];
int group_count = 0;

/* Function prototypes */
void displayMenu();
void initializeSystem();
void shutdownSystem();
void addVisitorToPark();
void suggestRideForVisitor();
void showQueueStatus();
void displayVisitorHistory();
void findShortestPath();
void showRidesByWaitTime();
void simulateRideCompletion();
void handleUndoRide();
void displayParkStatistics();
void addNewRide();
void removeRide();
void removeVisitor();
void editRideDetails();
void toggleRideStatus();

int main() {
    initializeSystem();
    
    // Start web server
    startWebServer(park_rides, visitor_groups, &group_count, ride_queues, wait_time_bst, park_map);
    
    int choice;
    
    do {
        displayMenu();
        
        // Keep polling server while waiting for input
        printf("Enter your choice (0-14): ");
        fflush(stdout);
        
        // Poll server while waiting for input
        while (!_kbhit()) {
            pollWebServer();
            Sleep(50);  // Sleep 50ms between polls
        }
        
        scanf("%d", &choice);
        while (getchar() != '\n');  // Clear input buffer
        
        switch (choice) {
            case 1:
                addVisitorToPark();
                break;
            case 2:
                suggestRideForVisitor();
                break;
            case 3:
                displayVisitorHistory();
                break;
            case 4:
                handleUndoRide();
                break;
            case 5:
                removeVisitor();
                break;
            case 6:
                addNewRide();
                break;
            case 7:
                removeRide();
                break;
            case 8:
                editRideDetails();
                break;
            case 9:
                toggleRideStatus();
                break;
            case 10:
                showQueueStatus();
                break;
            case 11:
                showRidesByWaitTime();
                break;
            case 12:
                displayParkStatistics();
                break;
            case 13:
                findShortestPath();
                break;
            case 14:
                simulateRideCompletion();
                break;
            case 0:
                printf("\nThank you for visiting! Saving data...\n");
                break;
            default:
                printError("Invalid choice!");
        }
        
        if (choice != 0) {
            pauseScreen();
        }
        
    } while (choice != 0);
    
    shutdownSystem();
    return 0;
}

/* Display main menu */
void displayMenu() {
    clearScreen();
    displayHeader("AMUSEMENT PARK MANAGEMENT SYSTEM");
    
    printf("\n=== VISITOR OPERATIONS ===\n");
    printf("1. Add Visitor/Group to Park\n");
    printf("2. Suggest Next Best Ride for Visitor\n");
    printf("3. Display Visitor Ride History\n");
    printf("4. Undo Last Ride Choice\n");
    printf("5. Remove Visitor from Park\n");
    
    printf("\n=== RIDE OPERATIONS ===\n");
    printf("6. Add New Ride to Park\n");
    printf("7. Remove Ride from Park\n");
    printf("8. Edit Ride Details\n");
    printf("9. Toggle Ride Status (Open/Closed)\n");
    
    printf("\n=== QUEUE & STATISTICS ===\n");
    printf("10. Show Queue Status for All Rides\n");
    printf("11. Show Rides by Wait Time (BST)\n");
    printf("12. Display Park Statistics\n");
    
    printf("\n=== PATHFINDING & SIMULATION ===\n");
    printf("13. Find Shortest Path Between Rides\n");
    printf("14. Enjoy Ride (Visitor Experience)\n");
    
    printf("\n0. Exit and Save Data\n");
    
    displaySeparator();
}

/* Initialize system */
void initializeSystem() {
    printInfo("Initializing Amusement Park System...");
    
    // Initialize ride list
    if (!initializeParkSystem(&park_rides)) {
        printError("Failed to initialize park system");
        exit(1);
    }
    
    // Initialize graph
    park_map = createGraph();
    if (park_map) {
        loadParkGraph(PARK_MAP_FILE, park_map);
    }
    
    // Initialize BST
    wait_time_bst = createBST();
    if (wait_time_bst && park_rides) {
        rebuildBST(wait_time_bst, park_rides);
    }
    
    // Initialize queues for each ride
    for (int i = 0; i < MAX_RIDES; i++) {
        ride_queues[i] = NULL;
    }
    
    // Initialize visitor histories
    for (int i = 0; i < MAX_VISITORS; i++) {
        visitor_histories[i] = NULL;
    }
    
    // Initialize visitor groups
    for (int i = 0; i < 100; i++) {
        visitor_groups[i] = NULL;
    }
    
    printSuccess("System initialized successfully!");
}

/* Shutdown system */
void shutdownSystem() {
    printInfo("Shutting down system and saving data...");
    
    // Stop web server
    stopWebServer();
    
    // Save data
    if (park_rides) {
        saveRidesToFile(RIDES_FILE, park_rides);
    }
    
    if (park_map) {
        saveParkGraph(PARK_MAP_FILE, park_map);
    }
    
    // Free memory
    shutdownParkSystem(park_rides);
    freeGraph(park_map);
    freeBST(wait_time_bst);
    
    for (int i = 0; i < MAX_RIDES; i++) {
        if (ride_queues[i]) freeDualQueue(ride_queues[i]);
    }
    
    for (int i = 0; i < MAX_VISITORS; i++) {
        if (visitor_histories[i]) freeStack(visitor_histories[i]);
    }
    
    for (int i = 0; i < group_count; i++) {
        if (visitor_groups[i]) freeVisitorGroup(visitor_groups[i]);
    }
    
    printSuccess("System shutdown complete. Goodbye!");
}

/* Add visitor to park */
void addVisitorToPark() {
    displayHeader("ADD VISITOR TO PARK");
    
    char name[MAX_NAME_LENGTH];
    getStringInput("Enter visitor name", name, MAX_NAME_LENGTH);
    
    int thrill_pref = getIntInput("Enter thrill preference", 1, 10);
    
    // Ask for ticket type
    printf("\nTicket Types:\n");
    printf("1. Normal Ticket (Standard Queue)\n");
    printf("2. Premium Ticket (3 Fast-Passes Included)\n");
    int ticket_choice = getIntInput("Select ticket type", 1, 2);
    TicketType ticket_type = (ticket_choice == 2) ? TICKET_PREMIUM : TICKET_NORMAL;
    
    int visitor_id = generateVisitorID();
    Visitor* visitor = createVisitorWithTicket(visitor_id, name, thrill_pref, ticket_type);
    
    if (!visitor) {
        printError("Failed to create visitor");
        return;
    }
    
    // Ensure current group exists; create only if missing
    if (visitor_groups[group_count] == NULL) {
        visitor_groups[group_count] = createVisitorGroup(generateGroupID());
    }
    
    addVisitorToGroup(visitor_groups[group_count], visitor);
    
    // Create history stack for visitor
    visitor_histories[visitor_id % MAX_VISITORS] = createStack(visitor_id);
    
    printSuccess("Visitor added successfully!");
    printf("Visitor ID: %d\n", visitor_id);
    printf("Ticket Type: %s\n", getTicketTypeName(ticket_type));
    if (ticket_type == TICKET_PREMIUM) {
        printf("Fast-Passes: %d\n", visitor->fast_passes_remaining);
    }
    printf("Assigned to Group: %d\n", visitor_groups[group_count]->group_id);
    
    if (visitor_groups[group_count]->size >= 5) {
        group_count++;
        if (group_count < 100 && visitor_groups[group_count] == NULL) {
            visitor_groups[group_count] = createVisitorGroup(generateGroupID());
        }
    }
}

/* Suggest ride for visitor */
void suggestRideForVisitor() {
    displayHeader("RIDE SUGGESTIONS");
    
    int visitor_id = getIntInput("Enter visitor ID", 1000, 9999);
    
    // Find visitor
    Visitor* visitor = NULL;
    for (int i = 0; i <= group_count; i++) {
        if (visitor_groups[i]) {
            visitor = findVisitorInGroup(visitor_groups[i], visitor_id);
            if (visitor) break;
        }
    }
    
    if (!visitor) {
        printError("Visitor not found!");
        return;
    }
    
    suggestNextRide(visitor, park_rides, visitor->current_location, 3);
}

/* Show queue status */
void showQueueStatus() {
    displayHeader("QUEUE STATUS FOR ALL RIDES");
    
    if (!park_rides || !park_rides->head) {
        printWarning("No rides available");
        return;
    }
    
    RideNode* current = park_rides->head;
    
    while (current) {
        Ride* ride = current->ride;
        DualQueue* dq = ride_queues[ride->id % MAX_RIDES];
        
        if (!dq) {
            dq = createDualQueue(ride->id, 4);
            ride_queues[ride->id % MAX_RIDES] = dq;
        }
        
        int total_size = getTotalQueueSize(dq);
        updateRideWaitTime(ride, total_size);
        
        printf("\n[%d] %s\n", ride->id, ride->name);
        printf("  Regular Queue: %d visitors\n", dq->regular_queue->size);
        printf("  Fast-Pass Queue: %d visitors\n", dq->fastpass_queue->size);
        printf("  Total Queue: %d visitors\n", total_size);
        printf("  Wait Time: %d minutes\n", ride->current_wait_time);
        printf("  Status: %s\n", ride->is_operational ? "OPEN" : "CLOSED");
        
        current = current->next;
    }
}

/* Display visitor history */
void displayVisitorHistory() {
    displayHeader("VISITOR RIDE HISTORY");
    
    int visitor_id = getIntInput("Enter visitor ID", 1000, 9999);
    
    Stack* history = visitor_histories[visitor_id % MAX_VISITORS];
    
    if (!history || isStackEmpty(history)) {
        printWarning("No ride history for this visitor");
        return;
    }
    
    displayHistory(history);
}

/* Find shortest path */
void findShortestPath() {
    displayHeader("SHORTEST PATH BETWEEN RIDES");
    
    if (!park_map || !park_rides) {
        printError("Park map not available");
        return;
    }
    
    displayAllRides(park_rides);
    
    // Find min and max ride IDs in park_rides
    int min_id = 100000, max_id = -1;
    RideNode* node = park_rides->head;
    while (node) {
        if (node->ride->id < min_id) min_id = node->ride->id;
        if (node->ride->id > max_id) max_id = node->ride->id;
        node = node->next;
    }
    if (min_id > max_id) {
        printError("No rides found in park!");
        return;
    }
    int start_id = getIntInput("Enter start ride ID", min_id, max_id);
    int end_id = getIntInput("Enter end ride ID", min_id, max_id);
    
    PathInfo* path = dijkstraShortestPath(park_map, start_id, end_id);
    
    if (path) {
        printf("\nShortest path found!\n");
        displayPath(path->path, path->path_length, park_rides);
        printf("Total distance: %d meters\n", path->total_distance);
        freePathInfo(path);
    } else {
        printError("No path found between these rides");
    }
}

/* Show rides by wait time */
void showRidesByWaitTime() {
    displayHeader("RIDES SORTED BY WAIT TIME");
    
    if (!wait_time_bst || !wait_time_bst->root) {
        printWarning("No rides in system");
        return;
    }
    
    displayRidesByWaitTime(wait_time_bst);
    
    printf("\nFind rides with maximum wait time?\n");
    int max_wait = getIntInput("Enter maximum wait time (minutes)", 0, 120);
    
    int count;
    Ride** rides = findRidesWithMaxWait(wait_time_bst->root, max_wait, &count);
    
    if (rides && count > 0) {
        printf("\nFound %d rides with wait time <= %d minutes:\n", count, max_wait);
        for (int i = 0; i < count; i++) {
            printf("  - %s (%d min)\n", rides[i]->name, rides[i]->current_wait_time);
        }
        free(rides);
    } else {
        printWarning("No rides found within specified wait time");
    }
}

/* Simulate ride completion */
void simulateRideCompletion() {
    displayHeader("SIMULATE RIDE COMPLETION");
    
    int visitor_id = getIntInput("Enter visitor ID", 1000, 9999);
    
    displayAllRides(park_rides);
    int ride_id = getIntInput("Enter ride ID", 0, MAX_RIDES - 1);
    
    Ride* ride = findRideById(park_rides, ride_id);
    if (!ride) {
        printError("Ride not found!");
        return;
    }
    
    // Find visitor
    Visitor* visitor = NULL;
    for (int i = 0; i <= group_count; i++) {
        if (visitor_groups[i]) {
            visitor = findVisitorInGroup(visitor_groups[i], visitor_id);
            if (visitor) break;
        }
    }
    
    if (!visitor) {
        printError("Visitor not found!");
        return;
    }
    
    // Add to history
    Stack* history = visitor_histories[visitor_id % MAX_VISITORS];
    if (history) {
        push(history, ride, (int)time(NULL));
    }
    
    // Update visitor stats
    float satisfaction = calculateSatisfactionScore(visitor, ride);
    updateVisitorStats(visitor, 100, satisfaction);
    updateVisitorLocation(visitor, ride->id);
    
    // Update ride stats
    incrementVisitorCount(ride, 1);
    
    printSuccess("Ride completed successfully!");
    printf("Satisfaction score: %.1f/100\n", satisfaction);
}

/* Undo last ride */
void handleUndoRide() {
    displayHeader("UNDO LAST RIDE CHOICE");
    
    int visitor_id = getIntInput("Enter visitor ID", 1000, 9999);
    
    // Find visitor
    Visitor* found_visitor = NULL;
    for (int i = 0; i < group_count; i++) {
        VisitorNode* node = visitor_groups[i]->head;
        while (node) {
            if (node->visitor->id == visitor_id) {
                found_visitor = node->visitor;
                break;
            }
            node = node->next;
        }
        if (found_visitor) break;
    }
    
    if (!found_visitor) {
        printWarning("Visitor not found");
        return;
    }
    
    if (undoLastRide(found_visitor)) {
        printSuccess("Last ride undone successfully!");
    } else {
        printWarning("No ride history to undo");
    }
}

/* Display park statistics */
void displayParkStatistics() {
    generateParkReport(park_rides, visitor_groups, group_count);
}

/* Add new ride to park */
void addNewRide() {
    displayHeader("ADD NEW RIDE");
    
    char name[MAX_NAME_LENGTH];
    getStringInput("Enter ride name", name, MAX_NAME_LENGTH);
    
    int capacity = getIntInput("Enter capacity (people per cycle)", 4, 20);
    int thrill = getIntInput("Enter thrill level", 1, 10);
    int wait = getIntInput("Enter initial wait time (minutes)", 0, 60);
    
    int new_id = 1;
    if (park_rides && park_rides->head) {
        RideNode* node = park_rides->head;
        while (node) {
            if (node->ride->id >= new_id) {
                new_id = node->ride->id + 1;
            }
            node = node->next;
        }
    }
    
    Ride* ride = createRide(new_id, name, capacity, thrill, wait);
    if (ride) {
        addRideToList(park_rides, ride);
        wait_time_bst->root = insertRide(wait_time_bst->root, ride);
        printSuccess("Ride added successfully!");
        printf("Ride ID: %d\n", new_id);
        displayRideInfo(ride);
    } else {
        printError("Failed to create ride");
    }
}

/* Remove ride from park */
void removeRide() {
    displayHeader("REMOVE RIDE");
    
    if (!park_rides || !park_rides->head) {
        printWarning("No rides available to remove");
        return;
    }
    
    displayAllRides(park_rides);
    
    int min_id = 100000, max_id = -1;
    RideNode* node = park_rides->head;
    while (node) {
        if (node->ride->id < min_id) min_id = node->ride->id;
        if (node->ride->id > max_id) max_id = node->ride->id;
        node = node->next;
    }
    
    int ride_id = getIntInput("Enter ride ID to remove", min_id, max_id);
    
    Ride* ride = findRideById(park_rides, ride_id);
    if (!ride) {
        printError("Ride not found!");
        return;
    }
    
    printf("\nAre you sure you want to remove '%s'?\n", ride->name);
    int confirm = getIntInput("1=Yes, 0=No", 0, 1);
    
    if (confirm) {
        wait_time_bst->root = deleteRide(wait_time_bst->root, ride->current_wait_time);
        removeRideFromList(park_rides, ride_id);
        
        if (ride_queues[ride_id % MAX_RIDES]) {
            freeDualQueue(ride_queues[ride_id % MAX_RIDES]);
            ride_queues[ride_id % MAX_RIDES] = NULL;
        }
        
        printSuccess("Ride removed successfully!");
    } else {
        printInfo("Removal cancelled");
    }
}

/* Edit ride details */
void editRideDetails() {
    displayHeader("EDIT RIDE DETAILS");
    
    if (!park_rides || !park_rides->head) {
        printWarning("No rides available to edit");
        return;
    }
    
    displayAllRides(park_rides);
    
    int min_id = 100000, max_id = -1;
    RideNode* node = park_rides->head;
    while (node) {
        if (node->ride->id < min_id) min_id = node->ride->id;
        if (node->ride->id > max_id) max_id = node->ride->id;
        node = node->next;
    }
    
    int ride_id = getIntInput("Enter ride ID to edit", min_id, max_id);
    
    Ride* ride = findRideById(park_rides, ride_id);
    if (!ride) {
        printError("Ride not found!");
        return;
    }
    
    displayRideInfo(ride);
    
    printf("\nWhat would you like to edit?\n");
    printf("1. Capacity\n");
    printf("2. Thrill Level\n");
    printf("3. Wait Time\n");
    printf("0. Cancel\n");
    
    int choice = getIntInput("Choice", 0, 3);
    
    switch (choice) {
        case 1:
            ride->capacity = getIntInput("New capacity", 4, 20);
            printSuccess("Capacity updated!");
            break;
        case 2:
            ride->thrill_level = getIntInput("New thrill level", 1, 10);
            printSuccess("Thrill level updated!");
            break;
        case 3:
            wait_time_bst->root = deleteRide(wait_time_bst->root, ride->current_wait_time);
            ride->current_wait_time = getIntInput("New wait time", 0, 120);
            wait_time_bst->root = insertRide(wait_time_bst->root, ride);
            printSuccess("Wait time updated!");
            break;
        case 0:
            printInfo("Edit cancelled");
            return;
    }
    
    displayRideInfo(ride);
}

/* Toggle ride operational status */
void toggleRideStatus() {
    displayHeader("TOGGLE RIDE STATUS");
    
    if (!park_rides || !park_rides->head) {
        printWarning("No rides available");
        return;
    }
    
    displayAllRides(park_rides);
    
    int min_id = 100000, max_id = -1;
    RideNode* node = park_rides->head;
    while (node) {
        if (node->ride->id < min_id) min_id = node->ride->id;
        if (node->ride->id > max_id) max_id = node->ride->id;
        node = node->next;
    }
    
    int ride_id = getIntInput("Enter ride ID", min_id, max_id);
    
    Ride* ride = findRideById(park_rides, ride_id);
    if (!ride) {
        printError("Ride not found!");
        return;
    }
    
    if (ride->is_operational) {
        markRideClosed(ride);
        printSuccess("Ride marked as CLOSED");
        printf("'%s' is now closed for maintenance\n", ride->name);
    } else {
        markRideOpen(ride);
        printSuccess("Ride marked as OPEN");
        printf("'%s' is now open for visitors\n", ride->name);
    }
}

/* Remove visitor from park */
void removeVisitor() {
    displayHeader("REMOVE VISITOR FROM PARK");
    
    int visitor_id = getIntInput("Enter visitor ID to remove", 1000, 9999);
    
    // Find and remove visitor from groups
    for (int i = 0; i <= group_count; i++) {
        if (visitor_groups[i]) {
            VisitorNode* current = visitor_groups[i]->head;
            VisitorNode* prev = NULL;
            
            while (current) {
                if (current->visitor->id == visitor_id) {
                    // Found the visitor, remove from group
                    if (prev) {
                        prev->next = current->next;
                    } else {
                        visitor_groups[i]->head = current->next;
                    }
                    
                    visitor_groups[i]->size--;
                    
                    // Free visitor history
                    if (visitor_histories[visitor_id % MAX_VISITORS]) {
                        freeStack(visitor_histories[visitor_id % MAX_VISITORS]);
                        visitor_histories[visitor_id % MAX_VISITORS] = NULL;
                    }
                    
                    printf("Removed visitor: %s (ID: %d)\n", current->visitor->name, visitor_id);
                    free(current->visitor);
                    free(current);
                    
                    printSuccess("Visitor removed successfully!");
                    return;
                }
                prev = current;
                current = current->next;
            }
        }
    }
    
    printError("Visitor not found!");
}
