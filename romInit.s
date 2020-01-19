/*
 * romInit.s  System initialization/brings up assembly routine
 *
 * History:
 *    2006/10/27 SH, Lee  Add big model support
 *    2006/10/30 SH, Lee  Add determine memory configuration
 *    2007/03/06 SH  Added Dcache write invalidate for CPU LX4181
 *    2008/08/01 SH	 Modify MAC default delay as 0x408
 *    2009/04/28 SH  Turns on NICLED1_MA22/NICLED0_MA21 if flash size >4MB
 *		     Merge SDRAM_AUTO_DETECT feature
*/
#define _ASMLANGUAGE
#include "eregdef.h"
#include "board.h"

#if defined(CONFIG_RAM_166)
#define RL6166_RAM_CLK		0
#define RL6028B_RAM_CLK		3
#define RL6085_RAM_CLK		3
#elif defined(CONFIG_RAM_133)
#define RL6166_RAM_CLK		7
#define RL6028B_RAM_CLK		0
#define RL6085_RAM_CLK		0
#else
#error "RAM Clock undefined"
#endif

	.extern C_Entry

#define RELOC(toreg,address) \
        bal     9f; \
9:; \
        la      toreg,address; \
        addu    toreg,ra; \
        la      ra,9b; \
        subu    toreg,ra

#define RVECENT(f,n) \
	b f; nop
#define XVECENT(f,bev) \
	b f; li k0,bev
	
#define CALL(proc)	\
	jal	proc;		\
	nop

	/* For hard or soft reset, jump forward to handler. 
	   otherwise, jump to hard reset entry point, unless 
	   exception handler is called. */

	.globl romInit
	
	.set noreorder
romInit:
_romInit:
	b	__romInit
	nop
romWarmInit:
_romWarmInit:
	b	__romReboot
	nop
	nop #tylo, for 8672 check
	nop
	RVECENT(_romInit,3)
	RVECENT(_romInit,4)
	RVECENT(_romInit,5)
	RVECENT(_romInit,6)
	RVECENT(_romInit,7)
	RVECENT(_romInit,8)
	RVECENT(_romInit,9)
	RVECENT(_romInit,10)
	RVECENT(_romInit,11)
	RVECENT(_romInit,12)
	RVECENT(_romInit,13)
	RVECENT(_romInit,14)
	RVECENT(_romInit,15)
	RVECENT(_romInit,16)
	RVECENT(_romInit,17) 
	RVECENT(_romInit,18)
	RVECENT(_romInit,19)
	RVECENT(_romInit,20)
	RVECENT(_romInit,21)
	RVECENT(_romInit,22)
	RVECENT(_romInit,23)
	RVECENT(_romInit,24)
	RVECENT(_romInit,25)
	RVECENT(_romInit,26)
	RVECENT(_romInit,27)
	RVECENT(_romInit,28)
	RVECENT(_romInit,29)
	RVECENT(_romInit,30)
	RVECENT(_romInit,31)
	RVECENT(_romInit,33)
	RVECENT(_romInit,34)
	RVECENT(_romInit,35)
	RVECENT(_romInit,36)
	RVECENT(_romInit,37)
	RVECENT(_romInit,38)
	RVECENT(_romInit,39)
	RVECENT(_romInit,40)
	RVECENT(_romInit,41)
	RVECENT(_romInit,42)
	RVECENT(_romInit,43)
	RVECENT(_romInit,44)
	RVECENT(_romInit,45)
	RVECENT(_romInit,46)
	RVECENT(_romInit,47)
	RVECENT(_romInit,48)
	/* This needs to be at offset 0x180 */
	b	romExcHandle
	li	k0, 0x180
	RVECENT(_romInit,49)
	RVECENT(_romInit,50)
	RVECENT(_romInit,51)
	RVECENT(_romInit,52)
	RVECENT(_romInit,53)
	RVECENT(_romInit,54)
	RVECENT(_romInit,55)
	RVECENT(_romInit,56)
	RVECENT(_romInit,57)
	RVECENT(_romInit,58)
	RVECENT(_romInit,59)
	RVECENT(_romInit,60)
	RVECENT(_romInit,61)
	RVECENT(_romInit,62)
	RVECENT(_romInit,63)	
	RVECENT(_romInit,64)
	RVECENT(_romInit,65)
	RVECENT(_romInit,66)
	RVECENT(_romInit,67)
	RVECENT(_romInit,68)
	RVECENT(_romInit,69)
	RVECENT(_romInit,70)
	RVECENT(_romInit,71)
	RVECENT(_romInit,72)
	RVECENT(_romInit,73)
	RVECENT(_romInit,74)
	RVECENT(_romInit,75)
	RVECENT(_romInit,76)
	RVECENT(_romInit,77)
	RVECENT(_romInit,78)
	RVECENT(_romInit,79)	
	RVECENT(_romInit,80)
	RVECENT(_romInit,81)
	RVECENT(_romInit,82)
	RVECENT(_romInit,83)
	RVECENT(_romInit,84)
	RVECENT(_romInit,85)
	RVECENT(_romInit,86)
	RVECENT(_romInit,87)
	RVECENT(_romInit,88)
	RVECENT(_romInit,89)
	RVECENT(_romInit,90)
	RVECENT(_romInit,91)
	RVECENT(_romInit,92)
	RVECENT(_romInit,93)
	RVECENT(_romInit,94)
	RVECENT(_romInit,95)	
	RVECENT(_romInit,96)
	RVECENT(_romInit,97)
	RVECENT(_romInit,98)
	RVECENT(_romInit,99)
	RVECENT(_romInit,100)
	RVECENT(_romInit,101)
	RVECENT(_romInit,102)
	RVECENT(_romInit,103)
	RVECENT(_romInit,104)
	RVECENT(_romInit,105)
	RVECENT(_romInit,106)
	RVECENT(_romInit,107)
	RVECENT(_romInit,108)
	RVECENT(_romInit,109)
	RVECENT(_romInit,110)
	RVECENT(_romInit,111)
	RVECENT(_romInit,112)
	RVECENT(_romInit,113)
	RVECENT(_romInit,114)
	RVECENT(_romInit,115)
	RVECENT(_romInit,116)
	RVECENT(_romInit,116)
	RVECENT(_romInit,118)
	RVECENT(_romInit,119)
	RVECENT(_romInit,120)
	RVECENT(_romInit,121)
	RVECENT(_romInit,122)
	RVECENT(_romInit,123)
	RVECENT(_romInit,124)
	RVECENT(_romInit,125)
	RVECENT(_romInit,126)
	RVECENT(_romInit,127)

	################################################################################
	#  Noop_Delay  <iteration> <register,default T1>
	################################################################################
	.macro Noop_Delay iter reg=t1
		li	\reg,\iter
		1:
		subu		\reg,1
		nop	
		bnez		\reg,1b
		nop			
	.endm
	
	################################################################################
	#  Detect_Clock_Source - Detect clock source and store result to v0
	#    v0=1 : 35.328Mhz
	#    v0=0 : 25Mhz
	################################################################################
	.macro Detect_Clock_Source
		li		t1,0xb8003308
		lw		t0,0(t1)
		nop
		srl		t0,t0,21
		and		v0,t0,1		
	.endm
	
	################################################################################
	#  Mem_Set_Word <StartAddr> <Size> <Pattern>  - Fill memory with pattern
	################################################################################
	.macro Mem_Set_Word addr size patt
		#define	CLR_TEMP1	t0
		#define	CLR_TEMP2	t2
		#define	CLR_TEMP3	t3
		li		CLR_TEMP1,\addr				//start address
		addiu	CLR_TEMP2,CLR_TEMP1,\size		//2k bytes, end address
		li		CLR_TEMP3,\patt
		1:	
		sw		CLR_TEMP3,0(CLR_TEMP1)
		addi	CLR_TEMP1,CLR_TEMP1,4
		bne		CLR_TEMP1,CLR_TEMP2,1b
		nop		
		#undef	CLR_TEMP1	
		#undef	CLR_TEMP2	
		#undef	CLR_TEMP3	
	.endm
	
	################################################################################
	#  USB_MacOS_fix
	################################################################################
	.macro USB_MacOS_fix
	#---cathy, modem power on issue in MAC OS
	#---set reg(0xf2)=0x10; to remove pull-up resistor
		li		t0,0x6110e100
		li		t1,0xb8003314
		sw		t0,0(t1)
		nop
		li		t0,0x10024002
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		li		t0,0x10024000
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		li		t0,0x10024002
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		li		t0,0x100F4002
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		li		t0,0x100F4000
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		li		t0,0x100F4002
		li		t1,0xb8030034
		sw		t0,0(t1)
		nop
		
		#---set reg(0xf0)=0xfc
		li      t0,0x61fce100
		li      t1,0xb8003314
		sw      t0,0(t1)
		nop
		li      t0,0x10004002
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
		li      t0,0x10004000
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
		li      t0,0x10004002
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
		li      t0,0x100F4002
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
		li      t0,0x100F4000
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
		li      t0,0x100F4002
		li      t1,0xb8030034
		sw      t0,0(t1)
		nop
	
	.endm // USB_reset_fix
	
	################################################################################
	#  Detect_RAM_Size
	################################################################################
	#	 here we auto detect the sdram size. according Designer's info's on memory controller behavior
	#	 use address range = 0x0000_0000 ~ 0x03FF_FFFF
	#	input address 0xA3F0_1234 => physical address = 0x03F0_1234
	#	predefine 16bits (bytes) DRAM => DRAM address = 0x03F0_1234 / 2 = 0x1F8_091A
	#	column address range a0~a9 (0x400) => 0x1F8_091A mod 0x400 = 11A (COL address)
	#	                                                0x1F8_091A / 0x400 = 0x7E02 ( for ROW / BANK)
	#	row address range a0~a12 (0x2000)  => 0x7E02 mod 0x2000 = 1E02 (ROW address)
	#                                                                    0x7E02 / 0x2000 = 3 (BANK address)
	# we have conclusion on MCR with 64MB setting:
	#	2MB *(0xa3f01234) == *(0xa1f01234)
	#	8MB *(0xa3f01234) == *(0xA3F01034)
	#	16MB *(0xa3f01234) == *(0xA3701634)
	#	32MB *(0xa3f01234) == *(0xA3F01634)
	#	64MB *(0xa3f01234) unique
	
	.macro Detect_RAM_Size
		li	t5,0xb8001000		//detect DDR or SDR on board.
		lw	t5,0(t5)
		nop
		and	t6,t5,0x80000000
		nop
		beqz	t6,is_SDRAM_MCR
		nop
		nop
		li 	t6, 0x00030000
		b	is_DDR_MCR
		nop
		nop
is_SDRAM_MCR:
		li	t6, 0x00000000		
is_DDR_MCR:		
		li  	t0, 0xb8001004
		or	t1, t6, 0x54880000 
		
		#set MCR to 64MB setting temprory
		sw	t1, 0(t0)
		nop
		li	t2, 0xAAAA5555
		li	t3, 0xA3f01234
		sw	$0, 0(t3)
		li	t3, 0xa1f01234
		sw	$0, 0(t3)
		li	t3, 0xA3F01034
		sw	$0, 0(t3)
		li	t3, 0xA3701634
		sw	$0, 0(t3)
		li	t3, 0xA3F01634
		sw	$0, 0(t3)		
		nop
		li	t3, 0xA3F01234
		sw	t2, 0(t3)
		nop
		li	t3, 0xA1f01234
		lw	t4, 0(t3)
		nop
		beq  t4, t2,  SDRAM_2MB
		nop
		li	t3, 0xA3F01034
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_8MB
		nop
		li	t3, 0xA3701634
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_16MB
		nop
		li	t3, 0xA3F01634
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_32MB
		nop
                # Otherwise, 64MB is the only possible setting
		b    SDRAM_64MB
		nop		
SDRAM_2MB:
		li	t2, 0x50000000
		b	setting_mcr
		nop
SDRAM_8MB:
		li	t2, 0x52080000
		b	setting_mcr
		nop		
SDRAM_16MB:
		or	t2, t6, 0x52480000 
		b	setting_mcr
		nop		
SDRAM_32MB:
		or	t2, t6, 0x54480000 
		b	setting_mcr
		nop		
SDRAM_64MB:
		or	t2, t6, 0x54880000 
		b	setting_mcr
		nop		
