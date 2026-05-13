#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dashboard.h"
#include "../core/system_context.h"
#include "../patient/patient.h"
#include "../bed/bed_manger.h"

/**
 * แสดง ID คนไข้ 3 คนล่าสุดที่เพิ่งเข้าสู่ระบบ (ยังไม่ถูกจัดคิวด้วย Heap)
 */
void displayUpcomingStream() {
    // ดึงตัวแปร agingList จาก gSystem และทำการ Cast ให้ถูกต้องตามโครงสร้าง Linked List
    Patient** headPtr = (Patient**)gSystem.agingList;
    if (headPtr == NULL || *headPtr == NULL) {
        printf("Empty");
        return;
    }

    Patient* curr = *headPtr;
    Patient* stream[3] = {NULL, NULL, NULL};
    int count = 0;

    // เก็บข้อมูลคนไข้ 3 รายการล่าสุด
    while (curr != NULL && count < 3) {
        stream[count] = curr;
        curr = curr->next;
        count++;
    }

    // แสดงผลย้อนกลับเพื่อให้เห็นลำดับจากเก่าไปใหม่ (เช่น 002 <- 003 <- 004)
    for (int i = count - 1; i >= 0; i--) {
        if (stream[i] != NULL) {
            printf("%s", stream[i]->id); 
            if (i > 0) printf(" <- ");
        }
    }
}

/**
 * ฟังก์ชันหลักสำหรับวาด Dashboard บน Terminal
 */
void displayDashboard() {
    char time_str[20];
    time_t raw_time = (time_t)gSystem.simulatedTime;
    struct tm* info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", info);

    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");
    
    // แสดงข้อมูลภาพรวม: จำนวนเตียงที่ถูกจอง, รอบเวลา (Tick), และเวลาจำลอง
    printf(" Beds: [%02d/30] | Tick: %d | Time: %s\n", 
            gSystem.beds ? gSystem.beds->occupiedBeds : 0, 
            gSystem.tickCount, 
            time_str);
    
    printf(" [ BED ALLOCATION ]\n");

    if (gSystem.beds != NULL) {
        BedNode* curr = gSystem.beds->head;

        // --- ส่วนของ ER Beds (เตียงหมายเลข 1-5 สำหรับ Severity 5) ---
        printf(" ER Beds (S5)   : ");
        for (int i = 0; i < 5 && curr != NULL; i++) {
            if (curr->isOccupied && curr->patient != NULL) {
                // แสดง ID ของคนไข้ที่ครองเตียงอยู่
                printf("[%s]", curr->patient->id); 
            } else {
                printf("[  -  ]"); // แสดงสถานะว่าง
            }
            curr = curr->next;
        }
        printf("\n");

        // --- ส่วนของ OPD Beds (แสดงตัวอย่าง 5 เตียงแรกจากทั้งหมด 25 เตียง) ---
        printf(" OPD Beds (S1-4): ");
        for (int i = 0; i < 5 && curr != NULL; i++) {
            if (curr->isOccupied && curr->patient != NULL) {
                printf("[%s]", curr->patient->id);
            } else {
                printf("[  -  ]");
            }
            curr = curr->next;
        }
        printf("... (Total 25)\n");
    }

    printf("-----------------------------------------------------------\n");
    printf(" Upcoming Stream: ");
    displayUpcomingStream(); // เรียกฟังก์ชันแสดงรายชื่อคนไข้ใหม่
    printf("\n-----------------------------------------------------------\n");
    printf(" Command > ");
}