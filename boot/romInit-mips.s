/*
 * romInit.s  System initialization/brings up assembly routine
 *
 * History:
 *    2015/12/03 tctsai  First version to bring up MIPS 1004Kc
*/
#define _ASMLANGUAGE
#include "eregdef.h"
#include "board.h"
#include "macro.s"

#if 1 /* for 1004k */

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define set_zero_64(dst_reg) \
    sw    zero, (0*4 - 64)(dst_reg); \
    sw    zero, (1*4 - 64)(dst_reg); \
    sw    zero, (2*4 - 64)(dst_reg); \
    sw    zero, (3*4 - 64)(dst_reg); \
    sw    zero, (4*4 - 64)(dst_reg); \
    sw    zero, (5*4 - 64)(dst_reg); \
    sw    zero, (6*4 - 64)(dst_reg); \
    sw    zero, (7*4 - 64)(dst_reg); \
    sw    zero, (8*4 - 64)(dst_reg); \
    sw    zero, (9*4 - 64)(dst_reg); \
    sw    zero, (10*4 - 64)(dst_reg); \
    sw    zero, (11*4 - 64)(dst_reg); \
    sw    zero, (12*4 - 64)(dst_reg); \
    sw    zero, (13*4 - 64)(dst_reg); \
    sw    zero, (14*4 - 64)(dst_reg); \
    sw    zero, (15*4 - 64)(dst_reg);
    
#define GCR_ADDR	(0xBFBF8000)
#define CKUSEG 		(0x00000000)
#define CKSEG0 		(0x80000000)
#define CKSEG1 		(0xA0000000)
#define CKSEG2 		(0xC0000000)
#define CKSEG3 		(0xE0000000)

#define CP0_INDEX	$0
#define CP0_INX		$0
#define CP0_RANDOM	$1
#define CP0_RAND	$1
#define CP0_ENTRYLO0	$2
#define CP0_TLBLO0	$2
#define CP0_ENTRYLO1	$3
#define CP0_TLBLO1	$3
#define CP0_CONTEXT	$4
#define CP0_CTXT	$4
#define CP0_PAGEMASK	$5
#define CP0_PAGEGRAIN	$5,1
#define CP0_WIRED	$6
#define CP0_HWRENA	$7
#define CP0_BADVADDR 	$8
#define CP0_VADDR 	$8
#define CP0_COUNT 	$9
#define CP0_ENTRYHI	$10
#define CP0_TLBHI	$10
#define CP0_COMPARE	$11
#define CP0_STATUS	$12
#define CP0_SR		$12
#define CP0_INTCTL	$12,1
#define CP0_SRSCTL	$12,2
#define CP0_SRSMAP	$12,3
#define CP0_CAUSE	$13
#define CP0_CR		$13
#define CP0_EPC 	$14
#define CP0_PRID	$15
#define CP0_EBASE	$15,1
#define CP0_CONFIG	$16
#define CP0_CONFIG0	$16,0
#define CP0_CONFIG1	$16,1
#define CP0_CONFIG2	$16,2
#define CP0_CONFIG3	$16,3
#define CP0_LLADDR	$17
#define CP0_WATCHLO	$18
#define CP0_WATCHHI	$19
#define CP0_DEBUG	$23
#define CP0_DEPC	$24
#define CP0_PERFCNT	$25
#define CP0_ERRCTL	$26
#define CP0_CACHEERR	$27
#define CP0_TAGLO	$28
#define CP0_ITAGLO	$28
#define CP0_DTAGLO	$28,2
#define CP0_TAGLO2	$28,4
#define CP0_DATALO	$28,1
#define CP0_IDATALO	$28,1
#define CP0_DDATALO	$28,3
#define CP0_DATALO2	$28,5
#define CP0_TAGHI	$29
#define CP0_DATAHI	$29,1
#define CP0_DATAHI2	$29,5
#define CP0_ERRPC	$30
#define CP0_DESAVE	$31