setting_mcr:
		li	t3, 0xb8001004
		nop
		sw	t2, 0(t3)
		nop

	.endm // Detect_SDRAM_Size


	
	################################################################################
	#  Reset_PCIE_MDIO_MAC  - PCIe MDIO and MAC reset
	################################################################################
	.macro Reset_PCIE_MDIO_MAC
		#define		REG_ADDR		t0
		#define		REG_DATA		t1
		#define		REG_TEMP		t2
		li		REG_DATA,0x1d400000
		li		REG_ADDR,0xb8003400
		sw		REG_DATA,0(REG_ADDR)
		nop		
		li		REG_DATA,0x1d500000
		li		REG_ADDR,0xb8003400
		sw		REG_DATA,0(REG_ADDR)
		nop		
		li		REG_DATA,0xFFFFFDFF
		li		REG_ADDR,0xb800330c
		lw		REG_TEMP,0(REG_ADDR)
		nop
		and		REG_DATA,REG_DATA,REG_TEMP
		nop
		sw		REG_DATA,0(REG_ADDR)
		nop
		li		REG_DATA,0x200
		li		REG_ADDR,0xb800330c
		lw		REG_TEMP,0(REG_ADDR)
		nop
		or		REG_DATA,REG_DATA,REG_TEMP
		nop
		sw		REG_DATA,0(REG_ADDR)
		nop	
		nop
		#undef		REG_ADDR	
		#undef		REG_DATA
		#undef		REG_TEMP
	.endm

	################################################################################
	#  Internal_Phy_Setup	
	################################################################################	
	# Int. LDO parameters setup 
	.macro Internal_Phy_Setup
		## Set Int. PHY initial value  ######
		#define 	Indirect_REG_ADDR	t0
		#define 	Write_DATA			t1
		#define 	Delay_Count			t2
		li		Indirect_REG_ADDR,0xb801805c	
		li		Write_DATA,0x841f0001		//int-phy page select : change to page 1
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop	
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x84107180		//PHY1:REG16, DATA=0x7180
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x841177b8		//PHY1:REG17, DATA=0x77b8
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x84129004		//PHY1:REG18, DATA=0x9004
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x84134400		//PHY1:REG19, DATA=0x4400
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x84140b20		//PHY1:REG20, DATA=0x0b20
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x8415041f		//PHY1:REG21, DATA=0x041f
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count	
		li		Write_DATA,0x8416035e		//PHY1:REG22, DATA=0x035e
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count	
		li		Write_DATA,0x84170b23		//PHY1:REG23, DATA=0x0b23
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		li		Write_DATA,0x841f0000		//int-phy page select : change to page 0
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
		Noop_Delay 200, Delay_Count
		// For ethernet : force 100M full deplex.
	#if 1
		li		Write_DATA,0x84002100		//For ethernet : force 100M full deplex.
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop
	#endif
		////////for fixed IVDD /////////////////////////////////////////////////
	#if 1
		li		Indirect_REG_ADDR,0xb8003214
		//li		Write_DATA,0x3F1				//for fixed IVDD equ 0.93V
		//li		Write_DATA,0x3F2				//for fixed IVDD equ 0.96V	
		li		Write_DATA,0x3F3				//for fixed IVDD equ 1.00V	
		//li		Write_DATA,0x3F5				//for fixed IVDD equ 1.09V
		sw		Write_DATA,0(Indirect_REG_ADDR)
		nop	
	#endif	
	/////////////////////////////////////////////////////////
		#undef Indirect_REG_ADDR
		#undef Write_DATA
		#undef Delay_Count
	.endm
	
	################################################################################
	#  SRAM_Map	 <Bus Address> <Size> <Base=0>
	#     Size  (0/1/2/3/4/5/6/7/8) -> (128/256/512/1k/2k/4k/8k/16k/32k)
	################################################################################	
	.macro SRAM_Map addr size base=0
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(\addr | 0x00000001)		//last bit it define enable
		li	SRAM_SIZE,\size					//32k bytes
		li	REG_TEMP,0xb8001300				//unmap addr reg.
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001304				//unmap size reg.
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004000				//map addr reg.
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004004				//map size reg.
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,\base					//
		li	REG_TEMP,0xb8004008				//Segment 0 Base reg.
		sw	SRAM_BASE,0(REG_TEMP)	
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm

	
	################################################################################
	#  Mem_Init_RL6085
	################################################################################
	.macro Mem_Init_RL6085
	#if 0
	#ifdef 	OCP570_LX_175_DDR166
		li		t0,0xffef1743		# OCP clock to 570MHz ,sdram=166Mhz,Lxbus=175MHz
	#elif		OCP570_LX_175_DDR133
		li		t0,0xffef1740		# OCP clock to 570MHz ,sdram=166Mhz,Lxbus=175MHz
	#elif		OCP550_LX_175_DDR166
		li		t0,0xffff1543		# OCP clock to 550MHz ,sdram=166Mhz,Lxbus=175MHz	
	#elif		OCP550_LX_175_DDR133
		li		t0,0xffef1540		# OCP clock to 550MHz ,sdram=133Mhz,Lxbus=175MHz		
	#elif		OCP400_LX_175_DDR133
		li		t0,0xffef0040		# OCP clock to 400MHz ,sdram=133Mhz,Lxbus=175MHz
	#elif		OCP400_LX_175_DDR166
		li		t0,0xffef0043		# OCP clock to 400MHz ,sdram=166Mhz,Lxbus=175MHz			
	#endif
	#endif //0
		li		t0,(0xffef0040|RL6085_RAM_CLK)		# OCP clock to 400MHz,Lxbus=175MHz
		li		t1,0xb8003200	
		sw		t0,0(t1)
		Noop_Delay 20
		li		t0, 0x0000140b
		//li		t0, 0x00000406	
		li		t1, 0xb8003304
		sw		t0, 0(t1)
		nop
		li		t0, 0x00028000
		li		t1, 0xb800332C
		sw		t0, 0(t1)
		nop		
		li		t0,0x90060000
		li		t1,0xb8003300
		sw		t0,0(t1)
		nop
		#with packet processor
		#li		t0,0x800008f0
		#without packet processor
		li		t0,0x000000f0
		li		t1,0xb800330c
		sw		t0,0(t1)
		nop
		# SDRAM/DDR Memory timing register, MUST set before MCR(config. reg)	
		li		t0,0x6d0b0ac0
		li		t1,0xb8001008
		sw		t0,0(t1)
		nop
	.endm
	
	
	################################################################################
	#  Mem_Init_RL6028B
	################################################################################
	.macro Mem_Init_RL6028B
		nop
	#============== 0xB8003200[bit 12:8] modification OCP clock to 340MHz ,sdram=116Mhz,original (0xffff0040) ==> (0xffff0644) ==================	
	#	li		t0,0xffff0644		# OCP clock to 340MHz ,sdram=116Mhz,Lxbus=175MHz
	#	li		t0,0xffff0044		# OCP clock to 400MHz ,sdram=116Mhz,Lxbus=175MHz
	#	li		t0,0xffff0040		# OCP clock to 400MHz ,sdram=133Mhz,Lxbus=175MHz	,IAD default use
	#	li		t0,0xffff1540		# OCP clock to 550MHz ,sdram=133Mhz,Lxbus=175MHz	,IAD's OCP clock Overfrequency to 550MHz	
	#	li		t0,0xffff0643		# OCP clock to 340MHz ,sdram=166Mhz,Lxbus=175MHz
	#	li		t0,0xffff0043		# OCP clock to 400MHz ,sdram=166Mhz,Lxbus=175MHz	,RTL8192su(OTG)
	#	li		t0,0xffff0b43		# OCP clock to 450MHz ,sdram=166Mhz,Lxbus=175MHz
	#	li		t0,0xffff0640		# OCP clock to 340MHz ,sdram=133Mhz,Lxbus=175MHz	//defaule use.
	#	li		t0,0xffff0140		# OCP clock to 500MHz ,sdram=133Mhz,Lxbus=175MHz
	#	li		t0,0xffff0143		# OCP clock to 500MHz ,sdram=166Mhz,Lxbus=175MHz
	#	li		t0,0xffff1340		# OCP clock to 530MHz ,sdram=133Mhz,Lxbus=175MHz	
	#	li		t0,0xffff1540		# OCP clock to 550MHz ,sdram=133Mhz,Lxbus=175MHz
	#	li		t0,0xffff1543		# OCP clock to 550MHz ,sdram=166Mhz,Lxbus=175MHz
	#	li		t0,0xffff1840		# OCP clock to 580MHz ,sdram=133Mhz,Lxbus=175MHz
	#	li		t0,0xffff1740		# OCP clock to 570MHz ,sdram=133Mhz,Lxbus=175MHz	
	#if 0
	#ifdef 	OCP570_LX_175_DDR166
		li		t0,0xffff1743		# OCP clock to 570MHz ,sdram=166Mhz,Lxbus=175MHz
	#elif		OCP570_LX_175_DDR133
		li		t0,0xffff1740		# OCP clock to 570MHz ,sdram=133Mhz,Lxbus=175MHz
	#elif		OCP600_LX_175_DDR133
		li		t0,0xffff0240		# OCP clock to 600MHz ,sdram=133Mhz,Lxbus=175MHz
	#elif		OCP600_LX_175_DDR166
		li		t0,0xffff0243		# OCP clock to 600MHz ,sdram=166Mhz,Lxbus=175MHz	
	#elif		OCP550_LX_175_DDR166
		li		t0,0xffff1543		# OCP clock to 550MHz ,sdram=166Mhz,Lxbus=175MHz	
	#elif		OCP550_LX_175_DDR133
		li		t0,0xffff1540		# OCP clock to 550MHz ,sdram=133Mhz,Lxbus=175MHz		
	#elif		OCP520_LX_175_DDR133
		li		t0,0xffff1240		# OCP clock to 550MHz ,sdram=133Mhz,Lxbus=175MHz			
	#elif		OCP400_LX_175_DDR133
		li		t0,0xffff0040		# OCP clock to 400MHz ,sdram=133Mhz,Lxbus=175MHz
	#elif		OCP400_LX_175_DDR166
		li		t0,0xffff0043		# OCP clock to 400MHz ,sdram=166Mhz,Lxbus=175MHz			
	#endif
	#endif
	#	li		t0,0xffff1640		# OCP clock to 560MHz ,sdram=133Mhz,Lxbus=175MHz
	#	li		t0,0xffff1643		# OCP clock to 560MHz ,sdram=166Mhz,Lxbus=175MHz
	#	li		t0,0xffff0240		# OCP clock to 600MHz ,sdram=133Mhz,Lxbus=175MHz	
	#	li		t0,0xffff0600		# OCP clock to 340MHz ,sdram=133Mhz,Lxbus=200MHz		
	#	li		t0,0xffff0000		# OCP clock to 400MHz ,sdram=133Mhz,Lxbus=200MHz		
	#	li		t0,0xffff0060		# OCP clock to 400MHz ,sdram=133Mhz,Lxbus=150MHz for special case (FIB IC)
	
		li		t0,(0xffff0040|RL6028B_RAM_CLK)		# OCP clock to 400MHz Lxbus=175MHz			
		li		t1,0xb8003200	
		sw		t0,0(t1)
		Noop_Delay 20
		li		t0,0x52480000		//16M, for SDRAM & DDR
		//li		t0,0x54480000		//32M, for SDRAM & DDR
		//li		t0,MCR0_VAL
		li		t1,0xB8001004
		sw		t0,0(t1)
		nop
	# SDRAM/DDR Memory timing register, MUST set before MCR(config. reg)
		li		t0,0x6d0b0ac0
		li		t1,0xb8001008
		sw		t0,0(t1)
		nop
	
		li		t0,0x000004F7
		li		t1,0xb8003304
		sw		t0,0(t1)
		nop
		#with packet processor
		#li		t0,0x800008f0
		#without packet processor
		li		t0,0x000000f0
		li		t1,0xb800330c
		sw		t0,0(t1)
		nop
	#if	0			//0: control by software, 1: hardware strapping.
		li		t1,0xb8003300			
		lw		t2,0(t1)
		nop
		li		t0,0x90000000	
		or		t2,t2,t0
		nop
		sw		t2,0(t1)
		nop
	#else
		li		t1,0xb8003300			//control by software, not hardware strapping.
		li		t0,0x90060000			//[n17..n16]=MDMODE[1:0]=00: GPIO (Default) 01: SDRAM MD[31:16],  =10: MAC MII, =11: debug
		//li		t0,0x90050000			//[n17..n16]=MDMODE[1:0]=00: GPIO (Default) 01: SDRAM MD[31:16],  =10: MAC MII, =11: debug
		//li		t0,0x90040000			//[n17..n16]=MDMODE[1:0]=00: GPIO (Default) 01: SDRAM MD[31:16],  =10: MAC MII, =11: debug
		nop
		sw		t0,0(t1)
		nop
	
	#endif
	
		
	#if 0
		li		t1, SCCR
		lw		t0, 0(t1)
		nop	
	#if defined(SYS_CLOCK_175MHZ) || defined(SDRAM_CLOCK_166MHZ) || defined(CPU_CLOCK_340MHZ) || defined(SDRAM_CLOCK_133MHZ)
		srl		t0, t0, 12
		sll		t0, t0, 12
	#if defined(SYS_CLOCK_175MHZ)
		addi		t0, t0, 0x40
	#endif
	#if defined(SDRAM_CLOCK_166MHZ) 
		addi		t0, t0, 0x03
	#endif
	#if defined(SDRAM_CLOCK_133MHZ) 
		nop	
	#endif
	#if defined(CPU_CLOCK_340MHZ)
		addi		t0, t0, 0x600
	#endif
		ori		t0, t0, 0x1500		//#	li		t0,0xffff1543		# OCP clock to 550MHz ,sdram=166Mhz,Lxbus=175MHz
		nop
		sw		t0, 0(t1)
		nop
	#endif
	
	#endif   //if 0

	.endm
	
	
	################################################################################
	#  Mem_Init_RL6166
	################################################################################
	.macro Mem_Init_RL6166
	
		##############################################
		# reg b800_1200: bit31-29,0:RAM clk/2, 1: RAM clk/4, 2: RAM clk/6, 3: RAM clk/8
		# Set default spi clock as RAM clk/6, e.g.200Mhz/6 = 33.333Mhz
		# cathy add
		//for RL6166 only
		li		t0,0x3f800000
		li		t1,0xb8001200
		sw		t0,0(t1)
		nop	
		
		li		t0,0x00000010		
		li		t1,0xb8003338
		sw		t0,0(t1)
		nop		
		li		t0,0x4CC3106D		
		li		t1,0xb8003204
		sw		t0,0(t1)
		nop
		nop
		nop
		Noop_Delay 200
		###############################################		
		li      t1,0xb8003318       
		lw		t0, 0(t1)
		nop	
		and     t0,t0,~0x00000200    //make sure bit9 =0
		sw      t0, 0(t1)
		nop
    #if 0
	## 0xb8003200 n0~3=0
		//li		t0,0xFFDF0017		//340,170,133
		//li		t0,0xFFDF0007		//340,180,133
		//li		t0,0xFFDF0d07		//400,180,133
	#ifdef LX_180_DDR166
		li		t0,0xFFDF0d00		//400,180,166*
	#elif LX_180_DDR150
		li		t0,0xFFDF0d05		//400,180,150
	#elif LX_180_DDR133
		li		t0,0xFFDF0d07		//400,180,133*
	#elif LX_212_DDR133
		li		t0,0xFFDF1c67			// for ext. 25MHz clock input, 400,212.5,133
	#elif LX_212_DDR166
		li		t0,0xFFDF1c60			// for ext. 25MHz clock input, 400,212.5,166	
	#elif LX_212_DDR200
		//li		t0,0xFFDF1c63			// for ext. 25MHz clock input, 400,212.5,200		
		li		t0,0xFFDF0463		// for ext. 25MHz clock input, 500,212.5,200
	#else  //DDR_133
		//li		t0,0xFFDF0d07		//400,180,133*		
	#endif
	#endif //0
		//li		t0,0xFFDF1b37		//260,150,133
		//li		t0,0xFFDF1b07		//260,180,133
		//li		t0,0xFFDF1607		//470,180,133
		//li		t0,0xFFDF1600		//470,180,166
		
		li			t0,(0xFFDF0d00|RL6166_RAM_CLK)
		li		t1,0xb8003200
		sw		t0,0(t1)
		nop
		// following nops are necessary for clk settle after above clk configuration
		nop
		nop
		nop
		nop
		nop
		nop	
		li		t0,0x0100000F		//SDRAM TX/RX delay parameter setup
		//li		t0,0x00c0040f		//SDRAM TX/RX delay parameter setup, for 216pin QA
		li		t1,0xb8003304
		sw		t0,0(t1)
		nop	
		nop	
		#for gmac
		li		t0,0x90002000		//GPA6/25Mout select, default 25Mout [n12=0] 
	//	li		t0,0x90003000		//GPA6/25Mout select, disable 25Mout, as normal gpio pin [n12=1] 
		li		t1,0xb8003300
		sw		t0,0(t1)
		nop
		
		//li		t0,0xfb001fff			//enable all
		//li		t0,0xfb001459		//disable [n11]PKTA, [n9]PCIe, [n8]PCM, [n7]USBPHY0, [n5]USBHOST, [n2]PTM_interleave, [n1]PTM_FAST
		li		t0,0xfb001ff9		//disable [n2]PTM_interleave, [n1]PTM_FAST
		li		t1,0xb800330c
		sw		t0,0(t1)
		nop			
	.endm
	
	
	################################################################################
	#  Mem_Init_RLE0390
	################################################################################
	.macro Mem_Init_RLE0390
		li		t0,0x00000010		
		li		t1,0xb8003338
		sw		t0,0(t1)
		nop		
		li		t0,0x4CC3106D		
		li		t1,0xb8003204
		sw		t0,0(t1)
		nop		
		Noop_Delay 200
		###############################################		
		li      t1,0xb8003318       
		lw		t0, 0(t1)
		nop	
		and     t0,t0,~0x00000200    //make sure bit9 =0
		sw      t0, 0(t1)
		nop
		## 0xb8003200 n0~3=0
	#ifdef 	LX_180_DDR166
		li		t0,0xFFCF0053		//400,180,166*
	#elif		LX_200_DDR166
		li		t0,0xFFCF0003		//400,200,166*
	#elif		LX_200_DDR200	
		li		t0,0xFFCF0006		//400,200,200*
	#elif		LX_200_DDR183	
		li		t0,0xFFCF000a		//400,200,183*
	#elif		LX_200_DDR133	
		li		t0,0xFFCF0000		//400,200,133*
	#elif		LX_180_DDR133	
		li		t0,0xFFCF0050		//400,180,133*
	#else			//DDR_133
		li		t0,0xFFCF0050		//400,180,133*
	#endif
		li		t1,0xb8003200
		sw		t0,0(t1)
		nop
			// following nops are necessary for clk settle after above clk configuration
		nop
		nop
		nop
		nop
		nop
		nop	
		li		t0,0x0000000F		//SDRAM TX/RX delay parameter setup, 
		//li		t0,0x0100000d		//SDRAM TX/RX delay parameter setup, 0xb8003304[n24] patch
		//li		t0,0x0100300d		//SDRAM TX/RX delay parameter setup, 0xb8003304[n24] patch
		//li		t0,0x00c0040f			//SDRAM TX/RX delay parameter setup, for 216pin QA
		li		t1,0xb8003304
		sw		t0,0(t1)
		nop	
		nop	
		#for gmac
		li		t0,0x90002000		//GPA6/25Mout select, default 25Mout [n12=0] 
		//li		t0,0x90003000		//GPA6/25Mout select, disable 25Mout, as normal gpio pin [n12=1] 
		li		t1,0xb8003300
		sw		t0,0(t1)
		nop
		
		//li		t0,0xfb001459			//disable [n11]PKTA, [n9]PCIe, [n8]PCM, [n7]USBPHY0, [n5]USBHOST, [n2]PTM_interleave, [n1]PTM_FAST
		li		t0,0xfb001659			//all enable 
		li		t1,0xb800330c
		sw		t0,0(t1)
		nop
	
		###############################################
		//For Calibration use, clk_m90 delay set to zero [n4..n1]
		li		t1,0xb8003330
		li		t2, 0x0c			//for isRL0390
		nop
		sw              t2, 0(t1)
		nop       	
		nop
		###############################################	
	.endm
	
