#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dashboard.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../bed/bed_manager.h"

/**
 * แสดง ID คนไข้ 3 คนล่าสุดที่เพิ่งเข้าสู่ระบบ
 */
void displayUpcomingStream() {
    Patient* curr = (Patient*)gSystem.agingList; 
    
    if (curr == NULL) {
        printf("Empty");
        return;
    }

    Patient* stream[3] = {NULL, NULL, NULL};
    int count = 0;

    // แก้ไขโลจิคตรงนี้: กรองเฉพาะคนที่ยังไม่ได้เตียง
    while (curr != NULL && count < 3) {
        // ตรวจสอบสถานะ: ต้องยังไม่ได้เตียง (ALLOCATED)
        if (curr->state == WAITING || curr->state == IN_QUEUE) {
            stream[count] = curr;
            count++;
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("Empty");
        return;
    }

    for (int i = count - 1; i >= 0; i--) {
        if (stream[i] != NULL) {
            printf("%s", stream[i]->id); 
            if (i > 0) printf(" <- ");
        }
    }
}

/**
 * ฟังก์ชันหลักสำหรับวาด Dashboard
 */
void displayDashboard() {
    char buffer[50];
    time_t t = (time_t)gSystem.simulatedTime;
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", tm_info);

    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");

    int occupied = (gSystem.beds != NULL) ? gSystem.beds->occupiedBeds : 0;
    long totalMinutes = (gSystem.simulatedTime - gSystem.baseTime) / 60;

    // แก้ 13 → MAX_ER_BEDS + MAX_OPD_BEDS
    printf(" Beds: [%02d/%d] | Tick: %d (%ld min) | Time: %s\n",
        occupied, MAX_ER_BEDS + MAX_OPD_BEDS, gSystem.tickCount, totalMinutes, buffer);

    printf(" [ BED ALLOCATION ]\n");

    if (gSystem.beds != NULL) {
        BedNode* curr = gSystem.beds->head;

        // ER Beds — วนตาม MAX_ER_BEDS
        printf(" ER Beds  (S5)  : ");
        for (int i = 0; i < MAX_ER_BEDS && curr != NULL; i++) {
            if (curr->isOccupied && curr->patient != NULL) {
                printf("[%s]", curr->patient->id);
            } else {
                printf("[  -  ]");
            }
            curr = curr->next;
        }
        printf("\n");

        // OPD Beds — วนตาม MAX_OPD_BEDS
        printf(" OPD Beds (S1-4): ");
        for (int i = 0; i < MAX_OPD_BEDS && curr != NULL; i++) {
            if (curr->isOccupied && curr->patient != NULL) {
                printf("[%s]", curr->patient->id);
            } else {
                printf("[  -  ]");
            }
            curr = curr->next;
        }
        printf("\n");
    }

    printf("-----------------------------------------------------------\n");
    printf(" Upcoming Stream: ");
    displayUpcomingStream();
    printf("\n-----------------------------------------------------------\n");
    printf(" Command > ");
}