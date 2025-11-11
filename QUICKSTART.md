# 🚀 Quick Start Guide

Get up and running with the Amusement Park Management System in 5 minutes!

## ⚡ Fast Track (3 Steps)

### Step 1: Build
```bash
cd d:\dsa_project
make
```

### Step 2: Run
```bash
park_system.exe
```

### Step 3: Try It Out
- Choose option `1` to add a visitor
- Choose option `2` to get ride suggestions
- Explore other features!

---
## 📋 Prerequisites Check

Before building, ensure you have:

- ✅ **GCC Compiler** (MinGW for Windows)
  ```bash
  gcc --version
  ```
  
- ✅ **Make Utility**
  ```bash
  make --version
  ```

If missing, install MinGW from: https://www.mingw-w64.org/

---

## 🎯 First-Time Setup

### 1. Verify Project Structure
```bash
cd d:\dsa_project
dir
```

You should see:
- `include/` folder (10 header files)
- `src/` folder (10 source files)
- `data/` folder (3 data files)
- `Makefile`
- `README.md`

### 2. Build the Project
```bash
make
```

Expected output:
```
gcc -Wall -Wextra -Iinclude -std=c99 -c src/main.c -o build/main.o
gcc -Wall -Wextra -Iinclude -std=c99 -c src/ride_manager.c -o build/ride_manager.o
...
Build successful! Run with: park_system.exe
```

### 3. Run the Program
```bash
park_system.exe
```

or

```bash
make run
```

---

## 🎮 Interactive Tutorial

### Tutorial 1: Add Your First Visitor

1. Start the program: `park_system.exe`
2. Select option `1` (Add Visitor/Group to Park)
3. Enter name: `John Doe`
4. Enter thrill preference: `7` (1-10 scale)
5. Note the Visitor ID displayed

**What happened?**
- Created a visitor with ID 1000+
- Assigned to a group
- Created a ride history stack

### Tutorial 2: Get Ride Recommendations

1. Select option `2` (Suggest Next Best Ride)
2. Enter the Visitor ID from Tutorial 1
3. See top 3 recommended rides!

**How it works:**
- Calculates priority based on thrill match, wait time, and distance
- Uses max-heap priority queue
- Shows best matches first

### Tutorial 3: Complete a Ride

1. Select option `7` (Simulate Ride Completion)
2. Enter your Visitor ID
3. Choose a ride ID (1-10)
4. See satisfaction score!

**What's tracked:**
- Ride added to history stack
- Visitor statistics updated
- Ride visitor count incremented

### Tutorial 4: View History

1. Select option `4` (Display Visitor Ride History)
2. Enter your Visitor ID
3. See all completed rides!

### Tutorial 5: Undo Last Ride

1. Select option `8` (Undo Last Ride)
2. Enter your Visitor ID
3. Last ride removed from history!

**Stack in action:**
- Pop operation removes top element
- History preserved

---

## 🎪 Sample Scenarios

### Scenario A: Thrill Seeker
```
1. Add visitor "Alex" with thrill preference 10
2. Get recommendations → Sky Screamer, Roller Coaster X
3. Complete Sky Screamer ride
4. Check satisfaction score (should be high!)
```

### Scenario B: Family Visit
```
1. Add "Parent" (thrill: 5)
2. Add "Child1" (thrill: 3)
3. Add "Child2" (thrill: 4)
4. Get recommendations for each
5. Find common rides (Ferris Wheel, Bumper Cars)
```

### Scenario C: Path Planning
```
1. Select option 5 (Find Shortest Path)
2. Start ride: 1 (Thunder Mountain)
3. End ride: 5 (Roller Coaster X)
4. See shortest path and distance!
```

---

## 🔧 Common Commands

### Build Commands
```bash
make              # Build project
make clean        # Remove build files
make rebuild      # Clean and rebuild
make run          # Build and run
make data         # Create sample data
make help         # Show help
```

### Running
```bash
park_system.exe   # Direct execution
make run          # Build and run
```