__romInit:
	.set	noreorder
		
	USB_MacOS_fix
 		
	mfc0    k0,C0_EPC               # save for nvram
	move    k1,ra                   # save for nvram
	mfc0    gp,C0_SR
	mfc0    t7,C0_CAUSE
	li      v0,SR_BEV
	mtc0    v0,C0_SR                # state unknown on reset
	mtc0    zero,C0_CAUSE           # clear software interrupts
	nop                             # paranoia
	.set	reorder

__romReboot:
	/* second entry point -- sw reboot inherits a0 start type            */
	.set	noreorder
	li	t0, SR_CU1 | SR_BEV
	nop
	mtc0	t0, C0_SR		/* disable all interrupts fpa on,    */
	nop
	nop
				   	/* prom exception handlers           */
	mtc0	zero, C0_CAUSE		/* clear all interrupts              */
	nop
	nop
	
	.set	reorder
	#--- bypass copying if in RAM
	

	bal 	1f
	nop
1:

	la		t0, 0x10000000
	nop
	and		t0, ra, t0
	beq		t0, zero, 2f				# decide if ram based on BIT28
	nop

	.set	noreorder
	
	
#ifdef SUPPORT_8672_PLATFORM
	#--- config lexra bus and sdram clock freque
	li		t1, SCCR
	lw		t0, 0(t1)
	nop

# check SCCR bit 21,20 and 16
	srl 		t0, t0, 0x10
	andi		t0, t0, 0x00000031
	beq 		t0, 17, isRL6166 
	nop
    beq			t0, 1, isRLE0390
    nop			
    beq			t0, 48, isRL6028B 
    nop			
    beq			t0, 49, CCC 
    nop
	
isRL6085:
	Mem_Init_RL6085
	b		MEM_CONF_DONE
	nop
	
isRLE0390:
	Mem_Init_RLE0390
	b		INTPHY_SET
	nop
	nop

isRL6166:
	Mem_Init_RL6166
	
#ifdef PCIe_module
	Reset_PCIE_MDIO_MAC
#endif 

INTPHY_SET:
	Internal_Phy_Setup
	
	SRAM_Map 0x600000, 0x8		# map 0x80600000 (32k) to SRAM
	
////////////enable GDMA module 
//enable GDMA module 0xb800330c[n10]=1
	li	t1,0xb800330c	
	lw	t7,0(t1)			//load 0xb800330c
	nop
	ori	t7,t7,0x400		//set 0xb800330c[n10]=1
 	nop
 	sw	t7,0(t1)			//enable GDMA module
 	nop
	
	Mem_Set_Word 0xa0600000,0x800,0x0  // fill 0

###############################################
	li		t0,0xb8001000		//detect DDR or SDR on board.
	lw		t2,0(t0)
	nop
	and		t1,t2,0x80000000
	beqz	t1,is_SDRAM
	nop
	
###############################################
# SDRAM/DDR Memory timing register, MUST set before MCR(config. reg)
#ifdef LX_180_DDR166
	//li        t0,0x910DA4C0       //DDR 166MHz, (4,4,8,13,10,2,3)*
	li        t0,0x6CEC8480		//DDR 166MHz, (3,3,7,12,8,2,2)
#elif		LX_212_DDR166
	li        t0,0x6CEC8480		//DDR 166MHz, (3,3,7,12,8,2,2)	
#elif		LX_200_DDR166
	li        t0,0x6CEC8480		//DDR 166MHz, (3,3,7,12,8,2,2)	
#elif		LX_200_DDR200
	//li        t0,0x6D0DA480		//DDR 200MHz, (3,3,8,13,10,2,3)	
	li        t0,0x6CEDA480		//DDR 200MHz, (3,3,8,13,10,2,3)		
#elif		LX_212_DDR200
	li        t0,0x6CEDA480		//DDR 200MHz, (3,3,8,13,10,2,3)			
#elif		LX_200_DDR183
	//li        t0,0x6D0DA480		//DDR 183MHz, (3,3,8,13,9,2,2)		
	li        t0,0x6CEDA480		//DDR 183MHz, ()	
#elif		LX_212_DDR133
	li      t0,0x48CAE280		//DDR 133MHz, (2,2,6,10,14,1,2) 	
#elif		LX_200_DDR133
	li      t0,0x48CAE280		//DDR 133MHz, (2,2,6,10,14,1,2) 
#elif		LX_180_DDR150
	li      t0,0x48CAE280		//DDR 133MHz, (2,2,6,10,14,1,2) 
#elif		LX_180_DDR133
	li        t0,0x48CAE280		//DDR 133MHz, (2,2,6,10,14,1,2) 	
#else  //DDR_133
   	//li      t0,0x6CEB84C0       //DDR 133MHz, (3,3,7,11,8,2,3)*
   	li		t0,0x48CAE280	 //DDR 133MHz, (2,2,6,10,14,1,2) 
#endif
  	//li        t0,0x912FB500       //DDR 200MHz, (4,4,9,15,11,2,4)*
	li		t1,0xb8001008
	sw		t0,0(t1)
	nop	
	nop
##############################################
#if 0			//this flag =1 for RL0390
// MRS configuration for DDR setting.
#define	MCR_PARA			t0
#define	MRS_VALUE			t1
#define	EMRS_VALUE			t2
#define	EDTCR_ADDR			t3
#define	EDTCR_ORG			t4
#define	MCR_ADDR			t5
#define	REG_TEMP			t6
#define	DELAY_CNT			t7
#define	MCR_ORG			t8
	li		MCR_ADDR,0xb8001004
	li		EDTCR_ADDR,0xb800100c
	li		MRS_VALUE,0x31					//DLL reset=0, TM=0, CAS# Latency=3, BurstType=0, BurstLength=1(2), n15..n14=00(Mode Register)
	//li		EMRS_VALUE,0x4006				//I/O Strength reduce (half), n15..n14=01(EMR), ODT 75ohm,DQS# enable	
	li		EMRS_VALUE,0x4406				//I/O Strength reduce (half), n15..n14=01(EMR), ODT 75ohm,DQS# disable
	//li		EMRS_VALUE,0x4404				//I/O Strength (full), n15..n14=01(EMR), ODT 75ohm,DQS# disable	
	//li		EMRS_VALUE,0x4442				//I/O Strength reduce (half), n15..n14=01(EMR), ODT 150ohm,DQS# disable
	lw		MCR_ORG,0(MCR_ADDR)			//Load MCR
	nop
	lw		EDTCR_ORG,0(EDTCR_ADDR)		//Load EDTCR
	nop
	nop
	or		MCR_PARA,MCR_ORG,0x20000		//MR MODE EN.	
#if 	0		//don't change MRS value.
	or		REG_TEMP,EDTCR_ORG,MRS_VALUE	//set value into MR_DATA	
	nop
	sw		REG_TEMP,0(EDTCR_ADDR)			//set MRS for DDR1 initial 
	nop
	nop
	sw		MCR_PARA,0(MCR_ADDR)			//send MRS for DDR1 initial 
	nop
#endif	
	nop
	or		REG_TEMP,EDTCR_ORG,EMRS_VALUE	//set value into MR_DATA
	nop
	sw		REG_TEMP,0(EDTCR_ADDR)			//set EMRS for DDR1 initial 
	nop
	nop
	nop	
	sw		MCR_PARA,0(MCR_ADDR)			//send EMRS for DDR1 initial 
	nop
	nop
	li		DELAY_CNT,500
1:
	subu		DELAY_CNT,1
	nop
	nop
	bnez		DELAY_CNT,1b
	nop
	nop		
	sw		MCR_ORG,0(MCR_ADDR)			//MR MODE Disable.
	nop
	nop
#undef	MCR_PARA
#undef	MRS_VALUE
#undef	EMRS_VALUE
#undef	EDTCR_ADDR
#undef	MCR_ADDR
#undef	REG_TEMP
#undef	MCR_ORG
#undef	DELAY_CNT
#undef	EDTCR_ORG
#endif
##############################################
//	SDR & DDR I/O PAD current select for test only. 
	//li		t0,0x00000c00		//data bus, driving upgrade
	//li		t0,0x00000c07		//all DDR bus, driving upgrade	
	//li		t0,0x00000c04		//DDR bus,data, clock driving upgrade	
	li		t0,0x00000c00		//DDR bus,data driving upgrade	
	li		t1,0xb8003310
	sw		t0,0(t1)
	nop		
	
###############################################
	//li		t0,0x00000000		//For Calibration use, clk_m delay set to zero [n8..n11]
	li		t1,0xb800332c
       li              t2, 0xfffff0ff
       lw              t0,0(t1)
       nop
       and             t0, t0, t2
       nop
       sw              t0, 0(t1)
       nop
###############################################		

	//li		t0,0x00b4A104		//default enable spreading spectrum n[23]=0, for DDR have to disable.
	li		t1,0xb8003334
	li		t2,0x800000			//default enable spreading spectrum n[23]=0, for DDR have to disable.
	lw		t0,0(t1)
	nop
	or		t0,t0,t2
	nop
	sw		t0,0(t1)	
	nop
# FIXME! Need real calibration!!
#ifdef 	LX_180_DDR166
	li		t0,0x54a00000		//166MHz, PLL analog calibration value for DDR only.*
#elif		LX_200_DDR166	
	li		t0,0x54a00000		//166MHz, PLL analog calibration value for DDR only.*
#elif		LX_212_DDR166
	li		t0,0x54a00000		//166MHz, PLL analog calibration value for DDR only.*	
#elif		LX_200_DDR200	
	li		t0,0x50700000		//200MHz, PLL analog calibration value for DDR only.*
#elif		LX_200_DDR183	
	li		t0,0x50700000		//183MHz, PLL analog calibration value for DDR only.*	
#elif		LX_200_DDR133
	li		t0,0x5ef00000			//133MHz, PLL analog calibration value for DDR only.*
#elif		LX_180_DDR150
	li		t0,0x54a00000		//150MHz, PLL analog calibration value for DDR only.*		
#elif		LX_212_DDR133
	li		t0,0x5ef00000			//133MHz, PLL analog calibration value for DDR only.*		
#elif		LX_180_DDR133
	li		t0,0x5ef00000			//133MHz, PLL analog calibration value for DDR only.*	
#else
	li		t0,0x5ef00000		//133MHz, PLL analog calibration value for DDR only.*		
#endif
	nop
	li		t1,0xb8001050
	sw		t0,0(t1)
	nop		
 	nop
 	nop

###############################################
	//li		t0,0x52480000		//16M, for SDRAM & DDR
	li		t0,0x544b0000		//32M, for SDRAM & DDR	//RL6166
	//li		t0,0x54480000		//32M, for SDRAM & DDR	//RLE0390
	//li		t0,0x544b0000		//32M, for SDRAM & DDR, reduce DDR chip output drive.
	//li		t0,0x9a480000		//32M, only for DDR dual 
	//li		t0,0x94480000		//32M, only for DDR
	//li		t0,MCR0_VAL
	li		t1,0xB8001004							//initial DDR signal re-send
	sw		t0,0(t1)
	nop
###############################################
	Noop_Delay 200	
###############################################
//#if 1		//////enable TX & DQS calibration //////
//////// Global paramter setup //////
#define CLK_133MHZ_FLAG	s2
	nop
#ifdef	LX_180_DDR166
 	move	CLK_133MHZ_FLAG,zero		//default run 166MHz
#elif		LX_212_DDR166
 	move	CLK_133MHZ_FLAG,zero		//default run 166MHz
#else	 	
	li		CLK_133MHZ_FLAG,1		//run in 133MHz
#endif
///////copy GENERATE PATTERN code to SRAM 0xA0606000 ////////
#define		CP_BLK_START_ADDR		t0
#define		CP_BLK_END_ADDR		t1
#define		CP_BLK_LENGTH			t2
#define		CP_BLK_TEMP1			t3
#define		CP_BLK_TEMP2			t4
	la		CP_BLK_START_ADDR, DQS_CALIBRATION_BEGIN
	la		CP_BLK_END_ADDR, DQS_CALIBRATION_FINISH
	sub		CP_BLK_LENGTH,CP_BLK_END_ADDR,CP_BLK_START_ADDR
	li		CP_BLK_TEMP1,0xa0600050											//save copy block length
	sw		CP_BLK_LENGTH,0(CP_BLK_TEMP1)

	li		CP_BLK_TEMP1,0xbfc00000											//boot start address.
	or		CP_BLK_START_ADDR,CP_BLK_START_ADDR,CP_BLK_TEMP1				//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	or		CP_BLK_END_ADDR,CP_BLK_END_ADDR,CP_BLK_TEMP1					//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	
	li		CP_BLK_TEMP1, 0xA0606000										//start address of SRAM reserve area  
	
1:	
	//nop
	lw		CP_BLK_TEMP2, 0(CP_BLK_START_ADDR)								//load code from flash
	nop
	sw		CP_BLK_TEMP2, 0(CP_BLK_TEMP1)									//restore code to SRAM
	//nop
	addi		CP_BLK_START_ADDR, CP_BLK_START_ADDR, 4
	addi		CP_BLK_TEMP1, CP_BLK_TEMP1, 4
	//nop
	bne		CP_BLK_START_ADDR, CP_BLK_END_ADDR, 1b
	nop
	//nop	
	li		CP_BLK_TEMP1, 0xA0606000										//
	//nop
	//nop	
	jal		CP_BLK_TEMP1													//jump to SRAM
	nop
	//nop	

#undef		CP_BLK_START_ADDR
#undef		CP_BLK_END_ADDR
#undef		CP_BLK_LENGTH
#undef		CP_BLK_TEMP1
#undef		CP_BLK_TEMP2
///////////////////////////////////////////////////////// 	
DQS_CALIBRATION_BEGIN:
	nop
	move	t6,ra																//save ra address
	li		t7,0xa0600054
	sw		t6,0(t7)
////GENERATE PATTERN /////////////////////////////
GENERATE_PATTERN:
#if 1
#define	PATTERN_TEMP1		t1
#define	PATTERN_TEMP2		t2
#define	PATTERN_LENGTH		t3
#define	PATTERN				t4		
#define	PATTERN1			t5
PATTERN_START:
	li		PATTERN,0x5a5aa5a5		//default pattern
	li		PATTERN1,0x0FFFF			//default pattern
