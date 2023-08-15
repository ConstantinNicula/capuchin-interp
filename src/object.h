#ifndef _OBJECT_H_
#define _OBJECT_H_


#include <stdint.h>
#include <stdbool.h>
#include "ast.h"
#include "environment.h"

typedef enum ObjectType{
    OBJECT_INTEGER,
    OBJECT_BOOLEAN, 
    OBJECT_NULL,
    OBJECT_RETURN_VALUE,
    OBJECT_ERROR,
    OBJECT_FUNCTION,
    _OBJECT_TYPE_CNT
} ObjectType_t;

const char* objectTypeToString(ObjectType_t type);

#define OBJECT_BASE_ATTRS \
    ObjectType_t type;

/************************************ 
 *     GENERIC OBJECT TYPE          *
 ************************************/

typedef struct Object {
    OBJECT_BASE_ATTRS;
} Object_t;

typedef char* (*ObjectInspectFn_t) (void*);
typedef void (*ObjectCleanupFn_t) (void**);
typedef void* (*ObjectCopyFn_t) (void*);

void cleanupObject(Object_t** obj);
Object_t* copyObject(Object_t* obj);

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
Integer_t* copyInteger(Integer_t* obj);

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
Boolean_t* copyBoolean(Boolean_t* obj);

char* booleanInspect(Boolean_t* obj);


/************************************ 
 *        NULL OBJECT TYPE          *
 ************************************/

typedef struct Null {
    OBJECT_BASE_ATTRS;
}Null_t;

Null_t* createNull();
void cleanupNull(Null_t** obj);
Null_t* copyNull(Null_t* obj);

char* nulllInspect(Null_t* obj);


/************************************ 
 *      RETURN OBJECT TYPE          *
 ************************************/

typedef struct ReturnValue {
    OBJECT_BASE_ATTRS;
    Object_t* value;
}ReturnValue_t;

ReturnValue_t* createReturnValue(Object_t*);
void cleanupReturnValue(ReturnValue_t** obj);
ReturnValue_t* copyReturnValue(ReturnValue_t* obj);

char* returnValueInspect(ReturnValue_t* obj);


/************************************ 
 *      ERROR OBJECT TYPE          *
 ************************************/

typedef struct Error {
    OBJECT_BASE_ATTRS;
    char* message;
}Error_t;

Error_t* createError(char* message);
void cleanupError(Error_t** obj);
Error_t* copyError(Error_t* obj);

char* errorInspect(Error_t* obj);

/************************************ 
 *    FUNCTION OBJECT TYPE          *
 ************************************/
typedef struct Environment Environment_t;

typedef struct Function {
    OBJECT_BASE_ATTRS;
    Vector_t* parameters;
    BlockStatement_t* body;
    Environment_t* environment;
} Function_t;

Function_t* createFunction(Vector_t* params, BlockStatement_t* body, Environment_t* env);
void cleanupFunction(Function_t** obj);
Function_t* copyFunction(Function_t* obj);

char* functionInspect(Function_t* obj);
uint32_t functionGetParameterCount(Function_t* obj);
Identifier_t** functionGetParameters(Function_t* obj);

#endif 