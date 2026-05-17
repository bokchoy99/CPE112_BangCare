#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include "../patient/patient.h"
#include "../bed/bed_manager.h"

/* -- Display functions (called by peek handlers) -------------- */
void displayHashID(Patient* p);
void displaySeverityList(int sev, Patient* list);
void displayBedDetail(BedNode* bed);
void displayAgingAnalysis(Patient* list);
void displayHelp(void);
void displayStats(void);
void displayAbout(void);

<<<<<<< Updated upstream
// ส่วน System Logic (เรียกใช้ใน handlePeek)
void systemPeekSeverity(int sev);
void systemPeekBed(int bedID);
void systemPeekAging();
void systemPeekHash(const char* id);

=======
/* -- Command entry point -------------------------------------- */
>>>>>>> Stashed changes
bool processCommand(char* input);

#endif