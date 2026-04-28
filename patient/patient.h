#ifndef PATIENT_H
#define PATIENT_H

#include <stdbool.h>

typedef enum {
    WAITING,
    IN_QUEUE,
    ALLOCATED,
    TREATING,
    DONE
} PatientState; [cite: 84]

typedef struct Patient {
    char id[10];           // Format: BC-XXX
    char name[100];
    int severity;          // 1-5
    int pain;              // 1-10
    int arrivalTime;       // in ticks
    int treatmentStartTime;
    PatientState state;
    
    // For List usage
    struct Patient *next;
    struct Patient *prev;
} Patient; [cite: 84]

// Memory Management
Patient* createPatient(const char* id, const char* name, int sev, int pain, int time);
void destroyPatient(Patient** p);

#endif