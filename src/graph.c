#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/graph.h"

/* Create graph */
Graph* createGraph() {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Error: Memory allocation failed for graph\n");
        return NULL;
    }
    
    g->num_nodes = 0;
    for (int i = 0; i < MAX_RIDES; i++) {
        g->nodes[i] = NULL;
    }
    
    return g;
}

/* Add ride to graph */
void addRideToGraph(Graph* g, int ride_id) {
    if (!g || ride_id < 0 || ride_id >= MAX_RIDES) return;
    
    if (g->nodes[ride_id] == NULL) {
        GraphNode* node = (GraphNode*)malloc(sizeof(GraphNode));
        if (!node) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }
        
        node->ride_id = ride_id;
        node->edges = NULL;
        g->nodes[ride_id] = node;
        g->num_nodes++;
    }
}

/* Connect two rides with an edge */
void connectRides(Graph* g, int ride1_id, int ride2_id, int distance) {
    if (!g) return;
    
    addRideToGraph(g, ride1_id);
    addRideToGraph(g, ride2_id);
    
    // Add edge from ride1 to ride2
    Edge* edge1 = (Edge*)malloc(sizeof(Edge));
    if (edge1) {
        edge1->destination_id = ride2_id;
        edge1->distance = distance;
        edge1->next = g->nodes[ride1_id]->edges;
        g->nodes[ride1_id]->edges = edge1;
    }
    
    // Add edge from ride2 to ride1 (undirected graph)
    Edge* edge2 = (Edge*)malloc(sizeof(Edge));
    if (edge2) {
        edge2->destination_id = ride1_id;
        edge2->distance = distance;
        edge2->next = g->nodes[ride2_id]->edges;
        g->nodes[ride2_id]->edges = edge2;
    }
}

/* Free graph */
void freeGraph(Graph* g) {
    if (!g) return;
    
    for (int i = 0; i < MAX_RIDES; i++) {
        if (g->nodes[i]) {
            Edge* edge = g->nodes[i]->edges;
            while (edge) {
                Edge* next = edge->next;
                free(edge);
                edge = next;
            }
            free(g->nodes[i]);
        }
    }
    
    free(g);
}

/* Print graph */
void printGraph(Graph* g, RideList* rides) {
    if (!g) return;
    
    printf("\n========== PARK MAP ==========\n");
    
    for (int i = 0; i < MAX_RIDES; i++) {
        if (g->nodes[i]) {
            Ride* ride = findRideById(rides, i);
            printf("\n[%d] %s:\n", i, ride ? ride->name : "Unknown");
            
            Edge* edge = g->nodes[i]->edges;
            while (edge) {
                Ride* dest_ride = findRideById(rides, edge->destination_id);
                printf("  -> [%d] %s (%d meters)\n",
                       edge->destination_id,
                       dest_ride ? dest_ride->name : "Unknown",
                       edge->distance);
                edge = edge->next;
            }
        }
    }
    
    printf("==============================\n");
}

/* Dijkstra's shortest path */
PathInfo* dijkstraShortestPath(Graph* g, int start_id, int end_id) {
    if (!g || !g->nodes[start_id] || !g->nodes[end_id]) return NULL;
    
    int dist[MAX_RIDES];
    int prev[MAX_RIDES];
    int visited[MAX_RIDES] = {0};
    
    for (int i = 0; i < MAX_RIDES; i++) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    
    dist[start_id] = 0;
    
    for (int count = 0; count < g->num_nodes; count++) {
        int min_dist = INT_MAX;
        int u = -1;
        
        for (int i = 0; i < MAX_RIDES; i++) {
            if (!visited[i] && g->nodes[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }
        
        if (u == -1) break;
        visited[u] = 1;
        
        if (u == end_id) break;
        
        Edge* edge = g->nodes[u]->edges;
        while (edge) {
            int v = edge->destination_id;
            if (!visited[v] && dist[u] != INT_MAX) {
                int new_dist = dist[u] + edge->distance;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    prev[v] = u;
                }
            }
            edge = edge->next;
        }
    }
    
    if (dist[end_id] == INT_MAX) return NULL;
    
    // Reconstruct path
    int path_length = 0;
    int temp = end_id;
    while (temp != -1) {
        path_length++;
        temp = prev[temp];
    }
    
    PathInfo* path_info = (PathInfo*)malloc(sizeof(PathInfo));
    path_info->path = (int*)malloc(sizeof(int) * path_length);
    path_info->path_length = path_length;
    path_info->total_distance = dist[end_id];
    
    temp = end_id;
    for (int i = path_length - 1; i >= 0; i--) {
        path_info->path[i] = temp;
        temp = prev[temp];
    }
    
    return path_info;
}

/* Display path */
void displayPath(int path[], int path_length, RideList* rides) {
    if (!path || path_length <= 0) {
        printf("No path available.\n");
        return;
    }
    
    printf("\n--- Path ---\n");
    for (int i = 0; i < path_length; i++) {
        Ride* ride = findRideById(rides, path[i]);
        printf("%d. [%d] %s\n", i + 1, path[i], ride ? ride->name : "Unknown");
        if (i < path_length - 1) printf("   |\n   v\n");
    }
    printf("------------\n");
}

/* Get distance between two rides */
int getDistanceBetweenRides(Graph* g, int ride1_id, int ride2_id) {
    if (!g || !g->nodes[ride1_id]) return -1;
    
    Edge* edge = g->nodes[ride1_id]->edges;
    while (edge) {
        if (edge->destination_id == ride2_id) {
            return edge->distance;
        }
        edge = edge->next;
    }
    
    return -1;
}

/* Find edge */
Edge* findEdge(GraphNode* node, int destination_id) {
    if (!node) return NULL;
    
    Edge* edge = node->edges;
    while (edge) {
        if (edge->destination_id == destination_id) {
            return edge;
        }
        edge = edge->next;
    }
    
    return NULL;
}

/* Free path info */
void freePathInfo(PathInfo* path_info) {
    if (path_info) {
        if (path_info->path) free(path_info->path);
        free(path_info);
    }
}

/* Calculate total distance */
int calculateTotalDistance(int path[], int path_length, Graph* g) {
    if (!path || path_length < 2 || !g) return 0;
    
    int total = 0;
    for (int i = 0; i < path_length - 1; i++) {
        int dist = getDistanceBetweenRides(g, path[i], path[i + 1]);
        if (dist > 0) total += dist;
    }
    
    return total;
}

/* Find nearest rides */
int* findNearestRides(Graph* g, int current_location, int n, int* count) {
    // Simplified implementation
    *count = 0;
    return NULL;
}

/* Optimize visitor route */
PathInfo* optimizeVisitorRoute(Graph* g, int start_location, int target_rides[], int count) {
    // Simplified implementation
    return NULL;
}

/* Visualize route */
void visualizeRoute(PathInfo* path_info, RideList* rides) {
    if (!path_info) return;
    displayPath(path_info->path, path_info->path_length, rides);
    printf("Total distance: %d meters\n", path_info->total_distance);
}

/* Build graph from file */
int buildGraphFromFile(Graph* g, const char* filename) {
    // Implementation in file_io.c
    return 0;
}
