#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "system_context.h"
#include "config.h"
#include "../system/system_controller.h"
#include "../data/hash_table.h" 
#include "../data/heap.h"
#include "../data/linked_list.h"
#include "../ui/dashboard.h"
#include "../ui/command.h"
#include "../utils/logger.h"

void initSystem() {
    gSystem.tickCount = 0;
    gSystem.baseTime = (long)time(NULL);
    gSystem.patientCounter = 1;
    gSystem.simulatedTime = gSystem.baseTime;
    gSystem.patientTable = createHashTable();

    gSystem.triageQueue = initHeap(100); //Priority
    gSystem.agingList = createLinkedList(); // Linked list
    
    initBeds();
    initLogger();
}

int main() {
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