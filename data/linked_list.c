#include <stdlib.h>
#include "linked_list.h"

void* createLinkedList() {
    // ในที่นี้เราจะใช้หัว List เป็น NULL (Pointer to Patient pointer)
    Patient** head = (Patient**)malloc(sizeof(Patient*));
    if (head) *head = NULL;
    return (void*)head;
}

void linkedListInsert(Patient** head, Patient* p) { // เช็คการสะกดชื่อให้ตรงเป๊ะ
    if (p == NULL) return;
    p->next = *head;
    *head = p;
}