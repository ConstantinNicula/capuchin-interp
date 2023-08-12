#include "object.h"

#include <stdlib.h>
#include "utils.h"
/************************************ 
 *     GENERIC OBJECT TYPE          *
 ************************************/


static ObjectCleanupFn_t objectCleanupFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectCleanupFn_t)cleanupInteger
};

static ObjectInspectFn_t objectInsepctFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectInspectFn_t)integerInspect
};

void cleanupObject(Object_t* obj) {
    if (0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectCleanupFn_t cleanupFn = objectCleanupFns[obj->type];
        cleanupFn((void**)&obj);
    }
}

char* objectInspect(Object_t* obj) {
    if (0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectInspectFn_t inspectFn = objectInsepctFns[obj->type];
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

Boolean_t* createBoolean(bool value) {
    Boolean_t* obj = malloc(sizeof(Boolean_t));
    if (obj == NULL)
        return NULL;

    *obj = (Boolean_t) {
        .type = OBJECT_BOOLEAN, 
        .value = value
    };

    return obj;
}
void cleanupBoolean(Boolean_t** obj) {
    if (*obj == NULL)
        return;
    free(*obj);
    *obj = NULL;
}

char* booleanInspect(Boolean_t* obj) {
    return obj->value ? cloneString("true") : cloneString("false");
}

/************************************ 
 *        NULL OBJECT TYPE          *
 ************************************/

Null_t* createNull() {
    return malloc(sizeof(Null_t));
}

void cleanupNull(Null_t** obj) {
     if (*obj == NULL)
        return;
    free(*obj);
    *obj = NULL;
}

char* nulllInspect(Null_t* obj) {
    return cloneString("null");
}
