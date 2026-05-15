#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command.h"
#include <time.h>

#include "../system/system_controller.h"
#include "../core/system_context.h"
#include "../bed/bed_manager.h"
#include "../data/hash_table.h"

/*-------------------------------------------
    GLOBSL DECLARATION
---------------------------------------------*/

void systemPeekSeverity(int sev);
void systemPeekBed(int bedId);
void systemPeekAging();
void systemPeekHash(const char* id);
/*-------------------------------------------
    Function UI Helper
---------------------------------------------*/
/* ABOUT
*/
void displayAbout() {
    printf("\033[H\033[J"); // Clear screen
    printf("\n=====================================\n");
    printf(" [ PROJECT INFORMATION ]\n");
    printf(" Project Name: BangCare (CPE112_BangCare)\n");
    printf(" Description : An Emergency Room triage management system\n");
    printf("               designed to optimize bed resources and\n");
    printf("               minimize patient waiting times.\n");
    printf(" Version      : 2.1 (5/14/2026)\n");
    printf(" Developers   : 68070503412 Karnsinee Nikrotamethanee\n");
    printf("                68070503422 Nichapa Piromying\n");
    printf("                68070503475 Patchara Nimittakunchai\n");
    printf("                68070503478 Patteera Pattaraporntaweewat\n");
    printf("=====================================\n");
}

/* HELP
*/
void displayHelp() {
    printf("\033[H\033[J");
    printf("\n=====================================\n");
    printf(" [ COMMANDS ]\n");
    printf("add <name> <severity> <pain> : Add patient into the system\n");
    printf("stat         : Show system statistics\n");
    printf("tick <n>     : Advance time by n unit (n x 2min)\n");
    printf("tick rec <n> : Advance time by n unit of recovery (n x 10min)\n");
    printf("peek <sev>   : View list of patients in specific severity level\n");
    printf("peek <bed>   : View details of the patient in specific bed\n");
    printf("peek hash <id>: View patient details by ID\n");
    printf("peek aging   : View patients near score adjustment threshold\n");
    printf("about        : Show project info\n");
    printf("cmd          : Show all command\n");
    printf("exit         : Close program\n");
    printf("=====================================\n");
}

/* STATS
*/
void displayStats() {
    printf("\033[H\033[J");
    printf("\n=====================================\n");
    printf("[ STATISTICS ]\n");
    printf("Resource Utilization: %d %%\n", 67); 
    printf("Triage Success Rate: %d\n", 67);
    printf("Average Wait Time: %.2f\n", 4.8);
    printf("Max Waiting Time: %.2f\n", 6.7);
    printf("Aging Impact: %d\n", 0);
    printf("Throughput: %d\n", 0);
    printf("=====================================\n");
}

/* PEEK DISPLAY*/

//Peek severity
void displaySeverityList(int sev, Patient* list) {
    printf("\n=====================================\n");
    printf(" [ PATIENTS IN SEVERITY S%d ]\n", sev);
    printf("-------------------------------------\n");
    
    int count = 0;
    Patient* curr = list;
    while (curr != NULL) {
        if (curr->severity == sev) {
            printf(" [%d] ID: %s | Name: %-15s | Pain: %d | State: %d\n", 
                   ++count, curr->id, curr->name, curr->pain, curr->state);
        }
        curr = curr->next;
    }
    
    if (count == 0) printf(" No patients found in this severity level.\n");
    printf("=====================================\n");
}

//Peek BedID
void displayBedDetail(BedNode* bed) {
    printf("\n=====================================\n");
    printf(" [ BED #%d INFORMATION ]\n", bed->idBed);
    
    if (bed->isOccupied && bed->patient != NULL) {
        printf(" Status      : OCCUPIED\n");
        printf(" Patient ID  : %s\n", bed->patient->id);
        printf(" Name        : %s\n", bed->patient->name);
        printf(" Severity    : S%d\n", bed->patient->severity);
        printf(" Remaining   : %d ticks\n", bed->patient->treatmentRemaining);
    } else {
        printf(" Status      : VACANT (Available)\n");
    }
    printf("=====================================\n");
}

//Peek severity
void displayAgingAnalysis(Patient* list) {
    printf("\n=====================================\n");
    printf(" [ AGING THRESHOLD WATCHLIST ]\n");
    printf(" (Showing patients within 2 ticks of promotion)\n");
    printf("-------------------------------------\n");

    int count = 0;
    Patient* curr = list;
    while (curr != NULL) {
        if (curr->state == WAITING || curr->state == IN_QUEUE) {
            int waitTicks = gSystem.tickCount - curr->arrivalTick;
            int ticksToNext = 5 - (waitTicks % 5); // อิงตาม AGING_THRESHOLD_TICKS

            if (ticksToNext <= 2) {
                printf(" ID: %-8s | %-12s | S%d -> S%d (in %d ticks)\n", 
                       curr->id, curr->name, curr->severity, 
                       (curr->severity < 5 ? curr->severity + 1 : 5), ticksToNext);
                count++;
            }
        }
        curr = curr->next;
    }
    if (count == 0) printf(" No patients near threshold at this time.\n");
    printf("=====================================\n");
}

