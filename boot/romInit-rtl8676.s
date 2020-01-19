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


#if defined(CONFIG_RAM_AUTO)
#define DRAM166		1
#elif defined(CONFIG_RAM_166)
#define DRAM166		1
#elif defined(CONFIG_RAM_133)
#define DRAM133		1
#else
#error "RAM Clock undefined"
#endif

/* definition MUST go before macro.s */
#include "macro.s"

#define NAND_FLASH_START 0x00000000
#define NAND_MEM_START 0x00000000
#define NAND_CHECK_READY()      \
123:;							\
	la		s0, NAND_CTRL_BASE+0x4; \
	lw    	t0, 0(s0);       \
	and   	t0, t0, 0x80000000;      \
	bne   	t0, 0x80000000, 123b;    \
	nop

#define PCIe_module

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

__romInit:
	.set	noreorder
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

///////////////////////////////////////////////////////////
#ifdef SUPPORT_8676_PLATFORM  //----------------------- start SUPPORT_8676_PLATFORM -------------------------------------//
	
	#--- the supported flash type 
	li		t1, Boot_Select
	lw		t0, 0(t1)
	nop
	and		t0, t0, 0x40
	nop
	beqz		t0, SPIandNOR
	nop

NAND:
	SRAM_Map 0x1fc01000, 0x5, 0x10, 0x1000	# map 0xbfc01000 (4K) to SRAM segment 1

	#--- copy next 4K bytes to 0x1fc01000 ---#	
	#ifdef CONFIG_NAND_PAGE_2K
	NAND_Copy_2048 	2, 4, 0x1fc01000, 0x20000
	#else
	NAND_Copy_512 	8, 16, 0x1fc01000, 0x20000
	#endif


SPIandNOR:

isRTL8676:
	Mem_Init_RTL8676
	
	
	////// SRAM mapping Segment 2 ,16k bytes	//////
	//SRAM_Map 0x00705000, 0x7, 0x00, 0x0000	# map 0x80705000 (16K) to SRAM segment 0
	SRAM_Map 0x00600000, 0x7, 0x30, 0x2000	# map 0x80600000 (16K) to SRAM segment 2
	//clean SRAM 
	Mem_Set_Word 0xa0600000,0x4000,0x0  // fill 0
		
###############################################
	li		t0,0xb8000308		//detect DDR or SDR on board.
	lw		t9,0(t0)
	nop
	srl 	t9,8
	and		t8,t9,0x2
	beqz	t8,is_SDRAM
	nop
###############################################	////////////// //Disable clock SSD  //////////////////////////
	li		t0,0x80000000		//Disable clock SSD for DDR, DDR SDRAM
	li		t1,0xb8000220
	sw		t0,0(t1)
	nop	
	Noop_Delay	100
############################################## 		
//	SDR & DDR I/O PAD current select for test only. 	
	//li		t0,0x00000407		//all DDR bus, driving upgrade	
	//li		t0,0x00000404		//DDR bus,data, clock driving upgrade	
	li		t0,0x0000400			//data bus, driving upgrade
	//li		t0,0x0				//driving normally
	li		t1,0xb8000314
	sw		t0,0(t1)
	nop	
############################################## 		
# SDRAM/DDR Memory timing register, MUST set before MCR(config. reg)
#ifdef DRAM166
	//li		t0,0x6CEC8480
	//li		t0,0x6CF58480		//for 1Gbit DRAM
	li		t0,0x6CFF8480		//for 2Gbit DRAM
	li		t2,0x54A00000		//RX DQS calibration default value, PLL analog calibration value for DDR only.*	.	
#else										//DRAM_133MHz:
	li		t0,0x6CFA7480		//for 2Gbit DRAM
	//li		t0,0x6CEBE280		//DTR parameter
	li		t2,0x5EF00000		//RX DQS calibration default value, PLL analog calibration value for DDR only.*	.
#endif
	nop
	li		t1,0xb8001008		//set_DTR:
	sw		t0,0(t1)
	nop	
	li		t1,0xb8001050
	sw		t2,0(t1)
	nop	
############################################## 	 	
#if 1		//DRAM PLL reset
		li 		t0,0x10000131
		li		t1,0xb800100c
	sw		t0,0(t1)
	nop		
		li		t0,0x524a0000
		li		t1,0xb8001004		
		sw		t0,0(t1)		
		Noop_Delay 1200		
#endif
	
