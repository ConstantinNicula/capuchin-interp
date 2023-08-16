#include <assert.h>
#include "environment.h"
#include "utils.h"
#include "gc.h"


Environment_t* createEnvironment(Environment_t* outer){
    Environment_t* env = gcMalloc(sizeof(Environment_t), GC_DATA_ENVIRONENT);
    *env = (Environment_t) {
        .store = createHashMap(),
        .outer = outer
    };
    return (!outer) ? gcGetExtRef(env) : env;
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


void gcCleanupEnvironment(Environment_t**env) {
    if (!(*env)) return;
    printf("gcCleanupEnvironment@0x%X\n", *env);
    // clean only scaffold, objects are owned by GC
    cleanupHashMap(&(*env)->store, NULL); 
    // outer is not ownded by us, don't clean GC will handle it.

    gcFree(*env);
    *env = NULL;
}

extern void gcMarkObject(Object_t* obj);

void gcMarkEnvironment(Environment_t*env) {
    printf("gcMarkEnvironment@0x%X\n", env);

    // iterate through hashmap and flag objects 
    HashMapIter_t iter = createHashMapIter(env->store);
    HashMapEntry_t* entry = hashMapIterGetNext(env->store, &iter);
    while (entry) {
        assert (entry->value != NULL);

        if (!gcMarkedAsUsed(entry->value)) {
            gcMarkUsed(entry->value);
            gcMarkObject(entry->value);
        }

        entry = hashMapIterGetNext(env->store, &iter);
    }

    // mark also outer env 
    if (env->outer && !gcMarkedAsUsed(env->outer)) {
        gcMarkUsed(env->outer);
        gcMarkEnvironment(env->outer);
    }

}