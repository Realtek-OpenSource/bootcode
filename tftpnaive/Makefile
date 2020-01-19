#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: tylo $
#
#   PURPOSE: makefile to build zLib inflate module                            *
#
#
include ../inc/config.mk

OBJS = arp.o bootp.o busywait.o icmp.o iob.o ip.o \
		net.o tftp.o udp.o re8670poll.o 

ifeq ($(SWITCH), rtl865x)		
OBJS += sw/swCore.o sw/swNic_poll.o sw/swTable.o sw/vlanTable.o
ifeq ($(CONFIG_EXTS_RTL8367B), y)
OBJS += rtl8367b/port.o rtl8367b/rtl8367b_asicdrv.o rtl8367b/smi.o
endif
endif 

LIB = tftpnaive.a

all : $(LIB)

clean :
	$(RM) $(RMFLAGS) $(OBJS) *.o *.a *.bak *~

include ../inc/configlib.mk








