#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "graph.h"
#include "../include/utils.h"

static int visitor_id_counter = 1000;
static int group_id_counter = 1;

/* Generate visitor ID */
int generateVisitorID() {
    return visitor_id_counter++;
}

/* Generate group ID */
int generateGroupID() {
    return group_id_counter++;
}

/* Calculate thrill match */
float calculateThrillMatch(int preference, int ride_level) {
    return 10 - abs(preference - ride_level);
}

/* Calculate satisfaction score */
float calculateSatisfactionScore(Visitor* visitor, Ride* ride) {
    if (!visitor || !ride) return 0.0f;
    
    float thrill_match = (float)calculateThrillMatch(visitor->thrill_preference, ride->thrill_level);
    float wait_penalty = (float)ride->current_wait_time / 2.0f;
    
    float score = (thrill_match * 10.0f) - wait_penalty;
    
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    
    return score;
}

/* Calculate estimated wait time */
int calculateEstimatedWaitTime(int queue_size, int ride_capacity) {
    if (ride_capacity <= 0) return 0;
    
    int cycles = (queue_size + ride_capacity - 1) / ride_capacity;
    return cycles * (BASE_RIDE_DURATION + BOARDING_TIME);
}

/* Format time */
void formatTime(int minutes, char* buffer) {
    if (!buffer) return;
    
    int hours = minutes / 60;
    int mins = minutes % 60;
    
    if (hours > 0) {
        sprintf(buffer, "%dh %dm", hours, mins);
    } else {
        sprintf(buffer, "%d min", mins);
    }
}

/* Get current timestamp */
void getCurrentTimestamp(char* buffer) {
    if (!buffer) return;
    
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
}

/* Display separator */
void displaySeparator() {
    printf("================================================\n");
}

/* Display header */
void displayHeader(const char* title) {
    printf("\n");
    displaySeparator();
    printf("  %s\n", title);
    displaySeparator();
}

/* Calculate average wait time */
float calculateAverageWaitTime(RideList* rides) {
    if (!rides || rides->count == 0) return 0.0f;
    
    int total = 0;
    RideNode* current = rides->head;
    
    while (current) {
        total += current->ride->current_wait_time;
        current = current->next;
    }
    
    return (float)total / rides->count;
}

/* Get most popular ride */
Ride* getMostPopularRide(RideList* rides) {
    if (!rides || !rides->head) return NULL;
    
    Ride* most_popular = rides->head->ride;
    RideNode* current = rides->head->next;
    
    while (current) {
        if (current->ride->total_visitors_served > most_popular->total_visitors_served) {
            most_popular = current->ride;
        }
        current = current->next;
    }
    
    return most_popular;
}

/* Get total visitors in park */
int getTotalVisitorsInPark(VisitorGroup* groups[], int group_count) {
    int total = 0;
    
    for (int i = 0; i < group_count; i++) {
        if (groups[i]) {
            total += groups[i]->size;
        }
    }
    
    return total;
}

/* Generate park report */
void generateParkReport(RideList* rides, VisitorGroup* groups[], int group_count) {
    displayHeader("PARK STATISTICS REPORT");
    
    printf("\n--- Ride Statistics ---\n");
    printf("Total Rides: %d\n", rides ? rides->count : 0);
    printf("Average Wait Time: %.1f minutes\n", calculateAverageWaitTime(rides));
    
    Ride* popular = getMostPopularRide(rides);
    if (popular) {
        printf("Most Popular Ride: %s (%d visitors)\n",
               popular->name, popular->total_visitors_served);
    }
    
    printf("\n--- Visitor Statistics ---\n");
    printf("Total Visitors: %d\n", getTotalVisitorsInPark(groups, group_count));
    printf("Total Groups: %d\n", group_count);
    
    displaySeparator();
}

/* Validate input */
int validateInput(int value, int min, int max) {
    return (value >= min && value <= max);
}

/* Get integer input */
int getIntInput(const char* prompt, int min, int max) {
    int value;
    
    do {
        printf("%s (%d-%d): ", prompt, min, max);
        if (scanf("%d", &value) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        while (getchar() != '\n');
        
        if (!validateInput(value, min, max)) {
            printf("Value must be between %d and %d.\n", min, max);
        }
    } while (!validateInput(value, min, max));
    
    return value;
}

/* Get string input */
void getStringInput(const char* prompt, char* buffer, int max_length) {
    printf("%s: ", prompt);
    fgets(buffer, max_length, stdin);
    
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

/* Free all memory */
void freeAllMemory(RideList* rides, Graph* graph, VisitorGroup* groups[], int group_count) {
    if (rides) freeRideList(rides);
    if (graph) freeGraph(graph);
    
    for (int i = 0; i < group_count; i++) {
        if (groups[i]) freeVisitorGroup(groups[i]);
    }
}

/* Print colored text */
void printColored(const char* text, const char* color) {
    printf("%s%s%s", color, text, COLOR_RESET);
}

/* Print success message */
void printSuccess(const char* message) {
    printf("%s[SUCCESS]%s %s\n", COLOR_GREEN, COLOR_RESET, message);
}

/* Print error message */
void printError(const char* message) {
    printf("%s[ERROR]%s %s\n", COLOR_RED, COLOR_RESET, message);
}

/* Print warning message */
void printWarning(const char* message) {
    printf("%s[WARNING]%s %s\n", COLOR_YELLOW, COLOR_RESET, message);
}

/* Print info message */
void printInfo(const char* message) {
    printf("%s[INFO]%s %s\n", COLOR_CYAN, COLOR_RESET, message);
}

/* Clear screen */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Pause screen */
void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

/* Min function */
int min(int a, int b) {
    return (a < b) ? a : b;
}

/* Max function */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/* Min float function */
float minf(float a, float b) {
    return (a < b) ? a : b;
}

/* Max float function */
float maxf(float a, float b) {
    return (a > b) ? a : b;
}