############################################## 	 	
	li		t0,0x52480000							//16M, for SDRAM & DDR
	li		t1,0xB8001004							//initial DDR signal re-send
	sw		t0,0(t1)
	nop			
#################################################		
 	and		t8,t9,0x1
 	beqz	t8,is_DDR1
 	nop
is_DDR2:
##############################################
// MRS configuration for DDR setting.
#if 1
// MRS configuration for DDR setting.
#define	MCR_PARA			t0
#define	MRS_VALUE			t1
#define	EMRS_VALUE			t2
#define	EDTCR_ADDR			t3
#define	EDTCR_ORG			t4
#define	MCR_ADDR			t5
#define	REG_TEMP			t6
#define	REG_TEMP2			t7
#define	MCR_ORG			t8
#define	REG_TEMP1			t9
	li		MCR_ADDR,0xb8001004
	li		EDTCR_ADDR,0xb800100c
	//li		MRS_VALUE,0x31					//DLL reset=0, TM=0, CAS# Latency=3, BurstType=0, BurstLength=1(2), n15..n14=00(Mode Register)
	//li		EMRS_VALUE,0x4406				//I/O Strength reduce (half), n15..n14=01(EMR), ODT 75ohm,DQS# disable
	lw		MCR_ORG,0(MCR_ADDR)			//Load MCR
	nop	
	or		MCR_PARA,MCR_ORG,0x20000		//MR MODE EN.	
#if defined(CONFIG_DDR2_8banks)
	li		REG_TEMP,0x50004406						//set EMRS value, supported 8 banks for DDR2, and modified WTR from 111b -> 010b
#else
	li		REG_TEMP,0x10004406						//set EMRS value, supported 4 banks for DDR2, and modified WTR from 111b -> 010b
#endif	
	nop
#if 1
	sw		REG_TEMP,0(EDTCR_ADDR)				//set EMRS for DDR1 initial 
1:																		
	lw		REG_TEMP2,0(MCR_ADDR)
	nop
	and		REG_TEMP2,REG_TEMP2,0x1				//check DCR_WR_BUSY
	nop
	bnez	REG_TEMP2,1b
	nop
#endif
	sw		MCR_PARA,0(MCR_ADDR)					//set MR mode enable 
1:
	lw		REG_TEMP2,0(MCR_ADDR)
	nop
	and		REG_TEMP2,REG_TEMP2,0x1				//check DCR_WR_BUSY
	nop
	bnez	REG_TEMP2,1b
	nop
#if 1	
	sw		MCR_ORG,0(MCR_ADDR)						//MR MODE Disable.
	nop
1:
	lw		REG_TEMP2,0(MCR_ADDR)
	nop
	and		REG_TEMP2,REG_TEMP2,0x1				//check DCR_WR_BUSY
	nop
	bnez	REG_TEMP2,1b
	nop
#endif
#undef	MCR_PARA
#undef	MRS_VALUE
#undef	EMRS_VALUE
#undef	EDTCR_ADDR
#undef	MCR_ADDR
#undef	REG_TEMP
#undef	MCR_ORG
#undef	REG_TEMP2
#undef	EDTCR_ORG
	b			DDR_Pre_Setup_finish
	nop
#endif	
##############################################	
is_DDR1:
#if 1
	li		t5,0xb8001004
	li		t1,0x52480000
	sw		t1,0(t5)
1:
	lw		t2,0(t5)
	nop
	and		t2,t2,0x1				//check DCR_WR_BUSY
	nop
	bnez		t2,1b
	nop	
	li		t0,0xb800100c
	li		t1,0x10004002								//modified WTR from 111b -> 010b
	sw		t1,0(t0)
	nop
	nop
	li		t1,0x524a0000
	sw		t1,0(t5)
	nop
	Noop_Delay 100
1:
	lw		t2,0(t5)
	nop
	and		t2,t2,0x1				//check DCR_WR_BUSY
	nop
	bnez		t2,1b
	nop		
	li		t1,0x52480000
	sw		t1,0(t5)
	nop
#endif

DDR_Pre_Setup_finish:
	Noop_Delay 1200		
###############################################
	////////////enable GDMA module 
	//enable GDMA module 0xb800030c[n10]=1
	li	t1,0xb800030c	
	lw	t7,0(t1)			//load 0xb800030c
	nop
	ori	t7,t7,0x400		//set 0xb800030c[n10]=1
 	nop
 	sw	t7,0(t1)			//enable GDMA module
 	nop
