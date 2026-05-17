#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system_controller.h"
#include "../core/config.h"
#include "../core/system_context.h"
#include "../patient/patient.h"
#include "../data/hash_table.h"
#include "../data/heap.h"
#include "../bed/bed_manager.h"
#include "../utils/logger.h"
#include "../utils/history_writer.h"
#include "../ui/command.h"
#include "../utils/id_generator.h"

/* -- Global instance ------------------------------------------ */
SystemContext gSystem;

SystemContext* getSystemContext(void) { return &gSystem; }

/* -- Forward declarations (module-private) -------------------- */
static void runAging(void);
static void insertToAgingList(Patient* p);
static void removeFromAgingList(Patient* p);
static void updateBedTreatments(void);


/* ============================================================
 * SECTION 1 — Time Management
 * ============================================================ */

static void updateSimulatedTime(void) {
    gSystem.simulatedTime = gSystem.baseTime +
                            (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

/*
 * systemTick
 * Advances the simulation by n ticks.
 *
 * Order per tick matters:
 *   1. runAging()           — promote severity before beds are evaluated.
 *   2. updateBedTreatments() — decrement timers, discharge, auto-allocate.
 *
 * Aging runs every other tick (tickCount % 2 == 0) to keep output readable.
 * If aging ran after updateBedTreatments, a patient could receive a bed
 * before their severity was promoted, causing the [AGING] log to be skipped.
 */
void systemTick(int n) {
    for (int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();
        runAging();
        updateBedTreatments();
    }
}

/* ============================================================
 * SECTION 2 — Patient Registration
 * ============================================================ */

/*
 * systemAddPatient
 * Creates a patient record, assigns a "BC-XXX" ID, inserts it into
 * the hash table and aging list, then attempts immediate bed allocation.
 * If no bed is available the patient enters the triage priority queue.
 */
void systemAddPatient(const char* name, int severity, int pain) {
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);
    if (!p) {
        printf("[ERROR] System Out of Memory: Failed to allocate Patient Node.\n");
        return;
    }

    generatePatientID(p->id, gSystem.patientCounter++);
    p->state        = IN_QUEUE;
    p->arrivalTick  = gSystem.tickCount;
    p->agingApplied = 0;

    hashTableInsert((HashTable*)gSystem.patientTable, p);
    insertToAgingList(p);
    gSystem.totalRegistered++;

    bool allocated = allocateBed(p);
    if (allocated) {
        /* Track S5 patients who got an ER bed immediately (not fallback OPD). */
        if (p->severity == 5 && p->state == ALLOCATED) {
            gSystem.totalS5Immediate++;
        }
        p->treatmentStartTime = gSystem.tickCount;
    } else {
        heapInsert((Heap*)gSystem.triageQueue, p);
    }

    printf("[SUCCESS] Registered: %s (ID: %s, S%d, Pain: %d)\n",
           p->name, p->id, p->severity, p->pain);
    logEvent(LOG_INFO, "SYSTEM", "Patient registered");
}

/* ============================================================
 * SECTION 3 — Aging System
 * ============================================================ */

/*
 * runAging
 * Scans the aging list and promotes any patient whose wait time
 * has crossed a new AGING_THRESHOLD_TICKS interval.
 *
 * States monitored: WAITING, IN_QUEUE, ALLOCATED_OPD.
 * Patients in ALLOCATED, TREATING, or DONE are skipped.
 *
 * On promotion the patient is removed from and re-inserted into the
 * priority queue so their new severity is reflected immediately.
 * Patients already at severity 5 emit a [CRITICAL] alert instead.
 */
static void runAging(void) {
    Patient* curr = (Patient*)gSystem.agingList;

    while (curr != NULL) {
        Patient* next = curr->next;

        bool shouldMonitor = (curr->state == WAITING   ||
                              curr->state == IN_QUEUE  ||
                              curr->state == ALLOCATED_OPD);

        if (!shouldMonitor) { curr = next; continue; }

        int waitTicks    = gSystem.tickCount - curr->arrivalTick;
        int promoteLevel = waitTicks / AGING_THRESHOLD_TICKS;

        if (promoteLevel <= curr->agingApplied) { curr = next; continue; }

        curr->agingApplied = promoteLevel;

        if (curr->severity < 5) {
            curr->severity++;
            gSystem.totalAgingBumps++;
            if (curr->state == IN_QUEUE) {
                heapRemove((Heap*)gSystem.triageQueue, curr);
                heapInsert((Heap*)gSystem.triageQueue, curr);
            }
            char msg[100];
            sprintf(msg, "%s promoted to S%d (waited %d ticks)", curr->id, curr->severity, waitTicks);
            logEvent(LOG_WARNING, "AGING", msg);
            printf("[AGING] %s promoted to S%d\n", curr->id, curr->severity);
        } else {
            char msg[100];
            sprintf(msg, "%s S5 waiting too long (%d ticks) - ER unavailable", curr->id, waitTicks);
            logEvent(LOG_WARNING, "AGING", msg);
            printf("[CRITICAL] %s has been waiting too long - ER bed unavailable!\n",
                   curr->id);
        }

        curr = next;
    }
}

/* ============================================================
 * SECTION 4 — Aging List Helpers (Doubly-Linked)
 * ============================================================ */

/* Prepends p to the aging list. O(1). */
static void insertToAgingList(Patient* p) {
    p->prev = NULL;
    p->next = (Patient*)gSystem.agingList;
    if (gSystem.agingList != NULL) {
        ((Patient*)gSystem.agingList)->prev = p;
    }
    gSystem.agingList = p;
}

/* Removes p from the aging list without freeing it. O(1). */
static void removeFromAgingList(Patient* p) {
    if (p->prev != NULL) p->prev->next = p->next;
    else                  gSystem.agingList = p->next;

    if (p->next != NULL) p->next->prev = p->prev;

    p->next = NULL;
    p->prev = NULL;
}

/* ============================================================
 * SECTION 5 — Bed Treatment Timer
 * ============================================================ */

/*
 * updateBedTreatments
 * Decrements each occupied bed's treatmentRemaining counter.
 * When it reaches zero the patient is discharged: their state
 * is set to DONE, they are removed from the aging list, the bed
 * is freed, and systemAutoAllocate() is called to fill the vacancy.
 */
static void updateBedTreatments(void) {
    if (!gSystem.beds) return;

    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        BedNode* next = curr->next; /* Save next before potential bed change. */

        if (curr->isOccupied && curr->patient != NULL) {
            curr->patient->treatmentRemaining--;

            if (curr->patient->treatmentRemaining <= 0) {
                Patient* p = curr->patient;
                p->state   = DONE;
                removeFromAgingList(p);

                /* Record wait time = ticks from arrival to treatment start. */
                int waitTicks = p->treatmentStartTime - p->arrivalTick;
                if (waitTicks < 0) waitTicks = 0;
                gSystem.totalDischarged++;
                gSystem.totalWaitTicks += waitTicks;
                if (waitTicks > gSystem.maxWaitTicks) gSystem.maxWaitTicks = waitTicks;

                /* Save to CSV before freeing the bed (patient pointer still valid). */
                writePatientHistory(p, curr->type, waitTicks);

                char msg[100];
                sprintf(msg, "Patient %s discharged from bed #%d (wait: %d ticks)",
                        p->id, curr->idBed, waitTicks);
                logEvent(LOG_INFO, "SYSTEM", msg);

                freeBed(curr->idBed);
                systemAutoAllocate();
            }
        }
        curr = next;
    }
}

