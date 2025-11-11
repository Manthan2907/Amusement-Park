# Next Steps - Enhanced Features Implementation

## ✅ What Has Been Completed

### 1. Ticket System Foundation
- ✅ Added `TicketType` enum (TICKET_NORMAL, TICKET_PREMIUM) to `visitor.h`
- ✅ Updated `Visitor` structure with:
  - `ticket_type` field
  - `fast_passes_remaining` field (Premium gets 3)
  - `entry_time` timestamp
- ✅ Implemented `createVisitorWithTicket()` in `visitor.c`
- ✅ Added `getTicketTypeName()` helper function
- ✅ Updated `displayVisitorInfo()` to show ticket type and fast-passes

### 2. Dual Queue System
- ✅ Created `DualQueue` structure in `queue_manager.h`
  - Separate queues for regular and fast-pass visitors
  - Configurable merge ratio (default 4:1)
- ✅ Implemented dual queue operations in `dual_queue.c`:
  - `createDualQueue()` - Initialize dual queue system
  - `enqueueDual()` - Smart enqueue based on ticket type
  - `dequeueDual()` - Merge queues with 4:1 ratio
  - `displayDualQueueStatus()` - Show both queue sizes
  - `freeDualQueue()` - Cleanup
  - `getTotalQueueSize()` - Combined size
- ✅ Updated Makefile to include `dual_queue.c`

### 3. Project Documentation
- ✅ Created `IMPLEMENTATION_STATUS.md` - Tracks all 12 prompts
- ✅ Created this `NEXT_STEPS.md` file

## 🚧 What Needs To Be Done Next

### PRIORITY 1: Update Main Menu (main.c)

You need to manually update `main.c` to:

1. **Add Dual Queue Support**
   ```c
   // Replace Queue* ride_queues[MAX_RIDES];
   // With:
   DualQueue* ride_queues[MAX_RIDES];
   ```

2. **Update addVisitorToPark() Function**
   Add ticket type selection:
   ```c
   void addVisitorToPark() {
       displayHeader("ADD VISITOR TO PARK");
       
       char name[MAX_NAME_LENGTH];
       getStringInput("Enter visitor name", name, MAX_NAME_LENGTH);
       
       int thrill_pref = getIntInput("Enter thrill preference", 1, 10);
       
       // NEW: Ask for ticket type
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
       
       // Rest of the function...
       if (visitor_groups[group_count] == NULL) {
           visitor_groups[group_count] = createVisitorGroup(generateGroupID());
       }
       
       addVisitorToGroup(visitor_groups[group_count], visitor);
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
   ```

3. **Update showQueueStatus() to use DualQueue**
   ```c
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
               dq = createDualQueue(ride->id, 4);  // 4:1 ratio
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
   ```

4. **Update initializeSystem() for DualQueue**
   ```c
   // In initializeSystem(), change:
   for (int i = 0; i < MAX_RIDES; i++) {
       ride_queues[i] = NULL;  // Will be created as DualQueue when needed
   }
   ```

5. **Update shutdownSystem() for DualQueue**
   ```c
   for (int i = 0; i < MAX_RIDES; i++) {
       if (ride_queues[i]) freeDualQueue(ride_queues[i]);
   }
   ```

### PRIORITY 2: Add Ride Management Menu

Add new menu options and functions to `main.c`:

1. **Update displayMenu()**
   ```c
   void displayMenu() {
       clearScreen();
       displayHeader("AMUSEMENT PARK MANAGEMENT SYSTEM");
       
       printf("\n=== VISITOR OPERATIONS ===\n");
       printf("1. Add Visitor/Group to Park\n");
       printf("2. Suggest Next Best Ride for Visitor\n");
       printf("3. Display Visitor Ride History\n");
       printf("4. Undo Last Ride Choice\n");
       
       printf("\n=== RIDE OPERATIONS ===\n");
       printf("5. Add New Ride to Park\n");
       printf("6. Remove Ride from Park\n");
       printf("7. Edit Ride Details\n");
       printf("8. Toggle Ride Status (Open/Closed)\n");
       
       printf("\n=== QUEUE & STATISTICS ===\n");
       printf("9. Show Queue Status for All Rides\n");
       printf("10. Show Rides by Wait Time (BST)\n");
       printf("11. Display Park Statistics\n");
       
       printf("\n=== PATHFINDING ===\n");
       printf("12. Find Shortest Path Between Rides\n");
       
       printf("\n=== SIMULATION ===\n");
       printf("13. Simulate Visitor Completing a Ride\n");
       
       printf("\n0. Exit and Save Data\n");
       
       displaySeparator();
   }
   ```

