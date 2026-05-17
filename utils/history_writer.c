#include "history_writer.h"
#include "../core/config.h"
#include <stdio.h>

void writePatientHistory(const Patient* p, const char* bedType, int waitTicks) {
    FILE* f = fopen(HISTORY_FILE, "a");
    if (!f) return;
    fprintf(f, "%s,%s,%d,%d,%s,%d,%d,%d,%d,%s\n",
        p->id, p->name, p->severity, p->pain,
        p->arrivalTime, p->treatmentStartTime,
        p->arrivalTick, waitTicks,
        waitTicks * TICK_UNIT_MINUTES, bedType);
    fclose(f);
}