////////////GEN data from 0xa0604000 to 0xa0604FFF, 4k bytes
	li		PATTERN_TEMP1,0xa0604000				//start address
	li		PATTERN_LENGTH,0x1000				//4k bytes, end address
1:	
	sw		PATTERN1,0(PATTERN_TEMP1)				//pattern1(0x0FFFF)
	addi		PATTERN_TEMP1,PATTERN_TEMP1,4
	not		PATTERN_TEMP2,PATTERN1				//pattern1 reverse (0xFFFF0000)
	sw		PATTERN_TEMP2,0(PATTERN_TEMP1)
	addi		PATTERN_TEMP1,PATTERN_TEMP1,4
	sw		PATTERN,0(PATTERN_TEMP1)				//pattern(0x5a5aa5a5)
	addi		PATTERN_TEMP1,PATTERN_TEMP1,4
	not		PATTERN_TEMP2,PATTERN				//pattern reverse(0xa5a55a5a)
	sw		PATTERN_TEMP2,0(PATTERN_TEMP1)
	addi		PATTERN_TEMP1,PATTERN_TEMP1,4
	subu		PATTERN_LENGTH,0x10
	bnez		PATTERN_LENGTH,1b
	nop
#if 0	
	move	t6,ra
	li		t7,0xa0600054
	sw		t6,0(t7)
	nop
	li		t7,0xa0600050
	lw		t8,0(t7)
	nop
	add		ra,t6,t8
	nop
	nop
	j		ra
	nop
	nop
	nop
#endif	
#undef	PATTERN_TEMP1
#undef	PATTERN_TEMP2
#undef	PATTERN_LENGTH	
#undef	PATTERN					
#undef	PATTERN1


#else
#define	DST_ADDR				t1
#define	DATA_TEMP1				t2
#define	PATTERN_LENGTH			t3
#define	PATTERN_SRC_ADDR		a0
#define	PATTERN_SRC_ADDR_END	a1
PATTERN_START:
////////////GEN data from 0xa0604000 to 0xa0604FFF, 4k bytes
	la		PATTERN_SRC_ADDR,PATTERN
	la		PATTERN_SRC_ADDR_END,PATTERN_END
	nop
	li		DST_ADDR,0xa0604000						//start address
	li		PATTERN_LENGTH,0x1000					//4k bytes, end address
3:
	la		PATTERN_SRC_ADDR,PATTERN
	beqz		PATTERN_LENGTH,	GEN_DATA_FINISH
	nop
	nop
1:	
	nop
	lw		DATA_TEMP1,0(PATTERN_SRC_ADDR)				//load PATTERN
	nop
	sw		DATA_TEMP1,0(DST_ADDR)						//store 
	nop
	nop
	addi		DST_ADDR,4
	addi		PATTERN_SRC_ADDR,4
	nop
	subu		PATTERN_LENGTH,4
	nop
	beq		PATTERN_SRC_ADDR,PATTERN_SRC_ADDR_END,3b
	nop
	nop
	bnez		PATTERN_LENGTH,1b
	nop
	nop
GEN_DATA_FINISH:
	nop
#undef	DST_ADDR
#undef	DATA_TEMP1
#undef	PATTERN_SRC_ADDR
#undef	PATTERN_SRC_ADDR_END
#undef	PATTERN_LENGTH
#endif
GENERATE_PATTERN_FINISH:
///////// generate data finish /////////
#if 0			//TX_DQS_CALIBRATION_START
TX_DQS_CALIBRATION_START:
################################################# 	
#======DDR TX Calibration start	===========
#define	TX_TEMP1		t1
#define	TX_TEMP2		t2
#define	TX_REG_ADDR	t4
#define	TX_REG_VAL		t5
#define	TX_CP_STEP		t6
#define	TX_PARA			t7
#define	TX_TEMP3		t8
#define	TX_TEMP4		t9
#define	TX_L0			s0
#define	TX_R0			s1
//s2 reg. is for global parameter.
#define	TX_DONE_L0		s3
#define	PATTERN			s4
#define	PATTERN1		s5
	li		TX_REG_ADDR,0xb8003304
	lw		TX_REG_VAL,0(TX_REG_ADDR)		//reload orginal value
	nop
	li		TX_CP_STEP,0x2000
	nop
	li		TX_PARA,0
	li		TX_DONE_L0,0
	move	TX_L0,zero
	move	TX_R0,zero
	
TX_START:
	li		PATTERN,0x5a5aa5a5		//default pattern
	li		PATTERN1,0x0FFFF			//default pattern
TX_PATTERN:
////////////GEN data from 0xa0600000 to 0xa0601FFF, 8k bytes
	li		TX_TEMP1,0xa0600000				//start address
	li		TX_TEMP2,0xa0602000				//8k bytes, end address
1:	
	nop
	sw		PATTERN1,0(TX_TEMP1)				//pattern1(0x0FFFF)
	nop
	addi		TX_TEMP1,TX_TEMP1,4
	nop
	nop
	not		TX_TEMP3,PATTERN1				//pattern1 reverse (0xFFFF0000)
	nop
	sw		TX_TEMP3,0(TX_TEMP1)
	nop
	addi		TX_TEMP1,TX_TEMP1,4
	nop
	nop	
	sw		PATTERN,0(TX_TEMP1)				//pattern(0x5a5aa5a5)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4
	nop
	nop	
	not		TX_TEMP3,PATTERN				//pattern reverse(0xa5a55a5a)
	nop
	sw		TX_TEMP3,0(TX_TEMP1)
	nop
	addi		TX_TEMP1,TX_TEMP1,4
	nop
	nop	
	bne		TX_TEMP1,TX_TEMP2,1b
	nop
	nop
///////// generate data finish /////////
TX_PRAR_CH:
////////////Clearn data from 0xa0612000 to 0xa0613FFF, 8k bytes , this will write area. /////
	li		TX_TEMP1,0xa0612000				//start address
	li		TX_TEMP2,0xa0614000				//8k bytes, end address
	li		TX_TEMP3,0
1:	
	nop
	sw		TX_TEMP3,0(TX_TEMP1)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4
	nop
	nop
	bne		TX_TEMP1,TX_TEMP2,1b
	nop
	nop
///////// Clearn data  finish /////////
/////////	set TX value	/////////
	and		TX_REG_VAL,TX_REG_VAL,0xFFFF0FFF	
	nop
	sll		TX_TEMP1,TX_PARA,12				//left shift 12 bit.
	nop
	or		TX_REG_VAL,TX_REG_VAL,TX_TEMP1
	nop
	sw		TX_REG_VAL,0(TX_REG_ADDR)
	nop
	nop
/////////	set TX value finish	/////////
/////////	send initial value to DDR /////////
 	li		TX_TEMP1,0xb8001004
 	li		TX_TEMP2,0x0
 	lw		TX_TEMP2,0(TX_TEMP1)	
 	nop
 	nop
 	sw		TX_TEMP2,0(TX_TEMP1)	
	nop
	nop
/////////	send initial value to DDR finish	/////////
TX_GDMA_START:
/////////	GDMA setup	/////////
 	li		TX_TEMP1,0xb800a000
 	li		TX_TEMP2,0x0
 	sw		TX_TEMP2,0(TX_TEMP1)			//Software reset GDMA, 0xb800a000[n31]=0	-> 1
 	nop
 	nop
 	li		TX_TEMP2,0x80000000
 	sw		TX_TEMP2,0(TX_TEMP1)			//Enable GDMA, 0xb800a000[n31]=1, and func set to memory copy [n27..n24]=0
 	nop
 	nop
 	li		TX_TEMP2,0x0
 	li		TX_TEMP1,0xb800a004
 	sw		TX_TEMP2,0(TX_TEMP1)			//Interrupt Mask Register clean
 	nop
 	nop
 	li		TX_TEMP2,0x90000000
 	li		TX_TEMP1,0xb800a008
 	sw		TX_TEMP2,0(TX_TEMP1)			//Interrupt status Register, [n31] and [n28] write 1 to clean
 	nop
 	nop
 ///////////GDMA memory copy (setup source)/////////////////////////////////
 	li		TX_TEMP2,0x00600000			//source data address
 	li		TX_TEMP1,0xb800a020
 	sw		TX_TEMP2,0(TX_TEMP1)			//setting source data address
 	nop
 	nop
  	li		TX_TEMP2,0x80001000			//[n31]=1, last data block, and source data length[n12..n0]
 	li		TX_TEMP1,0xb800a024
 	sw		TX_TEMP2,0(TX_TEMP1)					//setting source data length
 	nop
 	nop
/////////////GDMA memory copy (setup destination)/////////////////////////////////
	//or		TX_TEMP2,TX_CP_STEP,0x00600000
	//addiu		TX_TEMP2,TX_CP_STEP,0x0010000	//destination data address
	li		TX_TEMP2,0x00612000
 	li		TX_TEMP1,0xb800a060
 	sw		TX_TEMP2,0(TX_TEMP1)			//setting destination data address
 	nop
 	nop
 	li		TX_TEMP2,0x80001000			//[n31]=1, last data block, and destination data length[n12..n0]
 	li		TX_TEMP1,0xb800a064
 	sw		TX_TEMP2,0(TX_TEMP1)			//setting destination data length
 	nop
 	nop
//////////////startting  GDMA process 	/////////////////////////////////
  	li		TX_TEMP2,0xC0000000			//GDMA_ENABLE | GDMA_POLL | GDMA_MEMCPY	//startting  GDMA process
 	li		TX_TEMP1,0xb800a000
 	sw		TX_TEMP2,0(TX_TEMP1)					//startting  GDMA 
 	nop
 	nop  	
//////////////polling GDMA copy done flag	/////////////////////////////////
  	li		TX_TEMP2,0x0				// clean data buffer
  	li		TX_TEMP3,0x80000000
 	li		TX_TEMP1,0xb800a008			// GDMA	Interrupt Status Register
1:
	nop
	nop
 	lw		TX_TEMP2,0(TX_TEMP1)					// load 
 	nop
 	nop
 	and 		TX_TEMP2,TX_TEMP2,TX_TEMP3	
 	nop
 	beqz		TX_TEMP2,1b				//GDMA_not_complete
 	nop
 	nop  
/////////////////  fill 1Mbytes data into DRAM  /////////////////////
#if 0		
	addiu	TX_CP_STEP,TX_CP_STEP,0x2000
	nop
	bne		TX_CP_STEP,0x100000,TX_GDMA_START	//fill 1Mbytes data into DRAM
	nop
	nop
#endif	
//////////software compare data /////////////////
	li		TX_TEMP1,0xa0612000		//start compare address
	li		TX_TEMP2,0xa0613000		//end compare address
1:	
	nop
	lw		TX_TEMP3,0(TX_TEMP1)				//read data from DDR
	nop
	nop
	bne		TX_TEMP3,PATTERN1,COMPARE_FAIL	//compare pattern	(0x0FFFF)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4				//address plus 4
	nop
	nop
	not		TX_TEMP4,PATTERN1				//reverse pattern1 (0xFFFF0000)
	lw		TX_TEMP3,0(TX_TEMP1)	
	nop
	nop
	bne		TX_TEMP3,TX_TEMP4,COMPARE_FAIL	//compare pattern	(0x0FFFF)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4				//address plus 4
	nop
	nop
	lw		TX_TEMP3,0(TX_TEMP1)	
	nop
	nop
	bne		TX_TEMP3,PATTERN,COMPARE_FAIL	//compare pattern	(0x5a5aa5a5)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4				//address plus 4
	nop
	nop
	not		TX_TEMP4,PATTERN				//reverse pattern1 (0xa5a55a5a)
	lw		TX_TEMP3,0(TX_TEMP1)	
	nop
	nop	
	bne		TX_TEMP3,TX_TEMP4,COMPARE_FAIL	//compare pattern	(0xa5a55a5a)
	nop
	nop
	addi		TX_TEMP1,TX_TEMP1,4				//address plus 4
	nop
	nop
	bne		TX_TEMP1,TX_TEMP2,1b			//compare 4kbytes range
	nop
	nop
	beq		TX_PARA,0x10,TX_SET_R0			//if Tx parameter more then 0x10, set R0 value
	nop
	nop
	addiu	TX_PARA,TX_PARA,1				//Tx parameter plus 
	nop
	nop	
	bnez		TX_DONE_L0,TX_PRAR_CH			//check L0 or R0 calibration now
	nop
	nop
	addiu	TX_DONE_L0,TX_DONE_L0,1			//get L0 value and set TX_DONE_L0 equ 1
	nop
	subu		TX_PARA,1
	nop
	move	TX_L0,TX_PARA
	nop
	b		TX_PRAR_CH
	nop
	nop

COMPARE_FAIL:
	nop
	nop
	bnez		TX_DONE_L0,TX_SET_R0			// TX_DONE_L0=0:
	nop
	nop
	addiu	TX_PARA,TX_PARA,1				//Tx parameter plus 
	nop
	beq		TX_PARA,0x10,TX_FINISH			//TX_PRAR max 0x10
	nop
	nop
	b		TX_PRAR_CH
	nop
	nop
TX_SET_R0:
	bnez		TX_PARA,TX_SET_R0_not_zero
	nop
	nop
	addiu	TX_PARA,TX_PARA,1
	nop
TX_SET_R0_not_zero:								//record calibration data into SRAM area.
	li		TX_TEMP2,0x0
	beqz		CLK_133MHZ_FLAG,CLK_166MHZ_RECORD_TX_DATA
	nop
	nop
	li		TX_TEMP2,0x10						//record 133MHz calibration data into SRAM addr from 0xa05f0010 to 0xa05f0014
	nop
CLK_166MHZ_RECORD_TX_DATA:
#ifdef	OLD_SRAM_MAP
	li		TX_TEMP1,0xa05f0004
#else	
	li		TX_TEMP1,0xa0607f04					//redefine store data to SRAM from 0x00607f00 to 0x00608000
#endif	
	add		TX_TEMP1,TX_TEMP1,TX_TEMP2
	subu		TX_PARA,1
	sw		TX_PARA,0(TX_TEMP1)					//save R0 value at 0xa00f0004
	move	TX_R0,TX_PARA
	nop
	nop
#ifdef	OLD_SRAM_MAP	
	li		TX_TEMP1,0xa05f0000					//save L0 value into 0xa00f0000
#else	
	li		TX_TEMP1,0xa0607f00					//save L0 value into 0xa0607f00
#endif	
	add		TX_TEMP1,TX_TEMP1,TX_TEMP2	
	sw		TX_L0,0(TX_TEMP1)					//save
	nop
	nop	
	
TX_FINISH:
#if 1
	//li		TX_TEMP1,4							//R0 sub L0 is moreless 4, the DRAM clock will down gread to 133MHz
	li		TX_TEMP1,2							//R0 sub L0 is moreless 4, the DRAM clock will down gread to 133MHz
	//li		TX_TEMP1,0							//R0 sub L0 is moreless 4, the DRAM clock will down gread to 133MHz
	sub		TX_TEMP2,TX_R0,TX_L0
	nop
	nop
	bnez		CLK_133MHZ_FLAG,TX_133MHz_ONGOING
	nop
	nop
	bgt		TX_TEMP1,TX_TEMP2,RUN133MHz			//if 4 > (R0-L0), jump RUN133MHz.
	nop
	nop
TX_166MHz_ONGOING:	
TX_133MHz_ONGOING:
	nop
  	add		TX_TEMP2,TX_L0,TX_R0
 	nop
 	nop	

	srl 		TX_TEMP2, TX_TEMP2, 1					//calibration value div 2		
	nop
	nop
	sll		TX_TEMP2, TX_TEMP2, 12				//
	nop
	nop

	li		TX_TEMP1,0xFFFF0FFF
	lw		TX_REG_VAL,0(TX_REG_ADDR)
	nop
	nop
	and		TX_REG_VAL,TX_REG_VAL,TX_TEMP1
	nop
	or		TX_REG_VAL,TX_REG_VAL,TX_TEMP2
	nop
	sw		TX_REG_VAL,0(TX_REG_ADDR)
	nop
	nop
