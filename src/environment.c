#include "environment.h"
#include "refcount.h"

Environment_t* createEnvironment(Environment_t* outer){
    Environment_t* env = createRefCountPtr(sizeof(Environment_t));
    *env = (Environment_t) {
        .store = createHashMap(),
        .outer = outer ? copyEnvironment(outer) : NULL    
    };
    return env;
}

Environment_t* copyEnvironment(Environment_t* env) {
    refCountPtrInc(env);
    return env;
}

void cleanupEnvironment(Environment_t**env) {
    if (!(*env) || refCountPtrDec(*env) != 0) return;
    
    cleanupHashMap(&(*env)->store, (HashMapElementCleanupFn_t)cleanupObject);
    if ((*env)->outer)
        cleanupEnvironment(&(*env)->outer);

    cleanupRefCountedPtr(*env);
    *env = NULL;
}

Object_t* environmentGet(Environment_t* env, const char* name){
    Object_t* obj = hashMapGet(env->store, name);
    if ( !obj  && env->outer != NULL)
    {
        obj = environmentGet(env->outer, name);
    }
    return obj ? copyObject(obj) : NULL; 
}

Object_t* environmentSet(Environment_t* env, const char* name, Object_t* obj){
    if(!obj) return NULL;
    hashMapInsert(env->store, name, copyObject(obj));
    return obj;
}

