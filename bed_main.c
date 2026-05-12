#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bed_system.h"

int main() {
    char command[50];
    int param;

    initBeds(); // สร้างเตียง 30 เตียง
    
    printf("=========================================\n");
    printf("      BED SYSTEM TEST ENVIRONMENT        \n");
    printf("=========================================\n");
    printf(" Commands:\n");
    printf("   - show            : ดูสถานะเตียง\n");
    printf("   - add <severity>  : จองเตียง (1-4=OPD, 5=ER)\n");
    printf("   - free <bed_id>   : คืนเตียง (ระบุหมายเลข 1-30)\n");
    printf("   - exit            : ออกจากโปรแกรม\n");
    printf("=========================================\n");

    while (true) {
        printf("\nBedTest > ");
        scanf("%s", command);

        if (strcmp(command, "show") == 0) {
            displayBedAllocation();
        } 
        else if (strcmp(command, "add") == 0) {
            scanf("%d", &param);
            allocateBed(param);
        } 
        else if (strcmp(command, "free") == 0) {
            scanf("%d", &param);
            freeBed(param);
        } 
        else if (strcmp(command, "exit") == 0) {
            break;
        } 
        else {
            printf("[ERROR] Unknown command.\n");
        }
    }
    return 0;
}
