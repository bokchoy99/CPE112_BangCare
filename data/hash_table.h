#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../patient/patient.h"

// โครงสร้างของ Hash Node สำหรับทำ Chaining (กรณี Hash Collision)
typedef struct HashNode {
    Patient* patient;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[101]; // ขนาดตาม HASH_TABLE_SIZE ใน config.h
} HashTable;

HashTable* createHashTable();
int hashFunction(const char* id);
void hashTableInsert(HashTable* ht, Patient* p);
Patient* hashTableLookup(HashTable* ht, const char* id);

#endif