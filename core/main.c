#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "system_context.h"
#include "config.h"
#include "../system/system_controller.h"

void displayDashboard() {
    // อ้างอิงรูปแบบจากเอกสาร [cite: 286-307]
    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");
    printf(" Beds: [%02d/30] | Tick: %d | Simulated Time: %ld\n", 
            0, gSystem.tickCount, gSystem.simulatedTime); // เลข 0 คือตัวอย่างจำนวนเตียงที่ใช้
    printf(" [ BED ALLOCATION ]\n");
    printf(" ER Beds (S5)   : [ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ]\n");
    printf(" OPD Beds (S1-4): [ EMPTY ][ EMPTY ][ EMPTY ]... (Total 25)\n");
    printf("-----------------------------------------------------------\n");
    printf(" Command > ");
}

void initSystem() {
    gSystem.tickCount = 0;
    gSystem.baseTime = (long)time(NULL);
    gSystem.patientCounter = 1;
    gSystem.simulatedTime = gSystem.baseTime;
}

int main() {
    char input[256];
    char command[20];
    char name[100];
    int severity, pain;

    initSystem();

    while (true) {
        displayDashboard(); // เรียกใช้ UI ทุกครั้งที่วน Loop

        if (fgets(input, sizeof(input), stdin) == NULL) break;

        int numArgs = sscanf(input, "%s %s %d %d", command, name, &severity, &pain);

        if (strcmp(command, "add") == 0) {
            if (numArgs == 4) {
                systemAddPatient(name, severity, pain);
            } else {
                printf("\n[ERROR] Usage: add <name> <severity> <pain>\n");
            }
        } 
        else if (strcmp(command, "tick") == 0) {
            systemTick(1); // เพิ่มทีละ 1 tick
        }
        else if (strcmp(command, "about") == 0) {
            printf("\033[H\033[J");
            printf("\n===========================================================\n");
            printf(" [ PROJECT INFORMATION ]\n");
            printf(" Project Name: BangCare (CPE112_BangCare)\n");
            printf(" Description : This is CPE112 final project about an Emergency Room triage management system\n");
            printf("               designed to optimize bed resources and\n");
            printf("               minimize patient waiting times.\n");
            printf(" Version      : 1.0 (4/28/2026)\n");
            printf(" Developers   : 68070503412 Karnsinee Nikrotamethanee\n");
            printf("                68070503422 Nichapa Piromying\n");
            printf("                68070503475 Patchara Nimittakunchai\n");
            printf("                68070503478 Patteera Pattaraporntaweewat\n");
            printf("===========================================================\n");
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else if (strlen(command) > 1) {
            printf("\n[ERROR] Unknown Command: '%s'\n", command);
        }
    }
    return 0;
}