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
PLR_FILE_ORG = plr.img
OUT_BOOTLOADER_BIN = SPN-plr-load_2k.bin
include ../inc/config.mk
include ../.config

ifeq ($(CONFIG_SPI_NAND_FLASH),y)
all : $(LIBS) $(OBJS_y) $(OBJS_l)	
	#cp $(RAM_FILE_ORG) .
	./bchenc -o $(OUT_BOOTLOADER_BIN) -plr $(PLR_FILE_ORG) 1 -uboot $(RAM_FILE_ORG) 1 -ulzma -umagic 0x6C6F6164 -ula 0x80f00000 -uea 0x80f00000 -ucomp 3
	#cp $(RAM_FILE_ORG) .
	#lzma e tmp.bin tmp.bin.lzma
	#./bch6enc -o $(OUT_BOOTLOADER_BIN) -plr $(PLR_FILE_ORG) 1 -uboot $(RAM_FILE_ORG) 1 -umagic 0x6C6F6164 -ula 0x80f00000 -uea 0x80f00000 -ucomp 0

endif

clean :
	$(RM) $(RMFLAGS) $(OBJS_y) $(OBJS_l) *.elf *.bin *.bak *~ ramfile.s $(RAM_FILE) *.map *.lzma tmp.bin *.hdr
