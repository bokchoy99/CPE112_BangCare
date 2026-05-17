#ifndef PATIENT_H
#define PATIENT_H
#include <stdbool.h>

/* -- Patient Lifecycle States --------------------------------- */
typedef enum {
    WAITING,        /* Registered; not yet in triage queue.                         */
    IN_QUEUE,       /* Sitting in the priority heap awaiting bed assignment.         */
    ALLOCATED,      /* Assigned to an appropriate bed.                              */
    ALLOCATED_OPD,  /* S5 patient placed in OPD because all ER beds are full;
                       aging monitor remains active.                                */
    TREATING,       /* Treatment in progress.                                       */
    DONE            /* Treatment complete; patient discharged.                      */
} PatientState;

/* -- Patient Record ------------------------------------------- */
typedef struct Patient {
    char         id[10];               /* Unique identifier: "BC-XXX".             */
    char         name[100];            /* Full name.                               */
    int          severity;             /* Triage level 1 (minor) – 5 (critical).  */
    int          pain;                 /* Self-reported pain 1 (low) – 10 (high). */
    char         arrivalTime[10];      /* Formatted arrival time "HH:MM".         */
    int          treatmentStartTime;   /* Tick when treatment began.               */
    int          treatmentRemaining;   /* Ticks remaining until discharge.         */
    int          arrivalTick;          /* Absolute tick at registration (aging).   */
    int          agingApplied;         /* Promotion level already applied; prevents
                                          duplicate severity bumps.               */
    PatientState state;                /* Current lifecycle state.                 */
    struct Patient *next;              /* Intrusive linked-list / heap pointer.    */
    struct Patient *prev;              /* Doubly-linked-list back pointer.         */
} Patient;

/* -- Prototypes ----------------------------------------------- */
Patient* createPatient(const char* id, const char* name, int sev, int pain, int tick);
void     destroyPatient(Patient** p);

#endif