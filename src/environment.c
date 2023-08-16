#include <assert.h>
#include "environment.h"
#include "utils.h"

Environment_t* createEnvironment(Environment_t* outer){
    Environment_t* env = mallocChk(sizeof(Environment_t));
    *env = (Environment_t) {
        .store = createHashMap(),
        .outer = outer
    };
    return env;
}

void cleanupEnvironment(Environment_t**env) {
    if (!(*env)) return;
    
    cleanupHashMap(&(*env)->store, (HashMapElemCleanupFn_t)cleanupObject);
    cleanupEnvironment(&(*env)->outer);

    free(*env);
    *env = NULL;
}

Object_t* environmentGet(Environment_t* env, const char* name){
    Object_t* obj = hashMapGet(env->store, name);
    if ( !obj  && env->outer != NULL) {
        obj = environmentGet(env->outer, name);
    }
    return obj; 
}

Object_t* environmentSet(Environment_t* env, const char* name, Object_t* obj){
    if(!obj) return NULL;
    hashMapInsert(env->store, name, obj);
    return obj;
}

void gcMarkEnvironment(Environment_t*env) {
    assert(0 && "TO DO: not yet implemented!");
}