#endif 	
#if 1
  	//li		TX_TEMP2,0x0000300F
  	li		TX_TEMP2,0x0000000F  	
 	li		TX_TEMP1,0xb8003304
 	sw		TX_TEMP2,0(TX_TEMP1)					//fixed TX delay
 	nop
 	nop
#endif

#undef	TX_TEMP1		
#undef	TX_TEMP2		
#undef	PATTERN			
#undef	PATTERN1
#undef	TX_REG_ADDR	
#undef	TX_REG_VAL
#undef	TX_CP_STEP
#undef	TX_PARA		
#undef	TX_TEMP3
#undef	TX_TEMP4
#undef	TX_L0			
#undef	TX_R0			
#undef	TX_DONE_L0
#======DDR TX Calibration end	===========
#endif		//TX_DQS_CALIBRATION_START
###############################################
#======Start  DDR calibration (GDMA)===========
#if 1
/////////  DQS calibration start  /////////
#define 	SW_SRAM_ADDR		t0
#define 	DQS_TEMP5		 	t1
#define 	RUN_CYCLE	 		t2
#define 	SW_DDCR_VAL 		t3
#define 	DQS_TEMP1	 		t4
#define 	DQS_TEMP2	 		t5
#define 	DQS_TEMP3	 		t6
#define 	DQS_TEMP4	 		t7
#define 	POINT_CURSOR 		t8
#define 	UP_LIMIT		 		t9
#define 	DQS_CALI_TEMP		s0
#define 	DATA_MASK			s1
//s2 reg. is for global parameter.
#define 	L0 					s3
#define 	R0 					s4
#define 	L1 					s5
#define 	R1 					s6
#define 	L_DONE_BIT			s7
	//.set reorder
//// insert DDR calibration version, record in 0xA0600018 //start///	
	li		DQS_TEMP1,0xA0600018
	li		DQS_TEMP2,0x1D00					//version: 1.00
	sw		DQS_TEMP2,0(DQS_TEMP1)
//// insert DDR calibration version, record in 0xA0600018 //end///		
DQS_CALIBRATION_START:	
///////Initial REG. default value ///////////
	li		SW_SRAM_ADDR,0xa0604000
	li		DQS_TEMP1, 0xb8001050
 	li		SW_DDCR_VAL, 0x40000000           			//  sw cal:0x80000000, hw cal: 0x00000000
	sw		SW_DDCR_VAL, 0(DQS_TEMP1)
	li		DQS_CALI_TEMP,0
 	li		UP_LIMIT, 31
 	li		RUN_CYCLE,0
 	move	L0, zero
 	move	L1, zero
 	move	L_DONE_BIT, zero
 	li		R0, 0
 	li		R1, 0
	//li		DQS_PATTEN,0x5a5aa5a5				//change pattern 	
///////Initial REG. default value finish///////////	
////////////GEN data to DDR, the address from 0xa0604000 to 0xa0605000, 4k bytes  ////////
//use TX generate data from 0xa0600000 to 0xa0602000, copy SRAM area to DDR area.
	nop
	li		DQS_TEMP1,0xA0612000				//start address	(DDR start address)
	li		DQS_TEMP2,0xA0613000				//end address	(DDR end address)
	li		DQS_TEMP4,0xA0604000				//start address	(SRAM start address)
1:	
	lw		DQS_TEMP3,0(DQS_TEMP4)				//read data form SRAM
	nop
	sw		DQS_TEMP3,0(DQS_TEMP1)				//write data to DDR
	addiu	DQS_TEMP1,DQS_TEMP1,4
	addiu	DQS_TEMP4,DQS_TEMP4,4	
	bne		DQS_TEMP1,DQS_TEMP2,1b	
	nop
//////////// GEN data to DDR finish ///////////////
CALIBRATE_DQS0:
	bgt		RUN_CYCLE,2,RUN133MHz					//if RUN_CYCLE > 2, jump RUN133MHz
	nop
 	li		POINT_CURSOR, 0
CALIBRATE_DQS0_LOOP1:
////////////Clearn data from 0xa0604000 to 0xa0604FFF, 4k bytes
	move	DQS_TEMP1,SW_SRAM_ADDR				//start address
	li		DQS_TEMP2,0x1000					//Clean length 4k bytes
	li		DQS_TEMP3,0
1:	
	sw		DQS_TEMP3,0(DQS_TEMP1)
	addiu	DQS_TEMP1,DQS_TEMP1,4
	subu		DQS_TEMP2,4
	nop
	bnez		DQS_TEMP2,1b
	nop
///////// Clearn data  finish /////////
DQS_PARAMETER_CH:
	li		SW_DDCR_VAL,0x40000000
	bltu		UP_LIMIT, POINT_CURSOR, CALIBRATE_DQS0_LOOP1_OUT
	nop
	sll		DQS_TEMP1, POINT_CURSOR, 25
	or		SW_DDCR_VAL, SW_DDCR_VAL, DQS_TEMP1				// SW_DDCR_VAL default equ 0x40000000, write DQS0 value.
	li		DQS_TEMP1,0xb8001050
	sw		SW_DDCR_VAL, 0(DQS_TEMP1)						//setting DQS0 value to 0xb8001050.
////////GDMA move data from DDR to SRAM  ///////////////////////////
//// GDMA use TX_TEMP1,TX_TEMP2,TX_TEMP3 REG.	/////////////////
 	li		DQS_TEMP1,0xb800a000
 	li		DQS_TEMP2,0x0
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Software reset GDMA, 0xb800a000[n31]=0	-> 1
 	li		DQS_TEMP2,0x80000000
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Enable GDMA, 0xb800a000[n31]=1, and func set to memory copy [n27..n24]=0
 	li		DQS_TEMP2,0x0
 	li		DQS_TEMP1,0xb800a004
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Interrupt Mask Register clean
 	li		DQS_TEMP2,0x90000000
 	li		DQS_TEMP1,0xb800a008
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Interrupt status Register, [n31] and [n28] write 1 to clean
 ///////////GDMA memory copy (setup source)/////////////////////////////////
 	li		DQS_TEMP2,0x00612000			//source data address
 	li		DQS_TEMP1,0xb800a020
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting source data address
  	li		DQS_TEMP2,0x80001000			//[n31]=1, last data block, and source data length[n12..n0]
 	li		DQS_TEMP1,0xb800a024
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting source data length
/////////////GDMA memory copy (setup destination)/////////////////////////////////
	li		DQS_TEMP2,0x00604000			//destination data address
 	li		DQS_TEMP1,0xb800a060
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting destination data address
 	li		DQS_TEMP2,0x80001000			//[n31]=1, last data block, and destination data length[n12..n0]
 	li		DQS_TEMP1,0xb800a064
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting destination data length
//////////////startting  GDMA process 	/////////////////////////////////
  	li		DQS_TEMP2,0xC0000000			//GDMA_ENABLE | GDMA_POLL | GDMA_MEMCPY	//startting  GDMA process
 	li		DQS_TEMP1,0xb800a000
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//startting  GDMA 
//////////////polling GDMA copy done flag	/////////////////////////////////
  	li		DQS_TEMP2,0x0					// clean data buffer
  	li		DQS_TEMP3,0x80000000
 	li		DQS_TEMP1,0xb800a008			// GDMA	Interrupt Status Register
1:
 	lw		DQS_TEMP2,0(DQS_TEMP1)			// load 
 	nop
 	and 		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3	
 	beqz		DQS_TEMP2,1b					//GDMA_not_complete
 	nop
////////GDMA move data from DDR to SRAM  finish ///////////////////////////	
	bnez		L_DONE_BIT, CALIBRATE_DQS0_R0_SET
	nop
	nop	
//////////software compare data /////////////////
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address
	li		DQS_TEMP2,0									
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP4,0
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0x00FF00FF	
 	subu		DQS_TEMP5,4	

1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4
	bne		DQS_TEMP2,DQS_TEMP4,CALIBRATE_DQS0_LOOP1_REENTRY	//compare pattern
	nop
	subu		DQS_TEMP5,4
	bnez		DQS_TEMP5,1b
	nop
	move 	L0, POINT_CURSOR
	addiu	L_DONE_BIT,L_DONE_BIT,1	
	nop
CALIBRATE_DQS0_R0_SET:	
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address
	li		DQS_TEMP2,0									
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP4,0
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0x00FF00FF	
	subu		DQS_TEMP5,4 	
1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4	
	bne		DQS_TEMP2,DQS_TEMP4,CALIBRATE_DQS0_LOOP1_R0_OUT	//compare pattern
	nop
	subu		DQS_TEMP5,4
	bnez		DQS_TEMP5,1b
	nop
	move 	R0, POINT_CURSOR
	//b		CALIBRATE_DQS0_LOOP1_REENTRY	
	nop	
CALIBRATE_DQS0_LOOP1_REENTRY:	
	addiu 	POINT_CURSOR, POINT_CURSOR, 1
	b 		CALIBRATE_DQS0_LOOP1
  	nop
##################
CALIBRATE_DQS0_LOOP1_R0_OUT:
	addiu	RUN_CYCLE,RUN_CYCLE,1
	add		DQS_TEMP1,R0,L0
	beqz		DQS_TEMP1,CALIBRATE_DQS0				//if (R0+L0) = 0, re-try agaim.
	nop
	sub		DQS_TEMP1,R0,L0
	blt		DQS_TEMP1,2,CALIBRATE_DQS0				//if (R0-L0) < 2 , the vale is very close, so re-try again.
	nop
	beq		R0,L0,CALIBRATE_DQS0					//if R0=L0, re-try again.
	nop
	beqz		R0,CALIBRATE_DQS0_LOOP1_REENTRY
	nop	
CALIBRATE_DQS0_LOOP1_OUT:	
###################
	move	DQS_TEMP1, L0							//move L0 to DQS_TEMP1
	add		DQS_TEMP1, DQS_TEMP1, R0				//DQS_TEMP1 equ sum( L0 + R0 )
	srl 		DQS_TEMP1, DQS_TEMP1, 1					//calibration value div 2		
#######################
#if	0
	add		DQS_TEMP2,DQS_TEMP1,L0						//DQS_TEMP2 equ sum ( ((L0+R0)/2+L0)
	nop
	srl		DQS_TEMP2,DQS_TEMP2,1						// 1/4 of calibration range 
	nop
	add		DQS_TEMP1,DQS_TEMP1,DQS_TEMP2
	nop
	srl		DQS_TEMP1,DQS_TEMP1,1						//3/8 of calibration range 
	nop
	nop
#endif
#if	0
	add		DQS_TEMP2,DQS_TEMP1,R0						//DQS_TEMP2 equ sum ( ((L0+R0)/2+R0)
	nop
	srl		DQS_TEMP2,DQS_TEMP2,1						// 3/4 of calibration range 
	nop
	add		DQS_TEMP1,DQS_TEMP1,DQS_TEMP2
	nop
	srl		DQS_TEMP1,DQS_TEMP1,1						//5/8 of calibration range 
	nop
	nop
#endif
#######################	
	sll		DQS_TEMP1, DQS_TEMP1, 25
	li		SW_DDCR_VAL, 0x40000000 				//initial analog calibration.		
	or		DQS_CALI_TEMP, SW_DDCR_VAL, DQS_TEMP1	//finish DQS0 calibration, save in DQS_CALI_TEMP
	li		DQS_TEMP1,0xb8001050
	sw		DQS_CALI_TEMP, 0(DQS_TEMP1)
//////////Starting DQS1 calibration //////////////
	move	L_DONE_BIT, zero
	li		RUN_CYCLE,0
CALIBRATE_DQS1:
	bgt		RUN_CYCLE,2,RUN133MHz					//if RUN_CYCLE > 2, jump RUN133MHz
	nop
 	move	POINT_CURSOR, zero	
CALIBRATE_DQS1_LOOP1: 	
////////////Clearn data from 0xa0604000 to 0xa0604FFF, 4k bytes
	move	DQS_TEMP1,SW_SRAM_ADDR				//start address
	li		DQS_TEMP2,0x1000						//Clean length 4k bytes
	li		DQS_TEMP3,0
1:	
	sw		DQS_TEMP3,0(DQS_TEMP1)
	addiu	DQS_TEMP1,DQS_TEMP1,4
	subu		DQS_TEMP2,4
	bnez		DQS_TEMP2,1b
	nop
///////// Clearn data  finish /////////
DQS1_PARAMETER_CH:
	move	SW_DDCR_VAL,DQS_CALI_TEMP							//load calibration method and DQS0 value.
	bltu		UP_LIMIT, POINT_CURSOR, CALIBRATE_DQS1_LOOP1_OUT	//out of parameter range
	nop
	sll		DQS_TEMP1, POINT_CURSOR, 20							//DQS1 set bit[n24..n20]
	or		SW_DDCR_VAL, SW_DDCR_VAL, DQS_TEMP1				// SW_DDCR_VAL default equ 0x40000000, write DQS0 value.
	li		DQS_TEMP1,0xb8001050
	sw		SW_DDCR_VAL, 0(DQS_TEMP1)						//setting DQS1 value to 0xb8001050.
////////GDMA move data from DDR to SRAM  ///////////////////////////
//// GDMA use TX_TEMP1,TX_TEMP2,TX_TEMP3 REG.	/////////////////
 	li		DQS_TEMP1,0xb800a000
 	li		DQS_TEMP2,0x0
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Software reset GDMA, 0xb800a000[n31]=0	-> 1
 	li		DQS_TEMP2,0x80000000
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Enable GDMA, 0xb800a000[n31]=1, and func set to memory copy [n27..n24]=0
 	li		DQS_TEMP2,0x0
 	li		DQS_TEMP1,0xb800a004
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Interrupt Mask Register clean
 	li		DQS_TEMP2,0x90000000
 	li		DQS_TEMP1,0xb800a008
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//Interrupt status Register, [n31] and [n28] write 1 to clean
 ///////////GDMA memory copy (setup source)/////////////////////////////////
 	li		DQS_TEMP2,0x00612000			//source data address
 	li		DQS_TEMP1,0xb800a020
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting source data address
  	li		DQS_TEMP2,0x80001000			//[n31]=1, last data block, and source data length[n12..n0]
 	li		DQS_TEMP1,0xb800a024
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting source data length
/////////////GDMA memory copy (setup destination)/////////////////////////////////
	li		DQS_TEMP2,0x00604000			//destination data address
 	li		DQS_TEMP1,0xb800a060
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting destination data address
 	li		DQS_TEMP2,0x80001000			//[n31]=1, last data block, and destination data length[n12..n0]
 	li		DQS_TEMP1,0xb800a064
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//setting destination data length
//////////////startting  GDMA process 	/////////////////////////////////
  	li		DQS_TEMP2,0xC0000000			//GDMA_ENABLE | GDMA_POLL | GDMA_MEMCPY	//startting  GDMA process
 	li		DQS_TEMP1,0xb800a000
 	sw		DQS_TEMP2,0(DQS_TEMP1)			//startting  GDMA 
//////////////polling GDMA copy done flag	/////////////////////////////////
  	li		DQS_TEMP2,0x0					// clean data buffer
  	li		DQS_TEMP3,0x80000000
 	li		DQS_TEMP1,0xb800a008			// GDMA	Interrupt Status Register
1:
 	lw		DQS_TEMP2,0(DQS_TEMP1)					// load 
 	nop
 	and 		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3	
 	beqz		DQS_TEMP2,1b					//GDMA_not_complete
 	nop
