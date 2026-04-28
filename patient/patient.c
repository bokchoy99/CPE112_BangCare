#include <stdlib.h>
#include <string.h>
#include "patient.h"

Patient* createPatient(const char* id, const char* name, int sev, int pain, int time) {
    Patient* p = (Patient*)malloc(sizeof(Patient));
    if (!p) return NULL;

    if (id) strncpy(p->id, id, 9);
    strncpy(p->name, name, 99);
    p->severity = sev;
    p->pain = pain;
    p->arrivalTime = time;
    p->state = WAITING;
    p->next = NULL;
    p->prev = NULL;
    
    return p;
}