#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bed_manager.h" 
#include "../core/system_context.h"
#include "../utils/logger.h"
#include "../data/hash_table.h"

void initBeds() {
    gSystem.beds = (BedList*)malloc(sizeof(BedList));
    gSystem.beds->head = NULL;
    gSystem.beds->tail = NULL;
    gSystem.beds->totalBeds = 13;
    gSystem.beds->occupiedBeds = 0;

    for (int i = 1; i <= 13; i++) {
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

    if (severity == 5) {
        while (curr != NULL && strcmp(curr->type, "ER") == 0) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                p->state = ALLOCATED; // อัปเดตสถานะคนไข้
                p->treatmentRemaining = 6; // ⭐ คนไข้ ER รักษานาน 6 Ticks (30 นาที)
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to ER bed");
                return true;
            }
            curr = curr->next;
        }
        while (curr != NULL) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                p->state = ALLOCATED; // อัปเดตสถานะคนไข้
                p->treatmentRemaining = 2; // ⭐ เคสนี้มานอนเตียง OPD ก็ใช้เวลา OPD (2 Ticks)
                gSystem.beds->occupiedBeds++;
                logEvent(LOG_INFO, "BED_MGR", "S5 Patient assigned to OPD bed (ER Full)");
                return true;
            }
            curr = curr->next;
        }
    } else {
        while (curr != NULL) {
            if (strcmp(curr->type, "OPD") == 0 && !curr->isOccupied) {
                curr->isOccupied = true;
                curr->patient = p;
                p->state = ALLOCATED; // อัปเดตสถานะคนไข้
                p->treatmentRemaining = 2; // ⭐ คนไข้ OPD ปกติรักษานาน 2 Ticks (10 นาที)
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

void fillAllBeds() {
    if (gSystem.beds == NULL) return;
    BedNode* curr = gSystem.beds->head;
    int dummy_count = 0;

    // วนลูปผ่านเตียงทั้งหมดในระบบ (ซึ่งตอนนี้เปลี่ยนโครงสร้างเป็นมีทั้งหมด 13 เตียงแล้ว)
    while (curr != NULL) {
        // ถ้าเตียงยังว่าง ให้สร้างคนไข้ Dummy เข้าไปใส่
        if (!curr->isOccupied) {
            // สร้างคนไข้ Dummy โดยใช้เวลา Tick ปัจจุบันของระบบ
            Patient* dummy = createPatient(NULL, "Dummy_Patient", 3, 5, gSystem.tickCount);
            if (dummy) {
                // ตั้งค่า ID และรายละเอียดเบื้องต้น
                sprintf(dummy->id, "DM-%03d", ++dummy_count);
                dummy->state = ALLOCATED;
                
                // เช็คประเภทเตียงจากโครงสร้างปัจจุบันอัตโนมัติ: เตียง ER (3 เตียงแรก) นาน 6 Ticks, เตียง OPD (10 เตียงหลัง) นาน 2 Ticks
                dummy->treatmentRemaining = (strcmp(curr->type, "ER") == 0) ? 6 : 2;

                // ผูกคนไข้เข้ากับเตียง
                curr->isOccupied = true;
                curr->patient = dummy;
                gSystem.beds->occupiedBeds++;

                // บันทึกข้อมูลลงทะเบียนใน Hash Table หลักด้วยเพื่อให้ระบบฟังก์ชัน Peek ค้นหาเจอ
                hashTableInsert((HashTable*)gSystem.patientTable, dummy);
            }
        }
        curr = curr->next;
    }
    
    logEvent(LOG_SYSTEM, "BED_MGR", "All 13 beds have been forcefully filled with Dummy patients.");
    printf("[SUCCESS] All remaining beds (3 ER / 10 OPD) are now FILLED with Dummy patients.\n");
}