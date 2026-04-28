#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_controller.h"
#include "../core/system_context.h"
#include "../core/config.h"
#include "../patient/patient.h"

// สร้างตัวแปร gSystem ไว้ที่นี่
SystemContext gSystem; 

void updateSimulatedTime() {
    gSystem.simulatedTime = gSystem.baseTime + (gSystem.tickCount * TICK_UNIT_MINUTES * 60);
}

void systemTick(int n) {
    for(int i = 0; i < n; i++) {
        gSystem.tickCount++;
        updateSimulatedTime();
    }
}

void systemAddPatient(const char* name, int severity, int pain) {
    if (severity < 1 || severity > 5 || pain < 1 || pain > 10) {
        printf("[ERROR] Invalid Input: Severity 1-5, Pain 1-10\n");
        return;
    }

    // เรียกใช้ createPatient จาก patient.h
    Patient* p = createPatient(NULL, name, severity, pain, gSystem.tickCount);

    if (p) {
        sprintf(p->id, "%s%03d", ID_PREFIX, gSystem.patientCounter++);
        printf("[SUCCESS] Registered: %s (ID: %s)\n", p->name, p->id);
        // p->state = WAITING; [cite: 116]
    }
}