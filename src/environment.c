#include "environment.h"

Environment_t* createEnvironment(){
    Environment_t* env = (Environment_t*)malloc(sizeof(Environment_t));
    if (!env) return NULL;
    *env = (Environment_t) {
        .store = createHashMap()    
    };
    return env;
}

void cleanupEnvironment(Environment_t**env) {
    if (!(*env)) return;
    cleanupHashMapElements((*env)->store, (HashMapElementCleanupFn_t)cleanupObject);
    cleanupHashMap(&(*env)->store);
    free(*env);
    *env = NULL;
}

Object_t* environmentGet(Environment_t* env, const char* name){
    Object_t* obj = hashMapGet(env->store, name);
    return obj ? cloneObject(obj) : NULL; 
}

Object_t* environmentSet(Environment_t* env, const char* name, Object_t* obj){
    if(!obj) return NULL;
    hashMapInsert(env->store, name, cloneObject(obj));
    return obj;
}

