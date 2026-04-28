#ifndef CONFIG_H
#define CONFIG_H

// Time Logic
#define TICK_UNIT_MINUTES 5        // 1 tick = 5 mins
#define RECOVERY_TICK_MINUTES 20   // recovery = 20 mins
#define AGING_THRESHOLD_TICKS 5    // Threshold for severity update

// Bed Capacity
#define MAX_ER_BEDS 5
#define MAX_OPD_BEDS 25

// Structure Limits
#define HASH_TABLE_SIZE 101
#define ID_PREFIX "BC-"

#endif