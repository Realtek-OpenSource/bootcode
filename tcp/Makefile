#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: yachang $
#
#   PURPOSE: makefile to build small web server
#
#

OBJS = err.o http.o ip.o mem.o memp.o pbuf.o \
		tcp.o tcp_input.o tcp_output.o stats.o

LIB = tcp.a

all : $(LIB)

clean :
	$(RM) $(RMFLAGS) $(OBJS) *.o *.a *.bak *~

include ../inc/configlib.mk







