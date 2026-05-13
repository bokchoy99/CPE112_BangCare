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
#include "../utils/id_generator.h"
#include "../bed/bed_manger.h"
#include "../data/linked_list.h"

// สร้างตัวแปร gSystem ไว้ที่นี่
SystemContext gSystem; 

void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

void systemTick(int n) {
    for(int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();      
        // [เพิ่ม] เรียกฟังก์ชันตรวจสอบ Aging ทุกคนในลิสต์
    }
}

void systemAddPatient(const char* name, int severity, int pain) {
    // 1. สร้างคนไข้และจัดการ ID/Hash Table/Queue (โค้ดเดิมของคุณ)
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);
    if (p) {
        generatePatientID(p->id, gSystem.patientCounter++);
        hashTableInsert((HashTable*)gSystem.patientTable, p);
        heapInsert((Heap*)gSystem.triageQueue, p);
        linkedListInsert((Patient**)gSystem.agingList, p);

        // 2. ลิงก์เข้าสู่ระบบเตียงทันที (ส่วนที่ต้องเพิ่ม)
        bool allocated = allocateBed(p); // ส่งตัวแปร Patient ไปจองเตียง
        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
    }
}