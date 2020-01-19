#
#  config.mk  Makefile Configuration
#
# History:
# 2006/10/27  SH, Lee  Added "ARCH" for the specific model support
# 2007/03/06  SH  Added specify CPU type definition
include ../.config
BOOTFILE = boot
ROOTFILE = root
LOADERFILE = loader

BOARD = combo
#BOARD = e8b
ifeq ($(BOARD), e8b)
CONFIG_NEW_FLASH_LAYOUT=y
endif

#using 4M or 32M flash space
ifeq ($(CONFIG_NAND_FLASH),y)
FLASH_START	= bfc00000
else
ifeq ($(CONFIG_NO_FLASH),y)
FLASH_START	= bfc00000
else
ifeq ($(CONFIG_RTL8685S),y)
FLASH_START	= b4000000
else
ifeq ($(CONFIG_RTL8685SB),y)
FLASH_START	= bfc00000
else
FLASH_START	= bd000000
endif #CONFIG_RTL8685SB
endif #CONFIG_RTL8685S
endif #CONFIG_NO_FLASH
endif #CONFIG_NAND_FLASH

ifeq ($(CONFIG_MIPS_RUNS_LOADER),y)
MEM_START       = 81000000
else
MEM_START	= 80000000
endif

# ql:enlarge mem from 8M to 16M
ifeq ($(BOARD), e8b)
ifeq ($(CONFIG_NEW_FLASH_LAYOUT),y)
MEM_LIMIT	= 80F00000
LOADER_START	= 80E00000
else
MEM_LIMIT	= 80800000
LOADER_START	= 80700000
endif
else
#select only one of these options
ifeq ($(CONFIG_MIPS_RUNS_LOADER),y)
CONFIG_MEM_8M=n
CONFIG_MEM_16M=n
CONFIG_MEM_32M=y
CONFIG_MEM_64M=n
else
CONFIG_MEM_8M=n
CONFIG_MEM_16M=y
CONFIG_MEM_32M=n
CONFIG_MEM_64M=n
endif

ifeq ($(CONFIG_MEM_8M),y)
MEM_LIMIT	= 80800000
LOADER_START	= 80700000
endif
ifeq ($(CONFIG_MEM_16M),y)
MEM_LIMIT       = 81000000
LOADER_START    = 80F00000
endif
ifeq ($(CONFIG_MEM_32M),y)
MEM_LIMIT       = 82000000
LOADER_START    = 81F00000
endif
ifeq ($(CONFIG_MEM_64M),y)
MEM_LIMIT       = 84000000
LOADER_START    = 83F00000
endif
endif

# big_model: Supports big model
# mini_model: Supports mini model
# No specify: Supports noraml demo board 
#ARCH = big_model
ARCH = 8672_fpga
#ARCH = mini_model

#switch support
SWITCH = rtl865x

EFLAGS = -DMEM_START=0x$(MEM_START) -DMEM_LIMIT=0x$(MEM_LIMIT) -DFLASH_START=0x$(FLASH_START) -DLOADER3_START=0x$(LOADER_START) \
         -D_LZMA_IN_CB
ifneq ($(CONFIG_NO_HTTP_SERVER), y)
EFLAGS += -DCONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
endif

#CFLAGS = -O1 -EB -G 0 -mips1 -mcpu=r3000 -c -x c -Wall \

#CFLAGS = -O1 -EB -G 0 -march=4180 -c -x c -Wall 
#CFLAGS = -O1 -EB -G 0 -march=5281 -c -x c -Wall 
CFLAGS = -O1 -EB -G 0 -c -x c -Wall \
		-pedantic -nostartfiles -nodefaultlibs -nostdlib -mno-abicalls -fno-pic -pipe \
		-include $(ROOTDIR)/autoconf.h \
		$(EFLAGS) \
		-I. -I../inc -I../zlib -I../bzlib 

#AFLAGS = -O0 -EB -mips1 -mcpu=r3000 -c -Wall -x assembler-with-cpp \

#AFLAGS = -O0 -EB -march=4180 -c -Wall -x assembler-with-cpp 
#AFLAGS = -O0 -EB -march=5281 -c -Wall -x assembler-with-cpp 
AFLAGS = -O0 -EB -c -Wall -x assembler-with-cpp \
		-pedantic -nostartfiles -nostdinc -mno-abicalls -fno-pic -pipe \
		-include $(ROOTDIR)/autoconf.h \
		$(EFLAGS) \
		-I. -I../inc -I../zlib 

CFLAGS += -g -gstabs+ 
AFLAGS += -g -gstabs+ 

