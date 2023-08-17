#include <string.h>
#include "builtin.h"
#include "utils.h"


Object_t* lenBuiltin(Vector_t* args);

void registerBuiltinFunctions(Environment_t* env) {
    environmentSet(env, "len", (Object_t*)createBuiltin(lenBuiltin));    
}

Object_t* lenBuiltin(Vector_t* args) {
    if (vectorGetCount(args) != 1) {
        char* err = strFormat("wrong number of arguments. got=%d, want=1", 
                                vectorGetCount(args));
        return (Object_t*)createError(err); 
    }
    
    Object_t** argBuf = vectorGetBuffer(args);
    switch(argBuf[0]->type) {
        case OBJECT_STRING:
            return (Object_t*)createInteger(strlen(((String_t*)argBuf[0])->value));
        default:
            char* err = strFormat("argument to `len` not supported, got %s", 
                                    objectTypeToString(argBuf[0]->type));
            return (Object_t*)createError(err);
    }
    return (Object_t*)createNull();
}