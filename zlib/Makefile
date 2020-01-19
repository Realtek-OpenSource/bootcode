#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: yachang $
#
#   PURPOSE: makefile to build zLib inflate module                            *
#
#

ifeq ($(CONFIG_RE8306), y)
OBJS = LzmaDecode.o
else
OBJS = crc32.o adler32.o infblock.o infcodes.o inflate.o \
		inftrees.o infutil.o zutil.o uncompr.o LzmaDecode.o
endif

LIB = inflateLib.a

all : $(LIB)

clean :
	$(RM) $(RMFLAGS) $(OBJS) *.o *.a *.bak *~

include ../inc/configlib.mk