###############################################
//////// Global paramter setup //////
#if defined(CONFIG_RAM_AUTO)
#define CLK_133MHZ_FLAG	s2
#ifdef	DRAM166
 	move	CLK_133MHZ_FLAG,zero		//default run 166MHz
#else	 	
	li		CLK_133MHZ_FLAG,1		//run in 133MHz
#endif	
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

	li		CP_BLK_TEMP1,FLASH_START											//boot start address.
	or		CP_BLK_START_ADDR,CP_BLK_START_ADDR,CP_BLK_TEMP1				//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	or		CP_BLK_END_ADDR,CP_BLK_END_ADDR,CP_BLK_TEMP1					//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	
	li		CP_BLK_TEMP1, 0xA0600100										//start address of SRAM reserve area  
	
1:	
	lw		CP_BLK_TEMP2, 0(CP_BLK_START_ADDR)								//load code from flash
	nop
	sw		CP_BLK_TEMP2, 0(CP_BLK_TEMP1)									//restore code to SRAM
	addi		CP_BLK_START_ADDR, CP_BLK_START_ADDR, 4
	addi		CP_BLK_TEMP1, CP_BLK_TEMP1, 4
	bne		CP_BLK_START_ADDR, CP_BLK_END_ADDR, 1b
	nop
	li		CP_BLK_TEMP1, 0xA0600100										//
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
#define	PATTERN_TEMP1		t1
#define	PATTERN_TEMP2		t2
#define	PATTERN_LENGTH		t3
#define	PATTERN				t4		
#define	PATTERN1			t5
PATTERN_START:
	li		PATTERN,0x5a5aa5a5							//default pattern
	li		PATTERN1,0x0FFFF								//default pattern
////////////GEN data from 0xa0602000 to 0xa0602FFF, 4k bytes
	li		PATTERN_TEMP1,0xa0602000				//start address
	li		PATTERN_LENGTH,0x1000						//4k bytes, end address
1:	
	sw		PATTERN1,0(PATTERN_TEMP1)				//pattern1(0x0FFFF)
	addi	PATTERN_TEMP1,PATTERN_TEMP1,4
	not		PATTERN_TEMP2,PATTERN1					//pattern1 reverse (0xFFFF0000)
	sw		PATTERN_TEMP2,0(PATTERN_TEMP1)
	addi	PATTERN_TEMP1,PATTERN_TEMP1,4
	sw		PATTERN,0(PATTERN_TEMP1)				//pattern(0x5a5aa5a5)
	addi	PATTERN_TEMP1,PATTERN_TEMP1,4
	not		PATTERN_TEMP2,PATTERN						//pattern reverse(0xa5a55a5a)
	sw		PATTERN_TEMP2,0(PATTERN_TEMP1)
	addi	PATTERN_TEMP1,PATTERN_TEMP1,4
	subu	PATTERN_LENGTH,0x10
	bnez	PATTERN_LENGTH,1b
	nop
#undef	PATTERN_TEMP1
#undef	PATTERN_TEMP2
#undef	PATTERN_LENGTH	
#undef	PATTERN					
#undef	PATTERN1
GENERATE_PATTERN_FINISH:
///////// generate data finish /////////

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
	li		DQS_TEMP2,0x1D02					//version: 1.02
	sw		DQS_TEMP2,0(DQS_TEMP1)
//// insert DDR calibration version, record in 0xA0600018 //end///	
DQS_CALIBRATION_START:	
///////Initial REG. default value ///////////
	li		SW_SRAM_ADDR,0xa0603000
	li		DQS_TEMP1, 0xb8001050
 	li		SW_DDCR_VAL, 0x40000000           			//  [n31]=1:sw cal, 0:hw cal
	sw		SW_DDCR_VAL, 0(DQS_TEMP1)
	li		DQS_CALI_TEMP,0
 	li		UP_LIMIT, 31
 	li		RUN_CYCLE,0
 	move	L0, zero
 	move	L1, zero
 	move	L_DONE_BIT, zero
 	li		R0, 0
 	li		R1, 0

///////Initial REG. default value finish///////////	
////////////GEN data to DDR, the address from 0xa0602000 to 0xa0602FFF, 4k bytes  ////////
	nop
	li		DQS_TEMP1,0xA0612000				//start address	(DDR start address)
	li		DQS_TEMP2,0xA0613000				//end address	(DDR end address)
	li		DQS_TEMP4,0xA0602000				//start address	(SRAM start address)
