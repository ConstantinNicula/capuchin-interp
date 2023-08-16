#include <malloc.h>

#include "environment.h"
#include "object.h"
#include "utils.h"
#include "gc.h"


typedef struct GCHandle {
    void* first;
    uint32_t objCount;
}GCHandle_t;

static GCHandle_t gcHandle = {.first = NULL, .objCount = 0 };

// Additional header data used for GC 
// *--------*---------
// | header |  data....
// *--------*----------
//          |-> return ptr 

typedef struct GCDataHeader {
    GCDataType_t type;
    uint8_t mark;
    void* next;
} GCDataHeader_t;

// Mark bits significance 
// *------------*-----+-----+
// | 7-3 Unused | ERB | IRB |
// *------------*-----+-----+
// ERB - external ref bit
// IRB - internal ref bit

#define MARK_UNUSED 0x00
#define INTERNAL_REF_BIT 0x01 
#define EXTERNAL_REF_BIT 0x02

/* Create global table of destructors */
typedef void (*GCCleanupFn_t) (void**);
typedef void (*GCMarkFn_t) (void*);

static GCCleanupFn_t gcCleanupFns[_GC_DATA_TYPE_CNT] = {
    [GC_DATA_OBJECT]=(GCCleanupFn_t)cleanupObject,
    [GC_DATA_ENVIRONENT]=(GCCleanupFn_t)cleanupEnvironment
};

static GCMarkFn_t gcMarkFns[_GC_DATA_TYPE_CNT] = {
    [GC_DATA_OBJECT]=(GCMarkFn_t)gcMarkObject,
    [GC_DATA_ENVIRONENT]=(GCMarkFn_t)gcMarkEnvironment
};

static void* createFatPtr(size_t size, GCDataType_t type, void* next);
static GCDataHeader_t* getHeader(void *ptr);
static void *getPtr(GCDataHeader_t* header);
static inline void setBit(GCDataHeader_t* header, uint8_t bitmask);
static inline void clearBit(GCDataHeader_t* header, uint8_t bitmask);
static inline bool isBitSet(GCDataHeader_t* header, uint8_t bitmask);



void* gcMalloc(size_t size, GCDataType_t type) {
    void* ptr = createFatPtr(size, type, gcHandle.first);

    GCDataHeader_t* header = getHeader(ptr);
    header->next = gcHandle.first;

    gcHandle.first = ptr;
    gcHandle.objCount++;
    return ptr;
}

void gcMarkUsed(void* ptr) {
    if (!ptr) return;
    GCDataHeader_t* header = getHeader(ptr);
    setBit(header, INTERNAL_REF_BIT);
}


void* gcGetExtRef(void* ptr) {
    if (!ptr) return NULL;
    GCDataHeader_t* header = getHeader(ptr);
    setBit(header, EXTERNAL_REF_BIT);
    return ptr;
}

void gcFreeExtRef(void* ptr) {
    if (!ptr) return;
    GCDataHeader_t* header = getHeader(ptr);
    if (!isBitSet(header, EXTERNAL_REF_BIT)) {
        perror("GC attempted free on non invalid external ref (potential double free)");
        exit(1);
    }
    clearBit(header, EXTERNAL_REF_BIT);
}

static void* createFatPtr(size_t size, GCDataType_t type, void* next) {
    GCDataHeader_t* ptr = malloc(sizeof(GCDataHeader_t) + size);
    if (!ptr) HANDLE_OOM();
    ptr->type = type;
    ptr->mark = 0;
    ptr->next = next;
    return (void*)((char*)ptr + sizeof(GCDataHeader_t));
}

static GCDataHeader_t* getHeader(void *ptr) {
    return (GCDataHeader_t*)((char*)ptr - sizeof(GCDataHeader_t));
}

static void *getPtr(GCDataHeader_t* header) {
    return (void*)((char*)header + sizeof(GCDataHeader_t));
}


static void gcFreeElem(void* ptr) {
    if (!ptr) return;
    GCDataHeader_t* header = getHeader(ptr);
    if ( header->type >= 0 && header->type < _GC_DATA_TYPE_CNT ){
        GCCleanupFn_t cleanupFunc = gcCleanupFns[header->type];
        if (cleanupFunc) 
            cleanupFunc(ptr);
    }
}

static void gcRecurseMark(void* ptr) {
    if (!ptr) return;
    GCDataHeader_t* header = getHeader(ptr);
    if ( header->type >= 0 && header->type < _GC_DATA_TYPE_CNT ){
        GCMarkFn_t markFunc = gcMarkFns[header->type];
        if (markFunc) 
            markFunc(ptr);
    }
}

static void gcMark() {
    void* ptr = gcHandle.first;
    while (ptr) {
        GCDataHeader_t* header = getHeader(ptr);
        if (isBitSet(header, EXTERNAL_REF_BIT) && !isBitSet(header, INTERNAL_REF_BIT)) {
            setBit(header, INTERNAL_REF_BIT);
            gcRecurseMark(ptr);
        }
        ptr = header->next;
    }
}

static void gcSweep() {
    if (!gcHandle.first) return;

    GCDataHeader_t sentinel = {.next = gcHandle.first};
    GCDataHeader_t* prev = &sentinel;
    GCDataHeader_t* curr = getHeader(gcHandle.first);
    
    while (!isBitSet(curr, INTERNAL_REF_BIT)) {
        prev->next = curr->next;
        gcFreeElem(getPtr(curr));
        curr = getHeader(prev->next);
    }

    gcHandle.first = sentinel.next;
}


static inline void setBit(GCDataHeader_t* header, uint8_t bitmask) {
    header->mark |= bitmask;
}
static inline void clearBit(GCDataHeader_t* header, uint8_t bitmask) {
    header->mark &= (~bitmask);
}
static inline bool isBitSet(GCDataHeader_t* header, uint8_t bitmask){
    return (header->mark & bitmask) != 0;
}
