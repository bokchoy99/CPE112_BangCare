#ifndef HISTORY_WRITER_H
#define HISTORY_WRITER_H
#include "../patient/patient.h"

void writePatientHistory(const Patient* p, const char* bedType, int waitTicks);
#endif