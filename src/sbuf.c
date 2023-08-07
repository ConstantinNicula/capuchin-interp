#include <malloc.h>
#include <string.h> 

#include "sbuf.h"

Strbuf_t* createStrbuf() {
    Strbuf_t* sbuf = (Strbuf_t*)malloc(sizeof(Strbuf_t));
    if (sbuf == NULL)
        return NULL;
    
    sbuf->str = NULL;
    sbuf->len = 0u;

    return sbuf;
}
void cleanupStrbuf(Strbuf_t** sbuf) {
    if (*sbuf == NULL) 
        return;
    
    if ((*sbuf)->str != NULL)
        free((*sbuf)->str);

    (*sbuf)->len = 0u;
    free(*sbuf);
}

char* detachStrbuf(Strbuf_t** sbuf) {
    if (*sbuf == NULL) 
        return NULL;

    char* str = (*sbuf)->str;
    
    (*sbuf)->len = 0u;
    free(*sbuf);
    
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