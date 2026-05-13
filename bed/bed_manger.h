#ifndef BED_SYSTEM_H
#define BED_SYSTEM_H
#include <stdbool.h>
#include "../patient/patient.h" //

typedef struct BedNode {
    int idBed;              
    char type[5];           
    bool isOccupied;        
    Patient* patient;        // เปลี่ยนจาก void* เป็น Patient*
    struct BedNode* prev;   
    struct BedNode* next;   
} BedNode;

typedef struct {
    BedNode* head;
    BedNode* tail;
    int totalBeds;
    int occupiedBeds;
} BedList;

void initBeds();
void displayBedAllocation();
bool allocateBed(Patient* p); // ปรับให้รับ Patient* แทน severity (เพราะ severity อยู่ในตัว p อยู่แล้ว)
bool freeBed(int idBed);        

#endif