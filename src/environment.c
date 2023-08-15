#include "environment.h"
#include "refcount.h"

Environment_t* createEnvironment(){
    Environment_t* env = createRefCountPtr(sizeof(Environment_t));
    *env = (Environment_t) {
        .store = createHashMap()    
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
    
    cleanupRefCountedPtr(*env);
    *env = NULL;
}

Object_t* environmentGet(Environment_t* env, const char* name){
    Object_t* obj = hashMapGet(env->store, name);
    return obj ? copyObject(obj) : NULL; 
}

Object_t* environmentSet(Environment_t* env, const char* name, Object_t* obj){
    if(!obj) return NULL;
    hashMapInsert(env->store, name, copyObject(obj));
    return obj;
}

