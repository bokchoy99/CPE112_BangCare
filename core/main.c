#include <stdio.h>
#include <time.h>
#include "system_context.h"
#include "config.h"

void initSystem() {
    gSystem.tickCount = 0;
    gSystem.baseTime = (long)time(NULL); // เวลาเครื่อง ณ ตอนเริ่ม 
    gSystem.patientCounter = 1;
    
    // คำนวณ simulatedTime ครั้งแรก
    gSystem.simulatedTime = gSystem.baseTime; 
    
    // Initialize structures... [cite: 6, 36]
    printf("BangCare System v1.0 Started at: %ld\n", gSystem.baseTime);
}

int main() {
    initSystem();
    
    // Main Loop ตาม Workflow 
    // 1. readCommand()
    // 2. processCommand()
    // 3. updateSystem()
    
    return 0;
}