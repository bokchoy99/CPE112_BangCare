#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "config.h"
#include "../patient/patient.h"
<<<<<<< HEAD
#include "../bed/bed_manger.h" // Include เพื่อให้รู้จัก BedList
=======
#include "../bed/bed.h"
>>>>>>> 7337bb0 (Add Aging List and Bed System)

typedef struct {
    int tickCount;           
    long baseTime;           
<<<<<<< HEAD
    long simulatedTime;      
    void* patientTable;      
    void* triageQueue;       
    void* agingList;         
    BedList* beds;           // เปลี่ยนจาก void* bedList เป็น BedList* beds
    int patientCounter;      
=======
    long simulatedTime;      // [cite: 90]
    void* patientTable;      // Hash Table [cite: 130]
    void* triageQueue;       // Binary Heap [cite: 135]
    void* agingList;         // Linked List [cite: 140]
    void* bedList;           // Doubly Linked List [cite: 144]
    Bed erBeds[MAX_ER_BEDS];
    Bed opdBeds[MAX_OPD_BEDS];
    int patientCounter;      // สำหรับรัน BC-XXX [cite: 126]
>>>>>>> 7337bb0 (Add Aging List and Bed System)
} SystemContext;

extern SystemContext gSystem; 

#endif