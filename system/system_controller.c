#include "system_controller.h"
#include "../core/system_context.h"
#include "../core/config.h"

// ฟังก์ชันอัปเดตเวลาตาม Time Model [cite: 82]
void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

// tick <n> : การเร่งเวลา [cite: 82, 89]
void systemTick(int n) {
    for(int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();
        
        // run aging ทุกครั้งที่ tick เพิ่ม 
        // runAging(); 
        
        // run recovery (ถ้ามี) 
        // runRecovery();
    }
}

// systemAddPatient: ทุก operation ต้องผ่าน system layer [cite: 92]
void systemAddPatient(const char* name, int severity, int pain) {
    Patient* p = createPatient(name, severity, pain);
    if (p) {
        // บันทึกลง Hash Table และเข้า Heap [cite: 86]
        // hashTableInsert(gSystem.patientTable, p);
        // heapInsert(gSystem.triageQueue, p);
    }
}