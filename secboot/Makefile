#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: yachang $
#
#   PURPOSE: makefile to build self-decompressed module                            *
#
#


RAM_FILE_ORG = ../loader/$(LOADERFILE).bin
RAM_FILE = ramFile.o
SECBOOT = secboot

include ../inc/config.mk
include ../.config


OBJS_y=secbootInit.o main.o LzmaDecode.o ../boot/memctl/mips_cache_ops.o ../loader/uart.o

LDSCRIPT=secboot.ld

CFLAGS += -DCOMPILE_BOOT

all : $(OBJS_y) $(RAM_FILE_ORG)
	cp $(RAM_FILE_ORG) tmp.bin
	lzma e tmp.bin tmp.bin.lzma
	$(BIN2ASM) tmp.bin.lzma > ramfile.s
	$(CC) $(AFLAGS) -o $(RAM_FILE) ramfile.s
	$(LD) $(LDFLAGS) -Ttext $(MEM_START) -Map $(SECBOOT).map -T $(LDSCRIPT) -o $(SECBOOT).elf $(OBJS_y) $(RAM_FILE)
	$(OBJCOPY) -S --gap-fill=255 -O binary $(SECBOOT).elf $(SECBOOT).bin

clean :
	$(RM) $(RMFLAGS) $(OBJS_y) $(OBJS_l) *.elf *.bin *.bak *~ ramfile.s $(RAM_FILE) *.map *.lzma tmp.bin *.hdr

%.o : %.s
	$(CC) $(AFLAGS) -o $@ $<




