#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "mongoose.h"  // Include this first to get struct definitions
#include "ride_manager.h"
#include "visitor.h"
#include "queue_manager.h"
#include "bst.h"
#include "stack.h"
#include "priority_queue.h"
#include "graph.h"

/* Web server configuration */
#define WEB_SERVER_PORT "8000"
#define WEB_ROOT_DIR "./web"

/* New handler function declarations */
void handleGetVisitorHistory(struct mg_connection *c, struct mg_http_message *hm);
void handleGetRideSuggestions(struct mg_connection *c, struct mg_http_message *hm);
void handleGetRidesByWaitTime(struct mg_connection *c);
void handleFindPath(struct mg_connection *c, struct mg_http_message *hm);
void handleUndoLastRide(struct mg_connection *c, struct mg_http_message *hm);

/* Function prototypes */
void startWebServer(RideList* rides, VisitorGroup** groups, int* group_count, 
                   DualQueue** queues, BST* bst, Graph* park_map);
void stopWebServer(void);
int isWebServerRunning(void);
void pollWebServer(void);

#endif /* WEB_SERVER_H */
