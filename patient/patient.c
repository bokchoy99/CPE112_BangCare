#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "patient.h"
#include "../core/config.h"
#include "../core/system_context.h"

/*
 * createPatient
 * Allocates and zero-initialises a new Patient.
 *
 * id  : Pass NULL to assign the ID later (e.g. dummy patients).
 * tick: Current simulation tick stored as arrivalTick for the aging system.
 *
 * Returns the new Patient pointer, or NULL on allocation failure.
 */
Patient* createPatient(const char* id, const char* name, int sev, int pain, int tick) {
    Patient* p = (Patient*)malloc(sizeof(Patient));
    if (!p) return NULL;

    if (id) strncpy(p->id, id, sizeof(p->id));

    strncpy(p->name, name, sizeof(p->name));
    p->severity          = sev;
    p->pain              = pain;
    p->arrivalTick       = tick;
    p->treatmentStartTime = 0;
    p->treatmentRemaining = 0;
    p->agingApplied      = 0;
    p->state             = WAITING;
    p->next              = NULL;
    p->prev              = NULL;

    /* Format arrivalTime from the current simulatedTime (not raw tick math)
     * so it reflects actual in-simulation wall-clock time "HH:MM". */
    time_t     simT  = (time_t)gSystem.simulatedTime;
    struct tm* tmInfo = localtime(&simT);
    strftime(p->arrivalTime, sizeof(p->arrivalTime), "%H:%M", tmInfo);

    return p;
}

/*
 * destroyPatient
 * Frees the Patient and sets the caller's pointer to NULL to prevent
 * dangling references. The caller must remove the patient from all
 * data structures (hash table, heap, aging list) before calling this.
 */
void destroyPatient(Patient** p) {
    if (p == NULL || *p == NULL) return;
    free(*p);
    *p = NULL;
}