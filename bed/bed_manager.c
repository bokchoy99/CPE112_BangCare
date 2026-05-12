#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bed_system.h"

BedList* globalBedList = NULL; 

void initBeds() {
    globalBedList = (BedList*)malloc(sizeof(BedList));
    globalBedList->head = NULL;
    globalBedList->tail = NULL;
    // 5.3 Capacity = 30 เตียง
    globalBedList->totalBeds = 30;
    globalBedList->occupiedBeds = 0;

    for (int i = 1; i <= 30; i++) {
        BedNode* newNode = (BedNode*)malloc(sizeof(BedNode));
        newNode->idBed = i;
        newNode->isOccupied = false;
        newNode->patient = NULL;
        newNode->next = NULL;
        newNode->prev = globalBedList->tail;

        // แบ่ง ER = 5 เตียง, OPD = 25 เตียง
        if (i <= 5) strcpy(newNode->type, "ER");
        else strcpy(newNode->type, "OPD");

        if (globalBedList->tail != NULL) globalBedList->tail->next = newNode;
        else globalBedList->head = newNode;
        
        globalBedList->tail = newNode;
    }
    // หมายเหตุ 5.3: จำนวนหมอ 30 คน (1 หมอ : 1 เตียง) ถูกจัดการโดยปริยายผ่านจำนวนเตียงอยู่แล้ว
}

// 5.2 RULE: การรับคนเข้าเตียง
bool allocateBed(int severity) {
    if (globalBedList == NULL) return false;
    BedNode* curr = globalBedList->head;

    // กรณีที่ 1: Severity 5 (ต้องเข้า ER เท่านั้น)
    if (severity == 5) {
        // วนหาเตียง ER (เตียง 1-5)
        while (curr != NULL && strcmp(curr->type, "ER") == 0) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                globalBedList->occupiedBeds++;
                printf("[SUCCESS] Allocated ER Bed #%d for S5 Patient.\n", curr->idBed);
                return true;
            }
            curr = curr->next;
        }

        // --- RULE 5.2: ถ้า ER เต็ม ต้อง Confirm ก่อนใช้ OPD ---
        printf("\n[WARNING] All ER Beds are FULL!\n");
        printf("Do you want to use an OPD bed for this S5 patient? (y/n): ");
        char confirm;
        scanf(" %c", &confirm); // มีเว้นวรรคหน้า %c เพื่อล้างปุ่ม Enter ในบัฟเฟอร์

        if (confirm == 'y' || confirm == 'Y') {
            // ถ้าตอบ y จะหาเตียง OPD ให้
            while (curr != NULL) {
                if (!curr->isOccupied) {
                    curr->isOccupied = true;
                    globalBedList->occupiedBeds++;
                    printf("[SUCCESS] Allocated OPD Bed #%d for S5 Patient (Confirmed).\n", curr->idBed);
                    return true;
                }
                curr = curr->next;
            }
            printf("[FAILED] OPD Beds are also FULL!\n");
        } else {
            printf("[INFO] Allocation cancelled. S5 Patient is waiting.\n");
        }
    } 
    // กรณีที่ 2: Severity 1-4 (เข้า OPD)
    else {
        // เลื่อน pointer ข้ามเตียง ER ไปยัง OPD (เตียงที่ 6 ขึ้นไป)
        while (curr != NULL && strcmp(curr->type, "ER") == 0) {
            curr = curr->next;
        }
        // หาเตียง OPD ที่ว่าง
        while (curr != NULL) {
            if (!curr->isOccupied) {
                curr->isOccupied = true;
                globalBedList->occupiedBeds++;
                printf("[SUCCESS] Allocated OPD Bed #%d for S%d Patient.\n", curr->idBed, severity);
                return true;
            }
            curr = curr->next;
        }
        printf("[FAILED] All OPD Beds are FULL!\n");
    }
    return false;
}

// ฟังก์ชันเคลียร์เตียงให้ว่าง 
bool freeBed(int idBed) {
    if (globalBedList == NULL) return false;
    BedNode* curr = globalBedList->head;
    
    while (curr != NULL) {
        if (curr->idBed == idBed) {
            if (curr->isOccupied) {
                curr->isOccupied = false;
                curr->patient = NULL;
                globalBedList->occupiedBeds--;
                printf("[SUCCESS] Bed #%d (%s) is now EMPTY.\n", idBed, curr->type);
                return true;
            } else {
                printf("[INFO] Bed #%d is already empty.\n", idBed);
                return false;
            }
        }
        curr = curr->next;
    }
    printf("[ERROR] Bed ID %d not found.\n", idBed);
    return false;
}

// แสดงสถานะเตียง
void displayBedAllocation() {
    if (globalBedList == NULL) return;
    BedNode* curr = globalBedList->head;

    printf("\n[ BED ALLOCATION: %d/30 ]\n", globalBedList->occupiedBeds);
    
    printf(" ER Beds (S5)   : ");
    for (int i = 0; i < 5; i++) {
        printf(curr->isOccupied ? "[ IN USE ]" : "[ EMPTY ]");
        curr = curr->next;
    }
    printf("\n");

    printf(" OPD Beds (S1-4): ");
    for (int i = 0; i < 5; i++) { // โชว์ตัวอย่างแค่ 5 เตียง
        printf(curr->isOccupied ? "[ IN USE ]" : "[ EMPTY ]");
        curr = curr->next;
    }
    printf("... (Total 25)\n");
}
