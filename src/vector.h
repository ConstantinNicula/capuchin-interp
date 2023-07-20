#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdint.h>
#include <stddef.h>


typedef struct Vector {
    uint32_t cap;
    uint32_t cnt;
    size_t elemSize;
    void* buf;
} Vector_t;


Vector_t* createVector(size_t elemSize);
void cleanupVector(Vector_t** vec); 

void vectorAppend(Vector_t* vec, void* elem);
uint32_t vectorGetCount(Vector_t* vec);
void* vectorGetBuffer(Vector_t* vec);
#endif 