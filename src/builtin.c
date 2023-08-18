#include <string.h>
#include "builtin.h"
#include "utils.h"


Object_t* lenBuiltin(Vector_t* args);
Object_t* firstBuiltin(Vector_t* args);
Object_t* lastBuiltin(Vector_t* args);
Object_t* restBuiltin(Vector_t* args);
Object_t* pushBuiltin(Vector_t* args);


void registerBuiltinFunctions(Environment_t* env) {
    environmentSet(env, "len", (Object_t*)createBuiltin(lenBuiltin));    
    environmentSet(env, "first", (Object_t*)createBuiltin(firstBuiltin));    
    environmentSet(env, "last", (Object_t*)createBuiltin(lastBuiltin));    
    environmentSet(env, "rest", (Object_t*)createBuiltin(restBuiltin));    
    environmentSet(env, "push", (Object_t*)createBuiltin(pushBuiltin));    
}

Object_t* lenBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 1) {
        char* err = strFormat("wrong number of arguments. got=%d, want=1", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = (Object_t**)vectorGetBuffer(args);
    switch(argBuf[0]->type) {
        case OBJECT_ARRAY: 
            return (Object_t*)createInteger(arrayGetElementCount((Array_t*)argBuf[0]));
        case OBJECT_STRING:
            return (Object_t*)createInteger(strlen(((String_t*)argBuf[0])->value));
        default:
            char* err = strFormat("argument to `len` not supported, got %s", 
                                    objectTypeToString(argBuf[0]->type));
            return (Object_t*)createError(err);
    }
    return (Object_t*)createNull();
}

Object_t* firstBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 1) {
        char* err = strFormat("wrong number of arguments. got=%d, want=1", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = (Object_t**)vectorGetBuffer(args);
    if (argBuf[0]->type != OBJECT_ARRAY) {
        char* err = strFormat("argument to `first` must be ARRAY, got %s", 
                                objectTypeToString(argBuf[0]->type));
        return (Object_t*)createError(err);                      
    }

    Array_t* arr = (Array_t*)argBuf[0];
    if (arrayGetElementCount(arr) > 0) {
        return arrayGetElements(arr)[0];
    }

    return (Object_t*) createNull();
}

Object_t* lastBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 1) {
        char* err = strFormat("wrong number of arguments. got=%d, want=1", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = (Object_t**)vectorGetBuffer(args);
    if (argBuf[0]->type != OBJECT_ARRAY) {
        char* err = strFormat("argument to `last` must be ARRAY, got %s", 
                                objectTypeToString(argBuf[0]->type));
        return (Object_t*)createError(err);                      
    }

    Array_t* arr = (Array_t*)argBuf[0];
    uint32_t len = arrayGetElementCount(arr); 
    if (len > 0) {
        return arrayGetElements(arr)[len - 1];
    }

    return (Object_t*) createNull();
}


Object_t* restBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 1) {
        char* err = strFormat("wrong number of arguments. got=%d, want=1", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = (Object_t**)vectorGetBuffer(args);
    if (argBuf[0]->type != OBJECT_ARRAY) {
        char* err = strFormat("argument to `rest` must be ARRAY, got %s", 
                                objectTypeToString(argBuf[0]->type));
        return (Object_t*)createError(err);                      
    }

    Array_t* arr = (Array_t*)argBuf[0];
    uint32_t len = arrayGetElementCount(arr); 
    Object_t** elems = arrayGetElements(arr);
    if (len > 0) {
        Vector_t* newElements = createVector();
        for (uint32_t i = 1; i < len; i++) {
            vectorAppend(newElements, copyObject(elems[i]));
        } 
        Array_t* newArr = createArray();
        newArr->elements = newElements;
        return (Object_t*)newArr;
    }

    return (Object_t*) createNull();
}

Object_t* pushBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 2) {
        char* err = strFormat("wrong number of arguments. got=%d, want=2", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = (Object_t**)vectorGetBuffer(args);
    if (argBuf[0]->type != OBJECT_ARRAY) {
        char* err = strFormat("argument to `push` must be ARRAY, got %s", 
                                objectTypeToString(argBuf[0]->type));
        return (Object_t*)createError(err);                      
    }

    Array_t* arr = (Array_t*)argBuf[0];
    Object_t* obj = (Object_t*)argBuf[1];
    
    uint32_t len = arrayGetElementCount(arr); 
    Object_t** elems = arrayGetElements(arr);
    
    Vector_t* newElements = createVector();
    for (uint32_t i = 0; i < len; i++) {
        vectorAppend(newElements, copyObject(elems[i]));
    } 
    // add new element 
    vectorAppend(newElements, obj);
    Array_t* newArr = createArray();
    newArr->elements = newElements;
    return (Object_t*)newArr;
    

    return (Object_t*) createNull();
}