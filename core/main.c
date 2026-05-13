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

void displayUpcomingStream() { // Upcoming stream last 3 -> ยังไม่ sort เป็น stream ก่อนเข้า heap
    Patient** headPtr = (Patient**)gSystem.agingList;
    
    if (headPtr == NULL || *headPtr == NULL) {
        printf("Empty");
        return;
    }

    Patient* curr = *headPtr;
    Patient* stream[3] = {NULL, NULL, NULL};
    int count = 0;

    // เก็บ 3 คนล่าสุด (004 -> 003 -> 002)
    while (curr != NULL && count < 3) {
        stream[count] = curr;
        curr = curr->next;
        count++;
    }

    // แสดงผลจากเก่าไปใหม่ (002 <- 003 <- 004)
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", stream[i]->id); 
        if (i > 0) printf(" <- ");
    }
}

void displayDashboard() {
    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");

    printf(" Beds: [%02d/30] | Tick: %d | Simulated Time: %ld\n", 
            0, gSystem.tickCount, gSystem.simulatedTime); // เลข 0 คือตัวอย่างจำนวนเตียงที่ใช้เพื่อtest

    //BED
#include "../system/systemm_controller.h" //p

void displayDashboard() {
    time_t t = gSystem.simulatedTime;
    struct tm *tm_info = localtime(&t);

    char buffer[50];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", tm_info);
    // อ้างอิงรูปแบบจากเอกสาร [cite: 286-307]
    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");
    printf(" Beds: [%02d/30] | Tick: %d (%d min) | Time: %s\n", 
        0,
        gSystem.tickCount,
        gSystem.tickCount * TICK_UNIT_MINUTES,
        buffer); // เลข 0 คือตัวอย่างจำนวนเตียงที่ใช้
    printf(" [ BED ALLOCATION ]\n");
    printf(" ER Beds (S5)   : [ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ]\n"); //emptyใส่เป็นค่าตัวอย่างไว้
    printf(" OPD Beds (S1-4): [ EMPTY ][ EMPTY ][ EMPTY ]... (Total 25)\n");
    printf("-----------------------------------------------------------\n");

    // Upcoming stream last 3 -> ยังไม่ sort เป็น stream ก่อนเข้า heap
    printf(" Upcoming Stream: ");
    displayUpcomingStream();
    printf("\n");

    printf("-----------------------------------------------------------\n");
    printf(" Command > ");
}

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