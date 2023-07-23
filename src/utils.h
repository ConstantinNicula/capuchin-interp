#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h> 

char* cloneString(const char* str);
char* cloneSubstring(const char* str, uint32_t len);

char* strFormat(const char* fmt, ...);
bool strToInteger(const char* str, int64_t* val);


#endif