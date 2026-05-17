#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dashboard.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../bed/bed_manager.h"

/* -- Internal helpers ----------------------------------------- */

static bool isWaitingForBed(PatientState state) {
    return state == WAITING || state == IN_QUEUE;
}

/*
 * displayUpcomingStream
 * Shows the last 3 patients who registered but have not yet been
 * assigned a bed, in reverse registration order (newest last).
 */
void displayUpcomingStream(void) {
    Patient* curr = (Patient*)gSystem.agingList;
    if (!curr) { printf("Empty"); return; }

    Patient* stream[3] = { NULL, NULL, NULL };
    int      count     = 0;

    while (curr != NULL && count < 3) {
        if (isWaitingForBed(curr->state)) {
            stream[count++] = curr;
        }
        curr = curr->next;
    }

    if (count == 0) { printf("Empty"); return; }

    /* Print oldest → newest with arrows. */
    for (int i = count - 1; i >= 0; i--) {
        printf("%s [S%d]", stream[i]->id, stream[i]->severity);
        if (i > 0) printf(" << ");
    }
}

/*
 * displayDashboard
 * Renders the full terminal dashboard:
 *   - System status line (beds, tick, simulated time).
 *   - ER and OPD bed grids.
 *   - Upcoming stream (most recent waiting patients).
 *   - Command prompt.
 */
void displayDashboard(void) {
    /* Format simulated wall-clock time for display. */
    char     timeStr[50];
    time_t   t      = (time_t)gSystem.simulatedTime;
    struct tm* tmInfo = localtime(&t);
    strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M", tmInfo);

    int  occupied     = gSystem.beds ? gSystem.beds->occupiedBeds : 0;
    int  totalBeds    = MAX_ER_BEDS + MAX_OPD_BEDS;
    long totalMinutes = (gSystem.simulatedTime - gSystem.baseTime) / 60;

    printf("\n============================================================================\n");
    printf("              BANGCARE : HOSPITAL TRIAGE MANAGEMENT SYSTEM\n");
    printf("============================================================================\n");
    printf(" Beds: [%02d/%02d] | Tick: %d (%ld min) | Time: %s\n",
           occupied, totalBeds, gSystem.tickCount, totalMinutes, timeStr);
    printf("----------------------------------------------------------------------------\n");
    printf(" [ BED ALLOCATION ]\n");

    if (gSystem.beds) {
        BedNode* curr = gSystem.beds->head;

        printf(" ER  Beds (S5)   : ");
        for (int i = 0; i < MAX_ER_BEDS && curr != NULL; i++, curr = curr->next) {
            printf("[%s]", curr->isOccupied && curr->patient
                           ? curr->patient->id : "  ---  ");
        }
        printf("\n");

        printf(" OPD Beds (S1-4) : ");
        for (int i = 0; i < MAX_OPD_BEDS && curr != NULL; i++, curr = curr->next) {
            printf("[%s]", curr->isOccupied && curr->patient
                           ? curr->patient->id : "  ---  ");
        }
        printf("\n");
    }

    printf("----------------------------------------------------------------------------\n");
    printf(" [ ACTIVITY LOG ] (last 5 events)\n");

    FILE* logFile = fopen(LOG_FILE, "r");
    if (logFile) {
        /* Read up to last 5 lines using a circular buffer. */
        char lines[5][256];
        int  lineCount = 0;
        int  idx       = 0;
        while (fgets(lines[idx], sizeof(lines[idx]), logFile) != NULL) {
            idx = (idx + 1) % 5;
            if (lineCount < 5) lineCount++;
        }
        fclose(logFile);

        /* Print in chronological order. */
        int start = (lineCount < 5) ? 0 : idx;
        for (int i = 0; i < lineCount; i++) {
            int lineIdx = (start + i) % 5;
            /* Strip trailing newline for clean display. */
            lines[lineIdx][strcspn(lines[lineIdx], "\n")] = '\0';
            printf("  %s\n", lines[lineIdx]);
        }
    } else {
        printf("  (no log entries yet)\n");
    }

    printf("----------------------------------------------------------------------------\n");
    printf(" [ INCOMING STREAM ] ");
    displayUpcomingStream();
    printf("\n============================================================================\n");
    printf(" Command > ");
}