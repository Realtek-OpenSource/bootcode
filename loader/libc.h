
#ifndef _SIZE_T
	typedef unsigned int	size_t;
	#define _SIZE_T 1
#endif

int atoi(const char *s);
void *memmove(void *dest, const void *src, size_t n);
void *bcopy(const void *src, void *dest, size_t n);
void *memset(void *s, int c, size_t n);
void bzero(void *s, int n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int cc);
char UpperChar(char ch);
int	IsHexNumber(char ch);
char	GetHexValue(unsigned char addr[],int len, 
		char (*poll)(void), void (*_put)(char),
		char (*_get)(void));


unsigned long inet_addr(const char *cp);

int printf(char *fmt, ...);
