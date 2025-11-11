#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/web_server.h"
#include "../include/mongoose.h"
#include "../include/visitor.h"
#include "../include/ride_manager.h"
#include "../include/utils.h"
#include "../include/config.h"
#include "../include/priority_queue.h"
#include "../include/bst.h"
#include "../include/graph.h"

/* External declarations from web_server.c */
extern RideList* g_rides;
extern VisitorGroup** g_groups;
extern int* g_group_count;
extern DualQueue** g_queues;
extern BST* g_bst;
extern Graph* g_park_map;  /* Park layout graph */

/* Helper function declarations */
extern void sendJSON(struct mg_connection *c, int status, const char *json);
extern void buildRideJSON(char *buffer, size_t size, Ride *r);

// Helper function to find visitor by ID
static Visitor* findVisitorById(int visitor_id) {
    for (int i = 0; i < *g_group_count; i++) {
        VisitorNode* node = g_groups[i]->head;
        while (node) {
            if (node->visitor->id == visitor_id) {
                return node->visitor;
            }
            node = node->next;
        }
    }
    return NULL;
}

void handleGetVisitorHistory(struct mg_connection *c, struct mg_http_message *hm) {
    int visitor_id = 0;
    sscanf(hm->uri.buf, "/api/visitors/%d/history", &visitor_id);
    
    if (visitor_id < 1000) {
        sendJSON(c, 400, "{\"error\":\"Invalid visitor ID\"}");
        return;
    }
    
    // Find visitor
    Visitor* visitor = findVisitorById(visitor_id);
    if (!visitor) {
        sendJSON(c, 404, "{\"error\":\"Visitor not found\"}");
        return;
    }
    
    // Get visitor's ride history
    RideHistoryEntry* history = getVisitorRideHistory(visitor);
    if (!history) {
        sendJSON(c, 404, "{\"error\":\"No ride history found\"}");
        return;
    }
    
    // Build history JSON
    char response[4096] = "{\"visitor_id\":";
    char temp[256];
    sprintf(temp, "%d,\"rides\":[", visitor_id);
    strcat(response, temp);
    
    RideHistoryEntry* entry = history;
    int first = 1;
    
    while (entry) {
        if (!first) strcat(response, ",");
        first = 0;
        
        Ride* ride = findRideById(g_rides, entry->ride_id);
        if (ride) {
            sprintf(temp, "{\"ride_id\":%d,\"name\":\"%s\",\"timestamp\":%lld}",
                    ride->id, ride->name, (long long)entry->timestamp);
            strcat(response, temp);
        }
        entry = entry->next;
    }
    
    strcat(response, "]}");
    sendJSON(c, 200, response);
}

void handleGetRideSuggestions(struct mg_connection *c, struct mg_http_message *hm) {
    int visitor_id = 0;
    sscanf(hm->uri.buf, "/api/visitors/%d/suggest", &visitor_id);
    
    if (visitor_id < 1000) {
        sendJSON(c, 400, "{\"error\":\"Invalid visitor ID\"}");
        return;
    }
    
    // Find visitor
    Visitor* visitor = findVisitorById(visitor_id);
    if (!visitor) {
        sendJSON(c, 404, "{\"error\":\"Visitor not found\"}");
        return;
    }
    
    // Get suggestions using priority queue
    PriorityQueue* pq = buildPriorityQueue(visitor, g_rides, visitor->current_location);
    if (!pq) {
        sendJSON(c, 500, "{\"error\":\"Failed to generate suggestions\"}");
        return;
    }
    
    // Build suggestions JSON
    char response[4096] = "{\"visitor_id\":";
    char temp[512];
    sprintf(temp, "%d,\"suggestions\":[", visitor_id);
    strcat(response, temp);
    
    int first = 1;
    for (int i = 0; i < 3 && !isPriorityQueueEmpty(pq); i++) {
        Ride* ride = extractMax(pq);
        if (!ride) break;
        
        if (!first) strcat(response, ",");
        first = 0;
        
        float thrill_match = calculateThrillMatch(visitor->thrill_preference, ride->thrill_level);
        float priority = calculatePriority(visitor, ride, abs(visitor->current_location - ride->id));
        
        sprintf(temp, "{\"ride_id\":%d,\"name\":\"%s\",\"wait_time\":%d,"
                "\"thrill_match\":%.1f,\"priority_score\":%.1f}",
                ride->id, ride->name, ride->current_wait_time,
                thrill_match, priority);
        strcat(response, temp);
    }
    
    strcat(response, "]}");
    freePriorityQueue(pq);
    sendJSON(c, 200, response);
}

