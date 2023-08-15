#ifndef _HMAP_H_
#define _HMAP_H_
#include <stdlib.h>
#include <stdint.h>

typedef struct HashMapEntry {
    char* key; // local ownership 
    void* value; // local ownership 
    struct HashMapEntry* next; 
} HashMapEntry_t;

typedef struct HashMap {
    HashMapEntry_t** buckets;
    uint32_t numBuckets;
    uint32_t itemCnt;
} HashMap_t;

typedef void (*HashMapElementCleanupFn_t) (void** elem);

HashMap_t* createHashMap();
void cleanupHashMapElements(HashMap_t* map, HashMapElementCleanupFn_t cleanupFn);
void cleanupHashMap(HashMap_t** map);

void hashMapInsert(HashMap_t* map, const char* key , void* value);
void* hashMapGet(HashMap_t* map, const char* key);

#endif 