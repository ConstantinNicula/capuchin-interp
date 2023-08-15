#include <malloc.h>
#include <string.h> 

#include "sbuf.h"
#include "refcount.h"

Strbuf_t* createStrbuf() {
    Strbuf_t* sbuf = createRefCountPtr(sizeof(Strbuf_t));
    
    sbuf->str = NULL;
    sbuf->len = 0u;

    return sbuf;
}
void cleanupStrbuf(Strbuf_t** sbuf) {
    if (!(*sbuf) || refCountPtrDec(*sbuf) != 0) 
        return;
    
    if ((*sbuf)->str != NULL)
        free((*sbuf)->str);

    (*sbuf)->len = 0u;
    cleanupRefCountedPtr(*sbuf);
}

char* detachStrbuf(Strbuf_t** sbuf) {
    if (*sbuf == NULL) 
        return NULL;

    // dangling refs still exist 
    if (refCountPtrDec(*sbuf) != 0)
        return (*sbuf)->str;

    // no more refs 
    char* str = (*sbuf)->str;
    (*sbuf)->len = 0u;
    cleanupRefCountedPtr(*sbuf);
    
    return str;
}

void strbufWrite(Strbuf_t* sbuf, const char* str) {
    uint32_t tlen = sbuf->len + strlen(str);
    
    if (sbuf->str == NULL) {
        sbuf->str = (char*) malloc(tlen + 1u);  
        sbuf->str[0]= '\0'; 
    } else {
        sbuf->str = (char*) realloc(sbuf->str, tlen + 1u);
    }
    
    if (sbuf->str == NULL){
        // TO DO: OOM error 
        return;
    }
    sbuf->len = tlen;
    strcat(sbuf->str, str);
}

void strbufConsume(Strbuf_t* sbuf, char* str) {
    if (str == NULL)
        return;
    strbufWrite(sbuf, str);
    free(str);
}