// ui/command.h
#ifndef COMMAND_H
#define COMMAND_H

#include "../patient/patient.h"
#include "../bed/bed_manager.h"

// ส่วน UI Display
void displayHashID(Patient* p);
void displaySeverityList(int sev, Patient* list);
void displayBedDetail(BedNode* bed);
void displayAgingAnalysis(Patient* list);

// ส่วน System Logic (เรียกใช้ใน handlePeek)
void systemPeekSeverity(int sev);
void systemPeekBed(int bedId);
void systemPeekAging();
void systemPeekHash(const char* id);

bool processCommand(char* input);

#endif