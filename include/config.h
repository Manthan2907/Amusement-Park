#ifndef CONFIG_H
#define CONFIG_H

/* System Configuration Constants */
#define MAX_RIDES 50
#define MAX_VISITORS 1000
#define MAX_QUEUE_SIZE 500
#define MAX_NAME_LENGTH 100
#define MAX_PATH_LENGTH 256
#define MAX_RIDE_CAPACITY 20
#define MIN_RIDE_CAPACITY 4

/* Thrill Level Constants */
#define MIN_THRILL_LEVEL 1
#define MAX_THRILL_LEVEL 10

/* Priority Queue Weights */
#define THRILL_MATCH_WEIGHT 40
#define WAIT_TIME_WEIGHT 30
#define DISTANCE_WEIGHT 20

/* File Paths */
#define RIDES_FILE "data/rides.txt"
#define VISITORS_FILE "data/visitors.txt"
#define PARK_MAP_FILE "data/park_map.txt"

/* Simulation Constants */
#define BASE_RIDE_DURATION 5  // minutes
#define BOARDING_TIME 2       // minutes per group

/* ANSI Color Codes for Terminal Output */
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_BOLD "\033[1m"

#endif /* CONFIG_H */
