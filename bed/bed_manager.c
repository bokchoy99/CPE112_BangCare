#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bed_manager.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../data/hash_table.h"
#include "../utils/logger.h"

// =======================================================
// สร้างเตียงทั้งหมดตอนเริ่มระบบ
// ER 5 เตียง (idBed 1-5), OPD 10 เตียง (idBed 6-15)
// =======================================================
void initBeds() {
    gSystem.beds = (BedList*)malloc(sizeof(BedList));
    gSystem.beds->head = NULL;
    gSystem.beds->tail = NULL;
    gSystem.beds->totalBeds = MAX_ER_BEDS + MAX_OPD_BEDS;
    gSystem.beds->occupiedBeds = 0;

    for (int i = 1; i <= MAX_ER_BEDS + MAX_OPD_BEDS; i++) {
        BedNode* newNode = (BedNode*)malloc(sizeof(BedNode));
        newNode->idBed = i;
        newNode->isOccupied = false;
        newNode->patient = NULL;
        newNode->next = NULL;
        newNode->prev = gSystem.beds->tail;

        if (i <= MAX_ER_BEDS) strcpy(newNode->type, "ER");
        else strcpy(newNode->type, "OPD");

        if (gSystem.beds->tail != NULL) gSystem.beds->tail->next = newNode;
        else gSystem.beds->head = newNode;
        gSystem.beds->tail = newNode;
    }

    logEvent(LOG_SYSTEM, "BED_MGR", "15 Beds initialized (5 ER, 10 OPD)");
}

// =======================================================
// จัดเตียงให้คนไข้
// S5 → หา ER ก่อน ถ้าเต็มค่อยไป OPD พร้อมแจ้งเตือน
// S1-S4 → หา OPD เท่านั้น
// =======================================================
bool allocateBed(Patient* p) {
    if (gSystem.beds == NULL || p == NULL) return false;

    BedNode* curr = gSystem.beds->head;
    int severity = p->severity;

    if (severity == 5) {
        // วนหาเตียง ER ที่ว่าง
        while (curr != NULL) {
            if (strcmp(curr->type, "ER") == 0 && !curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                p->state = ALLOCATED;
                p->treatmentRemaining = TREAT_TIME_S5;
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to ER bed");
                return true;
            }
            curr = curr->next;
        }

        // ER เต็ม → reset แล้ววนหา OPD แทน
        curr = gSystem.beds->head;
        while (curr != NULL) {
            if (strcmp(curr->type, "OPD") == 0 && !curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;

                // FIX #2: ใช้ ALLOCATED_OPD แทน ALLOCATED
                // เพื่อให้ runAging() ยังคง monitor S5 ที่อยู่ใน OPD
                // และออก [CRITICAL] เมื่อรอนานเกิน AGING_THRESHOLD_TICKS
                p->state = ALLOCATED_OPD;

                p->treatmentRemaining = TREAT_TIME_S5;
                gSystem.beds->occupiedBeds++;
                printf("[ALERT] Patient %s (S5) moved to OPD Bed #%d — ER beds full.\n",
                       p->id, curr->idBed);
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to OPD bed (ER Full)");
                return true;
            }
            curr = curr->next;
        }

    } else {
        // S1-S4 หาเตียง OPD
        while (curr != NULL) {
            if (strcmp(curr->type, "OPD") == 0 && !curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                p->state = ALLOCATED;

                switch (p->severity) {
                    case 4: p->treatmentRemaining = TREAT_TIME_S4; break;
                    case 3: p->treatmentRemaining = TREAT_TIME_S3; break;
                    case 2: p->treatmentRemaining = TREAT_TIME_S2; break;
                    default: p->treatmentRemaining = TREAT_TIME_S1; break;
                }

                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "Patient assigned to OPD bed");
                return true;
            }
            curr = curr->next;
        }
    }

    return false;
}

// =======================================================
// คืนเตียงเมื่อคนไข้รักษาเสร็จ
// =======================================================
bool freeBed(int idBed) {
    if (gSystem.beds == NULL) return false;

    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        if (curr->idBed == idBed) {
            if (curr->isOccupied) {
                curr->isOccupied = false;
                curr->patient = NULL;
                gSystem.beds->occupiedBeds--;

                char msg[64];
                sprintf(msg, "Bed #%d is now free", idBed);
                logEvent(LOG_INFO, "BED_MGR", msg);
                printf("[SUCCESS] Bed #%d is now EMPTY.\n", idBed);
                return true;
            }
            return false;
        }
        curr = curr->next;
    }
    return false;
}

// =======================================================
// เติมเตียงที่ว่างด้วยคนไข้ Dummy สำหรับ Demo
// =======================================================
void fillAllBeds() {
    if (gSystem.beds == NULL) return;

    BedNode* curr = gSystem.beds->head;
    static int dummy_count = 0;
    int offset = 0;

    while (curr != NULL) {
        if (!curr->isOccupied) {
            Patient* dummy = createPatient(NULL, "Dummy_Patient", 3, 5, gSystem.tickCount);
            if (dummy) {
                sprintf(dummy->id, "DM-%03d", ++dummy_count);
                dummy->state = ALLOCATED;

                if (strcmp(curr->type, "ER") == 0) {
                    // FIX #3: ทุก DM ใน ER อยู่ครบ TREAT_TIME_S5 (15 tick)
                    // เดิม: TREAT_TIME_S5 - (offset % 5) → DM บาง slot discharge
                    //        เร็วมากหลัง fillbeds ทำให้ S5 ที่ add ตามมาได้ ER
                    //        แทน OPD → Case 3 (ER full → fallback OPD) ทดสอบไม่ได้
                    dummy->treatmentRemaining = TREAT_TIME_S5;
                } else {
                    // OPD — สลับ severity S1-S4 ให้หลากหลาย (คงเดิม)
                    switch (offset % 4) {
                        case 0: dummy->treatmentRemaining = TREAT_TIME_S4; break;
                        case 1: dummy->treatmentRemaining = TREAT_TIME_S3; break;
                        case 2: dummy->treatmentRemaining = TREAT_TIME_S2; break;
                        case 3: dummy->treatmentRemaining = TREAT_TIME_S1; break;
                    }
                }

                offset++;
                curr->isOccupied = true;
                curr->patient = dummy;
                gSystem.beds->occupiedBeds++;
                hashTableInsert((HashTable*)gSystem.patientTable, dummy);
            }
        }
        curr = curr->next;
    }

    logEvent(LOG_SYSTEM, "BED_MGR", "All 15 beds have been forcefully filled with Dummy patients.");
    printf("[SUCCESS] All remaining beds (%d ER / %d OPD) are now FILLED with Dummy patients.\n",
           MAX_ER_BEDS, MAX_OPD_BEDS);
}