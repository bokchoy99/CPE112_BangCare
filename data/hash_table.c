#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

/*
 * createHashTable
 * Allocates and zero-initialises the hash table.
 * Returns NULL on allocation failure.
 */
HashTable* createHashTable(void) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) return NULL;
    for (int i = 0; i < 101; i++) {
        ht->buckets[i] = NULL;
    }
    return ht;
}

/*
 * hashFunction
 * Maps a patient ID string (e.g. "BC-042") to a bucket index [0, 100]
 * using the djb2 algorithm (seed 5381, times-33 step).
 */
int hashFunction(const char* id) {
    unsigned long hash = 5381;
    int c;
    while ((c = *id++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (int)(hash % 101);
}

/*
 * hashTableInsert
 * Prepends a new HashNode to the bucket chain — O(1) insertion.
 * The hash table is the sole owner of patient data; do not free
 * a patient while it is still in the table.
 */
void hashTableInsert(HashTable* ht, Patient* p) {
    if (!ht || !p) return;

    int index = hashFunction(p->id);

    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    if (!node) return;

    node->patient    = p;
    node->next       = ht->buckets[index];
    ht->buckets[index] = node;
}

/*
 * hashTableLookup
 * Walks the chain at the computed bucket and compares IDs.
 * Returns the matching Patient pointer, or NULL if not found.
 */
Patient* hashTableLookup(HashTable* ht, const char* id) {
    if (!ht || !id) return NULL;

    int index = hashFunction(id);
    HashNode* curr = ht->buckets[index];

    while (curr != NULL) {
        if (strcmp(curr->patient->id, id) == 0) {
            return curr->patient;
        }
        curr = curr->next;
    }
    return NULL;
}