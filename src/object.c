#include "object.h"

#include <stdlib.h>
#include "utils.h"
/************************************ 
 *     GENERIC OBJECT TYPE          *
 ************************************/


static ObjectCleanupFn_t objectCleanupFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectCleanupFn_t)cleanupInteger,
    [OBJECT_BOOLEAN]=(ObjectCleanupFn_t)cleanupBoolean,
    [OBJECT_NULL]=(ObjectCleanupFn_t)cleanupNull,
    [OBJECT_RETURN_VALUE]=(ObjectCleanupFn_t)cleanupReturnValue
};

static ObjectInspectFn_t objectInsepctFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectInspectFn_t)integerInspect,
    [OBJECT_BOOLEAN]=(ObjectInspectFn_t)booleanInspect,
    [OBJECT_NULL]=(ObjectInspectFn_t)nulllInspect,
    [OBJECT_RETURN_VALUE]=(ObjectInspectFn_t)returnValueInspect
};

void cleanupObject(Object_t** obj) {
    if (!(*obj))
        return;
        
    if (0 <= (*obj)->type && (*obj)->type < _OBJECT_TYPE_CNT) {
        ObjectCleanupFn_t cleanupFn = objectCleanupFns[(*obj)->type];
        if (!cleanupFn) return;
        cleanupFn((void**)obj);
    }
}

char* objectInspect(Object_t* obj) {
    if (0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectInspectFn_t inspectFn = objectInsepctFns[obj->type];
        if (!inspectFn) return NULL;
        return inspectFn(obj);
    }
    return NULL;
}

ObjectType_t objectGetType(Object_t* obj) {
    return obj->type;
}


/************************************ 
 *     INTEGER OBJECT TYPE          *
 ************************************/

Integer_t* createInteger(int64_t value) {
    Integer_t* obj = malloc(sizeof(Integer_t));
    if (obj == NULL)
        return NULL;
    
    *obj = (Integer_t){
        .type = OBJECT_INTEGER,
        .value = value
    };

    return obj;
}

void cleanupInteger(Integer_t** obj) {
    if (*obj == NULL)
        return;
    free(*obj);
    *obj = NULL;
}

char* integerInspect(Integer_t* obj) {
    return strFormat("%d", obj->value);
}



/************************************ 
 *     BOOLEAN OBJECT TYPE          *
 ************************************/

static Boolean_t TRUE_OBJ =  {.type = OBJECT_BOOLEAN, .value = true};
static Boolean_t FALSE_OBJ =  {.type = OBJECT_BOOLEAN, .value = false};

Boolean_t* createBoolean(bool value) {
    return value ? &TRUE_OBJ : &FALSE_OBJ;
}

void cleanupBoolean(Boolean_t** obj) {
    *obj = NULL;
}

char* booleanInspect(Boolean_t* obj) {
    return obj->value ? cloneString("true") : cloneString("false");
}

/************************************ 
 *        NULL OBJECT TYPE          *
 ************************************/

static Null_t NULL_OBJ = {.type = OBJECT_NULL};

Null_t* createNull() {
    return &NULL_OBJ;
}

void cleanupNull(Null_t** obj) {
    *obj = NULL;
}

char* nulllInspect(Null_t* obj) {
    return cloneString("null");
}

/************************************ 
 *      RETURN OBJECT TYPE          *
 ************************************/

ReturnValue_t* createReturnValue(Object_t* value) {
    ReturnValue_t* ret = (ReturnValue_t*) malloc(sizeof(ReturnValue_t));
    if (!ret)
        return NULL;
    *ret= (ReturnValue_t) {
        .type = OBJECT_RETURN_VALUE, 
        .value = value
    };

    return ret;
}

void cleanupReturnValue(ReturnValue_t** obj) {
    if (!(*obj))
        return;
    // Note: intenionally does not free inner obj. 
    free(*obj);
    *obj = NULL;    
}

char* returnValueInspect(ReturnValue_t* obj) {
    return objectInspect(obj->value);
}

