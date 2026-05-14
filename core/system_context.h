#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "config.h"
#include "../bed/bed_manager.h"
#include "../patient/patient.h" //idk why its red this line bruh

typedef struct {
    int tickCount;           
    long baseTime;           
    long simulatedTime;      // systemTime + tick = simulatedTime
    void* patientTable;      // Hash Table 
    void* triageQueue;       // Binary Heap
    void* agingList;         // Linked List
    BedList* beds;        // Bed Node = Doubly Linked List 
    BedNode erBeds[MAX_ER_BEDS]; //Bed ER
    BedNode opdBeds[MAX_OPD_BEDS]; //Bed OPD
    int patientCounter;      // สำหรับรัน ID BC-XXX
} SystemContext;

extern SystemContext gSystem; 

// ประกาศ Getter เผื่อไว้ใช้ในบางจุด
SystemContext* getSystemContext();

#endif