#include <stdio.h>
#include <time.h>
#include "logger.h"
#include "../core/system_context.h"
#include "../core/config.h"

static const char* LEVEL_LABELS[] = { "INFO", "WARN", "ERR ", "SYS " };

/* Clears system_log.txt and writes the session header.
 * Also creates patient_history.csv with column headers if it does not exist. */
void initLogger(void) {
    FILE* f = fopen(LOG_FILE, "w");
    if (f) {
        fprintf(f, "=== BANGCARE SYSTEM LOG STARTED ===\n");
        fclose(f);
        printf("[SYSTEM] Log file created: %s\n", LOG_FILE);
    } else {
        printf("[WARN] Could not create log file: %s\n", LOG_FILE);
    }

    FILE* csv = fopen(HISTORY_FILE, "r");
    if (!csv) {
        csv = fopen(HISTORY_FILE, "w");
        if (csv) {
            fprintf(csv, "ID,Name,Severity,Pain,ArrivalTime,TreatmentStartTick,"
                         "ArrivalTick,WaitTicks,WaitMinutes,BedType\n");
            fclose(csv);
        }
    } else {
        fclose(csv);
    }
}

/*
 * logEvent
 * Appends a timestamped entry to system_log.txt.
 * Timestamp is derived from simulatedTime so log entries reflect
 * in-simulation time, not wall-clock time.
 *
 * Format: [HH:MM:SS] [LEVEL] [MODULE  ] message
 */

void logEvent(LogLevel level, const char* module, const char* message) {
    FILE* f = fopen(LOG_FILE, "a");
    if (!f) return;

    time_t     t    = (time_t)gSystem.simulatedTime;
    struct tm* info = localtime(&t);
    char       timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", info);

    fprintf(f, "[%s] [%s] [%-8s] %s\n",
            timeStr, LEVEL_LABELS[level], module, message);
    fclose(f);
}