/*
 * romInit.s  System initialization/brings up assembly routine
 *
 * History:
 *    2013/07/30 David, Chen  Add for handle nand flash bootup in 8685
*/
#define _ASMLANGUAGE
#include "eregdef.h"
#include "board.h"
#include "macro.s"

	.extern C_Entry
	
#define RVECENT(f,n) \
	b f; nop
	
	/* For hard or soft reset, jump forward to handler. 
	   otherwise, jump to hard reset entry point, unless 
	   exception handler is called. */

	.globl romInit_B
	
	.set noreorder
romInit_B:
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
	la	t0, C_Entry
	jr	t0
	nop
	
/*******************************************************************************
*
* romExcHandle - rom based exception/interrupt handler, do nothing but hang
*/

	.ent	romExcHandle
romExcHandle:
hangExc:
	b	hangExc			/* HANG UNTIL REBOOT                 */
	.end	romExcHandle


	