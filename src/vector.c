#include <malloc.h>
#include <string.h>
#include "vector.h"
#include "utils.h"
#include "refcount.h"

Vector_t* createVector(size_t elemSize) {
    Vector_t* vec = createRefCountPtr(sizeof(Vector_t));
    vec->elemSize = elemSize;
    
    vec->cap = 0u;
    vec->cnt = 0u;
    vec->buf = NULL;
    return vec;
}

Vector_t* copyVector(Vector_t* vec) {
    refCountPtrInc(vec);
    return vec;
}



void cleanupVectorContents(Vector_t*vec, VectorElementCleanupFn_t cleanupFn) {
    if (!vec || !cleanupFn) 
        return;
        
    char* eptr = vec->buf;
    for (uint32_t i = 0; i < vec->cnt; i++)
    {
        cleanupFn((void**) eptr);
        eptr += vec->elemSize;
    }
    memset(vec->buf, 0, vec->cnt * vec->elemSize);
    vec->cnt = 0;
}

void cleanupVector(Vector_t** vec, VectorElementCleanupFn_t cleanupFn) {
    if (!*vec || !cleanupFn || refCountPtrDec(*vec) != 0)
        return;
    
    cleanupVectorContents(*vec, cleanupFn);
    free((*vec)->buf);
    (*vec)->buf = NULL;
    
    cleanupRefCountedPtr(*vec);
    *vec = NULL;
}


void vectorAppend(Vector_t* vec, void* elem) {
    if (vec->cnt >= vec->cap)
    {
        if (vec->buf != NULL) {
            // no more space, reallocate
            vec->cap = 2u * vec->cap;
            vec->buf = (void*)realloc(vec->buf, vec->elemSize * vec->cap);
            if (!vec->buf) HANDLE_OOM();
        }
        else 
        {
            // initial allocation
            vec->cap = 1;
            vec->buf = (void*)malloc(vec->elemSize * vec->cap);
            if (!vec->buf) HANDLE_OOM();
        }
    }
    
    uint8_t *vp = ((uint8_t*)vec->buf) + (vec->cnt * vec->elemSize);
    memmove((void*)vp, elem, vec->elemSize);
    vec->cnt++;
}

uint32_t vectorGetCount(Vector_t* vec) {   
    return vec->cnt;
}

void* vectorGetBuffer(Vector_t* vec) {
    return vec->buf;
}