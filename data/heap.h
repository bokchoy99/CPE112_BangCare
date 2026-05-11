#ifndef HEAP_H
#define HEAP_H

#include "../patient/patient.h"

typedef struct {
    Patient** data;   // Array ของ Pointer ผู้ป่วย
    int size;         // จำนวนคนไข้ใน Queue ปัจจุบัน
    int capacity;     // ความจุสูงสุด
} Heap;


Heap* initHeap(int capacity);
void heapInsert(Heap* h, Patient* p);
Patient* heapPop(Heap* h);
int comparePriority(Patient* a, Patient* b);
void destroyHeap(Heap* h);

#endif