#include <stdlib.h>
#include "linked_list.h"

void* createLinkedList() {
    // ในที่นี้เราจะใช้หัว List เป็น NULL (Pointer to Patient pointer)
    Patient** head = (Patient**)malloc(sizeof(Patient*));
    if (head) *head = NULL;
    return (void*)head;
}

void listInsert(void* headPtr, Patient* p) {
    if (!headPtr || !p) return;
    Patient** head = (Patient**)headPtr;

    // แทรกที่หัว List (O(1))
    p->next = *head;
    if (*head) (*head)->prev = p;
    *head = p;
    p->prev = NULL;
}