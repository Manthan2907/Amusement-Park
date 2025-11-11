#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/web_server.h"
#include "../include/mongoose.h"
#include "../include/config.h"
#include "../include/utils.h"
#include "../include/priority_queue.h"
#include "../include/stack.h"

/* Function declarations from web_server_handlers.c */
void handleGetVisitorHistory(struct mg_connection *c, struct mg_http_message *hm);
void handleGetRideSuggestions(struct mg_connection *c, struct mg_http_message *hm);
void handleUndoLastRide(struct mg_connection *c, struct mg_http_message *hm);
void handleGetRidesByWaitTime(struct mg_connection *c);
void handleFindPath(struct mg_connection *c, struct mg_http_message *hm);

/* Global references to park data */
RideList* g_rides = NULL;
VisitorGroup** g_groups = NULL;
int* g_group_count = NULL;
DualQueue** g_queues = NULL;
BST* g_bst = NULL;
Graph* g_park_map = NULL;
static struct mg_mgr mgr;
static int server_running = 0;

/* Helper function to create JSON response */
void sendJSON(struct mg_connection *c, int status, const char *json) {
    mg_http_reply(c, status, "Content-Type: application/json\r\n", "%s", json);
}

/* Helper to build visitor JSON */
static void buildVisitorJSON(char *buffer, size_t size, Visitor *v) {
    snprintf(buffer, size,
        "{\"id\":%d,\"name\":\"%s\",\"current_location\":%d,"
        "\"thrill_preference\":%d,\"rides_completed\":%d,"
        "\"total_distance_traveled\":%d,\"satisfaction_score\":%.2f,"
        "\"ticket_type\":%d,\"fast_passes_remaining\":%d,\"entry_time\":%d}",
        v->id, v->name, v->current_location, v->thrill_preference,
        v->rides_completed, v->total_distance_traveled, v->satisfaction_score,
        v->ticket_type, v->fast_passes_remaining, v->entry_time);
}

/* Helper to build ride JSON */
void buildRideJSON(char *buffer, size_t size, Ride *r) {
    snprintf(buffer, size,
        "{\"id\":%d,\"name\":\"%s\",\"thrill_level\":%d,"
        "\"capacity\":%d,\"is_operational\":%d,"
        "\"wait_time\":%d,\"queue_size\":%d}",
        r->id, r->name, r->thrill_level, r->capacity, r->is_operational,
        r->current_wait_time, 
        g_queues[r->id] ? (g_queues[r->id]->regular_queue->size + g_queues[r->id]->fastpass_queue->size) : 0);
}

/* GET /api/visitors - Get all visitors */
static void handleGetVisitors(struct mg_connection *c) {
    char response[8192] = "[";
    int first = 1;
    
    for (int i = 0; i < *g_group_count; i++) {
        VisitorNode *node = g_groups[i]->head;
        while (node) {
            if (!first) strcat(response, ",");
            first = 0;
            
            char visitor_json[512];
            buildVisitorJSON(visitor_json, sizeof(visitor_json), node->visitor);
            strcat(response, visitor_json);
            
            node = node->next;
        }
    }
    
    strcat(response, "]");
    sendJSON(c, 200, response);
}

/* POST /api/visitors - Add new visitor */
static void handleAddVisitor(struct mg_connection *c, struct mg_http_message *hm) {
    char name[MAX_NAME_LENGTH] = "";
    int thrill = 5, ticket_type = 0;
    double thrill_d = 5, ticket_d = 0;
    
    char *str = mg_json_get_str(hm->body, "$.name");
    if (str) {
        strncpy(name, str, MAX_NAME_LENGTH - 1);
        name[MAX_NAME_LENGTH - 1] = '\0';
        free(str);
    }
    if (mg_json_get_num(hm->body, "$.thrill_preference", &thrill_d)) {
        thrill = (int)thrill_d;
    }
    if (mg_json_get_num(hm->body, "$.ticket_type", &ticket_d)) {
        ticket_type = (int)ticket_d;
    }
    
    if (strlen(name) == 0) {
        sendJSON(c, 400, "{\"error\":\"Name is required\"}");
        return;
    }
    
    // Create visitor with generated ID
    int new_id = generateVisitorID();  // Uses the counter starting from 1000
    Visitor *visitor = createVisitorWithTicket(new_id, name, thrill, ticket_type);
    
    if (!visitor) {
        sendJSON(c, 500, "{\"error\":\"Failed to create visitor\"}");
        return;
    }
    
    // Add to first group or create new group
    if (*g_group_count == 0) {
        g_groups[0] = createVisitorGroup(0);
        (*g_group_count)++;
    }
    
    addVisitorToGroup(g_groups[0], visitor);
    
    char response[512];
    buildVisitorJSON(response, sizeof(response), visitor);
    sendJSON(c, 201, response);
}