#define CP0_STATUS_IE   (1<<0)
#define CP0_STATUS_EXL  (1<<1)
#define CP0_STATUS_ERL  (1<<2)

#define CP0_CONF_CACHABLE_NC_WT_nWA   0
#define CP0_CONF_UNCACHED             2
#define CP0_CONF_CACHABLE_NC_WB_WA    3
#define CP0_CONF_UNCACHED_ACCELERATED 7
#define CP0_CONF_CACHE_MASK           0x7

#define Index_Invalidate_I    0x00
#define Index_Writeback_Inv_D 0x01
#define Index_Load_Tag_I      0x04
#define Index_Load_Tag_D      0x05
#define Index_Store_Tag_I     0x08
#define Index_Store_Tag_D     0x09
#define Index_Store_Tag_SD    0x0B
#define Hit_Invalidate_I      0x10
#define Hit_Invalidate_D      0x11
#define Fill_I                0x14
#define Hit_Writeback_Inv_D   0x15

#define GCR_CONFIG_ADDR 	  0xbfbf8000

#define zero $0
#define AT   $1
#define v0   $2  /* return value */
#define v1   $3
#define a0   $4  /* argument registers */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8  /* caller saved */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16 /* callee saved */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24 /* caller saved */
#define t9   $25
#define k0   $26
#define k1   $27
#define gp   $28
#define sp   $29
#define s8   $30
#define ra   $31

#define CACHELINE_SIZE		32

#define ICACHE_SIZE	65536
#define DCACHE_SIZE	32768
#define MIPS_MAX_CACHE_SIZE	65536

#define CPU_BASIC_INIT    \
cpu_basic_init:    \
    /* Clear watch registers */\
    mtc0    zero, CP0_WATCHLO;\
    mtc0    zero, CP0_WATCHHI;\
    /* STATUS register */\
    mfc0    k0, CP0_STATUS;\
    li      k1, ~(CP0_STATUS_IE|CP0_STATUS_EXL|CP0_STATUS_ERL);\
    and     k0, k1;\
    mtc0    k0, CP0_STATUS;\
    /* (Watch Pending), SW0/1 should be cleared */\
    mtc0    zero, CP0_CAUSE;\
    /* Timer */\
    mtc0    zero, CP0_COUNT;\
    mtc0    zero, CP0_COMPARE;
    
#define CACHE_INIT  /*for reorder*/ \
cache_init:    /* Enable cache. However, one should NOT access cached SRAM and cached DRAM until they are initialized. */\
    mfc0    t0, CP0_CONFIG;    \
    li      t1, ~CP0_CONF_CACHE_MASK;    \
    and     t0, t0, t1;    \
    li      t1, CP0_CONF_CACHABLE_NC_WB_WA;    \
    or      t0, t0, t1;    \
    mtc0    t0, CP0_CONFIG;


#define ICACHE_INIT    \
icache_init:    /*for reorder*/\
    mtc0    zero, CP0_TAGLO;        \
    /* clear tag to invalidate */    \
    li      t0, CKSEG0;    \
    li      t1, ICACHE_SIZE;    \
    addu    t1, t0, t1;    \
1:  cacheop(Index_Store_Tag_I, t0);    \
    addiu   t0, CACHELINE_SIZE;    \
    bne     t0, t1, 1b;    \
    nop;	\
    /* fill once, so data field parity is correct */    \
    li      t0, CKSEG0;    \
2:  cacheop(Fill_I, t0);    \
    addiu   t0, CACHELINE_SIZE;    \
    bne     t0, t1, 2b;    \
    nop;	\
    /* invalidate again - prudent but not strictly neccessary */    \
    li      t0, CKSEG0;    \
1:  cacheop(Index_Store_Tag_I, t0);    \
    addu    t0, CACHELINE_SIZE;    \
    bne     t0, t1, 1b;    \
    nop;	