//Peek ID
void displayHashID(Patient* p) {
    if (p == NULL) {
        printf("\n[ERROR] Patient not found in Hash Table.\n");
        return;
    }

    printf("\n=====================================\n");
    printf(" [ PATIENT HASH DATA ]\n");
    printf(" ID          : %s\n", p->id);
    printf(" Name        : %s\n", p->name);
    printf(" Severity    : S%d\n", p->severity);
    printf(" Pain Level  : %d\n", p->pain);
    printf(" Arrival Time: %s\n", p->arrivalTime); 
    
    // เชื่อมโยงสถานะ: ถ้าเป็น ALLOCATED หรือ TREATING แสดงว่าได้เตียงแล้ว
    bool inBed = (p->state == ALLOCATED || p->state == TREATING);
    printf(" Status      : %s\n", inBed ? "IN BED" : (p->state == DONE ? "DONE" : "WAITING"));
    printf("=====================================\n");
}

/*-------------------------------------------
    Function LOGIC Helper
---------------------------------------------*/

void handleTick(char* input) {
    char cmd[20], sub[20];
    int units = 0;

    // กรณี tick rec <n>
    if (sscanf(input, "%s %s %d", cmd, sub, &units) == 3 && strcmp(sub, "rec") == 0) {
        systemTick(units * 2); 
        printf("[SYSTEM] Advanced %d units of recovery time (%d minutes).\n", units, units * 10);
    }
    // กรณี tick <n>
    else if (sscanf(input, "%s %d", cmd, &units) == 2) {
        systemTick(units);
        printf("[SYSTEM] Advanced %d units of time (%d minutes).\n", units, units * 2); 
    } else {
        printf("[ERROR] Usage: tick <n> or tick rec <n>\n");
    }
}

/* PEEK */
void handlePeek(char* input) {
    char cmd[20], sub[20], extra[100];
    int numArgs = sscanf(input, "%s %s %s", cmd, sub, extra);

    if (numArgs < 2) {
        printf("[ERROR] Usage: peek <sev|bed_id|aging|hash>\n");
        return;
    }

    if (strcmp(sub, "aging") == 0) {
        systemPeekAging(); // ฟังก์ชันสมมติในระบบของคุณ
    } 
    else if (strcmp(sub, "hash") == 0 && numArgs == 3) {
        systemPeekHash(extra); // extra คือ id
    } 
    else {
        // ตรวจสอบว่าเป็นตัวเลข (Severity หรือ Bed ID)
        int val = atoi(sub);
        if (val > 0 && val <= 5) {
            systemPeekSeverity(val);
        } else {
            systemPeekBed(val);
        }
    }
}

void systemPeekSeverity(int sev) {
    // ดึงหัว List จาก AgingList มากรอง
    displaySeverityList(sev, (Patient*)gSystem.agingList);
}

void systemPeekBed(int bedId) {
    if (gSystem.beds == NULL) return;

    BedNode* curr = gSystem.beds->head;
    while (curr != NULL) {
        if (curr->idBed == bedId) {
            displayBedDetail(curr);
            return;
        }
        curr = curr->next;
    }
    printf("\n[ERROR] Bed ID %d not found in system.\n", bedId);
}

void systemPeekAging() {
    displayAgingAnalysis((Patient*)gSystem.agingList);
}

/*-------------------------------------------
    Cmd Main Processor
---------------------------------------------*/

bool processCommand(char* input) {
    char command[20], arg1[100];
    int arg2, arg3;
    int numArgs = sscanf(input, "%s %s %d %d", command, arg1, &arg2, &arg3);

    // 1. Exit Check
    if (strcmp(command, "exit") == 0) return false;

    // 2. Command Dispatcher ตัวเช็คว่ามีไรต่อท้ายcmdไหม
    if (strcmp(command, "add") == 0) {
        if (numArgs == 4) systemAddPatient(arg1, arg2, arg3);
        else printf("\n[ERROR] Usage: add <name> <severity> <pain>\n");
    } 
    else if (strcmp(command, "tick") == 0) {
        handleTick(input);
    } 
    else if (strcmp(command, "peek") == 0) {
        handlePeek(input);
    }
    else if (strcmp(command, "stat") == 0) {
        displayStats();
    }
    else if (strcmp(command, "about") == 0) {
        displayAbout();
    }
    else if (strcmp(command, "cmd") == 0) {
        displayHelp();
    }
    else if (strcmp(command, "free") == 0) {
        if (numArgs >= 2) freeBed(atoi(arg1));
        else printf("[ERROR] Usage: free <bed_id>\n");
    }
    else if (strcmp(command, "fillbeds") == 0) {
        fillAllBeds();
    }
    else if (strlen(command) > 0) {
        printf("\n[ERROR] Unknown Command: '%s'\n", command);
    }

    return true;
}