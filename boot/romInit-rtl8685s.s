/*
 * romInit.s  System initialization/brings up assembly routine
 *
 * History:
 *    2006/10/27 SH, Lee  Add big model support
 *    2006/10/30 SH, Lee  Add determine memory configuration
 *    2007/03/06 SH  Added Dcache write invalidate for CPU LX4181
*/
#define _ASMLANGUAGE
#include "eregdef.h"
#include "board.h"
#include "macro.s"

	.extern C_Entry
	
#define _USE_DMEM
	
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
	li		t0, SR_CU1 | SR_BEV
	nop
	mtc0	t0, C0_SR		/* disable all interrupts fpa on,    */
	nop
	nop
	
	/* prom exception handlers           */
	mtc0	zero, C0_CAUSE		/* clear all interrupts              */
	nop
	nop
	
	.set	reorder
	/* bypass copying if in RAM */
	bal 	1f
	nop
1:

	la		t0, 0x10000000	
	and		t0, ra, t0
	beq		t0, zero, 2f				# decide if ram based on BIT28
	nop

	.set	noreorder

	/* CPU rlx5281: 1. Invalidate icache 2. Invalidate and flush dcache */
	rlx5281_cache_flush
	
	/* initialize and start COP3 */
	mfc0	t0,$12
	nop
	or		t0,0x80000000
	mtc0	t0,$12
	nop
	nop
	
	##################################################
	##      DCache Write Allocation !!
	##################################################
#ifdef CONFIG_DCACHE_WA_EN
	rlx5281_dcache_wa_en
#else
	rlx5281_dcache_wa_dis
#endif

#ifdef _USE_DMEM	
	##################################################
	##      Setting DMEM0 !!
	##################################################
DMEM_SETTING:
	/* Disable DMEM0 */
	mtc0	zero, $20
	li		t1, 0x00000800
	mtc0	t1, $20
	nop
	nop

	/* Load DMEM0 range */
	li		t0, 0x85000000		# address without SDRAM
	and		t0, 0x1ffffc00    	# translate to physical address
	mtc3	t0, $4      		# $4: d-ram base
	nop
	nop
	ori		t0, t0,  4096 - 1
	mtc3	t0, $5      		# $5: d-ram top
	nop
	nop
	
	/* Enable DMEM0 */
	mtc0	$0, $20				# CCTL
	li		t1, 0x00000400 		# bit 10 of COP0 reg 20
    mtc0 	t1, $20
	nop
	nop
	
	##################################################
	##      Setting DMEM1 !!
	##################################################
	/* Disable DMEM1 */
	mtc0	zero, $20, 1 # CCTL
	nop
	nop
	li		t1,0x00000800 
	mtc0	t1, $20, 1
	nop
	nop			

	/* Load DMEM1 range */
	li		t0, 0x85001000		# address without SDRAM
	and		t0, 0x1ffffc00    	# translate to physical address
	mtc3	t0, $6      		# $6: d-ram base
	nop
	nop
	ori		t0, t0,  4096 - 1
	mtc3	t0, $7      		# $7: d-ram top
	nop
	nop
		
	/* Enable DMEM1 */
	mtc0	$0, $20, 1			# CCTL
	li		t1, 0x00000400 		# bit 10 of COP0 reg 20
    mtc0 	t1, $20, 1
	nop
	nop
	
	##################################################
	##      Set Stack Pointer !!
	##################################################
	/* Set temporary stack pointer */
	li      t0, 0x85002000
	move    sp, t0
#endif
		
	#--- the supported flash type 
	li		t1, Boot_Select
	lw		t0, 0(t1)
	nop
	and		t0, t0, 0x2000000
	nop
	beqz	t0, SPIandNOR
	nop
	
NAND:
	#ifdef CONFIG_NAND_FLASH
	#--- copy next 4K --> 32K bytes to 0x9fc01000 ---#	
	#ifdef CONFIG_NAND_PAGE_2K
	NAND_Copy_2048 	2, 16, 0x9fc01000, 0x20000
	#else
	NAND_Copy_512 	8, 64, 0x9fc01000, 0x20000
	#endif
	#endif
SPIandNOR:	
	##################################################
	##	External memory init !!
	#################################################
#ifndef CONFIG_NAND_FLASH
#ifndef CONFIG_NO_FLASH
	/* Map 0xbfc00000 to 0xb4000000 */
	li      t0, 0xb8001000
	lw      t1, 0(t0)
    nop
	li      t2, 0xfffbffff
	and     t1, t1, t2
	sw      t1, 0(t0)
    nop

	/* default SPI clock */
	li      t0, 0xB8001200
	li      t1, 0xffc00000
	sw      t1, 0(t0)
    nop