/* ============================================================
 * SECTION 6 — Auto Allocation
 * ============================================================ */

/*
 * upgradeOpdToEr
 * After a bed is freed, scans the aging list for S5 patients currently in
 * ALLOCATED_OPD state. If an ER bed is now available, moves the patient:
 *   1. Free their current OPD bed.
 *   2. Assign them the ER bed.
 *   3. Update state to ALLOCATED and log the upgrade.
 * Called after every freeBed() — before systemAutoAllocate() — so S5 patients
 * get priority over queued patients.
 */
static void upgradeOpdToEr(void) {
    BedNode* erBed = findFreeBed("ER");
    if (!erBed) return; /* No ER bed available. */

    Patient* curr = (Patient*)gSystem.agingList;
    while (curr != NULL) {
        if (curr->state == ALLOCATED_OPD && curr->severity == 5) {
            /* Find which OPD bed this patient currently occupies. */
            BedNode* bed = gSystem.beds->head;
            while (bed != NULL) {
                if (bed->patient == curr) {
                    int oldBedId = bed->idBed;

                    /* Detach from OPD bed without triggering discharge logic. */
                    bed->isOccupied = false;
                    bed->patient    = NULL;
                    gSystem.beds->occupiedBeds--;

                    /* Assign ER bed. */
                    erBed->isOccupied = true;
                    erBed->patient    = curr;
                    curr->state       = ALLOCATED;
                    gSystem.beds->occupiedBeds++;

                    char msg[100];
                    sprintf(msg, "%s upgraded from OPD bed #%d to ER bed #%d",
                            curr->id, oldBedId, erBed->idBed);
                    logEvent(LOG_INFO, "SYSTEM", msg);
                    printf("[UPGRADE] %s (S5) moved from OPD Bed #%d to ER Bed #%d\n",
                           curr->id, oldBedId, erBed->idBed);
                    return; /* One upgrade per call — re-check will handle the rest. */
                }
                bed = bed->next;
            }
        }
        curr = curr->next;
    }
}

/*
 * systemAutoAllocate
 * 1. First upgrades any S5 ALLOCATED_OPD patients to ER if ER is now free.
 * 2. Then drains the triage heap into remaining free beds.
 */
void systemAutoAllocate(void) {
    /* Upgrade pass — S5 OPD patients get ER priority before queued patients. */
    upgradeOpdToEr();

    /* Heap drain pass. */
    while (1) {
        Patient* p = heapPeek((Heap*)gSystem.triageQueue);
        if (!p) break;

        heapPop((Heap*)gSystem.triageQueue);

        if (!allocateBed(p)) {
            heapInsert((Heap*)gSystem.triageQueue, p);
            break;
        }

        p->treatmentStartTime = gSystem.tickCount;
        if (p->severity == 5 && p->state == ALLOCATED) {
            gSystem.totalS5Immediate++;
        }

        char msg[100];
        sprintf(msg, "Auto-allocated bed for %s (S%d)", p->id, p->severity);
        logEvent(LOG_INFO, "SYSTEM", msg);
        printf("[AUTO-ALLOC] %s (S%d) assigned to bed\n", p->id, p->severity);
    }
}

/* ============================================================
 * SECTION 7 — Peek Helpers
 * ============================================================ */

/* Looks up a patient by ID in the hash table and displays their record. */
void systemPeekHash(const char* id) {
    Patient* p = hashTableLookup((HashTable*)gSystem.patientTable, id);
    if (p != NULL) displayHashID(p);
    else printf("[ERROR] Patient ID: %s is NOT FOUND in Active Registry.\n", id);
}

/* Displays all patients near their next aging threshold. */
void systemPeekAging(void) {
    displayAgingAnalysis((Patient*)gSystem.agingList);
}