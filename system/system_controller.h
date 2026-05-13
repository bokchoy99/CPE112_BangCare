#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

// ⭐ ประกาศ function ให้ main.c รู้จัก
void systemAddPatient(const char* name, int severity, int pain);
void systemTick(int n);

#endif