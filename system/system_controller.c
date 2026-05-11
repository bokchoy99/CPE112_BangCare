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
    if (severity < 1 || severity > 5 || pain < 1 || pain > 10) {
        printf("[ERROR] Invalid Input: Severity 1-5, Pain 1-10\n");
        return;
    }

    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);

    if (p) {
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++);
        
        // 1. เก็บใน Hash (สำหรับค้นหา)
        hashTableInsert((HashTable*)gSystem.patientTable, p);
        
        // 2. เก็บใน Heap (สำหรับจัดคิวรักษา)
        heapInsert((Heap*)gSystem.triageQueue, p);

        // 3. เก็บใน Linked List (สำหรับโชว์ใน Queue Dashboard และ Aging)
        listInsert(gSystem.agingList, p);

        p->state = 0; // สมมติ 0 คือ WAITING ตาม Enum ใน patient.h

        //Dashboard register confirmation
        printf("[HASH] %s stored in registry.\n", p->id);
        printf("[TRIAGE] %s added to priority queue.\n", p->id);
        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
    }
}