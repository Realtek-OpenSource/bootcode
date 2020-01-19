*****************************************************************
*
*  Boot loader - Readme.txt
*
* History:
* 	2006/08/17 SH,Lee  Added System Configuration 
*	2008/12/24 SH,Lee  Information about RTL8672 platform
*
*****************************************************************

- Build Procedures

build procedures for v00.01.05 and above:
1.in Linux server, run 
  $cd boot72
  $make cleanall
  $make
  you will get boot.elf for ICE and boot.bin for rom

build procedures:
1.use cygwin, linux can not build, it has _gp problem
2.in cygwin, run 
  make cleanall
3.in cygwin, run
  make
  you will get boot.elf for ICE and boot.bin for rom

For cygwin build. the util/bin2as.exe has bug in some 
cygwin version use gcc to build. I re-build bin2as 
with MSVC6.0. Change the line
BIN2ASM = ../util/bin2as
to
BIN2ASM = ../util/win/Release/bin2as
in inc/config.mk

*boot0(for OSK verify).bin is for old OSK to download
 code. change the entry to 0x80080000.

*boot014(lzma int phy pci vB-tylo test).bin is for 
 board hang test, it place decompressed image to 0x80000000
 instead of the address which stored in "entry".



- System Configurations

For RTL8671 platform:

1. Flash and SDRAM size configuration

   Define the variable MCR0_VAL of inc/board.h to the proper value.

   As below, Flash and SDRAM size will be configured as 8M and 16M, respectively.
	#define MCR0_VAL        0xe2800000      //8M/16M
	//#define MCR0_VAL      0x52800000      //2M/8M
	//#define MCR0_VAL      0x62800000      //2M/16M
	//#define MCR0_VAL      0xA2800000      //4M/16M


2. Internel/Externel ETH PHY selection and PCI support

   In inc/board.h, modify the definition of SICR_VAL:
	//#define SICR_VAL      0xb7053a19    //for external phy & PCI
	#define SICR_VAL        0xb7053819    //for internal phy & PCI
   This config will support PCI and internel ETH PHY.


3. SAMSUNG SDRAM supports
   
   Enable the definition "#define SAMSUNG_SUPPORT" in inc/board.h to support
SAMSUNG SDRAM.


4. Mini-model supports

   Due to mini-model using GPA6 as AA21, GPA6 will default configure to output 0 as 
   using the lower 4M flash address.
   
   Enable the definition "#define MINIMODEL_SUPPORT" in inc/board.h to support 
   mini-model board.



For RTL8672 platform:

1. Platform Definition

    In inc/config.mk, define "BOARD" to specify the working platform.
    Set "e8b" for E8B project, the others please set "combo".


2. CPU and LX BUS Clock Setting

  CPU Clock:
    In inc/board.h, define "CPU_CLOCK_340MHZ" in AFLAG for setting CPU clock to 340Mhz.
    Otherwise(not defined), CPU will stay the default setting 400Mhz.

  LX BUS Clock:
    In inc/board.h, define "SYS_CLOCK_175MHZ" in CFLAG for setting LX bus clock to 175Mhz.
    Otherwise, LX bus will stay the default setting, 200Mhz.

  SDRAM Clock:
    In inc/board.h, define "SDRAM_CLOCK_116MHZ" for setting SDRAM clock to 116Mhz and define "SDRAM_CLOCK_166MHZ" for setting it to 166Mhz. Otherwise, SDRAM Clock will stay the default setting 133Mhz.

*Notice: Baud rate of UART output is calculated according to LX bus clk. 
         The setting of LX bus clock should be same in both bootloader and runtime code!

3. Configure Memory and Flash Size
  
  Memory Size:
    Set the proper definition "MCR0_VAL" in inc/board.h.
    If the size of SDRAM is 32MB, please define the flag "CONFIG_32M_SDRAM" in inc/board.h".
   
  Flash Size:
    Set the proper definition "BOARD_PARAM_FLASHSIZE" in inc/board.h.
    
    #define BOARD_PARAM_FLASHSIZE       0xfffe8000 //4M     0xfffb8000 //2M

    
4. Internal/External Eth PHY Selection

    Set the proper definiton "BOARD_PARAM_MIISEL" in inc/board.h.
    Using internal PHY as defualt setting, 0.
    
    #define BOARD_PARAM_MIISEL      0   // Using Int. PHY
    

5. Notices for DDR (2010/02/24 by Kevin)
    1. Fix clock rate as 166Mhz when using DDR.
    2. 25Mout/GPA6 select, default 25Mout for RTL8306N (n12=0)
    3. Module enable
    4. PCIe MDIO and MAC reset 
    5. Band gap and ethernet 100Mhz output voltage
    6. Initial signal, send to DDR chipset.
    7. Calibration for DDR DQS signals.
   