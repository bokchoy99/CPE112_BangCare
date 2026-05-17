#ifndef BED_MANAGER_H
#define BED_MANAGER_H
#include <stdbool.h>
#include "../patient/patient.h"

/* -- Structures ----------------------------------------------- */

typedef struct BedNode {
    int            idBed;       /* 1–5 = ER, 6–30 = OPD.            */
    char           type[5];     /* "ER" or "OPD".                   */
    bool           isOccupied;
    Patient*       patient;     /* NULL when bed is free.            */
    struct BedNode* prev;
    struct BedNode* next;
} BedNode;

typedef struct {
    BedNode* head;
    BedNode* tail;
    int      totalBeds;
    int      occupiedBeds;
} BedList;

/* -- Prototypes ----------------------------------------------- */
void     initBeds(void);
void     displayBedAllocation(void);
bool     allocateBed(Patient* p);
bool     freeBed(int idBed);
void     fillAllBeds(void);
BedNode* getBed(int idBed);
BedNode* findFreeBed(const char* type);

#endif