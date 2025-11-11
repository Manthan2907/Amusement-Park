#ifndef GRAPH_H
#define GRAPH_H

#include "config.h"
#include "ride_manager.h"

/* Edge Structure */
typedef struct Edge {
    int destination_id;
    int distance;  // In meters
    struct Edge* next;
} Edge;

/* Graph Node Structure (Adjacency List) */
typedef struct GraphNode {
    int ride_id;
    Edge* edges;  // Linked list of edges
} GraphNode;

/* Graph Structure */
typedef struct Graph {
    GraphNode* nodes[MAX_RIDES];
    int num_nodes;
} Graph;

/* Path Structure for Dijkstra */
typedef struct PathInfo {
    int* path;
    int path_length;
    int total_distance;
} PathInfo;

/* Function Prototypes */

// Graph Creation and Management
Graph* createGraph();
void addRideToGraph(Graph* g, int ride_id);
void connectRides(Graph* g, int ride1_id, int ride2_id, int distance);
void freeGraph(Graph* g);

// Graph Display
void printGraph(Graph* g, RideList* rides);
void displayPath(int path[], int path_length, RideList* rides);

// Pathfinding Algorithms
PathInfo* dijkstraShortestPath(Graph* g, int start_id, int end_id);
int* findNearestRides(Graph* g, int current_location, int n, int* count);
int calculateTotalDistance(int path[], int path_length, Graph* g);

// Route Optimization
PathInfo* optimizeVisitorRoute(Graph* g, int start_location, int target_rides[], int count);
void visualizeRoute(PathInfo* path_info, RideList* rides);

// Helper Functions
int getDistanceBetweenRides(Graph* g, int ride1_id, int ride2_id);
Edge* findEdge(GraphNode* node, int destination_id);
void freePathInfo(PathInfo* path_info);

// Graph Loading
int buildGraphFromFile(Graph* g, const char* filename);

#endif /* GRAPH_H */