/* GET /api/rides - Get all rides */
static void handleGetRides(struct mg_connection *c) {
    char response[8192] = "[";
    int first = 1;
    
    RideNode *node = g_rides->head;
    while (node) {
        if (!first) strcat(response, ",");
        first = 0;
        
        char ride_json[512];
        buildRideJSON(ride_json, sizeof(ride_json), node->ride);
        strcat(response, ride_json);
        
        node = node->next;
    }
    
    strcat(response, "]");
    sendJSON(c, 200, response);
}

/* POST /api/rides - Add new ride */
static void handleAddRide(struct mg_connection *c, struct mg_http_message *hm) {
    char name[MAX_NAME_LENGTH] = "";
    int thrill = 5, capacity = 20, duration = 5;
    double thrill_d = 5, capacity_d = 20, duration_d = 5;
    char *str = mg_json_get_str(hm->body, "$.name");
    if (str) {
        strncpy(name, str, MAX_NAME_LENGTH - 1);
        name[MAX_NAME_LENGTH - 1] = '\0';
        free(str);
    }
    if (mg_json_get_num(hm->body, "$.thrill_level", &thrill_d)) thrill = (int)thrill_d;
    if (mg_json_get_num(hm->body, "$.capacity", &capacity_d)) capacity = (int)capacity_d;
    if (mg_json_get_num(hm->body, "$.duration", &duration_d)) duration = (int)duration_d;
    
    if (strlen(name) == 0) {
        sendJSON(c, 400, "{\"error\":\"Name is required\"}");
        return;
    }
    
    // Create ride
    Ride *ride = createRide(g_rides->count + 1, name, capacity, thrill, duration);
    if (!ride) {
        sendJSON(c, 500, "{\"error\":\"Failed to create ride\"}");
        return;
    }
    
    addRideToList(g_rides, ride);
    
    // Create queue for ride
    if (ride->id < MAX_RIDES) {
        g_queues[ride->id] = createDualQueue(ride->id, 4);
        // Initialize wait time based on queue size and capacity
        ride->current_wait_time = calculateEstimatedWaitTime(0, ride->capacity);
    }
    
    char response[512];
    buildRideJSON(response, sizeof(response), ride);
    sendJSON(c, 201, response);
}

/* GET /api/queues - Get queue status */
static void handleGetQueues(struct mg_connection *c) {
    char response[8192] = "[";
    int first = 1;
    
    RideNode *node = g_rides->head;
    while (node) {
        Ride *ride = node->ride;
        DualQueue *queue = g_queues[ride->id];
        
        if (queue) {
            if (!first) strcat(response, ",");
            first = 0;
            
            char queue_json[512];
            snprintf(queue_json, sizeof(queue_json),
                "{\"ride_id\":%d,\"ride_name\":\"%s\",\"size\":%d,"
                "\"capacity\":%d,\"wait_time\":%d,\"premium_size\":%d,\"is_operational\":%d}",
                ride->id, ride->name,
                queue->regular_queue->size + queue->fastpass_queue->size,
                ride->capacity, ride->current_wait_time,
                queue->fastpass_queue->size, ride->is_operational);
            strcat(response, queue_json);
        }
        
        node = node->next;
    }
    
    strcat(response, "]");
    sendJSON(c, 200, response);
}

/* DELETE /api/visitors/:id - Delete visitor */
static void handleDeleteVisitor(struct mg_connection *c, struct mg_http_message *hm) {
    // Parse visitor ID from URI (e.g., /api/visitors/1001)
    int visitor_id = 0;
    sscanf(hm->uri.buf, "/api/visitors/%d", &visitor_id);
    
    if (visitor_id < 1000) {
        sendJSON(c, 400, "{\"error\":\"Invalid visitor ID\"}");
        return;
    }
    
    // Find and remove visitor
    for (int i = 0; i < *g_group_count; i++) {
        if (g_groups[i]) {
            VisitorNode *current = g_groups[i]->head;
            VisitorNode *prev = NULL;
            
            while (current) {
                if (current->visitor->id == visitor_id) {
                    if (prev) {
                        prev->next = current->next;
                    } else {
                        g_groups[i]->head = current->next;
                    }
                    g_groups[i]->size--;
                    free(current->visitor);
                    free(current);
                    sendJSON(c, 200, "{\"message\":\"Visitor deleted\"}");
                    return;
                }
                prev = current;
                current = current->next;
            }
        }
    }
    
    sendJSON(c, 404, "{\"error\":\"Visitor not found\"}");
}

