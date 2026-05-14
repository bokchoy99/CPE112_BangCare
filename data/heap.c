#include <stdlib.h>
#include <stdio.h>

#include "heap.h"

/**
 * 1. Initialize Heap
 * ตั้งความจุ capavity size
 */
Heap* initHeap(int capacity) {
    Heap* h = (Heap*)malloc(sizeof(Heap));
    if (!h) return NULL;
    h->data = (Patient**)malloc(sizeof(Patient*) * capacity);
    h->size = 0;
    h->capacity = capacity;
    return h;
}

/**
 * 2. Priority Logic (หัวใจสำคัญของ Triage)
 * คืนค่า 1 ถ้า a มีลำดับความสำคัญสูงกว่า b
 */
int comparePriority(Patient* a, Patient* b) {
    // กฎข้อที่ 1: Severity สูงกว่าต้องมาก่อน
    if (a->severity != b->severity) {
        return a->severity > b->severity;
    }
    // กฎข้อที่ 2: ถ้า Severity เท่ากัน ให้ดูที่ความเจ็บปวด (Pain Score)
    if (a->pain != b->pain) {
        return a->pain > b->pain;
    }
    // กฎข้อที่ 3: ถ้าเท่ากันหมด ใครมาก่อน (ArrivalTime น้อยกว่า) ได้รักษาก่อน
    return a->arrivalTime < b->arrivalTime;
}

/**
 * 3. Heap Insert (Up-Heap / Bubble Up)
 */
void heapInsert(Heap* h, Patient* p) {
    if (h->size >= h->capacity) return;

    int i = h->size++;
    h->data[i] = p;

    // ลอยตัวขึ้นไปหา Root ตามลำดับความสำคัญ
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (comparePriority(h->data[i], h->data[parent])) {
            Patient* temp = h->data[i];
            h->data[i] = h->data[parent];
            h->data[parent] = temp;
            i = parent;
        } else {
            break;
        }
    }
}

/**
 * 4. Heap Pop (Down-Heap / Bubble Down)
 * 
 * 
 */
Patient* heapPop(Heap* h) {
    if (h->size == 0) return NULL;

    Patient* root = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int highest = i;

        if (left < h->size && comparePriority(h->data[left], h->data[highest])) {
            highest = left;
        }
        if (right < h->size && comparePriority(h->data[right], h->data[highest])) {
            highest = right;
        }

        if (highest != i) {
            Patient* temp = h->data[i];
            h->data[i] = h->data[highest];
            h->data[highest] = temp;
            i = highest;
        } else {
            break;
        }
    }
    return root;
}

void destroyHeap(Heap* h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

// LOG notification Corfirmation
void logHeapInsert(Patient* p) {
    if (p != NULL) {
        printf("[SUCCESS]  %s added to triage queue.\n", p->id);
    }
}

void logHeapRemove(Patient* p) {
    if (p != NULL) {
        printf("[SUCCESS] %s removed from triage queue.\n", p->id);
    }
}
