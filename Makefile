# Makefile for Amusement Park Ride Flow Management System

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0
LDFLAGS = -lm -lws2_32

# Directories
SRC_DIR = src
INC_DIR = include
DATA_DIR = data
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/ride_manager.c \
          $(SRC_DIR)/visitor.c \
          $(SRC_DIR)/visitor_history.c \
          $(SRC_DIR)/queue_manager.c \
          $(SRC_DIR)/dual_queue.c \
          $(SRC_DIR)/priority_queue.c \
          $(SRC_DIR)/graph.c \
          $(SRC_DIR)/stack.c \
          $(SRC_DIR)/bst.c \
          $(SRC_DIR)/file_io.c \
          $(SRC_DIR)/utils.c \
          $(SRC_DIR)/web_server.c \
          $(SRC_DIR)/mongoose.c \
          $(SRC_DIR)/web_server_handlers.c

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executable name
TARGET = park_system

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(DATA_DIR)" mkdir "$(DATA_DIR)"

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo.
	@echo Build successful! Run with: $(TARGET).exe
	@echo.

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@if exist "$(TARGET).exe" del /q "$(TARGET).exe"
	@echo Clean complete!

# Run the program
run: all
	@echo.
	@echo Running Amusement Park System...
	@echo.
	@$(TARGET).exe

# Create sample data files
data: directories
	@echo Creating sample data files...
	@echo id,name,capacity,thrill_level,wait_time > $(DATA_DIR)\rides.txt
	@echo 1,Thunder Mountain,12,8,25 >> $(DATA_DIR)\rides.txt
	@echo 2,Carousel Dreams,20,2,5 >> $(DATA_DIR)\rides.txt
	@echo 3,Sky Screamer,8,10,35 >> $(DATA_DIR)\rides.txt
	@echo 4,Ferris Wheel,16,3,10 >> $(DATA_DIR)\rides.txt
	@echo 5,Roller Coaster X,10,9,30 >> $(DATA_DIR)\rides.txt
	@echo 6,Bumper Cars,18,4,8 >> $(DATA_DIR)\rides.txt
	@echo 7,Water Rapids,14,7,20 >> $(DATA_DIR)\rides.txt
	@echo 8,Haunted House,12,6,15 >> $(DATA_DIR)\rides.txt
	@echo.
	@echo id,name,thrill_preference > $(DATA_DIR)\visitors.txt
	@echo 1001,Alice Johnson,8 >> $(DATA_DIR)\visitors.txt
	@echo 1002,Bob Smith,3 >> $(DATA_DIR)\visitors.txt
	@echo 1003,Charlie Brown,10 >> $(DATA_DIR)\visitors.txt
	@echo 1004,Diana Prince,5 >> $(DATA_DIR)\visitors.txt
	@echo 1005,Eve Wilson,7 >> $(DATA_DIR)\visitors.txt
	@echo.
	@echo ride1_id,ride2_id,distance > $(DATA_DIR)\park_map.txt
	@echo 0,1,150 >> $(DATA_DIR)\park_map.txt
	@echo 0,2,100 >> $(DATA_DIR)\park_map.txt
	@echo 1,3,200 >> $(DATA_DIR)\park_map.txt
	@echo 1,4,180 >> $(DATA_DIR)\park_map.txt
	@echo 2,4,120 >> $(DATA_DIR)\park_map.txt
	@echo 2,6,160 >> $(DATA_DIR)\park_map.txt
	@echo 3,5,140 >> $(DATA_DIR)\park_map.txt
	@echo 4,7,190 >> $(DATA_DIR)\park_map.txt
	@echo 5,7,170 >> $(DATA_DIR)\park_map.txt
	@echo 6,8,130 >> $(DATA_DIR)\park_map.txt
	@echo 7,8,110 >> $(DATA_DIR)\park_map.txt
	@echo.
	@echo Sample data files created successfully!

# Rebuild everything
rebuild: clean all

# Help target
help:
	@echo.
	@echo Amusement Park Management System - Makefile Help
	@echo ================================================
	@echo.
	@echo Available targets:
	@echo   make          - Build the project
	@echo   make all      - Build the project (same as make)
	@echo   make clean    - Remove build files
	@echo   make run      - Build and run the program
	@echo   make data     - Create sample data files
	@echo   make rebuild  - Clean and rebuild everything
	@echo   make help     - Show this help message
	@echo.

.PHONY: all clean run data rebuild help directories
