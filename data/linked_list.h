#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "../patient/patient.h"

/* Reuses Patient->next as the link pointer; no separate node wrapper needed. */
void* createLinkedList(void);
void  linkedListInsert(Patient** head, Patient* p);

#endif