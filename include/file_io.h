#ifndef FILE_IO_H
#define FILE_IO_H

#include "config.h"
#include "ride_manager.h"
#include "visitor.h"
#include "graph.h"

/* Function Prototypes */

// Ride File Operations
int loadRidesFromFile(const char* filename, RideList* rides);
int saveRidesToFile(const char* filename, RideList* rides);

// Visitor File Operations
int loadVisitorsFromFile(const char* filename, VisitorGroup* visitors);
int saveVisitorsToFile(const char* filename, VisitorGroup* visitors);

// Graph/Park Map File Operations
int loadParkGraph(const char* filename, Graph* graph);
int saveParkGraph(const char* filename, Graph* graph);

// Helper Functions
char* trimWhitespace(char* str);
int parseCSVLine(char* line, char* tokens[], int max_tokens);
int validateRideData(int id, int capacity, int thrill_level);
int validateVisitorData(int id, int thrill_preference);

// Error Handling
void logError(const char* message);
void logInfo(const char* message);

// Data Export
int exportStatisticsToCSV(const char* filename, RideList* rides);
int exportVisitorHistoryToCSV(const char* filename, VisitorGroup* visitors);

#endif /* FILE_IO_H */
