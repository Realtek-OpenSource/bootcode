
#include "libc.h"

int atoi(const char *s)
{
    int idx = 0;
    int sign = 1, val = 0;

    if (s == 0)
        return 0;
    while (s[idx] != 0)
    {
        if ((s[idx] == ' ') || (s[idx] == '\t'))
            idx++;
        else
            break;
    }
    if (s[idx] == 0)
        return 0;
    else if (s[idx] == '-')
    {
        sign = -1;
        idx++;
    }
    while ((s[idx] >= '0') && (s[idx] <= '9'))
    {
        val = val * 10 + (s[idx] - '0');
        idx++;
    }
    return (sign*val);
}

int itoa( char *string, unsigned long version )
{
    int divisor = 1000000000;
    int i;
    int result;

    if ( 0 != version )
    {
	do 
	{
	    result = version / divisor;
	    version = version - result * divisor;
	    divisor = divisor / 10;
	} while ( 0 == result );
	
	i = 0;
	
	string[i++] = (char)result + '0';
	
	while ( 0 != divisor )
	{
	    result = version / divisor;
	    version = version - result * divisor;
	    divisor = divisor / 10;
	    string[i++] = (char)result + '0';
	}
    }
    else
    {
	i = 1;
	string[0] = '0';
    }

    return i;
}

void *memmove(void *dest, const void *src, size_t n)
{
    char *d, *s;
    int i;

//printf("----- memmove -----\n\r");
    if ((dest == 0) || (src == 0) || (n <= 0))
        return dest;
    d = (char *)dest;
    s = (char *)src;
    if (d <= s)
    {
        for (i=0; i<n; i++)
            d[i] = s[i];
    }
    else
    {
        for (i=n-1; i>=0; i--)
            d[i] = s[i];
    }
        return dest;
}

void *bcopy(const void *src, void *dest, size_t n)
{
//printf("----- memcpy -----\n\r");
    return (memmove(dest, src, n));
}


void *memcpy(void *dest, const void *src, size_t n)
{
//printf("----- memcpy -----\n\r");
    return (memmove(dest, src, n));
}

void *memset(void *s, int c, size_t n)
{
    int i;
    char *d;

//printf("------ memset ------\n\r");
    if ((s == 0) || (n <= 0))
        return s;
    d = (char *)s;
    for (i=0; i<n; i++)
        d[i] = (char)c;
    return s;
}

void bzero(void *s, int n)
{
	memset(s, 0, n);
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    size_t idx = 0;
    unsigned char *c1, *c2;

//printf("----- memcmp -----\n\r");
    c1 = (unsigned char *)s1;
    c2 = (unsigned char *)s2;

    while (idx < n)
        if (c1[idx] == c2[idx])
            idx++;
        else if (c1[idx] > c2[idx])
           return 1;
        else
           return -1;
        
   return 0;
}

int
strlen(const char *str)
{
  const char *s;

  if (str == 0)
    return 0;
  for (s = str; *s; ++s);
  return s-str;
}

char *strcpy(char *dest, const char *src)
{
    int idx = 0;

//printf("..... strcpy .....\n\r");

    if ((dest == 0) || (src == 0))
        return 0;
    while (src[idx] != 0)
    {
        dest[idx] = src[idx];
        idx++;
    }
    dest[idx] = 0;
    return dest;
}


char *strcat(char *dest, const char *src)
{
    int idx = 0;
    char *d;

//printf("..... strcat .....\n\r");
    if ((dest == 0) || (src == 0))
        return dest;
    d = dest;
    while (*d != 0)
        d++;
    idx = 0;
    while (src[idx] != 0)
    {
        d[idx] = src[idx];
        idx++;
    }
    d[idx] = 0;
    return dest;
}


int strcmp(const char *s1, const char *s2)
{
    int idx = 0;

//printf("..... strcmp .....\n\r");

    if ((s1 == 0) || (s2 == 0))
        return (*s1-*s2);
    while ((s1[idx] != 0) && (s2[idx] != 0))
        if (s1[idx] == s2[idx])
            idx++;
        else
            return (s1[idx]-s2[idx]);

    if ((s1[idx] == 0) && (s2[idx] == 0))
        return 0;
    else
        return (s1[idx]-s2[idx]);
}


char *strchr(const char *s, int cc)
{
    char *p;
    char c = (char)cc;

//printf("..... strchr .....\n\r");

    if (s == 0)
        return 0;
    p = (char *)s;
    while (*p != 0)
    {
        if (*p == c)
            return p;
        p++;
    }
    if (c == 0)
        return p;
    return 0;
}

/*****************************************
 * Convert 'a' - 'z' into 'A' - 'Z'
 *****************************************/
char	UpperChar(char ch)
{
	if ((ch >= 'a') && (ch <= 'z')){
		ch -= ('a' - 'A');
	}
	return ch;
}


/***********************************************************
 * Convert to Hex number (etc. 0 .. 9 and A .. F or a .. f)
 * return 0xff with error input
 * others return 0 .. 16
 ************************************************************/
int	IsHexNumber(char ch)
{
	ch = UpperChar(ch);

	if ((ch >= '0') && (ch <= '9'))
		return (ch - '0');

	if ((ch >= 'A') && (ch <= 'F'))
		return (ch - 'A' + 10);

	return 0xff;
}

/******************************************
 * Get Hex value from spec. device
 * save result into arrary
 * If _ESC return -1
 * else return 0
 ******************************************/
char	GetHexValue(unsigned char addr[],int len, 
		char (*poll)(void), void (*_put)(char),
		char (*_get)(void))
{

 int	index;
 char	key,hexkey;
 
 index = 0;
 len <<= 1;
 while (index < len){
	if (poll()){
		key = _get();

		if (key == ' ')
			continue;

		if (key == 0x1b)
			return -1;

		if ((key == 0x08) && (index > 0)){
			index--;
			_put(key);
			continue;
		}

		hexkey = IsHexNumber(key);
		if ((unsigned char)hexkey != 0xff)	{
			_put(key);
			/*
			combine two nibble into a Hex number
			when index = 0, 2, 4, 6, 8, 10 ... first nibble
			index = 1, 3, 5, 7, 9, 11 ... last nibble
			*/
			if ((index % 2) == 0){
				addr[index/2] = hexkey;
			}
			else{
				addr[index/2] <<= 4;
				addr[index/2] |= hexkey;
			}
			index++;
		}
		
	}
 }
 return 0;
}


#define INADDR_NONE	0
/*
 * inet_addr -- Convert a string containing a dotted address "a.b.c.d"
 *              into an unsigned long network byte order ip address.
 * parameters:  cp -- a dotted string ip address
 * return:      INADDR_NONE -- invalid data (0xffffffff)
 *              other -- ip address
 */

unsigned long inet_addr(const char *cp)
{
   int i;
   unsigned long ip = 0;
   int cur[4];
   unsigned char *b = (unsigned char *)&ip;

   /* check whether any field is greater than 255 */
   for (i=0; i<4; i++)
   {
      cur[i] = atoi(cp);
      if (cur[i] > 255)	/* greater than 255 */
         return (INADDR_NONE);
      b[i] = cur[i];
      if (i == 3)
          break;
      cp = strchr(cp, '.');
      if (cp == 0)	/* too few dots */
      	return (INADDR_NONE);
      cp++;
   }
   return (ip);
}

