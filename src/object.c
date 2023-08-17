#include <stdlib.h>
#include <assert.h>

#include "object.h"
#include "utils.h"
#include "sbuf.h"
#include "gc.h"


const char* tokenTypeStrings[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]="INTEGER",
    [OBJECT_BOOLEAN]="BOOLEAN", 
    [OBJECT_NULL]="NULL",
    [OBJECT_ERROR]="ERROR",
    [OBJECT_RETURN_VALUE]="RETURN_VALUE"
};

const char* objectTypeToString(ObjectType_t type) {
    if (0 <= type && type <= _OBJECT_TYPE_CNT) {
        return tokenTypeStrings[type];
    }
    return "";
}


/************************************ 
 *     GENERIC OBJECT TYPE          *
 ************************************/

static ObjectInspectFn_t objectInsepctFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectInspectFn_t)integerInspect,
    [OBJECT_BOOLEAN]=(ObjectInspectFn_t)booleanInspect,
    [OBJECT_NULL]=(ObjectInspectFn_t)nulllInspect,
    [OBJECT_RETURN_VALUE]=(ObjectInspectFn_t)returnValueInspect,
    [OBJECT_ERROR]=(ObjectInspectFn_t)errorInspect,
    [OBJECT_FUNCTION]=(ObjectInspectFn_t)functionInspect
};

static ObjectCopyFn_t objectCopyFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectCopyFn_t)copyInteger,
    [OBJECT_BOOLEAN]=(ObjectCopyFn_t)copyBoolean,
    [OBJECT_NULL]=(ObjectCopyFn_t)copyNull,
    [OBJECT_RETURN_VALUE]=(ObjectCopyFn_t)copyReturnValue,
    [OBJECT_ERROR]=(ObjectCopyFn_t)copyError,
    [OBJECT_FUNCTION]=(ObjectCopyFn_t)copyFunction
};


Object_t* copyObject(Object_t* obj) {
    if (obj && 0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectCopyFn_t copyFn = objectCopyFns[obj->type];
        if (!copyFn) return (Object_t*)createNull();
        return copyFn(obj);
    }
    return (Object_t*)createNull();
}


char* objectInspect(Object_t* obj) {
    if (obj && 0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectInspectFn_t inspectFn = objectInsepctFns[obj->type];
        if (!inspectFn) return cloneString(""); 
        return inspectFn(obj);
    }
    return cloneString(""); 
}


ObjectType_t objectGetType(Object_t* obj) {
    return obj->type;
}

void gcCleanupObject(Object_t** obj);
void gcMarkObject(Object_t* obj);


/************************************ 
 *     INTEGER OBJECT TYPE          *
 ************************************/

Integer_t* createInteger(int64_t value) {
    Integer_t* obj = gcMalloc(sizeof(Integer_t), GC_DATA_OBJECT);
    
    *obj = (Integer_t){
        .type = OBJECT_INTEGER,
        .value = value
    };

    return obj;
}

void cleanupInteger(Integer_t** obj) {
    if (!(*obj)) return;
    gcFree(*obj);
    *obj = NULL; 
}

void gcMarkInteger(Integer_t* obj) {
    // no objects owned by gc
}

Integer_t* copyInteger(Integer_t* obj) {
    return createInteger(obj->value);
}

char* integerInspect(Integer_t* obj) {
    return strFormat("%d", obj->value);
}



/************************************ 
 *     BOOLEAN OBJECT TYPE          *
 ************************************/

Boolean_t* createBoolean(bool value) {
    Boolean_t* ret = gcMalloc(sizeof(Boolean_t), GC_DATA_OBJECT);
    *ret = (Boolean_t) {
        .type = OBJECT_BOOLEAN,
        .value = value
    };
    return ret;
}

void cleanupBoolean(Boolean_t** obj) {
    if (!(*obj)) return;
    gcFree(*obj);
    *obj = NULL; 
}

void gcMarkBoolean(Boolean_t* obj) {
    // no objects owned by gc
}


Boolean_t* copyBoolean(Boolean_t* obj) {
    return createBoolean(obj->value);
}

char* booleanInspect(Boolean_t* obj) {
    return obj->value ? cloneString("true") : cloneString("false");
}

/************************************ 
 *        NULL OBJECT TYPE          *
 ************************************/
Null_t* createNull() {
    Null_t* ret = gcMalloc(sizeof(Null_t), GC_DATA_OBJECT);
    *ret = (Null_t) {.type = OBJECT_NULL};
    return ret;
}

void cleanupNull(Null_t** obj) {
    if (!(*obj)) return;
    gcFree(*obj);
    *obj = NULL;    
}

void gcMarkNull(Null_t* obj) {
    // no objects owned by gc
}

Null_t* copyNull(Null_t* obj) {
    return createNull();
}

char* nulllInspect(Null_t* obj) {
    return cloneString("null");
}

/************************************ 
 *      RETURN OBJECT TYPE          *
 ************************************/

ReturnValue_t* createReturnValue(Object_t* value) {
    ReturnValue_t* ret = gcMalloc(sizeof(ReturnValue_t), GC_DATA_OBJECT);
    *ret= (ReturnValue_t) {
        .type = OBJECT_RETURN_VALUE, 
        .value = value
    };

    return ret;
}

void cleanupReturnValue(ReturnValue_t** obj) {
    if (!(*obj)) return;
    // Note: intenionally does not free inner obj. 
    gcFree(*obj);
    *obj = NULL;    
}

