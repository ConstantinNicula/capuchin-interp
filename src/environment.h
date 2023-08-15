#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "hmap.h"
#include "object.h"

typedef struct Environment {
    HashMap_t* store;
} Environment_t;

Environment_t* createEnvironment();
void cleanupEnvironment(Environment_t**env);

Object_t* environmentGet(Environment_t* env, const char* name);
Object_t* environmentSet(Environment_t* env, const char* name, Object_t* obj);

#endif