#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bed_manager.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../data/hash_table.h"
#include "../utils/logger.h"

/* -- Internal helpers ----------------------------------------- */

static void assignBed(BedNode* bed, Patient* p, int treatTime, PatientState state) {
    bed->isOccupied        = true;
    bed->patient           = p;
    p->state               = state;
    p->treatmentRemaining  = treatTime;
    gSystem.beds->occupiedBeds++;
}

static int treatTimeForSeverity(int severity) {
    switch (severity) {
        case 5: return TREAT_TIME_S5;
        case 4: return TREAT_TIME_S4;
        case 3: return TREAT_TIME_S3;
        case 2: return TREAT_TIME_S2;
        default: return TREAT_TIME_S1;
    }
}

/* -- Public API ----------------------------------------------- */

/*
 * initBeds
 * Builds the doubly-linked bed list at system start.
 * IDs 1–MAX_ER_BEDS → "ER", remaining → "OPD".
 */
void initBeds(void) {
    gSystem.beds = (BedList*)malloc(sizeof(BedList));
    gSystem.beds->head         = NULL;
    gSystem.beds->tail         = NULL;
    gSystem.beds->totalBeds    = MAX_ER_BEDS + MAX_OPD_BEDS;
    gSystem.beds->occupiedBeds = 0;

    for (int i = 1; i <= MAX_ER_BEDS + MAX_OPD_BEDS; i++) {
        BedNode* node   = (BedNode*)malloc(sizeof(BedNode));
        node->idBed      = i;
        node->isOccupied = false;
        node->patient    = NULL;
        node->next       = NULL;
        node->prev       = gSystem.beds->tail;

        strcpy(node->type, i <= MAX_ER_BEDS ? "ER" : "OPD");

        if (gSystem.beds->tail != NULL) gSystem.beds->tail->next = node;
        else gSystem.beds->head = node;
        gSystem.beds->tail = node;
    }

    logEvent(LOG_SYSTEM, "BED_MGR", "Beds initialized (5 ER, 25 OPD)");
}

BedNode* findFreeBed(const char* type) {
    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        if (strcmp(curr->type, type) == 0 && !curr->isOccupied) return curr;
        curr = curr->next;
    }
    return NULL;
}

/*
 * allocateBed
 * Assigns the best available bed based on severity.
 *
 * S5: tries ER first; falls back to OPD if ER is full
 *     (state = ALLOCATED_OPD so aging keeps monitoring).
 * S1–S4: OPD only.
 *
 * Returns true on success, false if no suitable bed exists.
 */
bool allocateBed(Patient* p) {
    if (!gSystem.beds || !p) return false;

    if (p->severity == 5) {
        BedNode* erBed = findFreeBed("ER");
        if (erBed) {
            assignBed(erBed, p, TREAT_TIME_S5, ALLOCATED);
            logEvent(LOG_INFO, "BED_MGR", "S5 patient assigned to ER bed");
            return true;
        }

        /* ER full — fall back to OPD with a staff alert. */
        BedNode* opdBed = findFreeBed("OPD");
        if (opdBed) {
            assignBed(opdBed, p, TREAT_TIME_S5, ALLOCATED_OPD);
            printf("[ALERT] %s (S5) moved to OPD Bed #%d — ER beds full.\n",
                   p->id, opdBed->idBed);
            logEvent(LOG_INFO, "BED_MGR", "S5 patient assigned to OPD bed (ER full)");
            return true;
        }
    } else {
        BedNode* opdBed = findFreeBed("OPD");
        if (opdBed) {
            assignBed(opdBed, p, treatTimeForSeverity(p->severity), ALLOCATED);
            logEvent(LOG_INFO, "BED_MGR", "Patient assigned to OPD bed");
            return true;
        }
    }

    return false;
}

/*
 * freeBed
 * Marks the bed as vacant after treatment is complete.
 * Returns false if the bed ID is not found or already vacant.
 */
bool freeBed(int idBed) {
    if (!gSystem.beds) return false;

    BedNode* bed = getBed(idBed);
    if (!bed || !bed->isOccupied) return false;

    bed->isOccupied = false;
    bed->patient    = NULL;
    gSystem.beds->occupiedBeds--;

    char msg[64];
    sprintf(msg, "Bed #%d is now free", idBed);
    logEvent(LOG_INFO, "BED_MGR", msg);
    printf("[SUCCESS] Bed #%d is now EMPTY.\n", idBed);
    return true;
}

/*
 * fillAllBeds
 * Populates every vacant bed with a dummy patient for demo/testing.
 *
 * ER  dummies: TREAT_TIME_S5 (15 ticks) — long enough that ER stays full
 *              during aging test cases.
 * OPD dummies: TREAT_TIME_S5 (15 ticks) — intentionally long so dummies
 *              do NOT discharge before aging has a chance to run.
 *              Previously used S1–S4 times (5–10 ticks) which caused dummies
 *              to free OPD beds at tick 5, giving waiting patients a bed
 *              before aging ever fired.
 */
void fillAllBeds(void) {
    if (!gSystem.beds) return;

    BedNode* curr         = gSystem.beds->head;
    static int dummyCount = 0;

    while (curr != NULL) {
        if (!curr->isOccupied) {
            Patient* dummy = createPatient(NULL, "Dummy_Patient", 3, 5, gSystem.tickCount);
            if (dummy) {
                sprintf(dummy->id, "DM-%03d", ++dummyCount);
                dummy->state              = TREATING;
                dummy->treatmentRemaining = TREAT_TIME_S5; /* 15 ticks for all */
                curr->isOccupied          = true;
                curr->patient             = dummy;
                gSystem.beds->occupiedBeds++;
                hashTableInsert((HashTable*)gSystem.patientTable, dummy);
            }
        }
        curr = curr->next;
    }

    logEvent(LOG_SYSTEM, "BED_MGR", "All beds filled with dummy patients.");
    printf("[SUCCESS] All beds are now FILLED with dummy patients.\n");
}

/* Returns the BedNode with the given idBed, or NULL if not found. */
BedNode* getBed(int idBed) {
    if (!gSystem.beds) return NULL;
    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        if (curr->idBed == idBed) return curr;
        curr = curr->next;
    }
    return NULL;
}