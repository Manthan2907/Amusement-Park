#ifndef UTILS_H
#define UTILS_H

#include "config.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "ride_manager.h"
#include "visitor.h"
#include "graph.h"

/* Function Prototypes */

// ID Generation
int generateVisitorID();
int generateGroupID();

// Calculation Helpers
float calculateThrillMatch(int preference, int ride_level);
float calculateSatisfactionScore(Visitor* visitor, Ride* ride);
int calculateEstimatedWaitTime(int queue_size, int ride_capacity);

// Time Formatting
void formatTime(int minutes, char* buffer);
void getCurrentTimestamp(char* buffer);

// Display Helpers
void displayRideInfo(Ride* ride);
void displayVisitorInfo(Visitor* visitor);
void displaySeparator();
void displayHeader(const char* title);

// Statistics Functions
float calculateAverageWaitTime(RideList* rides);
Ride* getMostPopularRide(RideList* rides);
int getTotalVisitorsInPark(VisitorGroup* groups[], int group_count);
void generateParkReport(RideList* rides, VisitorGroup* groups[], int group_count);

// Input Validation
int validateInput(int value, int min, int max);
int getIntInput(const char* prompt, int min, int max);
void getStringInput(const char* prompt, char* buffer, int max_length);

// Memory Management
void freeAllMemory(RideList* rides, Graph* graph, VisitorGroup* groups[], int group_count);

// Color Output
void printColored(const char* text, const char* color);
void printSuccess(const char* message);
void printError(const char* message);
void printWarning(const char* message);
void printInfo(const char* message);

// Utility Functions
void clearScreen();
void pauseScreen();
int min(int a, int b);
int max(int a, int b);
float minf(float a, float b);
float maxf(float a, float b);

#endif /* UTILS_H */
