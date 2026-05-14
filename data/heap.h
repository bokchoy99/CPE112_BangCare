#ifndef HEAP_H
#define HEAP_H

#include "../patient/patient.h"

typedef struct {
    Patient** data;   // Array ของ Pointer ผู้ป่วย
    int size;         // จำนวนคนไข้ใน Queue ปัจจุบัน
    int capacity;     // ความจุสูงสุด
} Heap;

//Heap initialization
Heap* initHeap(int capacity);
void heapInsert(Heap* h, Patient* p);
Patient* heapPop(Heap* h);

//Priority Logic 
int comparePriority(Patient* a, Patient* b);

//After got allocated
void destroyHeap(Heap* h);
void heapRemove(Patient* p);

#endif