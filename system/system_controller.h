#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

/* -- Public system-layer API ---------------------------------- */
/* All operations on patients and beds must go through these functions.
 * Direct calls to heapInsert(), allocateBed(), etc. are prohibited.  */

void systemAddPatient(const char* name, int severity, int pain);
void systemTick(int n);
void systemAutoAllocate(void);
void systemPeekHash(const char* id);
void systemPeekAging(void);

#endif