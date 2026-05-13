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
<<<<<<< HEAD
#include "../utils/id_generator.h"
#include "../bed/bed_manger.h"
#include "../data/linked_list.h"
=======
//p
#include "../data/heap.h" // use heapRemove / insert 
//void heapRemove(Patient* p); 
//void heapInsert(Patient* p);
void insertToAgingList(Patient* p); //เอาคนไข้เข้าคิวรอ
void removeFromAgingList(Patient* p); //เอาคนไข้ออกจากคิว
void runAging(); //อัปเดต severity
>>>>>>> 7337bb0 (Add Aging List and Bed System)

// สร้างตัวแปร gSystem ไว้ที่นี่
SystemContext gSystem; 

void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

void systemTick(int n) {
    //loop ตามจำนวน tick ที่พึ่งส่งเข้ามา
    for(int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();      
        // [เพิ่ม] เรียกฟังก์ชันตรวจสอบ Aging ทุกคนในลิสต์
        gSystem.tickCount++;//เวลาในระบบเพิ่มขึ้น 1 tick
        updateSimulatedTime(); 
        // scan ทุก 2 tick (10 นาที)
        if (gSystem.tickCount % 2 == 0) {
            runAging();
        }
    }
}
void systemAddPatient(const char* name, int severity, int pain) {
    // 1. สร้างคนไข้และจัดการ ID/Hash Table/Queue (โค้ดเดิมของคุณ)
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);
<<<<<<< HEAD
    if (p) {
        generatePatientID(p->id, gSystem.patientCounter++);
=======


    if (p) {
<<<<<<< HEAD
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++);
        
        // 1. เก็บใน Hash (สำหรับค้นหา)
>>>>>>> 7337bb0 (Add Aging List and Bed System)
        hashTableInsert((HashTable*)gSystem.patientTable, p);
        heapInsert((Heap*)gSystem.triageQueue, p);
        linkedListInsert((Patient**)gSystem.agingList, p);

        // 2. ลิงก์เข้าสู่ระบบเตียงทันที (ส่วนที่ต้องเพิ่ม)
        bool allocated = allocateBed(p); // ส่งตัวแปร Patient ไปจองเตียง
        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
=======
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++);//สร้าง id
        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
        p->state = IN_QUEUE;
        p->arrivalTick = gSystem.tickCount; //p บันทึกว่าคนไข้เข้ามาตอนไหน
        p->agingApplied = 0; //p ยังไม่เคยถูก aging
        insertToAgingList(p); // ใส่เข้า aging list p
        heapInsert(p);//เพิ่มเข้าheap priority q
        
>>>>>>> 107adec (Add Aging List and Bed System)
    }
}

//p

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
        heapRemove(current);//เอาออกจากheapก่อนแก้ priority
        current->severity++;//เพิ่มฟามรุนแรง
        heapInsert(current); //ใส่กลับไปที่เดิม
        current->agingApplied = promoteLevel; //บันทึกว่า aging เปลี่ยนไปแล้วกี่รอบ
//แสดงผล
        printf("[AGING] %s -> S%d\n",
            current->id, current->severity);
    }

        current = next; // คนต่อไป
    }
}
//p
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


