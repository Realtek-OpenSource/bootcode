#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: yachang $
#
#   PURPOSE: makefile to build self-decompressed module                            *
#
#

#OBJS = romInit.o main.o gzip.o
#LIBS = ../zlib/inflateLib.a
RAM_FILE_ORG = ../loader/$(LOADERFILE).bin
RAM_FILE = ramFile.o

include ../inc/config.mk
#ccwei 111107
include ../.config
#end

OBJS_y =
OBJS_$(CONFIG_RTL8676)  += romInit-rtl8676.o
OBJS_$(CONFIG_RTL8676S) += romInit-rtl8676s.o
OBJS_$(CONFIG_RTL8685)  += romInit-rtl8685.o ./memctl/memctl_cali_dram.o ./memctl/memctl_8685.o ./memctl/memctl_utils.o ./memctl/board_mem_diag.o
OBJS_$(CONFIG_RTL8685S) += romInit-rtl8685s.o ./memctl/memctl_cali_dram_8685s.o ./memctl/memctl_8685.o ./memctl/memctl_utils.o ./memctl/board_mem_diag_8685s.o
ifeq ($(CONFIG_RTL8685SB),y)
OBJS_$(CONFIG_RLX) += romInit-rtl8685sb.o ./memctl/memctl_cali_dram_8685s.o ./memctl/memctl_8685.o ./memctl/memctl_utils.o ./memctl/board_mem_diag_8685s.o
OBJS_$(CONFIG_MIPS) += romInit-mips.o ./memctl/ipc_mips.o ./memctl/memctl_cali_dram_8685s.o ./memctl/mips_cache_ops.o
endif

ifeq ($(CONFIG_BOOT_MIPS),y)
OBJS_y += ./memctl/boot_mips.o ./memctl/ipc_rlx.o
endif

ifeq ($(CONFIG_RTL8685),y)
ifeq ($(CONFIG_NAND_FLASH),y)
OBJS_y += uart.o
else
OBJS_y += main.o LzmaDecode.o ../loader/uart.o
endif
else
ifneq ($(CONFIG_MIPS),y)
OBJS_y += main.o LzmaDecode.o ../loader/uart.o
endif
endif
CFLAGS += -DCOMPILE_BOOT

#ccwei
ifeq ($(CONFIG_NAND_FLASH),y)
ifeq ($(CONFIG_RTL8685),y)
NAND_BOOT_B = nbootB
NAND_BOOT = nbootA
OBJS_l += romInitB.o main.o LzmaDecode.o board_lx_freq.o uart.o ../loader/rtk_nand_read.o
else
NAND_BOOT = nboot
OBJS_y += ../loader/rtk_nand_read.o
endif
all : $(LIBS) $(OBJS_y) $(OBJS_l)	
	cp $(RAM_FILE_ORG) tmp.bin
	lzma e tmp.bin tmp.bin.lzma
	$(LD) $(LDFLAGS) -Map $(NAND_BOOT).map -T $(NAND_BOOT).ld -o $(NAND_BOOT).elf $(OBJS_y) $(LIBS)
	$(OBJCOPY) -S --gap-fill=255 -O binary  -R .bss $(NAND_BOOT).elf $(NAND_BOOT).bin

ifeq ($(CONFIG_RTL8685),y)	
	$(LD) $(LDFLAGS) -Map $(NAND_BOOT_B).map -T $(NAND_BOOT_B).ld -o $(NAND_BOOT_B).elf $(OBJS_l) $(LIBS)
	$(OBJCOPY) -S --gap-fill=255 -O binary  -R .bss $(NAND_BOOT_B).elf $(NAND_BOOT_B).bin
endif
ifeq ($(CONFIG_NAND_PAGE_2K),y)
	gcc append.c -o append
ifeq ($(CONFIG_RTL8685),y)	
	./append $(NAND_BOOT).bin $(NAND_BOOT).pad.bin 16384 0xaa
	./append $(NAND_BOOT_B).bin $(NAND_BOOT_B).pad.bin 16384 0xaa
	cat $(NAND_BOOT).pad.bin $(NAND_BOOT_B).pad.bin > nboot_AB.bin
	./append nboot_AB.bin nboot_pad.bin 131072 0xaa
else
	./append nboot.bin nboot_pad.bin 131072 0xaa
endif
ifeq ($(CONFIG_CHECK_HDR),y)
	../genhead -i tmp.bin.lzma -o vmload.hdr -k 0x6C6F6164 -f 0xbfc00000  -a 0x80000000 -e 0x80000000
	cat vmload.hdr tmp.bin.lzma >tmp.bin.vmhdr.lzma
	./append tmp.bin.vmhdr.lzma tmp_pad.bin.lzma 131072 0xbb
else
	./append tmp.bin.lzma tmp_pad.bin.lzma 131072 0xbb
