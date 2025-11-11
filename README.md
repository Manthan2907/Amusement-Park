# 🎢 Amusement Park Management System

A comprehensive C-based amusement park management system with web UI, featuring real-time visitor tracking, ride management, queue systems, and interactive browser interface using advanced data structures and algorithms.

## 🌐 Web UI Features
- **Real-time browser interface** at http://localhost:8000
- **Auto-refresh** every 5 seconds
- **Amusement park theme** with animations
- **Dual interface**: Terminal menu + Web UI work simultaneously
- **REST API** endpoints for all operations

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Data Structures Used](#data-structures-used)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [System Components](#system-components)
- [File Formats](#file-formats)
- [Algorithms](#algorithms)
- [Future Enhancements](#future-enhancements)

## 🎯 Overview

This system simulates an amusement park management platform that handles:
- Ride operations and queue management
- Visitor tracking and group management
- Intelligent ride recommendations
- Path optimization across the park
- Real-time wait time calculations
- Ride history and undo functionality

## ✨ Features

## 🖼️ Ride Images & Thumbnails
- Every uploaded image in `web/images/` now appears as a ride thumbnail in the web UI.
- Each ride in `data/rides.txt` is mapped to an image by name or thrill level.
- To add a new ride with a custom image:
  1. Place your image in `web/images/` (e.g. `spaceshot.jpg`).
  2. Add a new line to `data/rides.txt` with a matching or descriptive name (e.g. `18,Space Shot,12,9,0`).
  3. The UI will automatically show the image as a thumbnail for that ride.
- All ride actions (enjoy, toggle, delete) work for every ride.
- See `web/images/IMAGE_GUIDE.txt` for naming conventions and mapping logic.

---

### Core Functionality
- **Visitor Management**: Add visitors individually or in groups with thrill preferences
- **Intelligent Ride Suggestions**: Priority-based recommendations using visitor preferences
- **Queue Management**: Real-time queue tracking for all rides
- **Path Finding**: Dijkstra's algorithm for shortest paths between rides
- **Ride History**: Track visitor ride history with undo capability
- **Wait Time Optimization**: BST-based search for rides by wait time
- **Data Persistence**: Save and load park data from files
- **Statistics & Reports**: Comprehensive park analytics

### User Interface
- Interactive menu-driven system
- Color-coded output for better readability
- Real-time status updates
- Input validation and error handling

## 🗂️ Data Structures Used

### 1. **Linked List** (Ride Management)
- **File**: `ride_manager.c/h`
- **Purpose**: Maintain dynamic list of all rides in the park
- **Operations**: O(1) insertion, O(n) search

### 2. **Doubly Linked List** (Visitor Groups)
- **File**: `visitor.c/h`
- **Purpose**: Manage visitor groups with bidirectional traversal
- **Operations**: Forward/backward traversal, group splitting/merging

### 3. **Queue** (Ride Queues)
- **File**: `queue_manager.c/h`
- **Purpose**: FIFO management of visitors waiting for rides
- **Operations**: O(1) enqueue/dequeue, wait time estimation

### 4. **Priority Queue (Max-Heap)** (Ride Suggestions)
- **File**: `priority_queue.c/h`
- **Purpose**: Recommend best rides based on multiple factors
- **Priority Formula**: 
  ```
  priority = (thrill_match × 40) - (wait_time × 3) - (distance × 20)
  ```
- **Operations**: O(log n) insert/extract

### 5. **Graph (Adjacency List)** (Park Map)
- **File**: `graph.c/h`
- **Purpose**: Represent park layout with rides as vertices
- **Operations**: Dijkstra's shortest path, distance calculations

### 6. **Stack** (Ride History)
- **File**: `stack.c/h`
- **Purpose**: Track visitor ride history with undo functionality
- **Operations**: O(1) push/pop, history queries

### 7. **Binary Search Tree** (Wait Time Optimization)
- **File**: `bst.c/h`
- **Purpose**: Organize rides by wait time for efficient searching
- **Operations**: O(log n) average case insert/search/delete

## 📁 Project Structure

```
amusement-park-system/
├── Makefile                    # Build configuration
├── README.md                   # This file
├── include/                    # Header files
│   ├── bst.h                  # Binary Search Tree
│   ├── config.h               # System constants
│   ├── file_io.h              # File operations
│   ├── graph.h                # Graph/Park map
│   ├── priority_queue.h       # Priority queue (heap)
│   ├── queue_manager.h        # Queue operations
│   ├── ride_manager.h         # Ride management
│   ├── stack.h                # Stack for history
│   ├── utils.h                # Utility functions
│   └── visitor.h              # Visitor management
├── src/                       # Source files
│   ├── bst.c                  # BST implementation
│   ├── file_io.c              # File I/O operations
│   ├── graph.c                # Graph algorithms
│   ├── main.c                 # Main program
│   ├── priority_queue.c       # Priority queue impl
│   ├── queue_manager.c        # Queue operations
│   ├── ride_manager.c         # Ride operations
│   ├── stack.c                # Stack implementation
│   ├── utils.c                # Helper functions
│   └── visitor.c              # Visitor operations
└── data/                      # Data files
    ├── park_map.txt           # Ride connections
    ├── rides.txt              # Ride information
    └── visitors.txt           # Visitor data
```

## 🚀 Installation

### Prerequisites
- GCC compiler (MinGW for Windows)
- Make utility
- Windows OS (can be adapted for Linux/Mac)

### Build Steps

1. **Clone or download the project**
   ```bash
   cd d:\dsa_project
   ```

2. **Create sample data files** (optional)
   ```bash
   make data
   ```

3. **Build the project**
   ```bash
   make
   ```

4. **Run the program**
   ```bash
   make run
   ```
   Or directly:
   ```bash
   park_system.exe
   ```

### Makefile Targets

| Command | Description |
|---------|-------------|
| `make` | Build the project |
| `make clean` | Remove build files |
| `make run` | Build and run |
| `make data` | Create sample data files |
| `make rebuild` | Clean and rebuild |
| `make help` | Show help message |

## 💻 Usage

### Main Menu Options

```
1. Add Visitor/Group to Park
   - Create new visitors with custom thrill preferences
   - Automatically assigns to groups

2. Suggest Next Best Ride for Visitor
   - AI-powered recommendations based on:
     * Thrill level match
     * Current wait times
     * Distance from current location

3. Show Queue Status for All Rides
   - Real-time queue sizes
   - Estimated wait times
   - Ride operational status

4. Display Visitor Ride History
   - Complete ride history per visitor
   - Timestamps and statistics

5. Find Shortest Path Between Rides
   - Dijkstra's algorithm implementation
   - Shows path and total distance

6. Show Rides by Wait Time (BST)
   - Sorted list of all rides
   - Filter by maximum wait time

7. Simulate Visitor Completing a Ride
   - Updates visitor statistics
   - Calculates satisfaction scores
   - Updates ride counters

8. Undo Last Ride Choice
   - Stack-based undo functionality
   - Removes last ride from history

9. Display Park Statistics
   - Total visitors and rides
   - Average wait times
   - Most popular rides

0. Exit and Save Data
   - Saves all data to files
   - Graceful shutdown
```

## 🔧 System Components

### Ride Manager (`ride_manager.c/h`)
- Create and manage rides
- Track ride statistics
- Update wait times dynamically
- Mark rides operational/closed

### Visitor Manager (`visitor.c/h`)
- Create visitors with preferences
- Manage visitor groups (doubly linked list)
- Track visitor statistics
- Group operations (split/merge)

### Queue Manager (`queue_manager.c/h`)
- FIFO queue for each ride
- Enqueue/dequeue operations
- Wait time estimation
- Queue merging for maintenance

### Priority Queue (`priority_queue.c/h`)
- Max-heap implementation
- Multi-factor priority calculation
- Top-N ride recommendations
- Dynamic priority updates

### Graph (`graph.c/h`)
- Adjacency list representation
- Dijkstra's shortest path
- Distance calculations
- Path visualization

### Stack (`stack.c/h`)
- Ride history tracking
- Undo functionality
- History queries
- Visit counting

### BST (`bst.c/h`)
- Wait time indexing
- Range queries
- Inorder traversal
- Optional AVL balancing

### File I/O (`file_io.c/h`)
- CSV parsing
- Data validation
- Load/save operations
- Error handling

### Utilities (`utils.c/h`)
- ID generation
- Input validation
- Statistics calculations
- Color output
- Time formatting

## 📄 File Formats

### rides.txt
```csv
id,name,capacity,thrill_level,wait_time
1,Thunder Mountain,12,8,25
2,Carousel Dreams,20,2,5
```

**Fields:**
- `id`: Unique ride identifier (0-49)
- `name`: Ride name (string)
- `capacity`: People per cycle (4-20)
- `thrill_level`: Intensity rating (1-10)
- `wait_time`: Current wait in minutes

### visitors.txt
```csv
id,name,thrill_preference
1001,Alice Johnson,8
1002,Bob Smith,3
```

**Fields:**
- `id`: Unique visitor ID (1000+)
- `name`: Visitor name
- `thrill_preference`: Preferred thrill level (1-10)

### park_map.txt
```csv
ride1_id,ride2_id,distance
0,1,150
1,3,200
```

**Fields:**
- `ride1_id`: First ride ID
- `ride2_id`: Second ride ID
- `distance`: Distance in meters

## 🧮 Algorithms

### 1. Priority Calculation
```c
priority = (thrill_match × 40) - (wait_time × 3) - (distance × 20)

where:
  thrill_match = 10 - |visitor_preference - ride_thrill_level|
  wait_time = current queue wait time in minutes
  distance = distance from current location in hundreds of meters
```

### 2. Dijkstra's Shortest Path
- **Time Complexity**: O(V² + E) with simple implementation
- **Space Complexity**: O(V)
- Used for finding optimal routes between rides

### 3. Wait Time Estimation
```c
wait_time = (queue_size / ride_capacity) × (ride_duration + boarding_time)
```

### 4. Satisfaction Score
```c
satisfaction = (thrill_match × 10) - (wait_time / 2)
Clamped to [0, 100]
```

## 🎓 Learning Objectives

This project demonstrates:
- ✅ **Linked Lists**: Dynamic ride and visitor management
- ✅ **Doubly Linked Lists**: Bidirectional group traversal
- ✅ **Queues**: FIFO ride queue management
- ✅ **Priority Queues**: Heap-based recommendations
- ✅ **Stacks**: History tracking with undo
- ✅ **Graphs**: Shortest path algorithms
- ✅ **BST**: Efficient searching and sorting
- ✅ **File I/O**: Data persistence
- ✅ **Memory Management**: Dynamic allocation/deallocation
- ✅ **Modular Design**: Separation of concerns

## 🚧 Future Enhancements

### Potential Features
1. **Multi-threading**: Concurrent visitor processing
2. **Real-time Simulation**: Time-based event system
3. **Weather Effects**: Dynamic ride closures
4. **Fast-Pass System**: VIP queue implementation
5. **Maintenance Scheduler**: Predictive maintenance
6. **Visitor Satisfaction AI**: Machine learning predictions
7. **Dynamic Pricing**: Demand-based ticket pricing
8. **Mobile App Integration**: REST API endpoints
9. **3D Park Visualization**: Graphical interface
10. **Historical Analytics**: Trend analysis

### Optimization Opportunities
- AVL tree balancing for BST
- A* algorithm for pathfinding
- Fibonacci heap for priority queue
- Hash tables for O(1) lookups
- Graph coloring for ride scheduling

## 📊 Sample Usage Scenario

### Family Visit Example
```
1. Add family members:
   - Parent (thrill: 5)
   - Child 1 (thrill: 3)
   - Child 2 (thrill: 7)

2. Get recommendations:
   - System suggests: Ferris Wheel (thrill: 3, wait: 10 min)
   - Alternative: Bumper Cars (thrill: 4, wait: 8 min)

3. Complete rides:
   - Visit Ferris Wheel
   - System updates satisfaction scores
   - History tracked for undo

4. Find next ride:
   - Shortest path from Ferris Wheel to Carousel
   - Distance: 120 meters
```

## 🐛 Troubleshooting

### Common Issues

**Build Errors:**
- Ensure GCC is in PATH
- Check Makefile line endings (Windows: CRLF)
- Verify all header files exist

**Runtime Errors:**
- Check data files exist in `data/` folder
- Verify file format (CSV with correct columns)
- Ensure sufficient memory available

**Data Not Saving:**
- Check write permissions in `data/` folder
- Verify disk space available
- Check for file locks

## 📝 License

This project is created for educational purposes as part of a Data Structures and Algorithms course.

## 👥 Contributing

This is an educational project. Suggestions for improvements:
1. Fork the repository
2. Create feature branch
3. Implement changes
4. Test thoroughly
5. Submit pull request

## 📞 Support

For issues or questions:
- Review the code comments
- Check the header files for function documentation
- Examine sample data files for format examples

## 🎉 Acknowledgments

- Data Structures and Algorithms course materials
- C programming best practices
- Amusement park management concepts

---

**Built with ❤️ using C and Data Structures**

*Last Updated: November 2024*
