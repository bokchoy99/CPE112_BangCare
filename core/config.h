#ifndef CONFIG_H
#define CONFIG_H

#define TICK_UNIT_MINUTES 2        // 1 tick = 2 mins
#define RECOVERY_TICK_MINUTES 10   // recovery = 10 mins 
#define AGING_THRESHOLD_TICKS 5    // threshold to change severity

#define MAX_ER_BEDS 3            
#define MAX_OPD_BEDS 10            
#define HASH_TABLE_SIZE 101        
#define ID_PREFIX "BC-"            

#endif