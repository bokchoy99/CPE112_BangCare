#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

// ฟังก์ชันสร้าง Table [cite: 9, 40]
HashTable* createHashTable() {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) return NULL;
    for (int i = 0; i < 101; i++) {
        ht->buckets[i] = NULL;
    }
    return ht;
}

// hashFunction สำหรับ ID "BC-XXX"
int hashFunction(const char* id) {
    unsigned long hash = 5381;
    int c;
    while ((c = *id++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % 101;
}

// การ Insert แบบ Chaining (เพิ่มที่หัว List จะเร็วที่สุด O(1)) [cite: 42, 80]
void hashTableInsert(HashTable* ht, Patient* p) {
    if (!ht || !p) return;
    
    int index = hashFunction(p->id);
    
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if (!newNode) return;
    
    newNode->patient = p;
    // นำโหนดใหม่ไปชี้ที่หัวเดิม แล้วเอาหัวใหม่ชี้ที่โหนดนี้
    newNode->next = ht->buckets[index];
    ht->buckets[index] = newNode;
}

// การ Lookup (ค้นหาตามโซ่ Chaining)
Patient* hashTableLookup(HashTable* ht, const char* id) {
    if (!ht || !id) return NULL;
    
    int index = hashFunction(id);
    HashNode* current = ht->buckets[index];
    
    while (current != NULL) {
        if (strcmp(current->patient->id, id) == 0) {
            return current->patient; // เจอคนไข้
        }
        current = current->next;
    }
    return NULL; // ไม่พบในระบบ 
}