#include "zlib.h"
#include "dlmalloc.h"

void* malloc( unsigned int bytes )
{
	return dlmalloc(bytes);
}

void *calloc(unsigned int nitems, unsigned int isize )
{
	return dlcalloc(nitems, isize);
}

int free(void *ptr )
{
	return dlfree(ptr);
}


#if 0
/* compilier will be happy.. */
#ifndef memcpy
void memcpy(dest, source, len)
    unsigned char* dest;
    const unsigned char* source;
    unsigned int  len;
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}
#endif

#ifndef memcmp
int memcmp(s1, s2, len)
    const unsigned char* s1;
    const unsigned char* s2;
    unsigned int  len;
{
    unsigned int j;

    for (j = 0; j < len; j++) {
        if (s1[j] != s2[j]) return 2*(s1[j] > s2[j])-1;
    }
    return 0;
}
#endif

#ifndef memset
void memset(dest, val, len)
    unsigned char* dest;
    int val;
    unsigned int  len;
{
    if (len == 0) return;
    do {
        *dest++ = val;  /* ??? to be unrolled */
    } while (--len != 0);
}
#endif

#endif /* 0 */