endif
	cat nboot_pad.bin tmp_pad.bin.lzma > nboot-load_2k.bin
endif

ifeq ($(CONFIG_NAND_PAGE_512),y)
	gcc append.c -o append
ifeq ($(CONFIG_RTL8685),y)	
	./append $(NAND_BOOT).bin $(NAND_BOOT).pad.bin 16384 0xaa
	./append $(NAND_BOOT_B).bin $(NAND_BOOT_B).pad.bin 16384 0xaa
	cat $(NAND_BOOT).pad.bin $(NAND_BOOT_B).pad.bin > nboot_AB.bin
	mv nboot_AB.bin nboot_pad.bin
else	
	./append nboot.bin nboot_pad.bin 16384 0xaa
endif
ifeq ($(CONFIG_CHECK_HDR),y)
	../genhead -i ./tmp.bin.lzma -o vmload.hdr -k 0x6C6F6164 -f 0xbfc00000 -a 0x80000000 -e 0x80000000
	cat vmload.hdr ./tmp.bin.lzma >tmp.bin.vmhdr.lzma
	./append tmp.bin.vmhdr.lzma tmp_pad.bin.lzma 65536 0xbb
else
	./append tmp.bin.lzma tmp_pad.bin.lzma 65536 0xbb
endif
	cat nboot_pad.bin tmp_pad.bin.lzma > nboot-load_512.bin
endif

else #!CONFIG_NAND_FLASH

ifeq ($(CONFIG_NO_FLASH),y)
LDSCRIPT=boot-rtl8685-slave.ld
else
ifeq ($(CONFIG_MIPS),y)
LDSCRIPT=boot-mips.ld
else
ifeq ($(CONFIG_BOOT_MIPS),y)
LDSCRIPT=boot-rtl8685sb-rlx.ld
else
LDSCRIPT=boot.ld
endif #CONFIG_BOOT_MIPS
endif #CONFIG_MIPS
endif #CONFIG_NO_FLASH

ifeq ($(CONFIG_MIPS),y)
ifeq ($(CONFIG_MIPS_RUNS_LOADER),y)
all : $(LIBS) $(OBJS_y)
	$(LD) $(LDFLAGS) -Map $(BOOTFILE).map -T $(LDSCRIPT) -o $(BOOTFILE).elf $(OBJS_y)
	$(OBJCOPY) -S --gap-fill=255 -O binary $(BOOTFILE).elf $(BOOTFILE).bin
	./append boot.bin boot_pad.bin 8192 0xff
	cat boot_pad.bin ../secboot/secboot.bin > mips.bin
else
all : $(LIBS) $(OBJS_y)
	$(LD) $(LDFLAGS) -Map $(BOOTFILE).map -T $(LDSCRIPT) -o $(BOOTFILE).elf $(OBJS_y)
	$(OBJCOPY) -S --gap-fill=255 -O binary $(BOOTFILE).elf $(BOOTFILE).bin
	cp $(BOOTFILE).bin mips.bin
endif #CONFIG_MIPS_RUNS_LOADER
else
all : $(LIBS) $(OBJS_y) $(RAM_FILE_ORG)
	cp $(RAM_FILE_ORG) tmp.bin
	#gzip -f9 tmp.bin
	#$(BIN2ASM) tmp.bin.gz > ramfile.s
	#$(CC) $(AFLAGS) -o $(RAM_FILE) ramfile.s
	lzma e tmp.bin tmp.bin.lzma
	$(BIN2ASM) tmp.bin.lzma > ramfile.s
	$(CC) $(AFLAGS) -o $(RAM_FILE) ramfile.s
	$(LD) $(LDFLAGS) -Map $(BOOTFILE).map -T $(LDSCRIPT) -o $(BOOTFILE).elf $(OBJS_y) $(RAM_FILE) $(LIBS)
	$(OBJCOPY) -S --gap-fill=255 -O binary $(BOOTFILE).elf $(BOOTFILE).bin
	#cp boot.bin /tftpboot
	../genhead  -i ./boot.bin -o vm.hdr -k 0xB0010002 -f 0xbfc00000  -a 0x80000000 -e 0x80000000
	cat vm.hdr ./boot.bin >8672boot.bin
ifeq ($(CONFIG_BOOT_MIPS),y)
	./append boot.bin boot_pad.bin 131072 0xff
	cat ./boot_pad.bin ../mips.bin >rtl8685sb.bin
endif #CONFIG_BOOT_MIPS
endif #CONFIG_MIPS

endif #CONFIG_NAND_FLASH			
clean :
	$(RM) $(RMFLAGS) $(OBJS_y) $(OBJS_l) *.elf *.bin *.bak *~ ramfile.s $(RAM_FILE) *.map *.lzma tmp.bin *.hdr

%.o : %.s
	$(CC) $(AFLAGS) -o $@ $<