////////GDMA move data from DDR to SRAM  finish ///////////////////////////	
	bnez		L_DONE_BIT, CALIBRATE_DQS1_R1_SET
	nop
//////////software compare data /////////////////
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address
	li		DQS_TEMP2,0									
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP4,0
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0xFF00FF00	
	subu		DQS_TEMP5,4 	

1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4	
	bne		DQS_TEMP2,DQS_TEMP4,CALIBRATE_DQS1_LOOP1_REENTRY	//compare pattern
	nop
	subu		DQS_TEMP5,4
	bnez		DQS_TEMP5,1b
	nop
	move 	L1, POINT_CURSOR
	addiu	L_DONE_BIT,L_DONE_BIT,1	
CALIBRATE_DQS1_R1_SET:	
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address
	li		DQS_TEMP2,0									
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP4,0
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0xFF00FF00	
	subu		DQS_TEMP5,4 	
1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4	
	bne		DQS_TEMP2,DQS_TEMP4,CALIBRATE_DQS1_LOOP1_R1_OUT	//compare pattern
	nop
	subu		DQS_TEMP5,4
	bnez		DQS_TEMP5,1b
	nop
	move 	R1, POINT_CURSOR
	//b		CALIBRATE_DQS1_LOOP1_REENTRY	
	nop	
CALIBRATE_DQS1_LOOP1_REENTRY:	
	addiu 	POINT_CURSOR, POINT_CURSOR, 1
	b 		CALIBRATE_DQS1_LOOP1
  	nop
CALIBRATE_DQS1_LOOP1_R1_OUT:
	addiu	RUN_CYCLE,RUN_CYCLE,1
	add		DQS_TEMP1,R1,L1
	beqz		DQS_TEMP1,CALIBRATE_DQS1				//if (R1+L1) = 0, re-try agaim.
	nop
	sub		DQS_TEMP1,R1,L1
	blt		DQS_TEMP1,2,CALIBRATE_DQS1				//if (R1-L1) < 2 , the vale is very close, so re-try again.
	nop
	beq		R1,L1,CALIBRATE_DQS1					//if R1=L1, re-try again.
	nop
	beqz		R1,CALIBRATE_DQS1_LOOP1_REENTRY
	nop	
CALIBRATE_DQS1_LOOP1_OUT:	
####################
	add		DQS_TEMP1, L1, R1							//DQS_TEMP1 equ sum( L1 + R1 )
	srl 		DQS_TEMP1, DQS_TEMP1, 1						//calibration value div 2		
#######################
#if	0
	add		DQS_TEMP2,DQS_TEMP1,L1						//DQS_TEMP2 equ sum ( ((L1+R1)/2+L1)
	nop
	srl		DQS_TEMP2,DQS_TEMP2,1						// 1/4 of calibration range 
	nop
	add		DQS_TEMP1,DQS_TEMP1,DQS_TEMP2
	nop
	srl		DQS_TEMP1,DQS_TEMP1,1						//3/8 of calibration range 
	nop
	nop
#endif	
#if	0
	add		DQS_TEMP2,DQS_TEMP1,R1						//DQS_TEMP2 equ sum ( ((L1+R1)/2+R1)
	nop
	srl		DQS_TEMP2,DQS_TEMP2,1						// 3/4 of calibration range 
	nop
	add		DQS_TEMP1,DQS_TEMP1,DQS_TEMP2
	nop
	srl		DQS_TEMP1,DQS_TEMP1,1						//5/8 of calibration range 
	nop
	nop
#endif
#######################	
	sll		DQS_TEMP1, DQS_TEMP1, 20					// left shift 20 bit for DQS1 parameter.
	or		DQS_CALI_TEMP, DQS_CALI_TEMP, DQS_TEMP1	//finish DQS1 calibration, save in DQS_CALI_TEMP
	li		DQS_TEMP1,0xb8001050
	sw		DQS_CALI_TEMP, 0(DQS_TEMP1)
////////////////////////////////////////////////////
RECORD_DATA:
	li		DQS_TEMP3,0x0
	beqz		CLK_133MHZ_FLAG,CLK_166MHZ_RECORD_DATA
	nop
	li		DQS_TEMP3,0x10
CLK_166MHZ_RECORD_DATA:	
	move	DQS_TEMP1, L0
	li		DQS_TEMP2, 0xa0600020
	add		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	nop
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, R0
	li		DQS_TEMP2, 0xa0600024
	add		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, L1
	li		DQS_TEMP2, 0xa0600028	
	add		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, R1
	li		DQS_TEMP2, 0xa060002c	
	add		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	sw		DQS_TEMP1, 0(DQS_TEMP2)
//////if 166MHz not stable, change to 133MHz/////////////////////////////////////////////
#ifdef	CALI_UNLIMIT
	//li		DQS_TEMP2,0x0								//if (R0-L0)<31, force in 133MHz
	b		CALIBRATE_OUT
	nop
#else
	li		DQS_TEMP2,0x8								//if (R0-L0)<8, the DRAM clock will down gread to 133MHz
#endif	
	//nop
	sub		DQS_TEMP1,R0,L0
	//nop
	bnez		CLK_133MHZ_FLAG,DQS_133MHz_ONGOING
	nop
	//nop	
	bge		DQS_TEMP2,DQS_TEMP1,RUN133MHz  
	nop
	//nop
	sub		DQS_TEMP1,R1,L1
	//nop	
	bge		DQS_TEMP2,DQS_TEMP1,RUN133MHz  
	nop
	//nop
DQS_133MHz_ONGOING:
	//nop
	b		CALIBRATE_OUT
	nop
	//nop
RUN133MHz:
///////CLK_166MHZ_RECORD_DATA: start/////////
	move	DQS_TEMP1, L0
	li		DQS_TEMP2, 0xa0600020
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, R0
	li		DQS_TEMP2, 0xa0600024
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, L1
	li		DQS_TEMP2, 0xa0600028	
	sw		DQS_TEMP1, 0(DQS_TEMP2)
	move	DQS_TEMP1, R1
	li		DQS_TEMP2, 0xa060002c		
	sw		DQS_TEMP1, 0(DQS_TEMP2)
///////CLK_166MHZ_RECORD_DATA: end/////////	
	//nop
	bnez		CLK_133MHZ_FLAG,FIXED_DQS
	nop
	//nop
	li		DQS_TEMP1,0xb8003200
	lw		DQS_TEMP2,0(DQS_TEMP1)
	nop
	li		DQS_TEMP3,0xFFFFFFF0
	and		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	addiu	DQS_TEMP2,DQS_TEMP2,7				//0xb8003200[n3..n0] to setup DRAM clock rate. RL6166(0:166MHz, 7:133MHz)
	//nop
	sw		DQS_TEMP2,0(DQS_TEMP1)
	//nop
	//nop
	li		DQS_TEMP3,1000						//delay count.
1:
	subu		DQS_TEMP3,1
	//nop
	//nop
	bnez		DQS_TEMP3,1b
	nop	
	//nop		
	li		DQS_TEMP1,0xb8001008
	li		DQS_TEMP2,0x48CAE280
	sw		DQS_TEMP2,0(DQS_TEMP1)
	//nop
	//nop
	li		DQS_TEMP1,0xb8001050
	li		DQS_TEMP2,0x5ef00000
	sw		DQS_TEMP2,0(DQS_TEMP1)
	//nop
	//nop
	li		DQS_TEMP1,0xb8001004
	lw		DQS_TEMP2,0(DQS_TEMP1)
	nop
	sw		DQS_TEMP2,0(DQS_TEMP1)
	//nop
	//nop
	li		CLK_133MHZ_FLAG,0x1
	//nop
#if	0	//if enable TX calibration, set to 1
	b		TX_DQS_CALIBRATION_START			
#else
	b		DQS_CALIBRATION_START
#endif
	nop
	//nop
CALIBRATE_OUT:
	beqz		R0,FIXED_DQS
	nop
	//nop
	beqz		R1,FIXED_DQS
	nop
	//nop
	b		FINISH_CALI_DQS
	nop
	//nop
FIXED_DQS:
  	//li		DQS_TEMP2,0x5ef00000					//RUN in 133MHz
  	li		DQS_TEMP2,0x54a00000					//RUN in 166MHz
 	li		DQS_TEMP1,0xb8001050
 	sw		DQS_TEMP2,0(DQS_TEMP1)					//fixed DQS
 	//nop
 	//nop
FINISH_CALI_DQS:
	//fixd DQS
#if	0
  	li		DQS_TEMP2,0x58b00000					//RUN in 133MHz
 	li		DQS_TEMP1,0xb8001050
 	sw		DQS_TEMP2,0(DQS_TEMP1)					//fixed DQS
#endif 	
	nop
    .set noreorder
#undef 	SW_SRAM_ADDR		
#undef 	CLK_133MHZ_FLAG
#undef 	RUN_CYCLE 		
#undef 	SW_DDCR_VAL 			
#undef 	DQS_TEMP1	 		
#undef 	DQS_TEMP2	 		
#undef 	DQS_TEMP3	 		
#undef 	DQS_TEMP4	 		
#undef 	POINT_CURSOR 		
#undef 	UP_LIMIT		 		
#undef 	DQS_CALI_TEMP		
#undef 	DQS_TEMP5			
#undef 	L0 					
#undef 	R0 					
#undef 	L1 					
#undef 	R1 					
#undef 	L_DONE_BIT
#endif
////////////////////////////////////////////////////////////////////
#undef CLK_133MHZ_FLAG
	li		t7,0xa0600054			//load ra into t6
	lw		t6,0(t7)
	nop
	li		t7,0xa0600050
	lw		t8,0(t7)
	nop
	add		ra,t6,t8					//ra addr plus copy length offset
	//nop
	//nop
	j		ra
	nop
	//nop
	//nop			
DQS_CALIBRATION_FINISH:
	nop
#################################################

	nop
        b               MEM_CONF_DONE
        nop
        nop
        nop
#===== end DDR calibration ================
is_SDRAM:
##############################################
//	SDR & DDR I/O PAD current select for test only. 
	//li		t0,0x00000c00		//data bus, driving upgrade
	//li		t0,0x00000c07		//all DDR bus, driving upgrade	
	//li		t0,0x00000c04		//DDR bus,data, clock driving upgrade	
	//li		t0,0x00000c00		//DDR bus,data driving upgrade	
	li		t0,0x0
	li		t1,0xb8003310
	sw		t0,0(t1)
	nop	
////////////// //Set Tx/Rx delay for SDRAM  //////////////////////////
	li		t0,0x0100000F		//SDRAM TX/RX delay parameter setup for RLE0315C
	li		t1,0xb8003304
	sw		t0,0(t1)
	nop	

###############################################
# SDRAM/DDR Memory timing register, MUST set before MCR(config. reg)

	li		t0,0x6d0b0ac0		//SDR
	li		t1,0xb8001008
	sw		t0,0(t1)
	nop
	nop
	nop
###############################################
	li		t0,0x52480000		//16M, for SDRAM & DDR
	//li		t0,0x54480000		//32M, for SDRAM & DDR
	//li		t0,MCR0_VAL
	li		t1,0xB8001004
	sw		t0,0(t1)
	nop
#if	1	//RX calibration......	
#if	1	//copy code to DMEM
///////copy SDRAM RX delay calibraton code to SRAM 0xA0606000 ////////
#define		CP_BLK_START_ADDR	t0
#define		CP_BLK_END_ADDR		t1
#define		CP_BLK_LENGTH			t2
#define		CP_BLK_TEMP1			t3
#define		CP_BLK_TEMP2			t4
	la		CP_BLK_START_ADDR, SDRAM_RX_CALI_BEGIN
	la		CP_BLK_END_ADDR, SDRAM_RX_CALI_FINISH
	sub		CP_BLK_LENGTH,CP_BLK_END_ADDR,CP_BLK_START_ADDR
	li		CP_BLK_TEMP1,0xa0600050											//save copy block length
	sw		CP_BLK_LENGTH,0(CP_BLK_TEMP1)

	li		CP_BLK_TEMP1,0xbfc00000											//boot start address.
	or		CP_BLK_START_ADDR,CP_BLK_START_ADDR,CP_BLK_TEMP1				//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	or		CP_BLK_END_ADDR,CP_BLK_END_ADDR,CP_BLK_TEMP1					//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	
	li		CP_BLK_TEMP1, 0xA0606000										//start address of SRAM reserve area  
	
1:	
	lw		CP_BLK_TEMP2, 0(CP_BLK_START_ADDR)								//load code from flash
	nop
	sw		CP_BLK_TEMP2, 0(CP_BLK_TEMP1)									//restore code to SRAM
	addi		CP_BLK_START_ADDR, CP_BLK_START_ADDR, 4
	addi		CP_BLK_TEMP1, CP_BLK_TEMP1, 4
	bne		CP_BLK_START_ADDR, CP_BLK_END_ADDR, 1b
	nop
	li		CP_BLK_TEMP1, 0xA0606000										//
	jal		CP_BLK_TEMP1													//jump to SRAM
	nop

#undef		CP_BLK_START_ADDR
#undef		CP_BLK_END_ADDR
#undef		CP_BLK_LENGTH
#undef		CP_BLK_TEMP1
#undef		CP_BLK_TEMP2
#endif		//copy code to DMEM
/////////////////////////////////////////////////////////
SDRAM_RX_CALI_BEGIN:
	nop
#if	1		//copy code to DMEM	
	move	t6,ra																//save ra address
	li		t7,0xa0600054
	sw		t6,0(t7)
#endif		//copy code to DMEM	
#======SDRAM RX Calibration start	===========
#define	RX_TEMP1		t1
#define	RX_TEMP2		t2
#define	RX_TEMP6		t3
#define	RX_REG_ADDR	t4
#define	RX_REG_VAL		t5
#define	RX_TEMP5		t6
#define	RX_PARA			t7
#define	RX_TEMP3		t8
#define	RX_TEMP4		t9
#define	RX_L0			s0
#define	RX_MSB__flag		s5
//// insert SDR calibration version, record in 0xA0600018 //start///	
	li		RX_TEMP1,0xA0600018
	li		RX_TEMP2,0x1D00					//version: 1.00
	sw		RX_TEMP2,0(RX_TEMP1)
//// insert SDR calibration version, record in 0xA0600018 //end///

	li		RX_REG_ADDR,0xb8003304
	lw		RX_REG_VAL,0(RX_REG_ADDR)		//reload orginal value
	nop
	li		RX_PARA,0
	move	RX_L0,zero
	move	RX_MSB__flag,zero
RX_PRAR_CH:
/////////	set RX value	/////////
	and		RX_REG_VAL,RX_REG_VAL,0xFFFFFFF0	
	nop
	or		RX_REG_VAL,RX_REG_VAL,RX_PARA
	nop
	sw		RX_REG_VAL,0(RX_REG_ADDR)
	nop
////////////save start GDMA address  ////////////////////////////////////////// 	
	li		RX_TEMP1,0xA0600020				//save  start GDMA address to 0xA0600020
 	li		RX_TEMP2,0xA0800000				//start GDMA address
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop
	li		RX_TEMP1,0xA0600028				//save  current GDMA address to 0xA0600028
	li		RX_TEMP3,0x1FFFFFFF
 	and		RX_TEMP2,RX_TEMP2,RX_TEMP3		//start GDMA address
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop 	 	
	li		RX_TEMP1,0xA0600024				//save  end GDMA address to 0xA0600024 
 	li		RX_TEMP2,0xA0900000				//end GDMA address
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop 	
////////////Clearn data from 0xa0612000 to 0xa0612FFF,  this will write area. /////
	li		RX_TEMP1,0xA0600020				//load start GDMA address from 0xA0600020
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	li		RX_TEMP1,0xA0600024				//load end GDMA address from 0xA0600024
	lw		RX_TEMP3,0(RX_TEMP1)
	nop	
	li		RX_TEMP4,0
