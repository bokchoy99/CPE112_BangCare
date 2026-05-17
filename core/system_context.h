#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <time.h>
#include "config.h"
#include "../bed/bed_manager.h"
#include "../patient/patient.h"

/* -- Global simulation state ---------------------------------- */
typedef struct {
    int      tickCount;        /* Simulation ticks elapsed since start.           */
    long     baseTime;         /* Wall-clock UNIX timestamp at system init.       */
    long     simulatedTime;    /* baseTime + tickCount * TICK_UNIT_MINUTES * 60.  */
    void*    patientTable;     /* HashTable* — owns all Patient records.          */
    void*    triageQueue;      /* Heap*      — priority queue for triage.         */
    void*    agingList;        /* Patient*   — head of the aging monitor list.    */
    BedList* beds;             /* Doubly-linked list of all beds.                 */
    int      patientCounter;   /* Auto-increment counter for "BC-XXX" IDs.       */

    /* -- Statistics counters ---------------------------------- */
    int      totalRegistered;  /* Total patients registered since start.          */
    int      totalDischarged;  /* Patients who completed treatment.               */
    int      totalAgingBumps;  /* Total severity promotions by aging system.      */
    int      totalS5Immediate; /* S5 patients who got ER bed immediately.         */
    long     totalWaitTicks;   /* Sum of wait ticks for all allocated patients.   */
    int      maxWaitTicks;     /* Longest wait time recorded (ticks).             */
} SystemContext;

extern SystemContext gSystem;

SystemContext* getSystemContext(void);

#endif