#define DCACHE_INIT    \
    dcache_init:    /*for reorder*/\
    mtc0    zero, CP0_TAGLO, 2;    \
    /* clear all tags */    \
    li      t0, CKSEG0;    \
    li      t1, DCACHE_SIZE;    \
    addu    t1, t0, t1;    \
    1:  cacheop(Index_Store_Tag_D, t0);    \
    addiu    t0, CACHELINE_SIZE;    \
    bne     t0, t1, 1b;    \
    nop;	\
    /* load from each line (in cached space) */    \
    li      t0, CKSEG0;    \
    2:  lw      zero, 0(t0);    \
    addiu    t0, CACHELINE_SIZE;    \
    bne     t0, t1, 2b;    \
    nop;	\
    /* clear all tags */    \
    li      t0, CKSEG0;    \
    1:  cacheop(Index_Store_Tag_D, t0);    \
    addiu   t0, CACHELINE_SIZE;    \
    bne     t0, t1, 1b;    \
    nop; 

#define CACHE_RESET    /*for reorder*/    \
    cache_reset:	\ 
    li      v0, MIPS_MAX_CACHE_SIZE;    \
    /* clear that much memory starting from zero. */    \
    li      a0, CKSEG1;        \
    addu    a1, a0, v0;        \
    1:  addiu   a0, 64;             \
    set_zero_64(a0);        \
    bne     a0, a1, 1b;        
    nop;


#endif //1004k


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

	.globl MIPS_romInit
	
	.set noreorder
MIPS_romInit:
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
	/* bypass copying if in RAM */
	bal 	1f
	nop
1:

	la	t0, 0x10000000	
	and	t0, ra, t0
	beq	t0, zero, 2f				# decide if ram based on BIT28
	nop

	.set	noreorder

#if 1 /* for MIPS 1004k */
	CPU_BASIC_INIT

	/*inti L2 cache*/
	// L2 Cache initialization routine
    	// Check L2 cache size
    	mfc0    t7, CP0_CONFIG2          // C0_Config2

    	// Isolate L2$ Line Size
    	ext     t6, t7, 4, 4        // extract SL

    	// Skip ahead if No L2$
    	beq     t6, zero, done_l2cache
    	nop
    	li      a2, 2
    	sllv    t6, a2, t6              // Now have true L2$ line size in bytes

    	// Isolate L2$ Sets per Way
    	ext     a0, t7, 8, 4        // extract SS
    	li      a2, 64
    	sllv    a0, a2, a0              // L2$ Sets per way

    	// Isolate L2$ Associativity
    	// L2$ Assoc (-1)
    	ext     a1, t7, 0, 4        // extract SA
    	add     a1, 1
    	mul     a0, a0, a1          // Get total number of sets
    	lui     a2, 0x8000          // Get a KSeg0 address for cacheops

    	// Clear L23TagLo/L23DATALo/L23DATAHi registers
    	mtc0    zero, CP0_TAGLO2
    	mtc0    zero, CP0_DATALO2
    	mtc0    zero, CP0_DATAHI2
    	move    a3, a0

    	// L2$ Index Store Tag Cache Op
    	// Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
next_L2cache_tag:
    	/*  Write Tag using index store tag */
    	cache   Index_Store_Tag_SD, 0(a2)    
    	add     a3, -1                          // Decrement set counter
    	bne     a3, zero, next_L2cache_tag  	// Done yet?
	nop
    	add     a2, t6                          // Get next line address

done_l2cache:
   	sync    0x3                //To ensure cache_op finish!

    	li      t0, GCR_CONFIG_ADDR //GCR BASE ADDR
    	lw      t1, 0x0008(t0)    	// GCR_BASE
	nop
    	ins     t1, zero, 0, 8    	// CCA Override disabled
    	sw      t1, 0x0008(t0)    	// GCR_BASE
	nop
	
	sync    0x3              	//Ensure CCA effective!
	
	/*bypass L2 cache*/	
	mfc0	t0, $16, 2
	li	t1, 1
	ins	t0, t1, 12, 1
	mtc0	t0, $16, 2
	nop

	CACHE_RESET
	ICACHE_INIT
	DCACHE_INIT
	CACHE_INIT   /* enable cache. k0's CCA = write-back for L1cache */
