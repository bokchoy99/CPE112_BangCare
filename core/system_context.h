#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "../patient/patient.h"

typedef struct {
    // Time System 
    int tickCount;           // จำนวน tick ที่สะสม
    long baseTime;           // เวลาเริ่มต้นระบบ (Unix Timestamp)
    long simulatedTime;      // baseTime + (tickCount * TICK_UNIT)

    // Data Structures 
    void* patientTable;      // Hash Table (Owner) 
    void* triageQueue;       // Priority Queue (Binary Heap)
    void* agingList;         // Linked List for Scanning
    void* bedList;           // Doubly Linked List for Beds

    // Counters
    int patientCounter;      // สำหรับรัน BC-XXX
} SystemContext;

extern SystemContext gSystem;

#endif