#ifdef _USE_DMEM
	/* Map 0xa0000000 (32K) t0 SRAM segment 0 */
	SRAM_Map 0x00000000, 0x8, 0x00, 0x0	
	
	/* Clear SRAM, and just clear the first 24K */
	Mem_Set_Word 0xa0000000,0x6000,0x0  
#else
	/* Map 0xa0800000 (8K) t0 SRAM segment 2 */
	SRAM_Map 0x05000000, 0x6, 0x20, 0x6000

	/* Set temporary stack pointer */
	li      t0, 0x85002000
	move    sp, t0

	/* Map 0xa0000000 (16K) t0 SRAM segment 0 */
	SRAM_Map 0x00000000, 0x7, 0x00, 0x0000	
	
	/* Map 0xa0004000 (8K) t0 SRAM segment 1 */
	SRAM_Map 0x00004000, 0x6, 0x10, 0x4000	
	
	/* Clear SRAM, and just clear the first 24K */
	Mem_Set_Word 0xa0000000,0x6000,0x0 
	
#endif	
	
	/* Copy part of Boot to SRAM */
	li		t0, 0xbfc00000
	li		t1, 0xbfc06000
	li		t2, 0xa0000000
1:	lw		t3, 0(t0)
	sw		t3, 0(t2)
	nop
  	addi	t0, t0, 4
	addi	t2, t2, 4
	bne		t0, t1, 1b
	nop	
#endif
#endif	

#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
	/* Get flash parameters */
	la		t0, boot_get_param
	jal		t0
	nop
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */

	/* Setup correct Voltage, such as SWR and DDRLDO output */
	la		t0, voltage_setup
	jal		t0
	nop

	/* Setup PLL */
	la		t0, pll_setup
	jal		t0
	nop

	/* Initialize UART */
	li		a0, 115200
	la		t0, initUart
	jal 	t0
	nop

	/* jump to memctrl_init */
	la		t0, memctlc_init_dram_8685
	jal		t0
	nop
#ifndef CONFIG_NAND_FLASH
#ifndef CONFIG_NO_FLASH	
 	/* Unmap SRAM */
	UN_SRAM_Map 0x00
#if 1
	UN_SRAM_Map 0x10
	UN_SRAM_Map 0x20
#endif
#endif
#endif
	.set	reorder
go_ahead:	
	#ifdef CONFIG_NAND_FLASH
	#--- the supported flash type 
	li		t1, NAND_CTRL_BASE
	lw		t0, 0(t1)
	nop
	and		t0, t0, 0x8000000
	nop
	beq		t0, 0x8000000, COPY_NAND
	#endif
	/* copy Boot+Loader to DRAM */
	la		t0, FLASH_START
	la		t1, FLASH_START + CONFIG_BOOT_SIZE
	la		t2, MEM_START
1:	lw		t3, 0(t0)
	sw		t3, 0(t2)
	nop

	addi	t0, t0, 4
	addi	t2, t2, 4
	bne		t0, t1, 1b
	nop
	
COPY_NAND:
	#ifdef CONFIG_NAND_FLASH
	#--- copy self(128k) to DRAM ---#		
	#ifdef CONFIG_NAND_PAGE_2K
	NAND_Copy_2048 	8, 64, 0x0, 0x20000
	#else	
	NAND_Copy_512 	32, 64, 0x0, 0x20000
	#endif	
	#endif
	/* CPU rlx5281: 1. Invalidate icache 2. Invalidate and flush dcache */
	rlx5281_cache_flush
	
	la		t0, MEM_START
	jr		t0
	nop
	

2:
	/* absolutely basic initialization to allow things to continue */
	/* setup bootloader stack */
	la	gp, _gp			/* set global ptr from cmplr         */

	la	sp, MEM_LIMIT-8	/* set temp stack ptr                */
	
	/* CPU rlx5281: 1. Invalidate icache 2. Invalidate and flush dcache */
	rlx5281_cache_flush

	/* Init BSS spaces */
	la	t0, _fbss
	la	t1, _ebss
	beq	t0, t1, 2f
1:	sw	zero, 0(t0)
	nop
  	addi	t0, t0, 4
	bne	t0, t1, 1b
	nop
2:
	/* jump to DRAM */
	#ifdef CONFIG_NAND_FLASH	
	la	t0, 0x80000000
	#else
	la	t0, C_Entry
	#endif	
	jr	t0
	nop

fail:
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