#endif

	##################################################
	##      Set Stack Pointer !!
	##################################################
	/* Set temporary stack pointer */
	li      t0, 0x9fc01ffc
	//li      t0, 0x8000fffc
	move    sp, t0

MIPS_ENTRY:	
	##################################################
	##	External memory init !!
	#################################################
#ifndef CONFIG_NAND_FLASH
#ifndef CONFIG_NO_FLASH
   	 /* Map 0xa0000000 (24K) to SRAM segment 1~3 for DSP Dram Calibration*/
    	DSP_SRAM_Map 0x00008000, 0x6, 0x10, 0x2000
    	DSP_SRAM_Map 0x0000a000, 0x6, 0x20, 0x4000
    	DSP_SRAM_Map 0x08000000, 0x6, 0x30, 0x8000
	
#endif
#endif
	nop	

	/* jump to DSP IPC simulator */
	la	t0, ipc_dsp_server
	jal	t0
	nop

	
	/*enable L2 cache*/
	sync 	0x3
	mfc0	t0, $16, 2
	li	t1, 0
	ins	t0, t1, 12, 1
	mtc0	t0, $16, 2
	nop
	sync	0x3
	
#ifndef CONFIG_NAND_FLASH
#ifndef CONFIG_NO_FLASH	
 	/* Unmap SRAM */
	DSP_UN_SRAM_Map 0x10
	DSP_UN_SRAM_Map 0x20
	DSP_UN_SRAM_Map 0x30
#endif
#endif

#ifndef CONFIG_MIPS_RUNS_LOADER
/* busy loop here, if we're not going to run loader*/
1:
	b	1b
	nop
#endif

	.set	reorder
go_ahead:	
	#ifdef CONFIG_NAND_FLASH
#--- the supported flash type 
	li	t1, NAND_CTRL_BASE
	lw	t0, 0(t1)
	nop
	and	t0, t0, 0x8000000
	nop
	beq	t0, 0x8000000, COPY_NAND
	nop
	#endif

	/* switch JTAG */
#ifdef CONFIG_JTAG_MIPS
	li	t0, 0xb8000108
	lw	t1, 0(t0)
	nop
	li	t2, 0xfffffffb
	and	t1, t1, t2
	sw 	t1, 0(t0)
	nop
#endif
	/* we can only access flash after 5281 finfishes loading loader from flash to dram*/
	/* we use one of the BIRs (0xb8004100) here to indicate the status of 5281 */ 
1:	
	li 	t0, 0xb8004100
	lw 	t1, 0(t0)
	nop
	beqz	t1, 1b
	nop

	/* enable 1004K SPIF access */
	li 	t0, 0xb8000214
	lw	t1, 0(t0)
	nop
	li	t2, 0x00008000
	or 	t1, t1, t2
	sw 	t1, 0(t0)
	nop
	
	/* copy secboot(with LZMA compressed loader)  to DRAM */
	la	t0, 0xb4022000
	la	t1, 0xb4022000 + CONFIG_BOOT_SIZE
	la	t2, MEM_START
1:	lw	t3, 0(t0)
	nop
	sw	t3, 0(t2)
	nop
	addi	t0, t0, 4
	addi	t2, t2, 4
	bne	t0, t1, 1b
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
	
2:
	/* absolutely basic initialization to allow things to continue */
	/* setup bootloader stack */
	la	gp, _gp			/* set global ptr from cmplr         */

	la	sp, MEM_LIMIT-8	/* set temp stack ptr                */
	
	/* jump to DRAM */
	#ifdef CONFIG_NAND_FLASH	
	la	t0, MEM_START
	#else
	la 	t0, MEM_START
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