1:	
	sw		RX_TEMP4,0(RX_TEMP2)
	nop
	addi		RX_TEMP2,RX_TEMP2,4
	nop
	bgt		RX_TEMP3,RX_TEMP2,1b
	nop
///////// Clearn data  finish /////////
#if	1		//GENERATE PATTERN
////GENERATE PATTERN /////////////////////////////
	li		RX_TEMP4,0x5a5aa5a5		//default pattern
	li		RX_TEMP5,0x0FFFF			//default pattern
////////////GEN data from 0xa0610000 to 0xa061FFFF, 64k bytes
	li		RX_TEMP2,0xA0600020					//load start GDMA address from 0xA0600020
	lw		RX_TEMP1,0(RX_TEMP2)
	nop
	li		RX_TEMP3,0x8000						//32k bytes, end address
1:	
	sw		RX_TEMP5,0(RX_TEMP1)				//pattern1(0x0FFFF)
	addi		RX_TEMP1,RX_TEMP1,4
	not		RX_TEMP2,RX_TEMP5					//pattern1 reverse (0xFFFF0000)
	sw		RX_TEMP2,0(RX_TEMP1)
	addi		RX_TEMP1,RX_TEMP1,4
	sw		RX_TEMP4,0(RX_TEMP1)				//pattern(0x5a5aa5a5)
	addi		RX_TEMP1,RX_TEMP1,4
	not		RX_TEMP2,RX_TEMP4					//pattern reverse(0xa5a55a5a)
	sw		RX_TEMP2,0(RX_TEMP1)
	addi		RX_TEMP1,RX_TEMP1,4
	subu		RX_TEMP3,0x10	
	bnez		RX_TEMP3,1b
	nop	
///////// generate data finish /////////	
#endif		//generate data finish

#if 	0
///////initial value for temp /////////////////////////////////////////
	//move	GDMA_COUNT,zero	
/////////	set RX value	/////////
	and		RX_REG_VAL,RX_REG_VAL,0xFFFFFFF0	
	nop
	or		RX_REG_VAL,RX_REG_VAL,RX_PARA
	nop
	sw		RX_REG_VAL,0(RX_REG_ADDR)
	nop
#endif	
RX_GDMA_START:
/////////	GDMA setup	,GDMA cp DRAM to DMEM /////////
 	li		RX_TEMP1,0xb800a000
 	li		RX_TEMP2,0x0
 	sw		RX_TEMP2,0(RX_TEMP1)			//Software reset GDMA, 0xb800a000[n31]=0	-> 1
	nop
 	li		RX_TEMP2,0x80000000
 	sw		RX_TEMP2,0(RX_TEMP1)			//Enable GDMA, 0xb800a000[n31]=1, and func set to memory copy [n27..n24]=0
 	nop
 	li		RX_TEMP2,0x0
 	li		RX_TEMP1,0xb800a004
 	sw		RX_TEMP2,0(RX_TEMP1)			//Interrupt Mask Register clean
 	nop
 	li		RX_TEMP2,0x90000000
 	li		RX_TEMP1,0xb800a008
 	sw		RX_TEMP2,0(RX_TEMP1)			//Interrupt status Register, [n31] and [n28] write 1 to clean
 	nop
 	li		RX_TEMP5,0x8000					//set cp block size
 ///////////GDMA memory copy (setup source, BLK0)/////////////////////////////////
 	li		RX_TEMP4,0xA0600028
 	lw		RX_TEMP3,0(RX_TEMP4)			//load current GDMA address.
 	nop
 	li		RX_TEMP1,0xb800a020
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a024
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK0)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a060
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a064
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop
 ///////////GDMA memory copy (setup source, BLK1)/////////////////////////////////
 	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a028
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a02C
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK1)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a068
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a06C
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop 	
 ///////////GDMA memory copy (setup source, BLK2)/////////////////////////////////
 	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a030
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a034
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK2)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a070
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a074
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop 	 	
 ///////////GDMA memory copy (setup source, BLK3)/////////////////////////////////
 	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a038
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a03C
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK3)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a078
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a07C
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop 	 	 	
 ///////////GDMA memory copy (setup source, BLK4)/////////////////////////////////
 	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a040
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a044
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK4)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a080
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a084
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop 	 	 	 	
  ///////////GDMA memory copy (setup source, BLK5)/////////////////////////////////
  	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a048
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a04C
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK5)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a088
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a08C
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop 	
  ///////////GDMA memory copy (setup source, BLK6)/////////////////////////////////
  	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a050
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a054
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK6)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a090
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x00001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a094
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop  	
  ///////////GDMA memory copy (setup source, BLK7)/////////////////////////////////
 	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
 	li		RX_TEMP1,0xb800a058
 	sw		RX_TEMP3,0(RX_TEMP1)			//setting source data address
 	nop
  	li		RX_TEMP2,0x80001000				//[n31]=1, last data block, and source data length[n12..n0]
 	li		RX_TEMP1,0xb800a05C
 	sw		RX_TEMP2,0(RX_TEMP1)					//setting source data length
 	nop
/////////////GDMA memory copy (setup destination, BLK7)/////////////////////////////////
	add		RX_TEMP4,RX_TEMP3,RX_TEMP5
 	li		RX_TEMP1,0xb800a098
 	sw		RX_TEMP4,0(RX_TEMP1)			//setting destination data address
 	nop
 	li		RX_TEMP2,0x80001000				//[n31]=1, last data block, and destination data length[n12..n0]
 	li		RX_TEMP1,0xb800a09C
 	sw		RX_TEMP2,0(RX_TEMP1)			//setting destination data length
 	nop
/////////////Save current GDMA address.  /////////////////////////////////////////////////////////
	addiu	RX_TEMP3,RX_TEMP3,0x1000		//source data address
	li		RX_TEMP1,0xA0600028				
	sw		RX_TEMP3,0(RX_TEMP1)			//Save current GDMA address.
	nop
//////////////startting  GDMA process 	/////////////////////////////////
  	li		RX_TEMP2,0xC0000000				//GDMA_ENABLE | GDMA_POLL | GDMA_MEMCPY	//startting  GDMA process
 	li		RX_TEMP1,0xb800a000
 	sw		RX_TEMP2,0(RX_TEMP1)					//startting  GDMA 
 	nop
//////////////polling GDMA copy done flag	/////////////////////////////////
  	li		RX_TEMP2,0x0					// clean data buffer
  	li		RX_TEMP3,0x80000000
 	li		RX_TEMP1,0xb800a008				// GDMA	Interrupt Status Register
 	li		RX_TEMP4,0xb800330c				//RX_TEMP4 IP module status
 	lw		RX_TEMP5,0(RX_TEMP4)			//RX_TEMP5 0xb800330c value.
 	nop
 	
1:
 	lw		RX_TEMP2,0(RX_TEMP1)			// load 
 	nop
 	/////////////add IP module power on/off start/////////////////////////// 	
	//li		RX_TEMP4,0xb800330c
	//lw		RX_TEMP5,0(RX_TEMP4)
	//nop
 	ori		RX_TEMP6,RX_TEMP5,0x1FFF	//enable all IP module
 	sw		RX_TEMP6,0(RX_TEMP4)
 	nop
 	li		RX_TEMP6,0x50
 3:	
 	subu		RX_TEMP6,1
 	nop
 	bnez		RX_TEMP6,3b
 	nop
 	and		RX_TEMP6,RX_TEMP5,~0x1BFF		//enable GDMA only
 	sw		RX_TEMP6,0(RX_TEMP4)
 	nop
 	li		RX_TEMP6,0x50
 3:	
 	subu		RX_TEMP6,1
 	nop
 	bnez		RX_TEMP6,3b
 	nop 	
 	/////////////add IP module power on/off end/////////////////////////// 	
 	and 		RX_TEMP2,RX_TEMP2,RX_TEMP3	
 	nop
 	beqz		RX_TEMP2,1b						//GDMA_not_complete
 	nop 	 	
 	/////////////add IP module power on/off start /////////////////////////// 	
 	sw		RX_TEMP5,0(RX_TEMP4)			//restore IP module status.
 	nop 	
 	/////////////add IP module power on/off end/////////////////////////// 	
//////////load current GDMA adddress, if not equ finish GDMA address, it will jump to
	li		RX_TEMP1,0xA0600028
	lw		RX_TEMP2,0(RX_TEMP1)			//load current GDMA adddress
	nop
	li		RX_TEMP4,0xA0000000
	or		RX_TEMP2,RX_TEMP2,RX_TEMP4
	/////////////////////////////
	li		RX_TEMP1,0xA0600024
	lw		RX_TEMP3,0(RX_TEMP1)			//load finish GDMA adddress
	nop	
 	bgt		RX_TEMP3,RX_TEMP2,RX_GDMA_START	//if finish address > current adddress, jump to RX_GDMA_START
 	nop
///////// save polling wait count  ////////////////
 	//li		RX_TEMP5,0xa0600060
 	//sw		RX_TEMP4,0(RX_TEMP5) 	
//////////software compare data /////////////////
	li		RX_TEMP3,0x8000
	li		RX_TEMP4,0xA0600024
	lw		RX_TEMP6,0(RX_TEMP4)	//load finish GDMA address
	nop	
	sub		RX_TEMP3,RX_TEMP6,RX_TEMP3	//compare last block
	nop
	li		RX_TEMP1,0x0FFFF			//Pattern 
	li		RX_TEMP2,0x5a5aa5a5		//Pattern1
	li		RX_TEMP5,0				//compare length
1:
	lw		RX_TEMP4,0(RX_TEMP3)		//load data source data from SRAM
	nop
	bne		RX_TEMP1,RX_TEMP4,SDRAM_RX_COMPARE_FAIL
	nop
	addiu	RX_TEMP3,RX_TEMP3,4			//load address plus 4
	lw		RX_TEMP4,0(RX_TEMP3)		//load data source data from SRAM
	nop
	not		RX_TEMP5,RX_TEMP1
	bne		RX_TEMP5,RX_TEMP4,SDRAM_RX_COMPARE_FAIL
	nop	
	addiu	RX_TEMP3,RX_TEMP3,4			//load address plus 4
	lw		RX_TEMP4,0(RX_TEMP3)		//load data source data from SRAM
	nop
	bne		RX_TEMP2,RX_TEMP4,SDRAM_RX_COMPARE_FAIL
	nop	
	addiu	RX_TEMP3,RX_TEMP3,4			//load address plus 4
	lw		RX_TEMP4,0(RX_TEMP3)		//load data source data from SRAM
	nop
	not		RX_TEMP5,RX_TEMP2
	bne		RX_TEMP5,RX_TEMP4,SDRAM_RX_COMPARE_FAIL
	nop	
	addiu	RX_TEMP3,RX_TEMP3,4			//load address plus 4
	bgt		RX_TEMP6,RX_TEMP3,1b
	nop
	////////check 0xb800332c[n5] bit //////////////////////////////
	//li		RX_TEMP1,0xb800332c
	//li		RX_TEMP3,0x20
	//lw		RX_TEMP2,0(RX_TEMP1)
	//nop
	//and		RX_TEMP4,RX_TEMP2,RX_TEMP3
	bnez		RX_MSB__flag,PLUS_16
	nop	
	////////////////////////////////////////////
	li		RX_TEMP3,1
	b		SAVE_RX_VALUE
	nop
PLUS_16:
	li		RX_TEMP3,0x10000
SAVE_RX_VALUE:
	li		RX_TEMP1,0xA060001C
	lw		RX_TEMP2,0(RX_TEMP1)				//load rx pass parameter
	nop
	//li		RX_TEMP3,1
	sll		RX_TEMP3,RX_TEMP3,RX_PARA
	nop
	or		RX_TEMP2,RX_TEMP2,RX_TEMP3
	sw		RX_TEMP2,0(RX_TEMP1)				//save rx pass parameter
	nop
	/////////////////////////////////////////////
	addiu	RX_PARA,RX_PARA,1					//Rx parameter plus 
	nop
	//beq		RX_PARA,0x10,RX_FINISH			//if Rx parameter more then 0x10, set R0 value
	bnez		RX_MSB__flag,n4_en					//check bit4, if bit4 enable, scan range from 0 to 3
	nop
	bne		RX_PARA,0x10,RX_PRAR_CH			//if Rx parameter more then 0x10, set R0 value
	nop
	b		set_bit4
	nop
n4_en:
	bne		RX_PARA,0x4,RX_PRAR_CH				//if Rx parameter more then 0x3, set R0 value
	nop
set_bit4:	
	////////reset RX_PARA and set 0xb800332c[n5] clk_rx_delay_sel[4]/////////////////////////////////////////
	li		RX_TEMP1,0xb800332c
	li		RX_TEMP3,0x20
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	and		RX_TEMP4,RX_TEMP2,RX_TEMP3
	beq		RX_TEMP4,0x20,RX_FINISH
	nop
	li		RX_PARA,0						//clean RX PARA
	or		RX_TEMP4,RX_TEMP2,RX_TEMP3		//set 0xb800332c[n5]
	nop
	li		RX_MSB__flag,0x1
	sw		RX_TEMP4,0(RX_TEMP1)
	nop
	b		RX_PRAR_CH
	nop

SDRAM_RX_COMPARE_FAIL:
	addiu	RX_PARA,RX_PARA,1				//Tx parameter plus 
	nop
	bnez		RX_MSB__flag,n4_fail
	nop
	beq		RX_PARA,0x10,RX_FINISH			//TX_PRAR max 0x10
	nop
	b		RX_PRAR_CH
	nop	
n4_fail:
	beq		RX_PARA,0x4,RX_FINISH			//TX_PRAR max 0x4
	nop
	b		RX_PRAR_CH
	nop
	
RX_FINISH:
	li		RX_TEMP1,0xb800332c
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	li		RX_TEMP3,0x20
	not		RX_TEMP3,RX_TEMP3
	and		RX_TEMP3,RX_TEMP3,RX_TEMP2
	sw		RX_TEMP3,0(RX_TEMP1)				//clean 0xb800332c[n5]
	////////////////////////////////////////////
	li		RX_TEMP1,0xA060001C
	lw		RX_TEMP2,0(RX_TEMP1)				//load rx pass parameter
	nop
	li		RX_TEMP3,0
	li		RX_TEMP4,1
1:	
	srl		RX_TEMP6,RX_TEMP2,RX_TEMP3
	and		RX_TEMP5,RX_TEMP4,RX_TEMP6
	addiu	RX_TEMP3,RX_TEMP3,1
	beq		RX_TEMP3,0x20,SDRAM_FIXED_VALUE
	nop
	beqz		RX_TEMP5,1b
	nop
	subu		RX_TEMP3,1
	move	RX_L0,RX_TEMP3
1:	
	addiu	RX_TEMP3,RX_TEMP3,1
	srl		RX_TEMP6,RX_TEMP2,RX_TEMP3
	and		RX_TEMP5,RX_TEMP4,RX_TEMP6	
	beq		RX_TEMP3,0x20,SDRAM_FIXED_VALUE
	nop	
	bnez		RX_TEMP5,1b
	nop
	add		RX_TEMP3,RX_TEMP3,RX_L0
	srl		RX_TEMP3,RX_TEMP3,1						//
	////////////////////////////////////
	li		RX_TEMP4,0xF
	bgt		RX_TEMP3,RX_TEMP4,en_bit4
	nop
	////////////////////////////////////
	li		RX_TEMP1,0xb8003304
	li		RX_TEMP4,0xFFFFFFF0
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	and		RX_TEMP2,RX_TEMP2,RX_TEMP4
	or		RX_TEMP3,RX_TEMP3,RX_TEMP2
	sw		RX_TEMP3,0(RX_TEMP1)				//load rx pass parameter
	nop	
	b		SDRAM_CALI_FINISH
	nop
