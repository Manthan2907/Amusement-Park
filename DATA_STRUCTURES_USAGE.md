# 🎢 Data Structures Usage in Amusement Park System

## 1. Stack
**Used for:** Visitor Ride History (Option 3)
- Implementation: `stack.h/stack.c`
- Each visitor has a stack that stores their ride history
- Push: When visitor completes a ride
- Pop: When undoing last ride (Option 4)
- LIFO principle ensures chronological tracking
- Example: If visitor rides [Coaster → Wheel → Tower], Tower is at top

## 2. Queue & Priority Queue
### Regular Queue
**Used for:** Ride Wait Lines (Option 10)
- Implementation: `queue_manager.h/queue_manager.c`
- FIFO structure for fair waiting system
- Dual queue system: Regular + FastPass
- Simulated ride completion times:
  - Roller Coaster: 5 minutes
  - Ferris Wheel: 10 minutes
  - Carousel: 7 minutes
  - etc.
- Queue updates every 30 seconds
- FastPass queue gets priority (4:1 ratio)
- Example: A roller coaster with 24 capacity processes queues every 5 minutes

### Priority Queue (Max-Heap)
**Used for:** Ride Suggestions (Option 2)
- Implementation: `priority_queue.h/priority_queue.c`
- Suggests best rides based on:
  ```
  Priority = (Thrill Match × 40) - (Wait Time × 30) - (Distance × 20)
  ```
- Higher priority rides bubble up to top
- Example: A perfect thrill match with short wait time gets suggested first

## 3. Trees
### Binary Search Tree (BST)
**Used for:** Wait Time Organization (Option 11)
- Implementation: `bst.h/bst.c`
- Organizes rides by wait time for efficient searching
- Left subtree: Shorter wait times
- Right subtree: Longer wait times
- O(log n) search efficiency for finding rides by wait time
- Example: Quick lookup of "rides with < 20 min wait"

## 4. Graph
**Used for:** Park Navigation (Option 13)
- Implementation: `graph.h/graph.c`
- Represents park layout as adjacency list
- Vertices: Rides
- Edges: Paths between rides
- Weights: Distances
- Uses Dijkstra's algorithm for shortest path
- Example: Finding shortest route between Roller Coaster and Ferris Wheel

## 5. Linked Lists
### Simple Linked List
**Used for:** Ride Management (Options 6-9)
- Implementation: `ride_manager.h/ride_manager.c`
- Stores all rides in the park
- Dynamic addition/removal of rides
- Example: Adding new ride or removing for maintenance

### Doubly Linked List
**Used for:** Visitor Groups (Option 1)
- Implementation: `visitor.h/visitor.c`
- Tracks visitor groups with bidirectional traversal
- Useful for group splitting/merging
- Example: Managing families or friend groups that split up

## Data Flow Example
When suggesting a ride (Option 2):
1. Use **Linked List** to get all available rides
2. Use **Priority Queue** to sort rides by suitability
3. Check **Queue** sizes for wait times
4. Use **Graph** to calculate distances
5. If ride chosen, push to history **Stack**
6. Update wait times in **BST**