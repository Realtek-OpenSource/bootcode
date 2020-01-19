#
# Copyright c       Realtek Semiconductor Corporation, 2002
# All rights reserved.                                                    
#
# $Author: yachang $
#
#   PURPOSE: makefile to build self-decompressed module                            *
#
#   History:
#    2006/10/27  SH, Lee  Add ARCH for big/minimodel support
#
include ../inc/config.mk
include ../.config


OBJS = crt0.o main.o dlmalloc.o uart.o decomp.o rtprintf.o libc.o \
	tick.o loader.o cmd.o xmodem.o gpio.o
ifneq ($(CONFIG_NO_FLASH), y)
OBJS += amd29LvMtd.o
endif
ifeq ($(CONFIG_SPI_NAND_FLASH), y)
OBJS += spi_nand.o
OBJS += rtk_nand_base.o
OBJS += rtk_nand_read.o
endif
ifeq ($(CONFIG_RTK_NORMAL_BBT), y)
OBJS += rtk_bbt.o
endif
ifeq ($(CONFIG_RTK_REMAP_BBT), y)
OBJS += rtk_bbt.o
endif

LIBS = ../zlib/inflateLib.a
ifneq ($(CONFIG_NO_NET), y)
LIBS += ../tftpnaive/tftpnaive.a
endif
ifneq ($(CONFIG_NO_HTTP_SERVER), y)
LIBS += ../tcp/tcp.a
endif


#ifeq ($(BOARD), combo)
ifeq ($(CONFIG_SPI_FLASH), y)
OBJS += spi_flash.o
endif
#endif

#ccwei
ifeq ($(CONFIG_NAND_FLASH), y)
OBJS += rtk_nand.o rtk_nand_read.o rtk_nand_base.o
endif
#endif

ifeq ($(CONFIG_RE8306), y)
OBJS += rtl8306.o
endif

ifeq ($(CONFIG_PCIE_HOST), y)
OBJS += pcie_host.o
endif

ifeq ($(ARCH), big_model)
OBJS += rtl8306.o gpio.o
endif

ifeq ($(CONFIG_RTL8685), y)
OBJS += ../boot/memctl/board_mem_diag.o
ifeq ($(CONFIG_RTL8685_MEMCTL_CHK), y)
OBJS += ../boot/memctl/memctl_8685.o ../boot/memctl/memctl_utils.o ../boot/memctl/memctl_cali_dram.o ../boot/memctl/memctl_check.o
endif
endif

ifeq ($(CONFIG_RTL8685S), y)
OBJS += ../boot/memctl/board_mem_diag_8685s.o
ifeq ($(CONFIG_RTL8685_MEMCTL_CHK), y)
OBJS += ../boot/memctl/memctl_8685.o ../boot/memctl/memctl_utils.o ../boot/memctl/memctl_cali_dram_8685s.o ../boot/memctl/memctl_check.o
endif
endif

ifeq ($(CONFIG_RTL8685SB), y)
OBJS += ../boot/memctl/board_mem_diag_8685s.o
ifeq ($(CONFIG_RTL8685_MEMCTL_CHK), y)
ifeq ($(CONFIG_BOOT_MIPS), y)
OBJS += ../boot/memctl/memctl_utils.o ../boot/memctl/memctl_check.o
else
OBJS += ../boot/memctl/memctl_8685.o ../boot/memctl/memctl_utils.o ../boot/memctl/memctl_cali_dram_8685s.o ../boot/memctl/memctl_check.o
endif #CONFIG_BOOT_MIPS
endif #CONFIG_RTL8685_MEMCTL_CHK
ifeq ($(CONFIG_MIPS), y)
OBJS += ../boot/memctl/mips_cache_ops.o
endif
ifeq ($(CONFIG_BTG_TEST), y)
OBJS += bus_traffic_generator.o
endif
endif #CONFIG_RTL8685SB

ifeq ($(CONFIG_MEM_TEST), y)
OBJS += ../boot/memctl/dram_test.o
ifeq ($(CONFIG_RTL8676S), y)
OBJS += ../boot/memctl/flash_test.o
endif
endif



all : $(LIBS) $(OBJS)
	$(LD) $(LDFLAGS) -Ttext $(LOADER_START) -Map $(LOADERFILE).map -T $(LOADERFILE).ld -o $(LOADERFILE).elf $(OBJS) $(RAM_FILE) $(LIBS)
	$(OBJCOPY) -S --gap-fill=0 -O binary $(LOADERFILE).elf $(LOADERFILE).bin
	
clean :
	$(RM) $(RMFLAGS) $(OBJS) *.elf *.bin *.bak *~ *.map *.gz *.o

crt0.o : crt0.s
	$(CC) $(AFLAGS) -o crt0.o crt0.s