#shlee add for enabling re8306
ifeq ($(ARCH), big_model)
AFLAGS += -DFlash_AA21_GPA5 -DCPU_LX4181
CFLAGS += -DCONFIG_RE8306 -DCONFIG_RTL8671  -DCONFIG_RE8305 -DFlash_AA21_GPA5 -DCPU_LX4181
endif
ifeq ($(ARCH), mini_model)
AFLAGS += -DMINIMODEL_SUPPORT
CFLAGS += -DMINIMODEL_SUPPORT
endif
#tylo, for 8672 fpga
ifeq ($(ARCH), 8672_fpga)
AFLAGS += -DRTL8672 -DCPU_LX4181
CFLAGS += -DRTL8672 -DCPU_LX4181
endif

ifeq ($(SWITCH), rtl865x)
AFLAGS += -DCONFIG_RTL8196C -DCONFIG_RTL8196D -DCONFIG_RTL865XC
CFLAGS += -DCONFIG_RTL8196C -DCONFIG_RTL8196D -DCONFIG_RTL865XC -I../tftpnaive/sw/include
ifeq ($(CONFIG_EXTS_RTL8367B),y)
CFLAGS += -I../tftpnaive/rtl8367b -DCHIP_RTL8367RB
endif
endif


#ql:20080721 START: add to support all IC
#CFLAGS += -DMULTI_IC_SUPPORT
#ql:20080721 END

#jim add to support e8b case
ifeq ($(BOARD), e8b)
ifeq ($(CONFIG_NEW_FLASH_LAYOUT),y)
CFLAGS += -DNEW_FLASH_LAYOUT
endif
CFLAGS += -DE8B_SUPPORT
#CFLAGS += -DCOPY_ON_DECOMPRESS
#jim add user account during web upgrade
CFLAGS += -DWEB_ACCOUNT_SUPPORT
CFLAGS += -DIMAGE_DOUBLE_BACKUP_SUPPORT
#CFLAGS += -DIMAGE_SINGLE_BACKUP_SUPPORT
CFLAGS += -DCONFIG_SPANSION_16M_FLASH
#CFLAGS += -DBANK_AUTO_SWITCH
CFLAGS += -DSUPPORT_DIRECT_16M_ADDR
CFLAGS += -DBOOT_FROM_NEWEST_IMAGE
#CFLAGS += -DSUPPORT_Z_NEW_LAYOUT
#CFLAGS += -DCONFIG_SPI_FLASH
endif

#CFLAGS += -DCONFIG_SPI_FLASH

ifeq ($(BOARD), combo)
CFLAGS += -DCOMBO

#CFLAGS += -DCONFIG_SPI_FLASH
#for link up RTL8306N, set it when using RTL8306N
CFLAGS += -DRTL8306N  
#for rl6166 ddr-133 and ddr-166 system configuration
#AFLAGS += -DDDR_133
#AFLAGS += -DDDR_166
#CFLAGS += -DBANK_AUTO_SWITCH
#CFLAGS += -DCONFIG_SPANSION_16M_FLASH

#--- Start: open these to support dual-image
ifeq ($(CONFIG_IMAGE_DUAL),y)
CFLAGS += -DNEW_FLASH_LAYOUT
CFLAGS += -DIMAGE_DOUBLE_BACKUP_SUPPORT
CFLAGS += -DSUPPORT_DIRECT_16M_ADDR
CFLAGS += -DBOOT_FROM_NEWEST_IMAGE
endif
#--- End: open these to support dual-image

endif

CFLAGS += -DSDRAM_AUTO_DETECT
#CFLAGS += -DFLASH_AUTO_DETECT
AFLAGS += -DSDRAM_AUTO_DETECT
#CFLAGS += -DSUPPORT_MULT_UPGRADE

ifeq ($(CONFIG_MIPS),y)
CC = $(CONFIG_MIPS_GCC_PATH_PREFIX)gcc
LD = $(CONFIG_MIPS_GCC_PATH_PREFIX)ld
AS = $(CONFIG_MIPS_GCC_PATH_PREFIX)as
AR = $(CONFIG_MIPS_GCC_PATH_PREFIX)ar
NM = $(CONFIG_MIPS_GCC_PATH_PREFIX)nm
CPP= $(CONFIG_MIPS_GCC_PATH_PREFIX)g++
OBJCOPY = $(CONFIG_MIPS_GCC_PATH_PREFIX)objcopy
else
CC = $(CONFIG_GCC_PATH_PREFIX)gcc
LD = $(CONFIG_GCC_PATH_PREFIX)ld
AS = $(CONFIG_GCC_PATH_PREFIX)as
AR = $(CONFIG_GCC_PATH_PREFIX)ar
NM = $(CONFIG_GCC_PATH_PREFIX)nm
CPP= $(CONFIG_GCC_PATH_PREFIX)g++
OBJCOPY = $(CONFIG_GCC_PATH_PREFIX)objcopy
endif
RM	= rm -f

#BIN2ASM = ../util/win/Release/bin2as
#APPEND0 = ../util/win/Release/append0
BIN2ASM = ../util/bin2as
APPEND0 = ../util/win/Release/append0
