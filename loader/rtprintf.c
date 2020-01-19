/*
	modify from Casey
	
	NOTE:	MUST change _DOPUTC to perporty
*/
#include <stdarg.h>
/*#include <ctype.h>*/
#include "uart.h"

#define MAXPFLEN 256             /* size of print buffer */
#define	_DOPUTC		putchar


static void insert(char *s, char c) /* insert c at head of string s */
{
    char *tmps;
  
    tmps = s;
    while (*tmps++);
    while( tmps > s ){
        tmps--;
        tmps[1] = tmps[0];
    }
    *s = c;
}
  
  
static void append(char *s, char c) /* append c to end of string s */
{
    while(*s++);
    *s-- = 0;
    *s = c;
}

static int vis_printf(char *buf, char *fmt, va_list args)
{
    int count;
    int pwidth,width,pcnt,base;
    unsigned long num;
    char fch, c;     /* format char */
    char *s, *bp;
    char ljust, zsup;   /* left justify, zero suppress flags */
    char sign;    /* signed number conversion flag */
    char letter = 0;     /* hex conversion letter increment */
    char islong;     /* long integer flag */
    char pflag;
  
    count = 0;
    *buf = 0;
    bp = buf;           /* point to start of buf */
    while((fch = *fmt++) != 0){
        while(*bp)        /* find end of current string */
            bp++;         /*  where next field starts */
        if( fch == '%' ){
            if ((fch= *fmt++) == '%')
                goto copy;
            if ((ljust = fch) == '-') /* left justify flag */
                fch = *fmt++;
            zsup = fch;         /* zero suppression */
            pwidth = sizeof(void *)*2;   /* minimum pointer print width */
            pcnt = 0;        /* printed length of current field */
            sign = 0;        /* default unsigned */
            pflag = 0;       /* not %p spec */
            base = 10;       /* default base */
            if (fch == '*')
                width = va_arg(args, int);
            else
            {
                for (width=0; fch>='0' && fch<='9'; fch=*fmt++)
                    width = width * 10 + fch - '0';
            }
            if ((islong = (fch & 0xdf)) == 'L')
                fch = *fmt++;
            switch( fch ){
                case 'd':
                    sign = 1;
                    goto donumber;
                case 'o':      /* octal */
                    base = 8;
                    goto donumber;
                case 'u':
                    goto donumber;
                case 'x':      /* hex */
                    base = 16;
                    letter = 'a'-10;
                    goto donumber;
                case 'X':
                    base = 16;
                    letter = 'A'-10;
                    goto donumber;
                case 'p':      /* void * */
                    pflag = 1;
                    if( width < pwidth )
                        width = pwidth;
                    base = 16;
                    letter = 'A'-10;
                    num = (long)va_arg(args, void *);
                    goto doptr;
                case 'c':
                    fch = (char) va_arg(args, int);
                    goto copy;
                case 's':
                    s = va_arg(args,char *);
                    while( *s ){     /* copy string to buf */
                        append(bp,*s++);
                        count++;
                        pcnt++;
                    }
                    for( ; pcnt<width; pcnt++){
                        count++;
                        if (ljust == '-')
                            append(bp, ' ');
                        else{
                            insert(bp, ' ');
                        }
                    }
                    goto endarg;
            }
            donumber:
            {
                if (islong == 'L')
                    num = va_arg(args, long int);      /* long argument */
                else if (sign)
                    num = (long)va_arg(args, int);     /* integer argument */
                else
                    num = (long)va_arg(args, unsigned int);  /* unsigned integer */
                if( sign && (num & 0x80000000) ){
                    sign = 1;     /* do sign */
                    num = -num;
                }else{
                    sign = 0;
                }
                doptr:
                while( num != 0l ){
                    c = num % base;
                    num /= base;
                    insert(bp, (char)(c > 9 ? c + letter : (char)c + '0'));
                    pcnt++;       /* count digits */
                    count++;
                }
                if(!*bp){
                    insert(bp, '0');    /* at least 1 zero */
                    pcnt++;
                    count++;
                }
                if( pflag ){
                    for(;pcnt < pwidth; pcnt++){
                        insert(bp, '0');
                        count++;
                    }
                }
                if (zsup != '0') zsup = ' ';  /* pad char */
                for (pcnt += sign ;pcnt < width; pcnt++)
                    if (ljust == 'L'){     /* left justified ? */
                        append(bp, ' ');
                        count++;
                    }else{
                        insert(bp, zsup);
                        count++;
                    }
                if (sign)
                    insert(bp, '-');
            }
  
        }else{
            copy:     append(bp++, fch);     /* copy char to output */
            count++;
        }
        endarg:
        continue;
    }
    return count;
}

int printf(char *fmt, ...)
{
char buf[MAXPFLEN];
char *p;
int len = 0;
va_list args;

	va_start(args,fmt);
	vis_printf(buf, fmt, args);
	p = buf;
	while( *p )
	{
		_DOPUTC(*p);
		p++;
		len++;
	}
	return(len);
}

int __eprintf(char *fmt, ...)
{
char buf[MAXPFLEN];
char *p;
int len = 0;
va_list args;

	va_start(args,fmt);
	vis_printf(buf, fmt, args);
	p = buf;
	while( *p )
	{
		_DOPUTC(*p);
		p++;
		len++;
	}
	return(len);
}


int sprintf(char *buf, char *fmt, ...)
{
int len;
va_list args;

	va_start(args,fmt);
	len = vis_printf(buf, fmt, args);
	return(len);
}