1:	
	lw		DQS_TEMP3,0(DQS_TEMP4)				//read data form SRAM
	sw		DQS_TEMP3,0(DQS_TEMP1)				//write data to DDR
	addiu	DQS_TEMP1,DQS_TEMP1,4
	addiu	DQS_TEMP4,DQS_TEMP4,4	
	bne		DQS_TEMP1,DQS_TEMP2,1b	
	nop
//////////// GEN data to DDR finish ///////////////
CALIBRATE_DQS0:
#if defined(CONFIG_RAM_AUTO)
	bgt		RUN_CYCLE,2,RUN133MHz					//if RUN_CYCLE > 2, jump RUN133MHz
#else
	bgt		RUN_CYCLE,2,FIXED_DQS					//if RUN_CYCLE > 2, jump FIXED_DQS
#endif	
	nop
 	li		POINT_CURSOR, 0
CALIBRATE_DQS0_LOOP1:
////////////Clearn data from 0xa0603000 to 0xa0603FFF, 4k bytes
	move	DQS_TEMP1,SW_SRAM_ADDR			//start address
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
	bltu	UP_LIMIT, POINT_CURSOR, CALIBRATE_DQS0_LOOP1_OUT
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
	and		DQS_TEMP2,SW_SRAM_ADDR,0x0FFFFFFF	//destination data address
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
//////////software compare data /////////////////
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0x00FF00FF	
 	subu	DQS_TEMP5,4	
	//bnez		L_DONE_BIT, CALIBRATE_DQS0_R0_SET
	//nop 	

1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK					//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4
	bne		DQS_TEMP2,DQS_TEMP4,CALIBRATE_DQS0_LOOP1_REENTRY	//compare pattern
	nop
	subu	DQS_TEMP5,4
	bnez	DQS_TEMP5,1b
	nop
	move 	L0, POINT_CURSOR
	addiu	L_DONE_BIT,L_DONE_BIT,1	
CALIBRATE_DQS0_R0_SET:	
	move	DQS_TEMP1,SW_SRAM_ADDR							//start compare address						
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0x00FF00FF	
	subu	DQS_TEMP5,4 	
1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM ( just use GDMA copy data from DDR to SRAM)
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4	
	bne		DQS_TEMP2,DQS_TEMP4,Check_R0_value					//compare pattern
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
Check_R0_value:  	
  	beqz		R0,L0_plus1_CLR_L_DONE					//if R0=0, L0 plus 1
  	nop
  	beq		R0,L0,L0_plus1_CLR_L_DONE				//if R0=L0, L0 plus 1
  	nop
  	sub		DQS_TEMP1,R0,L0
	blt		DQS_TEMP1,2,L0_plus1_CLR_L_DONE  		//if (R0-L0) < 2 ,L0 plus 1
	nop
  	b		CALIBRATE_DQS0_LOOP1_R0_OUT
  	nop
L0_plus1_CLR_L_DONE:
	li		L_DONE_BIT,0
	li		R0,0
	b		CALIBRATE_DQS0_LOOP1_REENTRY
	nop
##################
CALIBRATE_DQS0_LOOP1_R0_OUT:
	addiu	RUN_CYCLE,RUN_CYCLE,1
	beqz		R0,CALIBRATE_DQS0
	nop
	add		DQS_TEMP1,R0,L0
	beqz		DQS_TEMP1,CALIBRATE_DQS0				//if (R0+L0) = 0, re-try agaim.
	nop
	sub		DQS_TEMP1,R0,L0
	blt		DQS_TEMP1,2,CALIBRATE_DQS0				//if (R0-L0) < 2 , the vale is very close, so re-try again.
	nop
	beq		R0,L0,CALIBRATE_DQS0					//if R0=L0, re-try again.
	nop
CALIBRATE_DQS0_LOOP1_OUT:	
###################
	beqz		R0,CALIBRATE_DQS0_LOOP1_R0_OUT
	nop
	move	DQS_TEMP1, L0										//move L0 to DQS_TEMP1
	add		DQS_TEMP1, DQS_TEMP1, R0				//DQS_TEMP1 equ sum( L0 + R0 )
	srl 		DQS_TEMP1, DQS_TEMP1, 1					//calibration value div 2		

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
#if defined(CONFIG_RAM_AUTO)
	bgt		RUN_CYCLE,2,RUN133MHz					//if RUN_CYCLE > 2, jump RUN133MHz
#else	
	bgt		RUN_CYCLE,2,FIXED_DQS					//if RUN_CYCLE > 2, jump FIXED_DQS	
