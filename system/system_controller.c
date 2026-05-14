#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_controller.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"
#include "../data/hash_table.h" 
#include "../data/heap.h"
#include "../data/linked_list.h"
#include "../bed/bed_manager.h"
#include "../utils/logger.h"

// =======================================================
// SECTION 0: GLOBAL FUNCTION & VARIABLES
// เพื่อให้ฟังก์ชันข้างบนรู้จักฟังก์ชันที่อยู่ด้านล่าง
// =======================================================
void runAging(); 
void insertToAgingList(Patient* p);
void removeFromAgingList(Patient* p);
void updateBedTreatments();

// =======================================================
// SECTION 1: Time Management
// =======================================================

// สร้างตัวแปร gSystem ไว้ที่นี่
SystemContext gSystem; 

void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

void systemTick(int n) {
    //loop ตามจำนวน tick ที่พึ่งส่งเข้ามา
    for(int i = 0; i < n; i++) {
        gSystem.tickCount++; //เวลาในระบบเพิ่มขึ้น 1 tick
        updateSimulatedTime();
        updateBedTreatments();
        
        // [เพิ่ม] เรียกฟังก์ชันตรวจสอบ Aging ทุกคนในลิสต์
        // scan ทุก 2 tick (10 นาที)
        if (gSystem.tickCount % 2 == 0) {
            runAging();
        }
    }
}

// =======================================================
// SECTION 2: Patient Registration
// =======================================================

void systemAddPatient(const char* name, int severity, int pain) {
    // 1. สร้างคนไข้และจัดการ ID/Hash Table/Queue (โค้ดเดิมของคุณ)
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);

    if (p) {
        // --- การตั้งค่าเบื้องต้น ---
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++); //สร้าง id
        p->state = IN_QUEUE;
        p->arrivalTick = gSystem.tickCount; //p บันทึกว่าคนไข้เข้ามาตอนไหน
        p->agingApplied = 0; //p ยังไม่เคยถูก aging

        // --- การจัดเก็บข้อมูล (Data Structures) ---
        // 1. เก็บใน Hash (สำหรับค้นหา)
        hashTableInsert((HashTable*)gSystem.patientTable, p);
        
        // เพิ่มเข้าheap priority q (ส่ง Heap* เข้าไปด้วยตามโครงสร้างใหม่)
        heapInsert((Heap*)gSystem.triageQueue, p); 
        
        // ใส่เข้า aging list p
        insertToAgingList(p); 

        // --- ระบบเตียง ---
        // 2. ลิงก์เข้าสู่ระบบเตียงทันที (ส่วนที่ต้องเพิ่ม)
        bool allocated = allocateBed(p);
        if (allocated) {
            p->state = ALLOCATED; 
        }
        
        //add log
        char log_msg[128];
        sprintf(log_msg, "Registered patient %s (%s) | Severity: S%d, Pain: %d | Status: %s", 
                p->name, p->id, p->severity, p->pain, allocated ? "ADMITTED" : "QUEUED");
        logEvent(LOG_INFO, "PATIENT", log_msg);

        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
    }
}

// =======================================================
// SECTION 3: Aging System Logic
// =======================================================

void runAging() { // aging system 

    Patient* current = (Patient*)gSystem.agingList;

    while (current != NULL) { // ทีละคน

        Patient* next = current->next; //ไว้กัน list พังตอนแก้
        
        // next > คัดคน > aging
        if (current->state != WAITING && current->state != IN_QUEUE) { //aging แค่คนที่ยังรอ
            current = next;
            continue;
        } //aginf เฉพาะคนที่รอคิวอยู่ ไม่ยุ่งกับคนที่ได้เตียงแล้ว

        int waitTicks = gSystem.tickCount - current->arrivalTick; //เวลารอ
        int promoteLevel = waitTicks / AGING_THRESHOLD_TICKS; //level

        //เช้คว่า 1. ถึงเวลา promoteแล้ว 2.severityยังไม่เกิน 5
        if (promoteLevel > current->agingApplied && current->severity < 5) {
            
            // หมายเหตุ: การแก้ priority ใน heap ต้องเอาออกแล้วใส่ใหม่เพื่อให้มัน Re-sort
            // heapRemove(current); //เอาออกจากheapก่อนแก้ priority (รอ Implement heapRemove)
            
            current->severity++; //เพิ่มฟามรุนแรง
            
            // ใส่กลับไปที่เดิมเพื่อให้ Heap เรียงลำดับใหม่
            heapInsert((Heap*)gSystem.triageQueue, current); 
            
            current->agingApplied = promoteLevel; //บันทึกว่า aging เปลี่ยนไปแล้วกี่รอบ

            //แสดงผล
            printf("[AGING] %s -> S%d\n", current->id, current->severity);
        }

        current = next; // คนต่อไป
    }
}

// =======================================================
// SECTION 4: Aging List Helpers (Linked List)
// =======================================================

void insertToAgingList(Patient* p) { //เอาคนไข้เข้าlist

    p->next = (Patient*)gSystem.agingList;
    p->prev = NULL;

    if (gSystem.agingList != NULL) {
        ((Patient*)gSystem.agingList)->prev = p;
    }

    gSystem.agingList = p; //head list
}

void removeFromAgingList(Patient* p) { // get out f list

    // ถ้ามีตัวก่อนหน้า
    if (p->prev != NULL) {
        p->prev->next = p->next;
    } 
    else {
        // ถ้า p เป็นตัวแรกของ list
        gSystem.agingList = p->next;
    }

    // ถ้ามีตัวถัดไป
    if (p->next != NULL) {
        p->next->prev = p->prev;
    }

    // ตัด p ออกจาก list
    p->next = NULL;
    p->prev = NULL;
}

void updateBedTreatments() {
    if (gSystem.beds == NULL) return;
    BedNode* curr = gSystem.beds->head;
    
    while (curr != NULL) {
        if (curr->isOccupied && curr->patient != NULL) {
            // ลดเวลาการรักษาลงทีละ 1 Tick
            curr->patient->treatmentRemaining--;
            
            // ถ้ารักษาเสร็จแล้ว (เวลาเหลือ 0 หรือน้อยกว่า)
            if (curr->patient->treatmentRemaining <= 0) {
                Patient* p = curr->patient;
                p->state = DONE; // ปรับสถานะเป็นรักษาเสร็จแล้ว
                
                // เอาออกจากคิวตรวจสอบ Aging ด้วยเพราะได้รักษาเสร็จแล้ว
                removeFromAgingList(p); 
                
                // บันทึก Log และเคลียร์เตียงให้ว่าง
                char log_msg[100];
                sprintf(log_msg, "Patient %s treatment complete. Freeing bed #%d", p->id, curr->idBed);
                logEvent(LOG_INFO, "SYSTEM", log_msg);
                
                freeBed(curr->idBed); // เรียกฟังก์ชันคืนเตียงเดิมของคุณ
            }
        }
        curr = curr->next;
    }
}