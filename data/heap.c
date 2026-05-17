#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

/* -- Internal helpers ----------------------------------------- */

static void swapPatients(Patient** a, Patient** b) {
    Patient* tmp = *a;
    *a = *b;
    *b = tmp;
}

static void bubbleUp(Heap* h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (comparePriority(h->data[i], h->data[parent])) {
            swapPatients(&h->data[i], &h->data[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

static void bubbleDown(Heap* h, int i) {
    while (1) {
        int left    = 2 * i + 1;
        int right   = 2 * i + 2;
        int highest = i;

        if (left  < h->size && comparePriority(h->data[left],  h->data[highest])) highest = left;
        if (right < h->size && comparePriority(h->data[right], h->data[highest])) highest = right;

        if (highest != i) {
            swapPatients(&h->data[i], &h->data[highest]);
            i = highest;
        } else {
            break;
        }
    }
}

/* -- Public API ----------------------------------------------- */

/*
 * comparePriority
 * Returns 1 if patient a should be treated before patient b.
 * Rule 1: Higher severity wins.
 * Rule 2: Higher pain score wins (tiebreaker).
 * Rule 3: Earlier arrivalTick wins (FIFO tiebreaker).
 */
int comparePriority(Patient* a, Patient* b) {
    if (a->severity != b->severity) return a->severity > b->severity;
    if (a->pain     != b->pain)     return a->pain     > b->pain;
    return a->arrivalTick < b->arrivalTick;
}

/* Allocates a Heap with the given capacity. Returns NULL on failure. */
Heap* initHeap(int capacity) {
    Heap* h = (Heap*)malloc(sizeof(Heap));
    if (!h) return NULL;
    h->data     = (Patient**)malloc(sizeof(Patient*) * capacity);
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

/* Inserts p and restores the heap property via bubble-up. */
void heapInsert(Heap* h, Patient* p) {
    if (!h || h->size >= h->capacity) return;
    h->data[h->size++] = p;
    bubbleUp(h, h->size - 1);
}

/* Removes and returns the highest-priority patient (root). */
Patient* heapPop(Heap* h) {
    if (!h || h->size == 0) return NULL;
    Patient* root   = h->data[0];
    h->data[0]      = h->data[--h->size];
    bubbleDown(h, 0);
    return root;
}

/* Returns the highest-priority patient without removing it. */
Patient* heapPeek(Heap* h) {
    if (!h || h->size == 0) return NULL;
    return h->data[0];
}

/*
 * heapRemove
 * Removes a specific patient from an arbitrary heap position and
 * re-establishes the heap property. Used by the aging system when
 * a patient's severity is escalated and their position must be updated.
 */
void heapRemove(Heap* h, Patient* p) {
    if (!h || h->size == 0) return;

    int idx = -1;
    for (int i = 0; i < h->size; i++) {
        if (h->data[i] == p) { idx = i; break; }
    }
    if (idx == -1) return;

    h->data[idx] = h->data[--h->size];
    bubbleDown(h, idx);
}

/* Frees the data array and the Heap struct. Does not free patients. */
void destroyHeap(Heap* h) {
    if (!h) return;
    free(h->data);
    free(h);
}