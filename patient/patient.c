#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patient.h"

Patient* createPatient(const char* id, const char* name, int sev, int pain, int simTime) {
    Patient* p = (Patient*)malloc(sizeof(Patient));
    if (!p) return NULL;
    if (id) strncpy(p->id, id, sizeof(p->id));
    strncpy(p->name, name, sizeof(p->name));
    p->severity = sev;
    p->pain = pain;
    p->arrivalTick = simTime;
    p->state = WAITING;
    p->next = NULL;
    p->prev = NULL;

    
    // // Convert simulation time to hh:mm
    // int totalSeconds = simTime; // adjust based on how you pass it
    // int hours = (totalSeconds / 3600) % 24;
    // int minutes = (totalSeconds % 3600) / 60;
    // snprintf(p->arrivalTime, sizeof(p->arrivalTime), "%02d:%02d", hours, minutes);
    return p;
}