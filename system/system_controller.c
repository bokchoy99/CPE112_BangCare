#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system_controller.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../data/hash_table.h"
#include "../data/heap.h"
#include "../data/linked_list.h"
#include "../bed/bed_manager.h"
#include "../utils/logger.h"
#include "../core/system_context.h" 
#include "../ui/command.h"

// =======================================================
// SECTION 0: GLOBAL FUNCTION & VARIABLES
// =======================================================
void runAging();
void insertToAgingList(Patient* p);
void removeFromAgingList(Patient* p);
void updateBedTreatments();
void systemAutoAllocate();

SystemContext gSystem; 

SystemContext* getSystemContext() {
    return &gSystem;
}

// =======================================================
// SECTION 1: Time Management
// =======================================================

// สร้างตัวแปร gSystem ไว้ที่นี่

void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

void systemTick(int n) {
    for (int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();

        // FIX #1: aging วิ่งก่อน updateBedTreatments()
        // เดิม: updateBedTreatments() → runAging()
        // แก้:  runAging()            → updateBedTreatments()
        // เหตุผล: autoAlloc อยู่ใน updateBedTreatments() ถ้าเรียกก่อน
        //         ผู้ป่วยได้เตียงก่อนที่ aging จะ promote severity
        //         ทำให้ state กลายเป็น ALLOCATED → aging skip → ไม่เห็น [AGING] log
        if (gSystem.tickCount % 2 == 0) {
            runAging();
        }
        updateBedTreatments();
    }
}

// =======================================================
// SECTION 2: Patient Registration
// =======================================================
void systemAddPatient(const char* name, int severity, int pain) {
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);
    if (p) {
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++);
        p->state = IN_QUEUE;
        p->arrivalTick = gSystem.tickCount;
        p->agingApplied = 0;

        hashTableInsert((HashTable*)gSystem.patientTable, p);
        insertToAgingList(p);

        bool allocated = allocateBed(p);
        if (allocated) {
            // FIX #2 (ส่วนที่ 1): ถ้า S5 ถูก fallback ไป OPD
            // allocateBed() จะ set state = ALLOCATED_OPD แล้ว
            // ห้าม override เป็น ALLOCATED ปกติ มิเช่นนั้น aging จะ skip
            if (p->state != ALLOCATED_OPD) {
                p->state = ALLOCATED;
            }
        } else {
            heapInsert((Heap*)gSystem.triageQueue, p);
        }

        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
    }
}

// =======================================================
// SECTION 3: Aging System Logic
// =======================================================
void runAging() {
    Patient* current = (Patient*)gSystem.agingList;
    while (current != NULL) {
        Patient* next = current->next;

        if (current->state != WAITING &&
            current->state != IN_QUEUE &&
            current->state != ALLOCATED_OPD) {
            current = next;
            continue;
        }

        int waitTicks    = gSystem.tickCount - current->arrivalTick;
        int promoteLevel = waitTicks / AGING_THRESHOLD_TICKS;

        if (promoteLevel > current->agingApplied && current->severity < 5) {
            current->severity++;
            if (current->state == IN_QUEUE) {
                heapRemove((Heap*)gSystem.triageQueue, current); // เอาออกก่อน
                heapInsert((Heap*)gSystem.triageQueue, current); // ใส่กลับใหม่
            }
            current->agingApplied = promoteLevel;
            printf("[AGING] %s -> S%d\n", current->id, current->severity);

        } else if (promoteLevel > current->agingApplied && current->severity == 5) {
            current->agingApplied = promoteLevel;
            printf("[CRITICAL] %s has been waiting too long! ER bed unavailable!\n", current->id);
        }

        current = next;
    }
}

// =======================================================
// SECTION 4: Aging List Helpers (Linked List)
// =======================================================
void insertToAgingList(Patient* p) {
    p->next = (Patient*)gSystem.agingList;
    p->prev = NULL;
    if (gSystem.agingList != NULL) {
        ((Patient*)gSystem.agingList)->prev = p;
    }
    gSystem.agingList = p;
}

void removeFromAgingList(Patient* p) {
    if (p->prev != NULL) {
        p->prev->next = p->next;
    } else {
        gSystem.agingList = p->next;
    }
    if (p->next != NULL) {
        p->next->prev = p->prev;
    }
    p->next = NULL;
    p->prev = NULL;
}

// =======================================================
// SECTION 5: Bed Treatment Timer
// ลดเวลารักษาทุก tick และ free เตียงเมื่อรักษาเสร็จ
// =======================================================
void updateBedTreatments() {
    if (gSystem.beds == NULL) return;
    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        if (curr->isOccupied && curr->patient != NULL) {
            curr->patient->treatmentRemaining--;

            if (curr->patient->treatmentRemaining <= 0) {
                Patient* p = curr->patient;
                p->state = DONE;

                removeFromAgingList(p);

                char log_msg[100];
                sprintf(log_msg, "Patient %s treatment complete. Freeing bed #%d", p->id, curr->idBed);
                logEvent(LOG_INFO, "SYSTEM", log_msg);

                freeBed(curr->idBed);
                systemAutoAllocate();
            }
        }
        curr = curr->next;
    }
}

// =======================================================
// SECTION 6: Auto Allocate
// =======================================================
void systemAutoAllocate() {
    while (1) {
        Patient* p = heapPeek((Heap*)gSystem.triageQueue);
        if (p == NULL) break;

        heapPop((Heap*)gSystem.triageQueue);

        bool allocated = allocateBed(p);
        if (!allocated) {
            heapInsert((Heap*)gSystem.triageQueue, p);
            break;
        }

        // FIX #2 (ส่วนที่ 3): ถ้า S5 ได้ OPD จาก autoAlloc
        // allocateBed() set state = ALLOCATED_OPD แล้ว — ห้าม override
        if (p->state != ALLOCATED_OPD) {
            p->state = ALLOCATED;
        }

        char msg[100];
        sprintf(msg, "Auto-allocated bed for %s (S%d)", p->id, p->severity);
        logEvent(LOG_INFO, "SYSTEM", msg);
        printf("[AUTO-ALLOC] %s (S%d) assigned to bed\n", p->id, p->severity);
    }
}