void handleGetRidesByWaitTime(struct mg_connection *c) {
    if (!g_bst) {
        sendJSON(c, 500, "{\"error\":\"BST not initialized\"}");
        return;
    }
    
    char response[4096] = "{\"rides\":[";
    char temp[512];
    int first = 1;
    
    // Visit BST nodes in-order
    void visitNode(BSTNode* node) {
        if (!node) return;
        
        visitNode(node->left);
        
        if (!first) strcat(response, ",");
        first = 0;
        
        buildRideJSON(temp, sizeof(temp), node->ride);
        strcat(response, temp);
        
        visitNode(node->right);
    }
    
    visitNode(g_bst->root);
    strcat(response, "]}");
    sendJSON(c, 200, response);
}

void handleFindPath(struct mg_connection *c, struct mg_http_message *hm) {
    double from_d = 0, to_d = 0;
    if (!mg_json_get_num(hm->body, "$.from_ride", &from_d) ||
        !mg_json_get_num(hm->body, "$.to_ride", &to_d)) {
        sendJSON(c, 400, "{\"error\":\"Missing from_ride or to_ride\"}");
        return;
    }
    
    int from_ride = (int)from_d;
    int to_ride = (int)to_d;
    
    // Verify rides exist
    if (!findRideById(g_rides, from_ride) || !findRideById(g_rides, to_ride)) {
        sendJSON(c, 404, "{\"error\":\"Invalid ride IDs\"}");
        return;
    }
    
    int path[MAX_RIDES];
    int distances[MAX_RIDES];
    int path_length = 0;
    
    // Find shortest path using Dijkstra's algorithm
    PathInfo* path_info = dijkstraShortestPath(g_park_map, from_ride, to_ride);
    if (!path_info || path_info->path_length == 0) {
        sendJSON(c, 500, "{\"error\":\"Failed to find path between rides\"}");
        if (path_info) freePathInfo(path_info);
        return;
    }
    
    // Build path JSON
    char response[4096] = "{\"path\":[";
    char temp[256];
    int curr_distance = 0;
    
    for (int i = 0; i < path_info->path_length; i++) {
        if (i > 0) {
            strcat(response, ",");
            curr_distance += getDistanceBetweenRides(g_park_map, path_info->path[i-1], path_info->path[i]);
        }
        
        Ride* ride = findRideById(g_rides, path_info->path[i]);
        sprintf(temp, "{\"ride_id\":%d,\"name\":\"%s\",\"distance\":%d}",
                ride->id, ride->name, curr_distance);
        strcat(response, temp);
    }
    
    strcat(response, "]}");
    freePathInfo(path_info);
    sendJSON(c, 200, response);
}

void handleUndoLastRide(struct mg_connection *c, struct mg_http_message *hm) {
    int visitor_id = 0;
    sscanf(hm->uri.buf, "/api/visitors/%d/undo", &visitor_id);
    
    if (visitor_id < 1000) {
        sendJSON(c, 400, "{\"error\":\"Invalid visitor ID\"}");
        return;
    }
    
    Visitor* visitor = findVisitorById(visitor_id);
    if (!visitor) {
        sendJSON(c, 404, "{\"error\":\"Visitor not found\"}");
        return;
    }
    
    if (undoLastRide(visitor)) {
        sendJSON(c, 200, "{\"message\":\"Last ride successfully undone\"}");
    } else {
        sendJSON(c, 404, "{\"error\":\"No rides to undo\"}");
    }
}