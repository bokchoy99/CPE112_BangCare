#include <stdlib.h>
#include "linked_list.h"

/* Allocates a zeroed head pointer (stored as void* in SystemContext). */
void* createLinkedList(void) {
    Patient** head = (Patient**)malloc(sizeof(Patient*));
    if (head) *head = NULL;
    return (void*)head;
}

/* Prepends p to the list rooted at *head — O(1). */
void linkedListInsert(Patient** head, Patient* p) {
    if (!p) return;
    p->next = *head;
    *head   = p;
}