#endif	
	nop
 	move	POINT_CURSOR, zero	
CALIBRATE_DQS1_LOOP1: 	
////////////Clearn data from 0xa0603000 to 0xa0603FFF, 4k bytes
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
	bltu	UP_LIMIT, POINT_CURSOR, CALIBRATE_DQS1_LOOP1_OUT	//out of parameter range
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
	and		DQS_TEMP2,SW_SRAM_ADDR,0x0FFFFFFF	//destination data address
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
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0xFF00FF00	
	subu		DQS_TEMP5,4 	
	//bnez		L_DONE_BIT, CALIBRATE_DQS1_R1_SET
	//nop
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
	li		DQS_TEMP3,0xA0612000
	li		DQS_TEMP5,0x1000									//length, end compare address
 	li		DATA_MASK,0xFF00FF00	
	subu		DQS_TEMP5,4 	
1:	
	lw		DQS_TEMP2,0(DQS_TEMP1)								//read data from SRAM
	nop
	lw		DQS_TEMP4,0(DQS_TEMP3)								//read data from DRAM
	nop
	and		DQS_TEMP2,DQS_TEMP2,DATA_MASK						//data and mask.
	and		DQS_TEMP4,DQS_TEMP4,DATA_MASK
	addiu	DQS_TEMP1,DQS_TEMP1,4								//address plus 4
	addiu	DQS_TEMP3,DQS_TEMP3,4								//address plus 4	
	bne		DQS_TEMP2,DQS_TEMP4,Check_R1_value					//compare pattern
	nop
	subu		DQS_TEMP5,4
	bnez		DQS_TEMP5,1b
	nop	
	move 	R1, POINT_CURSOR
	nop		
CALIBRATE_DQS1_LOOP1_REENTRY:	
	addiu 	POINT_CURSOR, POINT_CURSOR, 1
	b 		CALIBRATE_DQS1_LOOP1
  nop
Check_R1_value:  	
  	beqz		R1,L1_plus1_CLR_L_DONE					//if R1=0, L1 plus 1
  	nop
  	beq		R1,L1,L1_plus1_CLR_L_DONE				//if R1=L1, L1 plus 1
  	nop
  	sub		DQS_TEMP1,R1,L1
	blt		DQS_TEMP1,2,L1_plus1_CLR_L_DONE  		//if (R1-L1) < 2 ,L1 plus 1
	nop
  	b		CALIBRATE_DQS1_LOOP1_R1_OUT
	nop
L1_plus1_CLR_L_DONE:
	li		L_DONE_BIT,0
	li		R1,0
	b		CALIBRATE_DQS1_LOOP1_REENTRY
	nop
CALIBRATE_DQS1_LOOP1_R1_OUT:
	addiu	RUN_CYCLE,RUN_CYCLE,1
	beqz		R1,CALIBRATE_DQS1						//if R1=0, re-try again.
	nop
	add		DQS_TEMP1,R1,L1
	beqz	DQS_TEMP1,CALIBRATE_DQS1				//if (R1+L1) = 0, re-try agaim.
	nop
	sub		DQS_TEMP1,R1,L1
	blt		DQS_TEMP1,2,CALIBRATE_DQS1				//if (R1-L1) < 2 , the vale is very close, so re-try again.
	nop
	beq		R1,L1,CALIBRATE_DQS1					//if R1=L1, re-try again.
	nop
	beqz	R1,CALIBRATE_DQS1_LOOP1_REENTRY
	nop	
CALIBRATE_DQS1_LOOP1_OUT:	
####################
	beqz		R1,CALIBRATE_DQS1_LOOP1_R1_OUT
	nop
	add		DQS_TEMP1, L1, R1							//DQS_TEMP1 equ sum( L1 + R1 )
	srl 	DQS_TEMP1, DQS_TEMP1, 1						//calibration value div 2		

#######################	
	sll		DQS_TEMP1, DQS_TEMP1, 20					// left shift 20 bit for DQS1 parameter.
	or		DQS_CALI_TEMP, DQS_CALI_TEMP, DQS_TEMP1	//finish DQS1 calibration, save in DQS_CALI_TEMP
	li		DQS_TEMP1,0xb8001050
	sw		DQS_CALI_TEMP, 0(DQS_TEMP1)
////////////////////////////////////////////////////
RECORD_DATA:
	li		DQS_TEMP3,0x0
