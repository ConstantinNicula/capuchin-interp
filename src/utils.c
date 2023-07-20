#include "utils.h"
#include <string.h>
#include <stdlib.h>

char* cloneString(const char* str) {
    return cloneSubstring(str, strlen(str));
}

char* cloneSubstring(const char* str, uint32_t len) {
    char* newStr = (char*) calloc(len + 1u, sizeof(char));
    if (newStr == NULL) {
        return NULL;
    } 
    memmove(newStr, str, len);
    return newStr;
}
