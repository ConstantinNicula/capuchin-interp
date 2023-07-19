#include <malloc.h>
#include <string.h>
#include "vector.h"


Vector_t* createVector(size_t elemSize) {
    Vector_t* vec = (Vector_t*) malloc(sizeof(Vector_t));
    if (vec == NULL)
        return NULL;
    vec->elemSize = elemSize;
    
    vec->cap = 0u;
    vec->cnt = 0u;
    vec->buf = NULL;
    return vec;
}


void cleanupVector(Vector_t** vec) {
    if (*vec == NULL)
        return;
    free((*vec)->buf);
    (*vec)->cnt = 0u;
    (*vec)->cap = 0u;
    (*vec)->buf = NULL;
    free(*vec);
    *vec = NULL;

}

void vectorAppend(Vector_t* vec, void* elem) {
    if (vec->cnt >= vec->cap)
    {
        if (vec->buf != NULL) {
            // no more space, reallocate
            vec->cap = 2u * vec->cap;
            vec->buf = (void*)realloc(vec->buf, vec->elemSize * vec->cap);
            if (vec->buf == NULL) {
                return; // TO DO: OOM error 
            }
        }
        else 
        {
            // initial allocation
            vec->cap = 1;
            vec->buf = (void*)malloc(vec->elemSize * vec->cap);
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