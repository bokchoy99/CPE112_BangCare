#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "system_context.h"
#include "config.h"
#include "../system/system_controller.h"
#include "../data/hash_table.h" 
#include "../data/heap.h"
#include "../data/linked_list.h"

void displayUpcomingStream() { // Upcoming stream last 3 -> ยังไม่ sort เป็น stream ก่อนเข้า heap
    Patient** headPtr = (Patient**)gSystem.agingList;
    
    if (headPtr == NULL || *headPtr == NULL) {
        printf("Empty");
        return;
    }

    Patient* curr = *headPtr;
    Patient* stream[3] = {NULL, NULL, NULL};
    int count = 0;

    // เก็บ 3 คนล่าสุด (004 -> 003 -> 002)
    while (curr != NULL && count < 3) {
        stream[count] = curr;
        curr = curr->next;
        count++;
    }

    // แสดงผลจากเก่าไปใหม่ (002 <- 003 <- 004)
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", stream[i]->id); 
        if (i > 0) printf(" <- ");
    }
}

void displayDashboard() {
    printf("\n===========================================================\n");
    printf("       BANGCARE: HOSPITAL TRIAGE MANAGEMENT SYSTEM         \n");
    printf("===========================================================\n");

    printf(" Beds: [%02d/30] | Tick: %d | Simulated Time: %ld\n", 
            0, gSystem.tickCount, gSystem.simulatedTime); // เลข 0 คือตัวอย่างจำนวนเตียงที่ใช้เพื่อtest

    //BED
    printf(" [ BED ALLOCATION ]\n");
    printf(" ER Beds (S5)   : [ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ][ EMPTY ]\n"); //emptyใส่เป็นค่าตัวอย่างไว้
    printf(" OPD Beds (S1-4): [ EMPTY ][ EMPTY ][ EMPTY ]... (Total 25)\n");
    printf("-----------------------------------------------------------\n");

    // Upcoming stream last 3 -> ยังไม่ sort เป็น stream ก่อนเข้า heap
    printf(" Upcoming Stream: ");
    displayUpcomingStream();
    printf("\n");

    printf("-----------------------------------------------------------\n");
    printf(" Command > ");
}

void initSystem() {
    gSystem.tickCount = 0;
    gSystem.baseTime = (long)time(NULL);
    gSystem.patientCounter = 1;
    gSystem.simulatedTime = gSystem.baseTime;
    gSystem.patientTable = createHashTable();

    gSystem.triageQueue = initHeap(100); //Priority
    gSystem.agingList = createLinkedList(); // Linked list
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
        else if (strcmp(command, "about") == 0) { //About
            printf("\033[H\033[J");
            printf("\n===========================================================\n");
            printf(" [ PROJECT INFORMATION ]\n");
            printf(" Project Name: BangCare (CPE112_BangCare)\n");
            printf(" Description : An Emergency Room triage management system\n");
            printf("               designed to optimize bed resources and\n");
            printf("               minimize patient waiting times.\n");
            printf(" Version      : 1.0 (4/28/2026)\n");
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
        else if (strcmp(command, "exit") == 0) { //exit
            break;
        }
        else if (strlen(command) > 1) {
            printf("\n[ERROR] Unknown Command: '%s'\n", command);
        }
    }
    return 0;
}