#ifndef _SBUF_H_
#define _SBUF_H_

#include <stdint.h>

typedef struct Strbuf {
    char* str;
    uint32_t len;
} Strbuf_t;

Strbuf_t* createStrbuf();
void cleanupStrbuf(Strbuf_t** sbuf);

void strbufWrite(Strbuf_t* sbuf, const char* str);
char* strbufDetach(Strbuf_t* sbuf);

#endif