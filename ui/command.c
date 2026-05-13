#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command.h"
#include "../system/system_controller.h"
#include "../core/system_context.h"
#include "../bed/bed_manger.h"


bool processCommand(char* input) {
    char command[20], name[100];
    int severity, pain;
    int numArgs = sscanf(input, "%s %s %d %d", command, name, &severity, &pain);

    // 1. exit -> return false
    if (strcmp(command, "exit") == 0) {
        return false; 
    }

    // 2. other command -> return true
    if (strcmp(command, "add") == 0) {
        if (numArgs == 4) {
            systemAddPatient(name, severity, pain);
        } else {
            printf("\n[ERROR] Usage: add <name> <severity> <pain>\n");
        }
    } 
        // เดี๋ยวแก้อีกที
    else if (strcmp(command, "tick") == 0) {
        systemTick(1);
    }
    else if (strcmp(command, "about") == 0) { //About
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
    else if(strcmp(command, "cmd") == 0){ //cmd Show all the Command
        printf("\033[H\033[J");
        printf("\n===========================================================\n");
        printf(" [ COMMANDS ]\n");
        printf("add <name> <severity> <pain> : Add patient into the system\n");
        printf("stat  : Show system statistics\n");
        printf("tick <n> : Advance time by n unit (n x 5 minutes)\n");
        printf("tick rec <n> : Advance time by n unit of recovery time (n x 25 minutes)\n");
        printf("peek <sev_num> : View the number and list of patients in that specific severity level.\n");
        printf("peek <type_bed_num> : View details of the patient in that specific bed.\n");
        printf("peek hash <id> : View patient details and display current status.\n");
        printf("peek aging : View the list of patients approaching the score adjustment threshold (within 1 minute).\n");
        printf("about : Show project info\n");
        printf("cmd : Show all command\n");
        printf("exit  : Close program\n");
        printf("===========================================================\n");
    }
    //อันนี้คร่าวๆก่อน เดี๋ยวค่อยแก้อีกที
    else if(strcmp(command, "stat") == 0){ //stat Show calculated stat
        printf("\033[H\033[J");
        printf("\n===========================================================\n");
        printf("[ STATISTICS ]\n");
        printf("Resource Utilization: %d %\n", 67); //เลขในตอนนี้ทั้งหมดนอกจากidเป็นค่าตัวอย่าง
        printf("Triage Success Rate: %d\n", 67);
        printf("Average Wait Time: %.2f\n", 4.8);
        printf("Max Waiting Time: %.2f\n", 6.7);
        printf("Aging Impact: %d\n", 0);
        printf("Throughput: %d\n", 0);
        printf("===========================================================\n");
    }
    else if (strcmp(command, "free") == 0) {
        if (numArgs >= 2) {
            int bedId = atoi(name); // ในที่นี้ name คือพารามิเตอร์ตัวที่สอง
            freeBed(bedId);
        }
    }
    else if (strlen(command) > 1) {
        printf("\n[ERROR] Unknown Command: '%s'\n", command);
    }
    return true;
}