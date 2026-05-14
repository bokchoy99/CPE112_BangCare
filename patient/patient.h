#ifndef PATIENT_H
#define PATIENT_H
#include <stdbool.h>

typedef enum {
    WAITING,
    IN_QUEUE,
    ALLOCATED,
    ALLOCATED_OPD,   // S5 ที่ fallback ไป OPD (ER full) — aging ยังคง monitor อยู่
    TREATING,
    DONE
} PatientState;

typedef struct Patient {
    char id[10];               // Format: BC-XXX
    char name[100];
    int severity;          // 1-5
    int pain;              // 1-10
    int arrivalTime;       // in ticks
    char arrivalTime[6];
    int treatmentStartTime;
    int treatmentRemaining;
    int arrivalTick;           // ใช้กับ aging system
    int agingApplied;          // กันเพิ่ม severity ซ้ำ
    PatientState state;
    struct Patient *next;
    struct Patient *prev;
} Patient;

Patient* createPatient(const char* id, const char* name, int sev, int pain, int time);
void destroyPatient(Patient** p);

#endif