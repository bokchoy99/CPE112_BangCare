#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../patient/patient.h"

/* -- Structures ----------------------------------------------- */

/* Single node in a collision chain. */
typedef struct HashNode {
    Patient*         patient;
    struct HashNode* next;
} HashNode;

/* Fixed-size bucket array; collisions resolved by chaining. */
typedef struct {
    HashNode* buckets[101]; /* Size follows HASH_TABLE_SIZE in config.h */
} HashTable;

/* -- Prototypes ----------------------------------------------- */
HashTable* createHashTable(void);
int        hashFunction(const char* id);
void       hashTableInsert(HashTable* ht, Patient* p);
Patient*   hashTableLookup(HashTable* ht, const char* id);

#endif