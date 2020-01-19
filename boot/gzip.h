/* gzip.h -- common declarations for all gzip modules
 * Copyright (C) 1992-1993 Jean-loup Gailly.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License, see the file COPYING.
 */

#define	GZIP_FAR
#define	GZIP_NEAR

#if defined(__STDC__) || defined(PROTO)
#  define OF(args)  args
#else
#  define OF(args)  ()
#endif


#ifdef __STDC__
   typedef void *voidp;
#else
   typedef char *voidp;
#endif

/* I don't like nested includes, but the string and io functions are used
 * too often
 */
//#include <stdio.h>

//#include <libc.h>
#  define memzero(s, n)     memset ((voidp)(s), 0, (n))
#ifdef	_NEVER

#if !defined(NO_STRING_H) || defined(STDC_HEADERS)
#  include <string.h>
#  if !defined(STDC_HEADERS) && !defined(NO_MEMORY_H) && !defined(__GNUC__)
#    include <memory.h>
#  endif
#  define memzero(s, n)     memset ((voidp)(s), 0, (n))
#else
#  include <strings.h>
#  define strchr            index 
#  define strrchr           rindex
#  define memcpy(d, s, n)   bcopy((s), (d), (n)) 
#  define memcmp(s1, s2, n) bcmp((s1), (s2), (n)) 
#  define memzero(s, n)     bzero((s), (n))
#endif

#endif

#ifndef RETSIGTYPE
#  define RETSIGTYPE void
#endif

#define local static

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

/* Return codes from gzip */
#define GZIP_OK      0
#define GZIP_ERROR   1
#define GZIP_WARNING 2
#define GZIP_FALSE	0
#define GZIP_TRUE	1

/* Compression methods (see algorithm.doc) */
#define STORED      0
#define COMPRESSED  1
#define GPACKED      2
#define LZHED       3
/* methods 4 to 7 reserved */
#define DEFLATED    8
#define MAX_METHODS 9
extern int method;         /* compression method */


#define BITS 16



/* To save memory for 16 bit systems, some arrays are overlaid between
 * the various modules:
 * deflate:  prev+head   window      d_buf  l_buf  outbuf
 * unlzw:    tab_prefix  tab_suffix  stack  inbuf  outbuf
 * inflate:              window             inbuf
 * unpack:               window             inbuf  prefix_len
 * unlzh:    left+right  window      c_table inbuf c_len
 * For compression, input is done in window[]. For decompression, output
 * is done in window except for unlzw.
 */

#ifndef	INBUFSIZ
#    define INBUFSIZ  0x10  /* input buffer size */
#endif
#define INBUF_EXTRA  0     /* required by unlzw() */

#ifndef	OUTBUFSIZ
#    define OUTBUFSIZ  0x10  /* output buffer size */
#endif
#define OUTBUF_EXTRA 0   /* required by unlzw() */


#ifdef DYN_ALLOC
#  define EXTERN(type, array)  extern type * GZIP_NEAR array
#  define DECLARE(type, array, size)  type * GZIP_NEAR array
#  define ALLOC(type, array, size) { \
      array = (type*)fcalloc((size_t)(((size)+1L)/2), 2*sizeof(type)); \
      if (array == NULL) error("insufficient memory"); \
   }
#  define FREE(array) {if (array != NULL) fcfree(array), array=NULL;}
#else
#  define EXTERN(type, array)  extern type array[]
#  define DECLARE(type, array, size)  type array[size]
#  define ALLOC(type, array, size)
#  define FREE(array)
#endif

EXTERN(uch, inbuf);          /* input buffer */
EXTERN(uch, outbuf);         /* output buffer */
EXTERN(uch, window);         /* Sliding window and suffix table (unlzw) */

extern unsigned long unzipSource;
extern unsigned long unzipDest;
extern long unzipLength;
extern unsigned long unzipTotalSZ; // total uncompress data size

extern unsigned insize; /* valid bytes in inbuf */
extern unsigned inptr;  /* index of next byte to be processed in inbuf */
extern unsigned outcnt; /* bytes in output buffer */

extern long bytes_in;   /* number of input bytes */
extern long bytes_out;  /* number of output bytes */

#define isize bytes_in
/* for compatibility with old zip sources (to be cleaned) */


typedef int file_t;     /* Do not use stdio */
#define NO_FILE  (-1)   /* in memory compression */


#define	PACK_MAGIC     "\037\036" /* Magic header for packed files */
#define	GZIP_MAGIC     "\037\213" /* Magic header for gzip files, 1F 8B */
#define	OLD_GZIP_MAGIC "\037\236" /* Magic header for gzip 0.5 = freeze 1.x */
#define	LZH_MAGIC      "\037\240" /* Magic header for SCO LZH Compress files*/
#define PKZIP_MAGIC    "\120\113\003\004" /* Magic header for pkzip files */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

/* internal file attribute */
#define UNKNOWN 0xffff
#define BINARY  0
#define ASCII   1

#ifndef WSIZE
#  define WSIZE 0x8000     /* window size--must be a power of two, and */
#endif                     /*  at least 32K for zip's deflate method */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

extern int exit_code;      /* program exit code */
extern int level;          /* compression level */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf(0))

#define seekable()    0  /* force sequential output */
#define translate_eol 0  /* no option -a yet */

#define tolow(c)  (isupper(c) ? (c)-'A'+'a' : (c))    /* force to lower case */

/* Macros for getting two-byte and four-byte header values */
#define SH(p) ((ush)(uch)((p)[0]) | ((ush)(uch)((p)[1]) << 8))
#define LG(p) ((ulg)(SH(p)) | ((ulg)(SH((p)+2)) << 16))
//#define SH(p) 	((ush)(uch)((p)[1]) | ((ush)(uch)((p)[0]) << 8))
//#define LG(p) 	((ulg)(SH((p)+2)) | ((ulg)(SH(p)) << 16))

/* Diagnostic functions */
#ifdef DEBUG
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {printf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#define WARN(msg) {printf msg; \
		   if (exit_code == GZIP_OK) exit_code = GZIP_WARNING;}

	/* in unzip.c */
extern int unzip      OF((void));


	/* in util.c: */
extern ulg  updcrc        OF((uch GZIP_FAR *s, unsigned n));
extern void clear_bufs    OF((void));
extern int  fill_inbuf    OF((int eof_ok));
extern void flush_window  OF((void));
extern void write_buf     OF((void GZIP_FAR *buf, unsigned cnt));
extern char *strlwr       OF((char *s));
extern void error         OF((char *m));
extern void warn          OF((char *a, char *b));
extern void read_error    OF((void));
extern void write_error   OF((void));

	/* in inflate.c */
extern int inflate OF((void));
unsigned int decompress_file(unsigned long src, unsigned long dst, unsigned long len);
unsigned int decompress_file2(unsigned long src, unsigned long dst, unsigned long len,void (*func)(void));