void gcMarkReturnValue(ReturnValue_t* obj) {
    if (!gcMarkedAsUsed(obj->value)) {
        gcMarkUsed(obj->value);
        gcMarkObject(obj->value);
    }
}

ReturnValue_t* copyReturnValue(ReturnValue_t* obj) {
    return createReturnValue(obj->value);
}

char* returnValueInspect(ReturnValue_t* obj) {
    return objectInspect(obj->value);
}

/************************************ 
 *      ERROR OBJECT TYPE          *
 ************************************/

Error_t* createError(char* message) {
    Error_t* err = gcMalloc(sizeof(Error_t), GC_DATA_OBJECT);

    *err = (Error_t) {
        .type = OBJECT_ERROR,
        .message = message
    };

    return err;
}

Error_t* copyError(Error_t* obj) {
    return createError(cloneString(obj->message));
}

void cleanupError(Error_t** err) {
    if (!(*err))
        return;
    free((*err)->message);
    gcFree(*err);
    *err = NULL;
}

void gcMarkError(Error_t* err) {
    // no objects owned by gc
}

char* errorInspect(Error_t* err) {
    return strFormat("ERROR: %s", err->message);
}


/************************************ 
 *    FUNCTION OBJECT TYPE          *
 ************************************/

Function_t* createFunction(Vector_t* params, BlockStatement_t* body, Environment_t* env) {
    Function_t* func = gcMalloc(sizeof(Function_t), GC_DATA_OBJECT);
    *func = (Function_t) {
        .type = OBJECT_FUNCTION,
        .parameters = copyVector(params, (VectorElemCopyFn_t) copyExpression),
        .body = copyBlockStatement(body),
        .environment = env, // weak copy to env
    };

    return func;
}

void cleanupFunction(Function_t** obj) {
    if(!(*obj)) 
        return;

    // full clean because these are owned by object & not by GC 
    cleanupVector(&(*obj)->parameters, (VectorElemCleanupFn_t)cleanupExpression);
    cleanupBlockStatement(&(*obj)->body);
    
    gcFree(*obj);
    *obj = NULL;
}

extern void gcMarkEnvironment(Environment_t*env);

void gcMarkFunction(Function_t* obj) { 
    if (!gcMarkedAsUsed(obj->environment)) {
        gcMarkUsed(obj->environment);
        gcMarkEnvironment(obj->environment);
    }
}

Function_t* copyFunction(Function_t* obj) {
    return createFunction(obj->parameters, obj->body, obj->environment);
}

char* functionInspect(Function_t* obj) {
    Strbuf_t* sbuf = createStrbuf();
    strbufWrite(sbuf, "fn(");
    
    uint32_t cnt = vectorGetCount(obj->parameters);
    Identifier_t** params = (Identifier_t**)vectorGetBuffer(obj->parameters);
    for (uint32_t i = 0; i < cnt; i++) {
        strbufConsume(sbuf, identifierToString(params[i]));
        if (i !=  (cnt - 1))
            strbufWrite(sbuf, ",");
    }
    
    strbufWrite(sbuf, ") {\n");
    strbufConsume(sbuf, blockStatementToString(obj->body));
    strbufWrite(sbuf, "\n}");

    return detachStrbuf(&sbuf);
}

uint32_t functionGetParameterCount(Function_t* obj) {
    return vectorGetCount(obj->parameters);
}

Identifier_t** functionGetParameters(Function_t* obj) {
    return (Identifier_t**)vectorGetBuffer(obj->parameters);
}

/************************************ 
 *      GARBAGE COLLECTION          *
 ************************************/

typedef void (*ObjectCleanupFn_t) (void**);
typedef void (*ObjectGcMarkFn_t) (void*);

static ObjectCleanupFn_t objectCleanupFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectCleanupFn_t)cleanupInteger,
    [OBJECT_BOOLEAN]=(ObjectCleanupFn_t)cleanupBoolean,
    [OBJECT_NULL]=(ObjectCleanupFn_t)cleanupNull,
    [OBJECT_RETURN_VALUE]=(ObjectCleanupFn_t)cleanupReturnValue,
    [OBJECT_ERROR]=(ObjectCleanupFn_t)cleanupError,
    [OBJECT_FUNCTION]=(ObjectCleanupFn_t)cleanupFunction
};

static ObjectGcMarkFn_t objectMarkFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectGcMarkFn_t)gcMarkInteger,
    [OBJECT_BOOLEAN]=(ObjectGcMarkFn_t)gcMarkBoolean,
    [OBJECT_NULL]=(ObjectGcMarkFn_t)gcMarkNull,
    [OBJECT_RETURN_VALUE]=(ObjectGcMarkFn_t)gcMarkReturnValue,
    [OBJECT_ERROR]=(ObjectGcMarkFn_t)gcMarkError,
    [OBJECT_FUNCTION]=(ObjectGcMarkFn_t)gcMarkFunction
};


void gcCleanupObject(Object_t** obj) {
    if (!(*obj)) return;
    if (0 <= (*obj)->type && (*obj)->type < _OBJECT_TYPE_CNT) {
        ObjectCleanupFn_t cleanupFn = objectCleanupFns[(*obj)->type];
        if (!cleanupFn) return;
        cleanupFn((void**)obj);
    }
}

void gcMarkObject(Object_t* obj) {
    if (obj && 0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectGcMarkFn_t markFn = objectMarkFns[obj->type];
        if (markFn) markFn(obj);
    }   
}
