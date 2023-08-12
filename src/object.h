#ifndef _OBJECT_H_
#define _OBJECT_H_


#include <stdint.h>
#include <stdbool.h>

typedef enum ObjectType{
    OBJECT_INTEGER,
    OBJECT_BOOLEAN, 
    OBJECT_NULL,
    _OBJECT_TYPE_CNT
} ObjectType_t;


#define OBJECT_BASE_ATTRS \
    ObjectType_t type

/************************************ 
 *     GENERIC OBJECT TYPE          *
 ************************************/

typedef struct Object {
    OBJECT_BASE_ATTRS;
} Object_t;

typedef char* (*ObjectInspectFn_t) (void*);
typedef void (*ObjectCleanupFn_t) (void**);

void cleanupObject(Object_t** obj);

char* objectInspect(Object_t* obj);
ObjectType_t objectGetType(Object_t* obj);

/************************************ 
 *     INTEGER OBJECT TYPE          *
 ************************************/

typedef struct Integer {
    OBJECT_BASE_ATTRS;
    int64_t value;
}Integer_t;

Integer_t* createInteger(int64_t value);
void cleanupInteger(Integer_t** obj);

char* integerInspect(Integer_t* obj);


/************************************ 
 *     BOOLEAN OBJECT TYPE          *
 ************************************/

typedef struct Boolean {
    OBJECT_BASE_ATTRS;
    bool value;
}Boolean_t;

Boolean_t* createBoolean(bool value);
void cleanupBoolean(Boolean_t** obj);

char* booleanInspect(Boolean_t* obj);


/************************************ 
 *        NULL OBJECT TYPE          *
 ************************************/

typedef struct Null {
    OBJECT_BASE_ATTRS;
}Null_t;

Null_t* createNull();
void cleanupNull(Null_t** obj);

char* nulllInspect(Null_t* obj);



#endif 