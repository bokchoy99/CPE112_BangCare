#ifndef CONFIG_H
#define CONFIG_H

#define TICK_UNIT_MINUTES 5        // 1 tick = 5 นาที
#define RECOVERY_TICK_MINUTES 20   // recovery = 20 นาที 
#define AGING_THRESHOLD_TICKS 5    // เกณฑ์การปรับ severity

#define MAX_ER_BEDS 5            
#define MAX_OPD_BEDS 25            
#define HASH_TABLE_SIZE 101        
#define ID_PREFIX "BC-"            

#endif