/* DELETE /api/rides/:id - Delete ride */
static void handleDeleteRide(struct mg_connection *c, struct mg_http_message *hm) {
    int ride_id = 0;
    sscanf(hm->uri.buf, "/api/rides/%d", &ride_id);
    
    Ride *ride = findRideById(g_rides, ride_id);
    if (!ride) {
        sendJSON(c, 404, "{\"error\":\"Ride not found\"}");
        return;
    }
    
    removeRideFromList(g_rides, ride_id);
    sendJSON(c, 200, "{\"message\":\"Ride deleted\"}");
}

/* PUT /api/rides/:id/toggle - Toggle ride status */
static void handleToggleRide(struct mg_connection *c, struct mg_http_message *hm) {
    int ride_id = 0;
    sscanf(hm->uri.buf, "/api/rides/%d/toggle", &ride_id);
    
    Ride *ride = findRideById(g_rides, ride_id);
    if (!ride) {
        sendJSON(c, 404, "{\"error\":\"Ride not found\"}");
        return;
    }
    
    ride->is_operational = !ride->is_operational;
    
    char response[256];
    snprintf(response, sizeof(response),
        "{\"message\":\"Ride status updated\",\"is_operational\":%d}",
        ride->is_operational);
    sendJSON(c, 200, response);
}

/* POST /api/rides/:id/experience - Visitor enjoys ride */
static void handleExperienceRide(struct mg_connection *c, struct mg_http_message *hm) {
    int ride_id = 0;
    sscanf(hm->uri.buf, "/api/rides/%d/experience", &ride_id);
    
    Ride *ride = findRideById(g_rides, ride_id);
    if (!ride) {
        sendJSON(c, 404, "{\"error\":\"Ride not found\"}");
        return;
    }
    
    if (!ride->is_operational) {
        sendJSON(c, 400, "{\"error\":\"Ride is closed for maintenance\"}");
        return;
    }
    
    // Get visitor ID from JSON body
    int visitor_id = 0;
    double visitor_id_d = 0;
    if (mg_json_get_num(hm->body, "$.visitor_id", &visitor_id_d)) {
        visitor_id = (int)visitor_id_d;
    }
    
    if (visitor_id < 1000) {
        sendJSON(c, 400, "{\"error\":\"Invalid visitor ID\"}");
        return;
    }
    
    // Find visitor
    Visitor *visitor = NULL;
    for (int i = 0; i < *g_group_count; i++) {
        if (g_groups[i]) {
            VisitorNode *node = g_groups[i]->head;
            while (node) {
                if (node->visitor->id == visitor_id) {
                    visitor = node->visitor;
                    break;
                }
                node = node->next;
            }
            if (visitor) break;
        }
    }
    
    if (!visitor) {
        sendJSON(c, 404, "{\"error\":\"Visitor not found\"}");
        return;
    }
    
    // Update visitor stats
    float satisfaction = calculateSatisfactionScore(visitor, ride);
    updateVisitorStats(visitor, 100, satisfaction);
    updateVisitorLocation(visitor, ride->id);
    
    // Add to ride history
    addRideToHistory(visitor, ride->id);
    
    // Update ride stats
    incrementVisitorCount(ride, 1);
    
    // Update visitor info
    visitor->satisfaction_score = satisfaction;
    
    char response[512];
    snprintf(response, sizeof(response),
        "{\"message\":\"Ride completed!\",\"satisfaction\":%.1f,\"visitor_name\":\"%s\"}",
        satisfaction, visitor->name);
    sendJSON(c, 200, response);
}

/* GET /api/stats - Get park statistics */
static void handleGetStats(struct mg_connection *c) {
    int total_visitors = 0;
    int premium_visitors = 0;
    float total_satisfaction = 0;
    int total_distance = 0;
    int active_rides = 0;
    int total_capacity = 0;
    int total_wait_time = 0;
    int ride_count = 0;
    
    // Count visitors
    for (int i = 0; i < *g_group_count; i++) {
        VisitorNode *node = g_groups[i]->head;
        while (node) {
            total_visitors++;
            if (node->visitor->ticket_type == TICKET_PREMIUM) {
                premium_visitors++;
            }
            total_satisfaction += node->visitor->satisfaction_score;
            total_distance += node->visitor->total_distance_traveled;
            node = node->next;
        }
    }
    
    // Count rides
    RideNode *node = g_rides->head;
    while (node) {
        ride_count++;
        if (node->ride->is_operational) {
            active_rides++;
        }
        total_capacity += node->ride->capacity;
        total_wait_time += node->ride->current_wait_time;
        node = node->next;
    }
    
    float avg_satisfaction = total_visitors > 0 ? total_satisfaction / total_visitors : 0;
    int avg_wait_time = ride_count > 0 ? total_wait_time / ride_count : 0;
    
    char response[1024];
    snprintf(response, sizeof(response),
        "{\"total_visitors\":%d,\"premium_visitors\":%d,"
        "\"avg_satisfaction\":%.2f,\"total_distance\":%d,"
        "\"total_rides\":%d,\"active_rides\":%d,"
        "\"avg_wait_time\":%d,\"total_capacity\":%d}",
        total_visitors, premium_visitors, avg_satisfaction, total_distance,
        ride_count, active_rides, avg_wait_time, total_capacity);
    
    sendJSON(c, 200, response);
}

