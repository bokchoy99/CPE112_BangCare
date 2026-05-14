#ifndef PATIENT_H
#define PATIENT_H

#include <stdbool.h>

typedef enum {
    WAITING,
    IN_QUEUE,
    ALLOCATED,
    TREATING,
    DONE
} PatientState;

typedef struct Patient {
    char id[10];           // Format: BC-XXX
    char name[100];
    int severity;          // 1-5
    int pain;              // 1-10
    int arrivalTime;       // in ticks
    int treatmentStartTime;
    int treatmentRemaining;
//p
    int arrivalTick; // use with aging (ใช้เฉพาะ aging system)
    int agingApplied; // กันมันเพิ่มซ้ำ

    PatientState state;
    
    // สำหรับโครงสร้างข้อมูล Linked List / Aging List
    struct Patient *next;
    struct Patient *prev;
} Patient;

// --- Function Prototypes (เพิ่มส่วนนี้เข้าไปครับ) ---
// ฟังก์ชันจัดการ Memory
Patient* createPatient(const char* id, const char* name, int sev, int pain, int time);
void destroyPatient(Patient** p);

#endif