2. **Add Ride Management Functions**
   ```c
   void addNewRide() {
       displayHeader("ADD NEW RIDE");
       
       char name[MAX_NAME_LENGTH];
       getStringInput("Enter ride name", name, MAX_NAME_LENGTH);
       
       int capacity = getIntInput("Enter capacity (people per cycle)", 4, 20);
       int thrill = getIntInput("Enter thrill level", 1, 10);
       int wait = getIntInput("Enter initial wait time (minutes)", 0, 60);
       
       // Find next available ride ID
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
           insertRide(wait_time_bst, ride);
           printSuccess("Ride added successfully!");
           printf("Ride ID: %d\n", new_id);
       } else {
           printError("Failed to create ride");
       }
   }
   
   void removeRide() {
       displayHeader("REMOVE RIDE");
       
       displayAllRides(park_rides);
       int ride_id = getIntInput("Enter ride ID to remove", 0, 100);
       
       Ride* ride = findRideById(park_rides, ride_id);
       if (!ride) {
           printError("Ride not found!");
           return;
       }
       
       printf("Are you sure you want to remove '%s'? (1=Yes, 0=No): ", ride->name);
       int confirm = getIntInput("", 0, 1);
       
       if (confirm) {
           deleteRide(wait_time_bst, ride->current_wait_time);
           removeRideFromList(park_rides, ride_id);
           printSuccess("Ride removed successfully!");
       }
   }
   
   void editRideDetails() {
       displayHeader("EDIT RIDE DETAILS");
       
       displayAllRides(park_rides);
       int ride_id = getIntInput("Enter ride ID to edit", 0, 100);
       
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
               break;
           case 2:
               ride->thrill_level = getIntInput("New thrill level", 1, 10);
               break;
           case 3:
               deleteRide(wait_time_bst, ride->current_wait_time);
               ride->current_wait_time = getIntInput("New wait time", 0, 120);
               insertRide(wait_time_bst, ride);
               break;
           case 0:
               return;
       }
       
       printSuccess("Ride updated successfully!");
   }
   
   void toggleRideStatus() {
       displayHeader("TOGGLE RIDE STATUS");
       
       displayAllRides(park_rides);
       int ride_id = getIntInput("Enter ride ID", 0, 100);
       
       Ride* ride = findRideById(park_rides, ride_id);
       if (!ride) {
           printError("Ride not found!");
           return;
       }
       
       if (ride->is_operational) {
           markRideClosed(ride);
           printSuccess("Ride marked as CLOSED");
       } else {
           markRideOpen(ride);
           printSuccess("Ride marked as OPEN");
       }
   }
   ```

3. **Update main() switch statement**
   ```c
   switch (choice) {
       case 1: addVisitorToPark(); break;
       case 2: suggestRideForVisitor(); break;
       case 3: displayVisitorHistory(); break;
       case 4: undoLastRide(); break;
       case 5: addNewRide(); break;
       case 6: removeRide(); break;
       case 7: editRideDetails(); break;
       case 8: toggleRideStatus(); break;
       case 9: showQueueStatus(); break;
       case 10: showRidesByWaitTime(); break;
       case 11: displayParkStatistics(); break;
       case 12: findShortestPath(); break;
       case 13: simulateRideCompletion(); break;
       case 0:
           printf("\nThank you for visiting! Saving data...\n");
           break;
       default:
           printError("Invalid choice!");
   }
   ```

### PRIORITY 3: Test the System

After making the above changes:

1. **Build**
   ```bash
   mingw32-make clean
   mingw32-make
   ```

2. **Test Ticket System**
   - Add a normal visitor
   - Add a premium visitor
   - Check that premium visitor gets 3 fast-passes

3. **Test Dual Queue**
   - Add visitors to a ride
   - Verify premium visitors go to fast-pass queue
   - Check queue status shows both queues

4. **Test Ride Management**
   - Add a new ride
   - Edit ride details
   - Remove a ride
   - Toggle ride status

## 📚 Remaining Features (For Later)

After completing the above, you can implement:
- Enhanced priority algorithm (weather, fatigue, time-of-day)
- A* pathfinding
- Real-time simulation
- AVL balancing
- Maintenance system
- Analytics and reporting

## 🎯 Summary

**You have the foundation ready. Now you need to:**
1. Update `main.c` to use `DualQueue` instead of `Queue`
2. Add ticket type selection in `addVisitorToPark()`
3. Add ride management menu options (add/remove/edit rides)
4. Update queue display to show dual queues
5. Test everything

The code is modular and ready - you just need to integrate it into the main menu!