en_bit4:
	li		RX_TEMP1,0xb8003304
	li		RX_TEMP4,0xFFFFFFF0
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	and		RX_TEMP2,RX_TEMP2,RX_TEMP4
	subu		RX_TEMP3,0x10
	or		RX_TEMP3,RX_TEMP3,RX_TEMP2	
	sw		RX_TEMP3,0(RX_TEMP1)				//load rx pass parameter
	nop	
	b		SDRAM_CALI_FINISH
	nop	
	
SDRAM_FIXED_VALUE:	
 	li		RX_TEMP2,0x0100000F
 	li		RX_TEMP1,0xb8003304
 	sw		RX_TEMP2,0(RX_TEMP1)					//fixed RX delay
 	nop
 	li		RX_TEMP1,0xb800332c
 	li		RX_TEMP2,0x0
 	sw		RX_TEMP2,0(RX_TEMP1)					//fixed RX delay
 	nop 	
 	nop	

SDRAM_CALI_FINISH:

#undef	RX_TEMP1
#undef	RX_TEMP2
#undef	RX_TEMP6
#undef	RX_REG_ADDR
#undef	RX_REG_VAL
#undef	RX_TEMP5
#undef	RX_PARA
#undef	RX_TEMP3
#undef	RX_TEMP4
#undef	RX_L0
#undef	RX_MSB__flag
#======SDRAM RX Calibration end===========
###############################################	
#if	1		//copy code to DMEM
	li		t7,0xa0600054			//load ra into t6
	lw		t6,0(t7)
	nop
	li		t7,0xa0600050
	lw		t8,0(t7)
	nop
	add		ra,t6,t8					//ra addr plus copy length offset
	j		ra
	nop
#endif		//copy code to DMEM	
SDRAM_RX_CALI_FINISH:
				nop
#endif		//RX calibration......				
				nop	
###############################################	
        b               MEM_CONF_DONE
        nop
        nop
        nop
CCC:
isRL6028B:      /* to be define later */
	Mem_Init_RL6028B

#endif

MEM_CONF_DONE:
	nop
#ifdef	MEM_FULL_TEST
///////copy Memory full test code to SRAM 0xA0606000 ////////
#define		CP_BLK_START_ADDR	t0
#define		CP_BLK_END_ADDR		t1
#define		CP_BLK_LENGTH			t2
#define		CP_BLK_TEMP1			t3
#define		CP_BLK_TEMP2			t4
	la		CP_BLK_START_ADDR, MEM_FULL_TEST_BEGIN
	la		CP_BLK_END_ADDR, MEM_FULL_TEST_FINISH
	sub		CP_BLK_LENGTH,CP_BLK_END_ADDR,CP_BLK_START_ADDR
	li		CP_BLK_TEMP1,0xa0600050											//save copy block length
	sw		CP_BLK_LENGTH,0(CP_BLK_TEMP1)

	li		CP_BLK_TEMP1,0xbfc00000											//boot start address.
	or		CP_BLK_START_ADDR,CP_BLK_START_ADDR,CP_BLK_TEMP1				//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	or		CP_BLK_END_ADDR,CP_BLK_END_ADDR,CP_BLK_TEMP1					//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	
	li		CP_BLK_TEMP1, 0xA0606000										//start address of SRAM reserve area  
	
1:	
	lw		CP_BLK_TEMP2, 0(CP_BLK_START_ADDR)								//load code from flash
	nop
	sw		CP_BLK_TEMP2, 0(CP_BLK_TEMP1)									//restore code to SRAM
	addi		CP_BLK_START_ADDR, CP_BLK_START_ADDR, 4
	addi		CP_BLK_TEMP1, CP_BLK_TEMP1, 4
	bne		CP_BLK_START_ADDR, CP_BLK_END_ADDR, 1b
	nop
	li		CP_BLK_TEMP1, 0xA0606000										//
	jal		CP_BLK_TEMP1													//jump to SRAM
	nop

#undef		CP_BLK_START_ADDR
#undef		CP_BLK_END_ADDR
#undef		CP_BLK_LENGTH
#undef		CP_BLK_TEMP1
#undef		CP_BLK_TEMP2
/////////////////////////////////////////////////////////
MEM_FULL_TEST_BEGIN:
	nop
	move	t6,ra																//save ra address
	li		t7,0xa0600054
	sw		t6,0(t7)
###############################################
#define	MEM_FULL_TEMP0		t0	
#define	MEM_FULL_TEMP1		t1
#define	MEM_FULL_TEMP2		t2
/////////////////////////////////////
	li		MEM_FULL_TEMP0,0xB8001004
	lw		MEM_FULL_TEMP1,0(MEM_FULL_TEMP0)								//load mem size
	nop
	andi		MEM_FULL_TEMP1,MEM_FULL_TEMP1,0x


###############################################	
	li		t7,0xa0600054			//load ra into t6
	lw		t6,0(t7)
	nop
	li		t7,0xa0600050
	lw		t8,0(t7)
	nop
	add		ra,t6,t8					//ra addr plus copy length offset
	j		ra
	nop
MEM_FULL_TEST_FINISH:
#endif
	/* setup bootloader stack */
	
#tylo, for 8672 fpga
#ifndef RTL8672
#ifdef MINIMODEL_SUPPORT
	li		t0,0x00400000	//config GPA6 to output 0
	li		t1,0xB9C01000
	sw		t0,0x00(t1)
	sw		zero,0x04(t1)	//full low GPA6
#elif Flash_AA21_GPA5
	li		t0,0x00200000	//config GPA5 to output 0
	li		t1,0xB9C01000
	sw		t0,0x00(t1)
	sw		zero,0x04(t1)	//full low GPA5
#else
        li              t0,0x00FF0000
        li              t1,0xB9C01000
        sw              zero,0x00(t1)
        sw              zero,0x04(t1)
	sw		t0,0x08(t1)
	sw		zero,0x0C(t1)
	sw		zero,0x10(t1)
	sw		zero,0x14(t1)
	sw		t0,0x18(t1)
	sw		zero,0x1C(t1)
#endif	
#endif
	#--- invalidate the icache and dcache with a 0->1 transition
	mtc0	zero, $20
	nop
	nop
#ifdef CPU_LX4181
	li	t0, 0x202
#else
	li	t0, 0x3
#endif
	mtc0	t0, $20
	nop
	nop
	mtc0	zero, $20
	nop
	nop
	#--- initialize and start COP3
	mfc0	t0,$12
	nop
	or		t0,0x80000000
	mtc0	t0,$12
	nop
	nop
	
	# disable IRAM
	li		t0, 0x84000000	#address without SDRAM
	and		t0, 0x0ffffc00    # translate to physical address
	mtc3	t0, $0      # $0: i-ram base
	nop
	nop
	li		t0, 4096 - 1
	mtc3	t0, $1      # $1: i-ram top
	nop
	nop
	
	# disable DRAM
	li	t0, 0x85000000	#address without SDRAM
	and	t0, 0x0ffffc00    # translate to physical address
	mtc3	t0, $4      # $4: d-ram base
	nop
	nop
	li	t0, 4096 - 1
	mtc3	t0, $5      # $5: d-ram top
	nop
	nop
	#--- enable icache and dcache
	mtc0	$0, $20	# CCTL
	nop
	nop
	.set	reorder

	#--- configure memory controller
	#---this is for ASIC

#--- determine memory configuraton 
#ifdef SDRAM_AUTO_DETECT
	Detect_RAM_Size
#endif //SDRAM_AUTO_DETECT

	li		t0, 0xbfc0ff00			# parameter start address
keep_search:	
	lw		t1, 0(t0)			
	nop
	beq		t1, 0x4265726c, found_param	# is "Berl" ?
	nop
	add		t0, 0x40
	beq		t0, 0xbfc10000, default_config	# not found parameter, use default
	nop
	b		keep_search
found_param:
	
	li		a0, 0x02800000			# init mcr0 
	lw		t1, 48(t0)			# load flash size
	nop
	li		v0, 0xffffffff			# check if parameters stored in flash is valid?
	beq		t1, v0, default_config		# default setting in board.h MCR0
	
	li		t2, 0xb8001100			# NOR flash configuration register
	sw		t1, 0(t2)			# set NFCR
	nop
#ifndef SDRAM_AUTO_DETECT	
	lw		t1, 52(t0)			# load mem size
	nop
	li		t2, 0xB8001004
	sw		t1, 0(t2)			# set memory configuration
#endif //#ifndef SDRAM_AUTO_DETECT
	b		go_ahead
default_config:
#ifndef SDRAM_AUTO_DETECT
	li		t0,MCR0_VAL
	li		t1,0xB8001004
	#tylo, for 8672 fpga
	sw		t0,0(t1)
#endif //#ifndef SDRAM_AUTO_DETECT
        li		t0,BOARD_PARAM_FLASHSIZE
	li		t1,0xB8001100
	sw		t0, 0(t1)

go_ahead:	
	# Turns on MA22,MA21 if flash size > 4MB
        li              t0, 0xB8001100                  # load flash size parameter
        lw              t1, 0(t0)
	nop
	and		t1, 0x00070000			# retrieve flash size 
	srl		t1, 16				# shift right 15 bits
	addiu		t1, -4				
	blez		t1, copy_to_ram			# skip if flash size <= 4MB
	nop
	li		t2, 0xb8003304			# turns on NICLED1_MA22, NICLED0_MA21
	lw		t1, 0(t2)	
	nop
	or		t1, t1, 0x00c00000
	sw		t1, 0(t2)
	nop

copy_to_ram:

#ifndef RTL8672
#	li		t0,MCR0_VAL
#	li		t1,0xB9000000
#	sw		t0,0(t1)

#---- determine memory configuration end here
	li		t0,MCR1_VAL
	li		t1,0xB9000004
	#tylo, for 8672 fpga
	#sw		t0,0(t1)

	#--- this is for ASIC
	li		t0,MCR2_VAL
	li		t1,0xB9000008
	#tylo, for 8672 fpga
	#sw		t0,0(t1)

    #--- hrchen, this is for device bus weighting
	li		t0,CPUC_VAL
	li		t1,0xB9C03000
	#tylo, for 8672 fpga
	#sw		t0,0(t1)

	#-- Enable system clock spectrum 
	#-- let memory driving from 14mA to 8mA
	#-- Read timing tuning from 00 to 10
        li		t0,0xF0257C0C 
	li		t1,0xb9c04004
	#tylo, for 8672 fpga
	#sw		t0,0(t1)
	
#ifdef SAMSUNG_SUPPORT 
       li              t0, 0xe825000c
       li              t1, 0xb9c04004
       sw              t0, 0(t1)
#endif


#--- shlee  determine D/F version 8671 reserve bit 22

	la	t3,0x00400000	
	la 	t0,0xb9c04000
	#tylo, for 8672 fpga
	#lw	t1,0(t0)
		
	la	t3, 0x00400000
	and	t2, t1, t3
# -- 	determine D/F version 8671 reserve bit 22 

	#write 0 to reset PCI	
	li		t0,SICR_VAL
	li		t1,0xB9C04000
	
#	andi	t2, t0, 0x00001000
#	beq     zero, t0, PCI_disabled
	and     t0,t0,~0x80000000
	
#--- shlee  determine D/F version 8671 reserve bit 22
	or	t0, t2, t0	
#--- determine D/F version 8671 reserve bit 22
	
	#tylo, for 8672 fpga
	#sw		t0,0(t1)
	
	#delay for > 1ms, 0x00010000 is near 500ms
	li		t2, 0x00001000
DEV_reset_delay:
    addi    t2, t2, -1
    nop
    bne     zero, t2, DEV_reset_delay

	#--- this is for System Interface
#--- shlee  determine D/F version 8671 reserve bit 22	
#ifdef Flash_AA21_GPA5
	or	t0, t0, 0x80000020  # GPIO interface as PCI
#else
	or 	t0, t0,0x80000000
#endif
	#tylo, for 8672 fpga
	#sw	t0, 0(t1)
#--- determine D/F version 8671 reserve bit 22
	
#-- shlee for version D/F   	li		t0,SICR_VAL
#PCI_disabled:
#-- shlee for version D/F	sw		t0,0(t1)

#endif //#ifndef RTL8672
	#--- copy itself to RAM
	la		t0, FLASH_START
	la		t1, FLASH_START+0x20000
	la		t2, MEM_START
1:	lw		t3, 0(t0)
	nop
	sw		t3, 0(t2)
	addi	t0, t0, 4
	addi	t2, t2, 4
	bne		t0, t1, 1b
	#tylo, fpga test
	nop
	#flush cache
	mtc0	zero, $20
	nop
	nop
	li	t0, 0x202

	mtc0	t0, $20
	nop
	nop
	mtc0	zero, $20
	nop
	nop	
	la	t0, MEM_START
	jr	t0
	nop
	

2:
	/* absolutely basic initialization to allow things to continue */
	#bal	basicInit
	#tylo, for 8672
	#for memory test 0xb8003200 = 0xffff0000
	#li		t0,0xffff0000
	#li		t1,0xb8003200
	#sw		t0,0(t1)
	nop

#ifdef SUPPORT_8672_PLATFORM
	#--- config lexra bus and sdram clock freque
	li		t1, SCCR
	lw		t0, 0(t1)
	nop

# check SCCR bit 21,20 and 16
	srl 		t0, t0, 0x10
	andi		t0, t0, 0x00000031
	beq 		t0, 17, AAA_ 
	nop
	nop
	beq 		t0, 1,isRLE0390_ 
	nop
	nop
#if 0	
# reload the SCCR value
    lw      t0, 0(t1)
    nop 
#if defined(SYS_CLOCK_175MHZ) || defined(SDRAM_CLOCK_166MHZ) || defined(CPU_CLOCK_340MHZ) || defined(SDRAM_CLOCK_133MHZ)
	srl		t0, t0, 12
	sll		t0, t0, 12
#if defined(SYS_CLOCK_175MHZ)
	addi		t0, t0, 0x40
#endif
#if defined(SDRAM_CLOCK_166MHZ) 
	addi		t0, t0, 0x03
#endif
#if defined(SDRAM_CLOCK_133MHZ) 
	nop
#endif
#if defined(CPU_CLOCK_340MHZ)
	addi		t0, t0, 0x600
#endif
	sw		t0, 0(t1)
	nop
#endif
#endif	//#if 0

isRLE0390_:
AAA_:
#endif
	/* setup bootloader stack */
	la	gp, _gp			/* set global ptr from cmplr         */

	la	sp, MEM_LIMIT-8	/* set temp stack ptr                */
	mtc0	zero, $20
	nop
	nop
#ifdef CPU_LX4181
	li	t0, 0x202
#else
	li	t0, 0x3
#endif
	mtc0	t0, $20
	nop
	nop
	mtc0	zero, $20
	nop
	nop
#	or	sp, K1BASE		/* make it uncached                  */

	/* Init BSS spaces */
	la	t0, _fbss
	la	t1, _ebss
	beq		t0, t1, 2f
1:	sw	zero, 0(t0)
	addi	t0, t0, 4
	bne	t0, t1, 1b
2:
	
	/* jump to SDRAM */
	la	t0, C_Entry
	jr	t0
	nop

fail:
	li	t0,0xbfc00000
	nop
	jr	t0
	nop

	b	fail



/*******************************************************************************
*
* romExcHandle - rom based exception/interrupt handler, do nothing but hang
*/

	.ent	romExcHandle
romExcHandle:
hangExc:
	b	hangExc			/* HANG UNTIL REBOOT                 */
	.end	romExcHandle

