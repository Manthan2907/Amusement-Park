#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/file_io.h"

/* Trim whitespace from string */
char* trimWhitespace(char* str) {
    if (!str) return NULL;
    
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

/* Parse CSV line */
int parseCSVLine(char* line, char* tokens[], int max_tokens) {
    int count = 0;
    char* token = strtok(line, ",");
    
    while (token && count < max_tokens) {
        tokens[count++] = trimWhitespace(token);
        token = strtok(NULL, ",");
    }
    
    return count;
}

/* Validate ride data */
int validateRideData(int id, int capacity, int thrill_level) {
    return (id >= 0 && 
            capacity >= MIN_RIDE_CAPACITY && capacity <= MAX_RIDE_CAPACITY &&
            thrill_level >= MIN_THRILL_LEVEL && thrill_level <= MAX_THRILL_LEVEL);
}

/* Validate visitor data */
int validateVisitorData(int id, int thrill_preference) {
    return (id >= 0 && 
            thrill_preference >= MIN_THRILL_LEVEL && 
            thrill_preference <= MAX_THRILL_LEVEL);
}

/* Load rides from file */
int loadRidesFromFile(const char* filename, RideList* rides) {
    if (!filename || !rides) return 0;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        logError("Could not open rides file");
        return 0;
    }
    
    char line[256];
    int count = 0;
    
    // Skip header line if present
    if (fgets(line, sizeof(line), file)) {
        // Check if it's a header
        if (strstr(line, "id") || strstr(line, "name")) {
            // Skip this line
        } else {
            rewind(file);
        }
    }
    
    while (fgets(line, sizeof(line), file)) {
        char* tokens[10];
        int token_count = parseCSVLine(line, tokens, 10);
        
        if (token_count >= 5) {
            int id = atoi(tokens[0]);
            char* name = tokens[1];
            int capacity = atoi(tokens[2]);
            int thrill_level = atoi(tokens[3]);
            int base_wait_time = atoi(tokens[4]);
            
            if (validateRideData(id, capacity, thrill_level)) {
                Ride* ride = createRide(id, name, capacity, thrill_level, base_wait_time);
                if (ride) {
                    addRideToList(rides, ride);
                    count++;
                }
            }
        }
    }
    
    fclose(file);
    return count;
}

/* Save rides to file */
int saveRidesToFile(const char* filename, RideList* rides) {
    if (!filename || !rides) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        logError("Could not open rides file for writing");
        return 0;
    }
    
    fprintf(file, "id,name,capacity,thrill_level,wait_time\n");
    
    RideNode* current = rides->head;
    int count = 0;
    
    while (current) {
        Ride* ride = current->ride;
        fprintf(file, "%d,%s,%d,%d,%d\n",
                ride->id,
                ride->name,
                ride->capacity,
                ride->thrill_level,
                ride->current_wait_time);
        count++;
        current = current->next;
    }
    
    fclose(file);
    return count;
}

/* Load visitors from file */
int loadVisitorsFromFile(const char* filename, VisitorGroup* visitors) {
    if (!filename || !visitors) return 0;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        logError("Could not open visitors file");
        return 0;
    }
    
    char line[256];
    int count = 0;
    
    // Skip header
    if (fgets(line, sizeof(line), file)) {
        if (!strstr(line, "id") && !strstr(line, "name")) {
            rewind(file);
        }
    }
    
    while (fgets(line, sizeof(line), file)) {
        char* tokens[10];
        int token_count = parseCSVLine(line, tokens, 10);
        
        if (token_count >= 3) {
            int id = atoi(tokens[0]);
            char* name = tokens[1];
            int thrill_preference = atoi(tokens[2]);
            
            if (validateVisitorData(id, thrill_preference)) {
                Visitor* visitor = createVisitor(id, name, thrill_preference);
                if (visitor) {
                    addVisitorToGroup(visitors, visitor);
                    count++;
                }
            }
        }
    }
    
    fclose(file);
    return count;
}

/* Save visitors to file */
int saveVisitorsToFile(const char* filename, VisitorGroup* visitors) {
    if (!filename || !visitors) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        logError("Could not open visitors file for writing");
        return 0;
    }
    
    fprintf(file, "id,name,thrill_preference\n");
    
    VisitorNode* current = visitors->head;
    int count = 0;
    
    while (current) {
        Visitor* visitor = current->visitor;
        fprintf(file, "%d,%s,%d\n",
                visitor->id,
                visitor->name,
                visitor->thrill_preference);
        count++;
        current = current->next;
    }
    
    fclose(file);
    return count;
}

/* Load park graph from file */
int loadParkGraph(const char* filename, Graph* graph) {
    if (!filename || !graph) return 0;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        logError("Could not open park map file");
        return 0;
    }
    
    char line[256];
    int count = 0;
    
    // Skip header
    if (fgets(line, sizeof(line), file)) {
        if (!strstr(line, "ride1") && !strstr(line, "ride2")) {
            rewind(file);
        }
    }
    
    while (fgets(line, sizeof(line), file)) {
        char* tokens[10];
        int token_count = parseCSVLine(line, tokens, 10);
        
        if (token_count >= 3) {
            int ride1_id = atoi(tokens[0]);
            int ride2_id = atoi(tokens[1]);
            int distance = atoi(tokens[2]);
            
            connectRides(graph, ride1_id, ride2_id, distance);
            count++;
        }
    }
    
    fclose(file);
    return count;
}

/* Save park graph to file */
int saveParkGraph(const char* filename, Graph* graph) {
    if (!filename || !graph) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        logError("Could not open park map file for writing");
        return 0;
    }
    
    fprintf(file, "ride1_id,ride2_id,distance\n");
    
    int count = 0;
    for (int i = 0; i < MAX_RIDES; i++) {
        if (graph->nodes[i]) {
            Edge* edge = graph->nodes[i]->edges;
            while (edge) {
                // Only write each edge once (avoid duplicates)
                if (i < edge->destination_id) {
                    fprintf(file, "%d,%d,%d\n", i, edge->destination_id, edge->distance);
                    count++;
                }
                edge = edge->next;
            }
        }
    }
    
    fclose(file);
    return count;
}

/* Log error */
void logError(const char* message) {
    fprintf(stderr, "[ERROR] %s\n", message);
}

/* Log info */
void logInfo(const char* message) {
    printf("[INFO] %s\n", message);
}

/* Export statistics to CSV */
int exportStatisticsToCSV(const char* filename, RideList* rides) {
    if (!filename || !rides) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) return 0;
    
    fprintf(file, "Ride ID,Name,Total Visitors,Avg Wait Time,Thrill Level\n");
    
    RideNode* current = rides->head;
    while (current) {
        Ride* ride = current->ride;
        fprintf(file, "%d,%s,%d,%d,%d\n",
                ride->id,
                ride->name,
                ride->total_visitors_served,
                ride->current_wait_time,
                ride->thrill_level);
        current = current->next;
    }
    
    fclose(file);
    return 1;
}

/* Export visitor history to CSV */
int exportVisitorHistoryToCSV(const char* filename, VisitorGroup* visitors) {
    if (!filename || !visitors) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) return 0;
    
    fprintf(file, "Visitor ID,Name,Rides Completed,Distance Traveled,Satisfaction\n");
    
    VisitorNode* current = visitors->head;
    while (current) {
        Visitor* visitor = current->visitor;
        fprintf(file, "%d,%s,%d,%d,%.1f\n",
                visitor->id,
                visitor->name,
                visitor->rides_completed,
                visitor->total_distance_traveled,
                visitor->satisfaction_score);
        current = current->next;
    }
    
    fclose(file);
    return 1;
}
