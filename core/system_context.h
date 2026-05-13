#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "../patient/patient.h"
#include "../bed/bed_manger.h" // Include เพื่อให้รู้จัก BedList

typedef struct {
    int tickCount;           
    long baseTime;           
    long simulatedTime;      
    void* patientTable;      
    void* triageQueue;       
    void* agingList;         
    BedList* beds;           // เปลี่ยนจาก void* bedList เป็น BedList* beds
    int patientCounter;      
} SystemContext;

extern SystemContext gSystem; 

#endif