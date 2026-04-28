#ifndef CONFIG_H
#define CONFIG_H

#define TICK_UNIT_MINUTES 5        // 1 tick = 5 นาที [cite: 96]
#define RECOVERY_TICK_MINUTES 20   // recovery = 20 นาที [cite: 97]
#define AGING_THRESHOLD_TICKS 5    // เกณฑ์การปรับ severity [cite: 142]

#define MAX_ER_BEDS 5              // [cite: 157]
#define MAX_OPD_BEDS 25            // [cite: 158]
#define HASH_TABLE_SIZE 101        
#define ID_PREFIX "BC-"            // [cite: 125]

#endif