---

## 📊 Understanding the Data

### Rides (data/rides.txt)
```csv
id,name,capacity,thrill_level,wait_time
1,Thunder Mountain,12,8,25
```

- **ID**: Unique identifier (1-10)
- **Name**: Ride name
- **Capacity**: People per cycle (4-20)
- **Thrill Level**: Intensity 1-10
- **Wait Time**: Current wait in minutes

### Visitors (data/visitors.txt)
```csv
id,name,thrill_preference
1001,Alice Johnson,8
```

- **ID**: Unique identifier (1000+)
- **Name**: Visitor name
- **Thrill Preference**: Preferred intensity 1-10

### Park Map (data/park_map.txt)
```csv
ride1_id,ride2_id,distance
0,1,150
```

- **Ride IDs**: Connected rides
- **Distance**: Walking distance in meters

---

## 🐛 Troubleshooting

### Problem: "gcc: command not found"
**Solution:** Install MinGW and add to PATH
```bash
# Add to PATH (Windows):
set PATH=%PATH%;C:\MinGW\bin
```

### Problem: "make: command not found"
**Solution:** Install Make utility or use mingw32-make
```bash
mingw32-make
```

### Problem: "Cannot open rides.txt"
**Solution:** Create sample data
```bash
make data
```

### Problem: Build errors
**Solution:** Clean and rebuild
```bash
make clean
make
```

### Problem: Program crashes
**Solution:** Check data file formats
- Ensure CSV format is correct
- No extra commas or spaces
- All required fields present

---

## 📚 Next Steps

### Learn More
1. Read `README.md` for complete documentation
2. Check `PROJECT_SUMMARY.txt` for overview
3. Follow `PROMPT_GUIDE.md` for enhancements

### Explore Features
- Try all 9 menu options
- Add multiple visitors
- Test with different thrill preferences
- Experiment with pathfinding

### Extend the System
- Follow prompts in `PROMPT_GUIDE.md`
- Add new features
- Optimize algorithms
- Create GUI (optional)

---

## 🎯 Quick Reference

### Menu Options
```
1 - Add Visitor          6 - Rides by Wait Time
2 - Get Suggestions      7 - Complete Ride
3 - Queue Status         8 - Undo Last Ride
4 - Visitor History      9 - Park Statistics
5 - Shortest Path        0 - Exit
```

### Data Structures
```
Linked List      → Ride management
Doubly LL        → Visitor groups
Queue            → Ride waiting lines
Priority Queue   → Recommendations
Graph            → Park map
Stack            → Ride history
BST              → Wait time search
```

### Key Files
```
main.c           → Main program
ride_manager.c   → Ride operations
visitor.c        → Visitor management
queue_manager.c  → Queue operations
priority_queue.c → Recommendations
graph.c          → Pathfinding
stack.c          → History tracking
bst.c            → Wait time search
```

---

## 💡 Pro Tips

1. **Start Simple**: Add 2-3 visitors first
2. **Test Features**: Try each menu option
3. **Check Data**: View data files to understand format
4. **Read Code**: Header files have great documentation
5. **Experiment**: Modify data files and see results
6. **Debug**: Use printf statements to trace execution
7. **Extend**: Follow PROMPT_GUIDE.md for enhancements

---

## 🎉 You're Ready!

The system is fully functional and ready to use. Start exploring and have fun managing your virtual amusement park!

### Quick Test Sequence
```
1. make
2. park_system.exe
3. Option 1 → Add visitor "Test" with thrill 7
4. Option 2 → Get suggestions for visitor 1000
5. Option 7 → Complete ride 1 for visitor 1000
6. Option 4 → View history for visitor 1000
7. Option 9 → See park statistics
8. Option 0 → Exit
```

**Congratulations! You've completed the quick start! 🎊**

---

*For detailed information, see README.md*
*For enhancements, see PROMPT_GUIDE.md*
*For overview, see PROJECT_SUMMARY.txt*
