#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "../core/system_context.h" 


void systemAddPatient(const char* name, int severity, int pain);
void systemTick(int n);
void updateSimulatedTime();

#endif