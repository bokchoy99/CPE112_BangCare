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

bool processCommand(char* input) {
    char command[20], name[100];
    int severity, pain;
    int numArgs = sscanf(input, "%s %s %d %d", command, name, &severity, &pain);

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
    } 
    else if (strcmp(command, "tick") == 0) {
        char subCommand[20];
        int units = 0;

        // tick rec <n>: เดินเวลา recovery
        if (sscanf(input, "%s %s %d", command, subCommand, &units) == 3) {
            if (strcmp(subCommand, "rec") == 0) {
                systemTick(units * 2); 
                printf("[SYSTEM] Advanced %d units of recovery time (%d minutes).\n", units, units * 10);
            }
        }
        // tick <n>: เดินเวลาปกติ
        else if (sscanf(input, "%s %d", command, &units) == 2) {
            systemTick(units);
            printf("[SYSTEM] Advanced %d ticks (%d minutes).\n", units, units * 5);
        }
    }
    else if (strcmp(command, "about") == 0) {
        printf("\033[H\033[J");
        printf("\n===========================================================\n");
        printf(" [ PROJECT INFORMATION ]\n");
        printf(" Project Name: BangCare (CPE112_BangCare)\n");
        printf(" Description : An Emergency Room triage management system\n");
        printf("               designed to optimize bed resources and\n");
        printf("               minimize patient waiting times.\n");
        printf(" Version      : 1.3 (5/12/2026)\n");
        printf(" Developers   : 68070503412 Karnsinee Nikrotamethanee\n");
        printf("                68070503422 Nichapa Piromying\n");
        printf("                68070503475 Patchara Nimittakunchai\n");
        printf("                68070503478 Patteera Pattaraporntaweewat\n");
        printf("===========================================================\n");
    }
    else if (strcmp(command, "cmd") == 0) {
        printf("\033[H\033[J");
        printf("\n===========================================================\n");
        printf(" [ COMMANDS ]\n");
        printf("add <name> <severity> <pain>  : Add patient into the system\n");
        printf("stat                          : Show system statistics\n");
        printf("tick <n>                      : Advance time by n unit (n x 2min)\n");
        printf("tick rec <n>                  : Advance time by n unit of recovery time (n x 10min)\n");
        printf("peek <sev_num>                : View patients in that severity level\n");
        printf("peek <type_bed_num>           : View patient details in that bed\n");
        printf("peek hash <id>                : View patient details and status\n");
        printf("peek aging                    : View patients approaching aging threshold\n");
        printf("about                         : Show project info\n");
        printf("cmd                           : Show all commands\n");
        printf("exit                          : Close program\n");
        printf("===========================================================\n");
    }
    else if (strcmp(command, "stat") == 0) {
        printf("\033[H\033[J");
        printf("\n===========================================================\n");
        printf("[ STATISTICS ]\n");
        printf("Resource Utilization: %d%%\n", 67);
        printf("Triage Success Rate: %d\n", 67);
        printf("Average Wait Time: %.2f\n", 4.8);
        printf("Max Waiting Time: %.2f\n", 6.7);
        printf("Aging Impact: %d\n", 0);
        printf("Throughput: %d\n", 0);
        printf("===========================================================\n");
    }
    else if (strcmp(command, "free") == 0) {
        if (numArgs >= 2) {
            int bedId = atoi(name);
            freeBed(bedId);
            systemAutoAllocate(); // พอเตียงว่าง ดึงคนในคิวมาใส่ทันที
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
        fillAllBeds(); // จำลองเตียงเต็มสำหรับ demo
    }
    else if (strlen(command) > 0) {
        printf("\n[ERROR] Unknown Command: '%s'\n", command);
    }

    return true;
}