#include <stdlib.h>
#include <assert.h>

#include "object.h"
#include "utils.h"
#include "sbuf.h"

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

static ObjectCleanupFn_t objectCleanupFns[_OBJECT_TYPE_CNT] = {
    [OBJECT_INTEGER]=(ObjectCleanupFn_t)cleanupInteger,
    [OBJECT_BOOLEAN]=(ObjectCleanupFn_t)cleanupBoolean,
    [OBJECT_NULL]=(ObjectCleanupFn_t)cleanupNull,
    [OBJECT_RETURN_VALUE]=(ObjectCleanupFn_t)cleanupReturnValue,
    [OBJECT_ERROR]=(ObjectCleanupFn_t)cleanupError,
    [OBJECT_FUNCTION]=(ObjectCleanupFn_t)cleanupFunction
};

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

static ObjectGcMarkFn_t objectMarkFns[_OBJECT_TYPE_CNT] = {
    /*
    [OBJECT_INTEGER]=(ObjectMarkFn_t)markInteger,
    [OBJECT_BOOLEAN]=(ObjectMarkFn_t)markBoolean,
    [OBJECT_NULL]=(ObjectMarkFn_t)markNull,
    [OBJECT_RETURN_VALUE]=(ObjectMarkFn_t)markReturnValue,
    [OBJECT_ERROR]=(ObjectMarkFn_t)markError,
    [OBJECT_FUNCTION]=(ObjectMarkFn_t)markFunction
    */
};



void cleanupObject(Object_t** obj) {
    if (!(*obj)) return;
        
    if (0 <= (*obj)->type && (*obj)->type < _OBJECT_TYPE_CNT) {
        ObjectCleanupFn_t cleanupFn = objectCleanupFns[(*obj)->type];
        if (!cleanupFn) return;
        cleanupFn((void**)obj);
    }
}

Object_t* copyObject(Object_t* obj) {
    if (obj && 0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectCopyFn_t copyFn = objectCopyFns[obj->type];
        if (!copyFn) return (Object_t*)createNull();
        return copyFn(obj);
    }
    return (Object_t*)createNull();
}

void gcMarkObject(Object_t* obj) {
    if (obj && 0 <= obj->type && obj->type < _OBJECT_TYPE_CNT) {
        ObjectGcMarkFn_t markFn = objectMarkFns[obj->type];
        if (markFn) markFn(obj);
    }   
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


/************************************ 
 *     INTEGER OBJECT TYPE          *
 ************************************/

Integer_t* createInteger(int64_t value) {
    Integer_t* obj = mallocChk(sizeof(Integer_t));
    
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

Integer_t* copyInteger(Integer_t* obj) {
    return createInteger(obj->value);
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

Boolean_t* copyBoolean(Boolean_t* obj) {
    return createBoolean(obj->value);
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
    ReturnValue_t* ret = mallocChk(sizeof(ReturnValue_t));
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
    Error_t* err = mallocChk(sizeof(Error_t));

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
    free(*err);
    *err = NULL;
}

char* errorInspect(Error_t* err) {
    return strFormat("ERROR: %s", err->message);
}


/************************************ 
 *    FUNCTION OBJECT TYPE          *
 ************************************/

Function_t* createFunction(Vector_t* params, BlockStatement_t* body, Environment_t* env) {
    Function_t* func = mallocChk(sizeof(Function_t));
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

    cleanupVector(&(*obj)->parameters, (VectorElemCleanupFn_t)cleanupExpression);
    cleanupBlockStatement(&(*obj)->body);
    
    free(*obj);
    *obj = NULL;
}

Function_t* copyFunction(Function_t* obj) {
    assert(0 && "TO DO: not yet implemented");
    return obj;
}

char* functionInspect(Function_t* obj) {
    Strbuf_t* sbuf = createStrbuf();
    strbufWrite(sbuf, "fn(");
    
    uint32_t cnt = vectorGetCount(obj->parameters);
    Identifier_t** params = vectorGetBuffer(obj->parameters);
    for (uint32_t i = 0; i < cnt - 1; i++) {
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