#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "../patient/patient.h"

typedef struct {
    int tickCount;           
    long baseTime;           
    long simulatedTime;      // [cite: 90]
    void* patientTable;      // Hash Table [cite: 130]
    void* triageQueue;       // Binary Heap [cite: 135]
    void* agingList;         // Linked List [cite: 140]
    void* bedList;           // Doubly Linked List [cite: 144]
    int patientCounter;      // สำหรับรัน BC-XXX [cite: 126]
} SystemContext;

extern SystemContext gSystem; // Global Context [cite: 274]

#endif