#if defined(CONFIG_RAM_AUTO)	
	beqz		CLK_133MHZ_FLAG,CLK_166MHZ_RECORD_DATA
	nop
	li		DQS_TEMP3,0x10
#endif	
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
#if defined(CONFIG_RAM_AUTO)
	li		DQS_TEMP2,0x8								//if (R0-L0)<8, the DRAM clock will down gread to 133MHz
	sub		DQS_TEMP1,R0,L0
	bnez		CLK_133MHZ_FLAG,DQS_133MHz_ONGOING
	nop	
	bge		DQS_TEMP2,DQS_TEMP1,RUN133MHz  
	nop
	sub		DQS_TEMP1,R1,L1	
	bge		DQS_TEMP2,DQS_TEMP1,RUN133MHz  
	nop
DQS_133MHz_ONGOING:
	//nop
	b		CALIBRATE_OUT
	nop
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
	bnez		CLK_133MHZ_FLAG,FIXED_DQS
	nop
	li		DQS_TEMP1,0xb8000200
	lw		DQS_TEMP2,0(DQS_TEMP1)
	li		DQS_TEMP3,~0x0F0000
	and		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	li		DQS_TEMP3,0x6								//0xb8000200[n19..n16] to setup DRAM clock rate. RTL8676(8:166MHz, 6:133MHz)
	sll		DQS_TEMP3,16
	or		DQS_TEMP2,DQS_TEMP2,DQS_TEMP3
	sw		DQS_TEMP2,0(DQS_TEMP1)
	li		DQS_TEMP3,300						//delay count.
1:
	subu		DQS_TEMP3,1
	bnez		DQS_TEMP3,1b
	nop		
	li		DQS_TEMP1,0xb8001008
	li		DQS_TEMP2,0x6CEB84C0
	sw		DQS_TEMP2,0(DQS_TEMP1)
	li		DQS_TEMP1,0xb8001050
	li		DQS_TEMP2,0x5ef00000
	sw		DQS_TEMP2,0(DQS_TEMP1)
############################################## 	 	
	//DRAM PLL reset
	li 		t0,0x10000131
	li		t1,0xb800100c
	sw		t0,0(t1)
	nop
	li		t0,0x524a0000
	li		t1,0xb8001004		
	sw		t0,0(t1)		
	Noop_Delay 1200
############################################## 		
	li		DQS_TEMP1,0xb8001004
	li		DQS_TEMP2,0x52480000
	sw		DQS_TEMP2,0(DQS_TEMP1)
	li		CLK_133MHZ_FLAG,0x1
	b		DQS_CALIBRATION_START
	nop	
#endif	
	
CALIBRATE_OUT:
	beqz		R0,FIXED_DQS
	nop
	beqz		R1,FIXED_DQS
	nop
	b		FINISH_CALI_DQS
	nop
	//nop
FIXED_DQS:
#ifdef DRAM166
 	li		DQS_TEMP2,0x54a00000					//RUN in 166MHz
#else
 	li		DQS_TEMP2,0x5ef00000					//RUN in 133MHz
#endif
 	li		DQS_TEMP1,0xb8001050
 	sw		DQS_TEMP2,0(DQS_TEMP1)					//fixed DQS
FINISH_CALI_DQS:
	
#if	0		//fixd DQS
  li		DQS_TEMP2,0x54A00000					//RUN in 166MHz
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
#else
#ifdef DRAM166
 	li		t0,0x54a00000			//RUN in 166MHz
#else
 	li		t0,0x5ef00000				//RUN in 133MHz
#endif
 	li		t1,0xb8001050
 	sw		t0,0(t1)					//fixed DQS	
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
	j		ra
	nop

DQS_CALIBRATION_FINISH:
	nop
#################################################
	nop
  b               MEM_CONF_DONE
	nop
#===== end DDR calibration ================

is_SDRAM:
##############################################
//	SDR & DDR I/O PAD current select for test only. 
	//li		t0,0x3407				//for SoC module QAboard
	//li		t0,0x3000
	li		t0,0x0
	li		t1,0xb8000314
	sw		t0,0(t1)
	nop	
////////////// //Set Tx/Rx delay for SDRAM  //////////////////////////
	li		t0,0x06000E00		//SDRAM TX hold time(1.95ns) and RX delay parameter setup for RTL8676,
	li		t1,0xb8000220
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
	li		t0,0x52080000		//8M, for SDRAM & DDR
	//li		t0,0x52480000		//16M, for SDRAM & DDR
	//li		t0,0x54480000		//32M, for SDRAM & DDR
	//li		t0,MCR0_VAL
	li		t1,0xB8001004
	sw		t0,0(t1)
	nop
