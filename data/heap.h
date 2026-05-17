#ifndef HEAP_H
#define HEAP_H
#include "../patient/patient.h"

/* -- Structure ------------------------------------------------ */

/* Array-backed binary max-heap used as the triage priority queue.
 * Priority order: severity DESC → pain DESC → arrivalTick ASC.  */
typedef struct {
    Patient** data;
    int       size;
    int       capacity;
} Heap;

/* -- Prototypes ----------------------------------------------- */
Heap*    initHeap(int capacity);
void     heapInsert(Heap* h, Patient* p);
Patient* heapPop(Heap* h);
Patient* heapPeek(Heap* h);
int      comparePriority(Patient* a, Patient* b);
void     heapRemove(Heap* h, Patient* p);
void     destroyHeap(Heap* h);

#endif