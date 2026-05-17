#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command.h"
#include "../system/system_controller.h"
#include "../core/system_context.h"
#include "../bed/bed_manager.h"
#include "../data/hash_table.h"

<<<<<<< Updated upstream
bool processCommand(char* input) {
    char cmd[20], name[100];
    int severity, pain;
    int numArgs = sscanf(input, "%s %s %d %d", cmd, name, &severity, &pain);
}
/* HELP */
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
    printf("cmd          : Show all cmd\n");
    printf("exit         : Close program\n");
=======
/* ============================================================
 * SECTION 1 — Display Functions
 * ============================================================ */

static const char* stateToString(PatientState s) {
    switch (s) {
        case WAITING:       return "WAITING";
        case IN_QUEUE:      return "IN_QUEUE";
        case ALLOCATED:     return "ALLOCATED";
        case ALLOCATED_OPD: return "ALLOCATED_OPD";
        case TREATING:      return "TREATING";
        case DONE:          return "DONE";
        default:            return "UNKNOWN";
    }
}

void displayHelp(void) {
    printf("\033[H\033[J");
    printf("\n=====================================\n");
    printf(" [ COMMANDS ]\n");
    printf("-------------------------------------\n");
    printf(" add <name> <severity> <pain>  : Register a new patient\n");
    printf(" tick <n>                      : Advance time by n ticks (n x 2 min)\n");
    printf(" tick rec <n>                  : Advance n recovery units (n x 10 min)\n");
    printf(" peek sev <num>                : List patients at a severity level\n");
    printf(" peek bed <bed_id>             : Show details for a specific bed\n");
    printf(" peek hash <id>                : Look up a patient by ID\n");
    printf(" peek aging                    : Show patients near severity promotion\n");
    printf(" stat                          : Show system statistics\n");
    printf(" about                         : Show project info\n");
    printf(" cmd                           : Show this command list\n");
    printf(" exit                          : Quit\n");
>>>>>>> Stashed changes
    printf("=====================================\n");
}

void displayStats(void) {
    printf("\033[H\033[J");
    printf("\n=====================================\n");
    printf(" [ STATISTICS ]\n");
    printf("-------------------------------------\n");

    int totalBeds   = MAX_ER_BEDS + MAX_OPD_BEDS;
    int occupied    = gSystem.beds ? gSystem.beds->occupiedBeds : 0;
    int utilPct     = totalBeds > 0 ? (occupied * 100 / totalBeds) : 0;

    /* S5 immediate = got ER bed without waiting / total S5 registered.
     * We approximate total S5 by checking agingList + discharged tracking. */
    float avgWait = (gSystem.totalDischarged > 0)
        ? ((float)gSystem.totalWaitTicks * TICK_UNIT_MINUTES / gSystem.totalDischarged)
        : 0.0f;
    float maxWait = (float)gSystem.maxWaitTicks * TICK_UNIT_MINUTES;

    float throughput = (gSystem.tickCount > 0)
        ? ((float)gSystem.totalDischarged / gSystem.tickCount)
        : 0.0f;

    printf(" Resource Utilization : %d %%  (%d/%d beds)\n", utilPct, occupied, totalBeds);
    printf(" Triage Success Rate  : %d S5 got ER immediately\n", gSystem.totalS5Immediate);
    printf(" Average Wait Time    : %.1f min\n", avgWait);
    printf(" Max Wait Time        : %.1f min\n", maxWait);
    printf(" Aging Impact         : %d promotion(s)\n", gSystem.totalAgingBumps);
    printf(" Throughput           : %d discharged  (%.2f/tick)\n",
           gSystem.totalDischarged, throughput);
    printf(" Total Registered     : %d\n", gSystem.totalRegistered);
    printf("=====================================\n");
}

void displayAbout(void) {
    printf("\033[H\033[J");
    printf("\n===========================================================\n");
    printf(" [ PROJECT INFORMATION ]\n");
    printf(" Name        : BangCare (CPE112_BangCare)\n");
    printf(" Description : Emergency Room triage management system\n");
    printf("               optimising bed resources and wait times.\n");
    printf(" Version     : 2.5 (5/17/2026)\n");
    printf(" Developers  : 68070503412  Karnsinee Nikrotamethanee\n");
    printf("               68070503422  Nichapa Piromying\n");
    printf("               68070503475  Patchara Nimittakunchai\n");
    printf("               68070503478  Patteera Pattaraporntaweewat\n");
    printf("===========================================================\n");
}

/* ============================================================
 * SECTION 2 — Peek Display Functions
 * ============================================================ */

