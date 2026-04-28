#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "../patient/patient.h"

typedef struct {
    int tickCount;
    long baseTime;
    long simulatedTime;
    void* patientTable;
    void* triageQueue;
    void* agingList;
    void* bedList;
    int patientCounter;
} SystemContext;

// ใช้ extern เพื่อบอกว่าตัวแปรนี้ถูกสร้างไว้ที่อื่น (ใน system_controller.c)
extern SystemContext gSystem; 

#endif