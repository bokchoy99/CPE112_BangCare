#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "system_context.h"
#include "config.h"
#include "../system/system_controller.h"
#include "../data/hash_table.h"
#include "../data/heap.h"
#include "../bed/bed_manager.h"
#include "../utils/logger.h"
#include "../ui/dashboard.h"
#include "../ui/command.h"

/*
 * initSystem
 * Bootstraps gSystem before the main loop:
 *   1. Zero counters and capture the wall-clock start time.
 *   2. Allocate the hash table (patient lookup).
 *   3. Allocate the triage priority queue (capacity 100).
 *   4. Initialise the aging monitor list (empty).
 *   5. Build all 15 beds (5 ER + 10 OPD).
 *   6. Open the event logger.
 */
static void initSystem(void) {
    gSystem.tickCount        = 0;
    gSystem.patientCounter   = 1;
    gSystem.baseTime         = (long)time(NULL);
    gSystem.simulatedTime    = gSystem.baseTime;
    gSystem.agingList        = NULL;
    gSystem.totalRegistered  = 0;
    gSystem.totalDischarged  = 0;
    gSystem.totalAgingBumps  = 0;
    gSystem.totalS5Immediate = 0;
    gSystem.totalWaitTicks   = 0;
    gSystem.maxWaitTicks     = 0;

    gSystem.patientTable = createHashTable();
    gSystem.triageQueue  = initHeap(100);

    initBeds();
    initLogger();
}

int main(void) {
    char input[256];
    initSystem();

    bool running = true;
    while (running) {
        displayDashboard();
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        running = processCommand(input);
    }

    return 0;
}