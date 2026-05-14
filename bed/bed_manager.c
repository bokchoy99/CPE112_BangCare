#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bed_manager.h" 
#include "../core/system_context.h"
#include "../utils/logger.h"

void initBeds() {
    gSystem.beds = (BedList*)malloc(sizeof(BedList));
    gSystem.beds->head = NULL;
    gSystem.beds->tail = NULL;
    gSystem.beds->totalBeds = 30;
    gSystem.beds->occupiedBeds = 0;

    for (int i = 1; i <= 30; i++) {
        BedNode* newNode = (BedNode*)malloc(sizeof(BedNode));
        newNode->idBed = i;
        newNode->isOccupied = false;
        newNode->patient = NULL;
        newNode->next = NULL;
        newNode->prev = gSystem.beds->tail;

        if (i <= 5) strcpy(newNode->type, "ER");
        else strcpy(newNode->type, "OPD");

        if (gSystem.beds->tail != NULL) gSystem.beds->tail->next = newNode;
        else gSystem.beds->head = newNode;
        
        gSystem.beds->tail = newNode;
    }
    logEvent(LOG_SYSTEM, "BED_MGR", "30 Beds initialized (5 ER, 25 OPD)");
}

bool allocateBed(Patient* p) {
    if (gSystem.beds == NULL || p == NULL) return false;
    BedNode* curr = gSystem.beds->head;
    int severity = p->severity;

    // --- กฎการหาเตียง ---
    if (severity == 5) {
        // หาใน ER ก่อน (เตียง 1-5)
        while (curr != NULL && strcmp(curr->type, "ER") == 0) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to ER bed");
                return true;
            }
            curr = curr->next;
        }
        // ถ้า ER เต็ม (Logic ถาม Confirm อยู่ในส่วน UI ของเพื่อน)
        // สำหรับฟังก์ชันนี้จะหาเตียง OPD ให้หาก ER เต็ม
        while (curr != NULL) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to OPD bed (ER Full)");
                return true;
            }
            curr = curr->next;
        }
    } else {
        // S1-4 หาใน OPD เท่านั้น
        while (curr != NULL) {
            if (strcmp(curr->type, "OPD") == 0 && !curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "Patient assigned to OPD bed");
                return true;
            }
            curr = curr->next;
        }
    }
    return false;
}

// เพิ่มฟังก์ชันนี้เพื่อแก้ Error 'undefined reference to freeBed'
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

void displayBedAllocation() {
    // ฟังก์ชันนี้จะถูกเรียกใช้ใน dashboard.c
    // หากต้องการใช้การแสดงผลแบบใหม่ ให้ปล่อยว่างไว้หรือย้าย logic ไปไว้ที่ dashboard.c ครับ
}