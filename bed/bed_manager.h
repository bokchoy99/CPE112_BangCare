#ifndef BED_MANAGER_H
#define BED_MANAGER_H
#include <stdbool.h>

#include "../patient/patient.h"

typedef struct BedNode {
    int idBed;              //id
    char type[5];           
    bool isOccupied;        //ว่างไหม
    Patient* patient;  //เก็บคนไข้ ดึงมากจาก patient.h
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
bool allocateBed(Patient* p); 
bool freeBed(int idBed);  
void fillAllBeds();      

#endif