/* HTTP event handler */
static void httpHandler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // CORS headers
        if (mg_strcmp(mg_str_n(hm->uri.buf, 5), mg_str("/api/")) == 0) {
            mg_printf(c, "HTTP/1.1 200 OK\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n");
            
            if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
                mg_printf(c, "\r\n");
                return;
            }
        }
        // API Routes
        if (mg_strcmp(hm->uri, mg_str("/api/visitors")) == 0) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                handleGetVisitors(c);
            } else if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
                handleAddVisitor(c, hm);
            }
        }
        else if (strncmp(hm->uri.buf, "/api/visitors/", 14) == 0) {
            if (mg_strcmp(hm->method, mg_str("DELETE")) == 0) {
                handleDeleteVisitor(c, hm);
            }
        }
        else if (mg_strcmp(hm->uri, mg_str("/api/rides")) == 0) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                handleGetRides(c);
            } else if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
                handleAddRide(c, hm);
            }
        }
        else if (strncmp(hm->uri.buf, "/api/rides/", 11) == 0) {
            if (mg_strcmp(hm->method, mg_str("DELETE")) == 0) {
                handleDeleteRide(c, hm);
            } else if (mg_strcmp(hm->method, mg_str("PUT")) == 0) {
                handleToggleRide(c, hm);
            } else if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
                handleExperienceRide(c, hm);
            }
        }
        else if (mg_strcmp(hm->uri, mg_str("/api/queues")) == 0) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                handleGetQueues(c);
            }
        }
        else if (mg_strcmp(hm->uri, mg_str("/api/stats")) == 0) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                handleGetStats(c);
            }
        }
        else if (strncmp(hm->uri.buf, "/api/visitors/", 14) == 0 && strstr(hm->uri.buf, "/history")) {
            handleGetVisitorHistory(c, hm);  // Get visitor ride history
        }
        else if (strncmp(hm->uri.buf, "/api/visitors/", 14) == 0 && strstr(hm->uri.buf, "/suggest")) {
            handleGetRideSuggestions(c, hm);  // Get ride suggestions
        }
        else if (mg_strcmp(hm->uri, mg_str("/api/rides/by-wait-time")) == 0) {
            handleGetRidesByWaitTime(c);  // BST-based wait time sorting
        }
        else if (mg_strcmp(hm->uri, mg_str("/api/pathfind")) == 0) {
            handleFindPath(c, hm);  // Find shortest path between rides
        }
        else if (strncmp(hm->uri.buf, "/api/visitors/", 14) == 0 && strstr(hm->uri.buf, "/undo")) {
            handleUndoLastRide(c, hm);  // Undo last ride
        }
        else {
            // Serve static files
            struct mg_http_serve_opts opts = {.root_dir = WEB_ROOT_DIR};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
    
}

/* Start web server */
void startWebServer(RideList* rides, VisitorGroup** groups, int* group_count,
                   DualQueue** queues, BST* bst, Graph* park_map) {
    printf("DEBUG: startWebServer called\n");
    g_rides = rides;
    g_groups = groups;
    g_group_count = group_count;
    g_queues = queues;
    g_bst = bst;
    g_park_map = park_map;
    
    mg_mgr_init(&mgr);
    
    char addr[64];
    snprintf(addr, sizeof(addr), "http://0.0.0.0:%s", WEB_SERVER_PORT);
    
    if (mg_http_listen(&mgr, addr, httpHandler, NULL) == NULL) {
        printf("Failed to start web server on port %s\n", WEB_SERVER_PORT);
        return;
    }
    
    server_running = 1;
    printf("\n🌐 Web UI started at http://localhost:%s\n", WEB_SERVER_PORT);
    printf("📱 Open your browser and visit: http://localhost:%s\n\n", WEB_SERVER_PORT);
}

/* Stop web server */
void stopWebServer(void) {
    if (server_running) {
        mg_mgr_free(&mgr);
        server_running = 0;
        printf("\n🔴 PARK CLOSED - Web server stopped.\n");
    }
}

/* Check if server is running */
int isWebServerRunning(void) {
    return server_running;
}

/* Poll web server (call this in main loop) */
void pollWebServer(void) {
    if (server_running) {
        mg_mgr_poll(&mgr, 100);  // Poll for 100ms
    }
}