/* Shows all patients in the aging list whose severity matches sev. */
void displaySeverityList(int sev, Patient* list) {
    printf("\n=====================================\n");
    printf(" [ PATIENTS - SEVERITY S%d ]\n", sev);
    printf("-------------------------------------\n");

    int count = 0;
    Patient* curr = list;
    while (curr != NULL) {
        if (curr->severity == sev) {
            printf(" [%d] %-8s | %-15s | Pain: %2d | State: %s\n",
                   ++count, curr->id, curr->name, curr->pain, stateToString(curr->state));
        }
<<<<<<< Updated upstream
    } 
    else if (strcmp(command, "tick") == 0) {
        char subCommand[20];
        int units = 0;

<<<<<<<<< Temporary merge branch 1
        // tick rec <n>: เดินเวลา recovery
        if (sscanf(input, "%s %s %d", command, subCommand, &units) == 3) {
            if (strcmp(subCommand, "rec") == 0) {
                systemTick(units * 2); 
                printf("[SYSTEM] Advanced %d units of recovery time (%d minutes).\n", units, units * 10);
=========
//Peek BedID
=======
        curr = curr->next;
    }

    if (count == 0) printf(" No patients at this severity level.\n");
    printf("=====================================\n");
}

/* Shows occupancy details for a single bed node. */
>>>>>>> Stashed changes
void displayBedDetail(BedNode* bed) {
    if (!bed) { printf("[ERROR] Bed not found.\n"); return; }

    printf("\n=====================================\n");
    printf(" [ BED #%d ]\n", bed->idBed);
    printf(" Type   : %s\n", bed->type);
    if (bed->isOccupied && bed->patient != NULL) {
        printf(" Status : OCCUPIED\n");
        printf(" ID     : %s\n",   bed->patient->id);
        printf(" Name   : %s\n",   bed->patient->name);
        printf(" Sev    : S%d\n",  bed->patient->severity);
        printf(" Time   : %d ticks remaining\n", bed->patient->treatmentRemaining);
    } else {
        printf(" Status : VACANT\n");
    }
    printf("=====================================\n");
}

/* Shows patients within 2 ticks of their next severity promotion. */
void displayAgingAnalysis(Patient* list) {
    printf("\n=====================================\n");
    printf(" [ AGING WATCHLIST ] (within 2 ticks of promotion)\n");
    printf("-------------------------------------\n");

    int count = 0;
    Patient* curr = list;
    while (curr != NULL) {
        /* Monitor same states as runAging: WAITING, IN_QUEUE, ALLOCATED_OPD */
        bool monitored = (curr->state == WAITING      ||
                          curr->state == IN_QUEUE     ||
                          curr->state == ALLOCATED_OPD);
        if (monitored) {
            int waitTicks   = gSystem.tickCount - curr->arrivalTick;
            int remainder   = waitTicks % AGING_THRESHOLD_TICKS;
            int ticksToNext = (remainder == 0 && waitTicks > 0)
                              ? AGING_THRESHOLD_TICKS
                              : AGING_THRESHOLD_TICKS - remainder;

            if (ticksToNext <= 2) {
                int nextSev = curr->severity < 5 ? curr->severity + 1 : 5;
                const char* label = (curr->severity == 5) ? "[CRITICAL RISK]" : "";
                printf(" %-8s | %-12s | S%d -> S%d in %d tick(s) %s\n",
                       curr->id, curr->name, curr->severity, nextSev, ticksToNext, label);
                count++;
            }
        }
        curr = curr->next;
    }

    if (count == 0) printf(" No patients near threshold.\n");
    printf("=====================================\n");
}

/* Shows a patient record fetched from the hash table. */
void displayHashID(Patient* p) {
    if (!p) { printf("[ERROR] Patient not found in registry.\n"); return; }

    const char* stateLabel;
    switch (p->state) {
        case ALLOCATED:
        case ALLOCATED_OPD:
        case TREATING:   stateLabel = "IN BED";  break;
        case DONE:       stateLabel = "DONE";    break;
        default:         stateLabel = "WAITING"; break;
    }

    printf("\n=====================================\n");
    printf(" [ PATIENT RECORD ]\n");
    printf(" ID       : %s\n",   p->id);
    printf(" Name     : %s\n",   p->name);
    printf(" Severity : S%d\n",  p->severity);
    printf(" Pain     : %d\n",p->pain);
    printf(" Arrived  : %s \n", p->arrivalTime);
    printf(" Status   : %s\n",   stateLabel);
    printf("=====================================\n");
}

/* ============================================================
 * SECTION 3 — Command Handlers
 * ============================================================ */

static void handleAdd(char* input) {
    char cmd[20], name[100];
    int  severity, pain;

    if (sscanf(input, "%s %s %d %d", cmd, name, &severity, &pain) == 4) {
        if (severity < 1 || severity > 5) {
            printf("[ERROR] Invalid Input: Severity must be 1 (Low) to 5 (Critical).\n");
            return;
        }
        if (pain < 1 || pain > 10) {
            printf("[ERROR] Invalid Input: Pain must be 1 to 10.\n");
            return;
        }
        systemAddPatient(name, severity, pain);
    } else {
        printf("[ERROR] Usage: add <name> <severity> <pain>\n");
    }
}

static void handleTick(char* input) {
    char cmd[20], sub[20];
    int  units = 0;

    if (sscanf(input, "%s %s %d", cmd, sub, &units) == 3 &&
        strcmp(sub, "rec") == 0) {
        printf("\n--- Advancing %d recovery unit(s) (%d min) ---\n", units, units * 10);
        systemTick(units * 5);
    } else if (sscanf(input, "%s %d", cmd, &units) == 2) {
        printf("\n--- Advancing %d tick(s) (%d min) ---\n", units, units * TICK_UNIT_MINUTES);
        systemTick(units);
    } else {
        printf("[ERROR] Usage: tick <n>  or  tick rec <n>\n");
    }
}

static void handlePeek(char* input) {
    char cmd[20], sub[20], extra[100] = {0};
    int  numArgs = sscanf(input, "%s %s %s", cmd, sub, extra);

    if (numArgs < 2) {
        printf("[ERROR] Usage: peek <sev|bed|aging|hash>\n");
        return;
    }
<<<<<<< Updated upstream
    else if (strcmp(cmd, "stat") == 0) {
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
    else if (strcmp(cmd, "free") == 0) {
        if (numArgs >= 2) {
            int bedID = atoi(name);
            freeBed(bedID);
            systemAutoAllocate(); // พอเตียงว่าง ดึงคนในคิวมาใส่ทันที
=======

    if (strcmp(sub, "aging") == 0) {
        systemPeekAging();
    } else if (strcmp(sub, "sev") == 0 && numArgs >= 3) {
        int sev = atoi(extra);
        if (sev < 1 || sev > 5) {
            printf("[ERROR] Severity must be 1–5.\n");
        } else {
            displaySeverityList(sev, (Patient*)gSystem.agingList);
>>>>>>> Stashed changes
        }
    } else if (strcmp(sub, "bed") == 0 && numArgs >= 3) {
        displayBedDetail(getBed(atoi(extra)));
    } else if (strcmp(sub, "hash") == 0 && numArgs >= 3) {
        systemPeekHash(extra);
    } else {
        printf("[ERROR] Unknown peek subcommand: '%s'\n", sub);
    }
<<<<<<< Updated upstream
    printf("\n[ERROR] Bed ID %d not found in system.\n", bedID);
=======
>>>>>>> Stashed changes
}

/* ============================================================
 * SECTION 4 — Main Command Dispatcher
 * ============================================================ */

/*
 * processCommand
 * Parses the first token of input and dispatches to the appropriate
 * handler. Returns false only when the user types "exit".
 */
bool processCommand(char* input) {
<<<<<<< Updated upstream
    char cmd[20], arg1[100];
    int arg2, arg3;
    int numArgs = sscanf(input, "%s %s %d %d", cmd, arg1, &arg2, &arg3);

    // 1. Exit Check
    if (strcmp(cmd, "exit") == 0) return false;

    // 2. Command Dispatcher ตัวเช็คว่ามีไรต่อท้ายcmdไหม
    if (strcmp(cmd, "add") == 0) {
        if (numArgs == 4) systemAddPatient(arg1, arg2, arg3);
        else printf("\n[ERROR] Usage: add <name> <severity> <pain>\n");
    } 
    else if (strcmp(cmd, "tick") == 0) {
        handleTick(input);
    } 
    else if (strcmp(cmd, "peek") == 0) {
        handlePeek(input);
    }
    else if (strcmp(cmd, "stat") == 0) {
        displayStats();
    }
    else if (strcmp(cmd, "about") == 0) {
        displayAbout();
    }
    else if (strcmp(cmd, "cmd") == 0) {
        displayHelp();
    }
    else if (strcmp(cmd, "free") == 0) {
        if (numArgs >= 2) freeBed(atoi(arg1));
        else printf("[ERROR] Usage: free <bed_id>\n");
    }
    else if (strcmp(cmd, "fillbeds") == 0) {
        fillAllBeds(); // จำลองเตียงเต็มสำหรับ demo
    }
    else if (strlen(cmd) > 0) {
        printf("\n[ERROR] Unknown Command: '%s'\n", cmd);
=======
    char command[20] = {0};
    sscanf(input, "%s", command);

    if (strcmp(command, "exit")     == 0) return false;

    if      (strcmp(command, "add")      == 0) handleAdd(input);
    else if (strcmp(command, "tick")     == 0) handleTick(input);
    else if (strcmp(command, "peek")     == 0) handlePeek(input);
    else if (strcmp(command, "stat")     == 0) displayStats();
    else if (strcmp(command, "about")    == 0) displayAbout();
    else if (strcmp(command, "cmd")      == 0) displayHelp();
    else if (strcmp(command, "free")     == 0) {
        char cmd[20]; int id;
        if (sscanf(input, "%s %d", cmd, &id) == 2) {
            if (freeBed(id)) systemAutoAllocate();
        } else {
            printf("[ERROR] Usage: free <bed_id>\n");
        }
    }
    else if (strcmp(command, "fillbeds") == 0) fillAllBeds();
    else if (strlen(command) > 0) {
        printf("[ERROR] Unknown Command: '%s'. Type 'cmd' for valid commands.\n", command);
>>>>>>> Stashed changes
    }

    return true;
}