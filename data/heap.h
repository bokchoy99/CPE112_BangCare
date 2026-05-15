#ifndef HEAP_H
#define HEAP_H
#include "../patient/patient.h"

typedef struct {
    Patient** data;
    int size;
    int capacity;
} Heap;

Heap* initHeap(int capacity);
void heapInsert(Heap* h, Patient* p);
Patient* heapPop(Heap* h);
Patient* heapPeek(Heap* h);
int comparePriority(Patient* a, Patient* b);
void destroyHeap(Heap* h);
void heapRemove(Heap* h, Patient* p); // เก็บแค่อันนี้

#endif