#ifndef CONFIG_NAND_FLASH	//-------------------------------------------CONFIG_NAND_FLASH--------------------------------------//
#if	1	//SDRAM RX calibration......
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

	li		CP_BLK_TEMP1,FLASH_START											//boot start address.
	or		CP_BLK_START_ADDR,CP_BLK_START_ADDR,CP_BLK_TEMP1				//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	or		CP_BLK_END_ADDR,CP_BLK_END_ADDR,CP_BLK_TEMP1					//transfer DRAM(0x80000000) to FLASH(0xbfc00000) address
	
	li		CP_BLK_TEMP1, 0xA0600100															//start address of SRAM reserve area  
	
1:	
	lw		CP_BLK_TEMP2, 0(CP_BLK_START_ADDR)								//load code from flash
	nop
	sw		CP_BLK_TEMP2, 0(CP_BLK_TEMP1)									//restore code to SRAM
	addi		CP_BLK_START_ADDR, CP_BLK_START_ADDR, 4
	addi		CP_BLK_TEMP1, CP_BLK_TEMP1, 4
	bne		CP_BLK_START_ADDR, CP_BLK_END_ADDR, 1b
	nop
	li		CP_BLK_TEMP1, 0xA0600100										//
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
	li		RX_TEMP2,0x1D02					//version: 1.02
	sw		RX_TEMP2,0(RX_TEMP1)
//// insert SDR calibration version, record in 0xA0600018 //end///

	li		RX_REG_ADDR,0xb8000220			//Delayline Control Register
	lw		RX_REG_VAL,0(RX_REG_ADDR)		//reload orginal value
	nop
	li		RX_PARA,0
	move	RX_L0,zero
RX_PRAR_CH:
/////////	set RX value	/////////
	and		RX_REG_VAL,RX_REG_VAL,0xFFFFE0FF	//[n12..n8]
	nop
	sll		RX_TEMP1,RX_PARA,8
	nop
	or		RX_REG_VAL,RX_REG_VAL,RX_TEMP1
	nop
	sw		RX_REG_VAL,0(RX_REG_ADDR)
	nop
////////////save start GDMA address  ////////////////////////////////////////// 	
	li		RX_TEMP1,0xA0600020				//save  start GDMA address to 0xA0600020
 	li		RX_TEMP2,0xA0400000				//start GDMA address		//change note 20120106. from 0xA0800000 -> 0xA0400000
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop
	li		RX_TEMP1,0xA0600028				//save  current GDMA address to 0xA0600028
	li		RX_TEMP3,0x1FFFFFFF
 	and		RX_TEMP2,RX_TEMP2,RX_TEMP3		//start GDMA address
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop 	 	
	li		RX_TEMP1,0xA0600024				//save  end GDMA address to 0xA0600024 
 	li		RX_TEMP2,0xA0420000				//end GDMA address		//change note 20120106. from 0xA0820000 -> 0xA0420000
 	sw		RX_TEMP2,0(RX_TEMP1)
 	nop 	
////////////Clearn data from 0xa0612000 to 0xa0612FFF,  this will write area. /////
////////////Clearn data from 0xa0800000 to 0xa08FFFFF,  this will write area. /////
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
 	li		RX_TEMP4,0xb800030c				//RX_TEMP4 IP module status
 	lw		RX_TEMP5,0(RX_TEMP4)			//RX_TEMP5 0xb800030c value.
 	nop
 	
1:
 	lw		RX_TEMP2,0(RX_TEMP1)			// load 
 	nop
 	/////////////add IP module power on/off start/////////////////////////// 	
	//li		RX_TEMP4,0xb800030c
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

//////////software compare data /////////////////
	li		RX_TEMP3,0x8000					//compare size 32k
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
	li		RX_TEMP3,1

SAVE_RX_VALUE:
	li		RX_TEMP1,0xA060001C
	lw		RX_TEMP2,0(RX_TEMP1)				//load rx pass parameter
	nop
	sll		RX_TEMP3,RX_TEMP3,RX_PARA
	nop
	or		RX_TEMP2,RX_TEMP2,RX_TEMP3
	sw		RX_TEMP2,0(RX_TEMP1)				//save rx pass parameter
	nop
	/////////////////////////////////////////////
	addiu	RX_PARA,RX_PARA,1					//Rx parameter plus 
	nop
	bne		RX_PARA,0x1F,RX_PRAR_CH			//if Rx parameter more then 0x10, set R0 value
	nop
	b			RX_FINISH										//add 20110601
	nop

SDRAM_RX_COMPARE_FAIL:
	addiu	RX_PARA,RX_PARA,1				//Tx parameter plus 
	nop
	beq		RX_PARA,0x1F,RX_FINISH			//TX_PRAR max 0x10
	nop
	b		RX_PRAR_CH
	nop	
	
RX_FINISH:
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
	sub		RX_TEMP6,RX_TEMP3,RX_L0
	add		RX_TEMP3,RX_TEMP3,RX_L0
	srl		RX_TEMP3,RX_TEMP3,1						//
	nop
	li		RX_TEMP2,0x8
	bge		RX_TEMP2,RX_TEMP6,SDRAM_FIXED_VALUE			//if R0-L0 <= 8, jump to Fixed value
	nop
	sll		RX_TEMP3,RX_TEMP3,8
	////////////////////////////////////
	li		RX_TEMP1,0xb8000220
	li		RX_TEMP4,0xFFFFE0FF
	lw		RX_TEMP2,0(RX_TEMP1)
	nop
	and		RX_TEMP2,RX_TEMP2,RX_TEMP4
	or		RX_TEMP3,RX_TEMP3,RX_TEMP2
	sw		RX_TEMP3,0(RX_TEMP1)				//load rx pass parameter
	nop	
	b		SDRAM_CALI_FINISH
	nop
	
SDRAM_FIXED_VALUE:	
 	li		RX_TEMP2,0x06000A00
 	li		RX_TEMP1,0xb8000220
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
#else		//Don't do SDRAM RX calibration while using NAND Flash
	li		t0,0x06000A00
 	li		t1,0xb8000220
 	sw		t0,0(t1)
#endif		//------------------------------------------------CONFIG_NAND_FLASH--------------------------------------//	
				nop	
###############################################	
        b               MEM_CONF_DONE
        nop
        nop
        nop
#endif
///////////////////////////////////////////////////////////
MEM_CONF_DONE:
	nop

	
	
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
	mtc3	t0, $2		# $2: i-ram 1 base
	nop
	nop
	li		t0, 4096 - 1
	mtc3	t0, $1      # $1: i-ram top
	nop
	mtc3	t0, $3		# $3: i-ram 1 top
	nop
	nop
	
	# disable DRAM
	li	t0, 0x85000000	#address without SDRAM
	and	t0, 0x0ffffc00    # translate to physical address
	mtc3	t0, $4      # $4: d-ram base
	nop
	mtc3	t0, $6		# $6: d-ram1 base
	nop
	li	t0, 4096 - 1
	mtc3	t0, $5      # $5: d-ram top
	nop
	mtc3	t0, $7		# $7, dram 2 top
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
	//Detect_RAM_Size
	Detect_RAM_Size_plus2
#endif //SDRAM_AUTO_DETECT

	#li		t0, (FLASH_START+0xff00)			# parameter start address
	li		t0, _PARM_START						# parameter start address
keep_search:	
	lw		t1, 0(t0)			
	nop
	beq		t1, 0x4265726c, found_param	# is "Berl" ?
	nop
	add		t0, 0x40
	#beq		t0, (FLASH_START+0x10000), default_config	# not found parameter, use default
	beq		t0, _PARM_END, default_config					# not found parameter, use default
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

	#--- the supported flash type 
	li		t1, NAND_CTRL_BASE
	lw		t0, 0(t1)
	nop
	and		t0, t0, 0x10000000
	nop
	beq		t0, 0x10000000, COPY_NAND

COPY_SPI_NOR:
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
	nop
	b		FINISH_COPY

COPY_NAND:	
	#--- copy self(128k) to RAM ---#		
	#ifdef CONFIG_NAND_PAGE_2K
	NAND_Copy_2048 	0, 64, 0x0, 0x20000
	#else	
	NAND_Copy_512 	0, 256, 0x0, 0x20000
	#endif

FINISH_COPY:
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


///////////////////////////////////////////////////////////
#ifdef SUPPORT_8676_PLATFORM


#endif
/////////////////////////////////////////////////////////////////////////////////////////
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
	//UN_SRAM_Map 		# unmap SRAM, czyao for FPGA_0412
	/* jump to SDRAM */
	la	t0, C_Entry
	jr	t0
	nop

fail:
	li	t0,CPU_REBOOT_ADDR
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

