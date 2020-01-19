 #----------------------------------------------------------------------------
 # Copyright (c) 2000 Embedded Performance Inc.
 #
 # This software is the property of Embedded Performance, Inc (EPI).
 # Embedded Performance specifically grants the user a license to use and
 # modify this software as required for operation in a product developed by
 # the user, provided this notice is not removed or altered.  
 # The user is also granted a limited binary distribution license and may
 # distribute binary versions of this software, as modifed by the user, as
 # part of the users product.
 # In no case may this software be distributed in source form, nor may
 # binary versions be supplied as part of a compilation or assembler toolkit.
 #
 # All other rights are reserved by EPI.
 #
 # EPI MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS
 # SOFTWARE.  IN NO EVENT SHALL EPI BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL
 # DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING, PERFORMANCE, OR
 # USE OF THIS SOFTWARE.
 #
 #----------------------------------------------------------------------------
/*
*************************************************************************
*									*
*                                                                       *
*  crt0.s: This file contants application startup and initializaton 	*
*          code.							*
*									*
*************************************************************************
*/

#define _ASMLANGUAGE
#include "eregdef.h"
#include "board.h"

/* these constants are simply default values for size of memory and stack
** size. If this startup code is used in a real application these should
** be adjust to fit the target board/application environment. 
*/
#define DEFAULT_STACK_SIZE	0x3000


	.file 	1 "crt0.s"

	.extern C_Entry		# user defined.
	.extern exception_handlers

/* Heap pointers, stack is positioned after heap */
	.bss
	.globl  _sys_heap_start
	.globl  _sys_heap_cur
	.globl  _sys_heap_end
_sys_heap_start:
	.space  4
_sys_heap_cur:
	.space  4
_sys_heap_end:
	.space  4


	.text
/*
** _start: Program entry point
*/
	.globl 	sysInit
	.ent	sysInit
sysInit:

	la	gp,_gp			/* set global ptr from cmplr */
	
	/* disable all interrupts but bus error, fpa usable. */

	.set	noreorder
	li	t0, SR_CU1|SR_CU0
	nop
	mtc0	t0, C0_SR		/* put on processor	*/

	mtc0	zero, C0_CAUSE
	nop
	nop
	mfc0	t0, C0_SR
	nop
	.set	reorder


	#--- configure memory controller
	#---this is for ASIC
	#li		$4,MCR0_VAL
	#li		$5,0xB9000000
	#sw		$4,0($5)
	#li		$4,MCR1_VAL
	#li		$5,0xB9000004
	#sw		$4,0($5)

	#--- this is for ASIC
	#li		$4,MCR2_VAL
	#li		$5,0xB9000008
	#sw		$4,0($5)

	#--- this is for System Interface
	#li		$4,SICR_VAL
	#li		$5,0xB9C04000
	#sw		$4,0($5)

	/* set stack to grow down from code */
	/* leave room for one param */
	la	sp, MEM_LIMIT-8	

	/* Init BSS spaces */
	la	t0, _fbss
	la	t1, _ebss
1:	sb	zero, 0(t0)
	addi	t0, t0, 1
	bne	t0, t1, 1b
	nop

#tylo, for spi test
#--- initialize and start COP3
#	mfc0	$8,$12
#	nop
#	nop
#	or		$8,0x80000000
#	mtc0	$8,$12
#	nop
#	nop 
	
	#--- load iram base and top
#	la		$8,0x80710000
#	la		$9,0x0ffffc00
#	and		$8,$8,$9
#	mtc3	$8,$0								# IW bas
#	nop
#	nop
#	addiu	$8,$8,0x7fff
#	mtc3	$8,$1								# IW top
#	nop
#	nop

	jal	C_Entry		/* never returns - starts up kernel */
	li	ra, R_VEC		/* load prom reset address */
	j	ra				/* just in case */
	.end	sysInit


/******************************************************************************
*
* sysWbFlush - flush the write buffer
*
* This routine flushes the write buffers, making certain all
* subsequent memory writes have occurred.  It is used during critical periods
* only, e.g., after memory-mapped I/O register access.
*
* RETURNS: N/A

* sysWbFlush (void)

*/
	.globl	sysWbFlush
	.ent	sysWbFlush
sysWbFlush:
	li	t0, K1BASE		/* load uncached address	*/
	lw	t0, 0(t0)		/* read in order to flush 	*/
	j	ra			/* return to caller		*/
	.end	sysWbFlush

#if 0

	.globl	intUnlock
	.ent	intUnlock
intUnlock:
	.set	noreorder
	mtc0	a0,C0_SR
	jr		ra	
	.set	reorder
	.end intUnlock


	.globl	intLock
	.ent	intLock
intLock:
	.set	noreorder
	mfc0	v0,C0_SR
 	nop
 	nop
 	li		t1,-2
 	and		t1,t1,v0
 	mtc0	t1,C0_SR
 	nop
 	nop
 	nop
 	nop
 	jr		ra
	.set	reorder
	.end intLock


#/*-------------------------------------------------------------------
#*
#** void _saveContext(void)
#**
#**-------------------------------------------------------------------
#*/
	.text
	.globl	_saveContext
	.ent	_saveContext
_saveContext:
	.set	noreorder
	.set	noat
	
	addiu	k0,sp,-128						# allocate stack
												# two more words for gp,sp to support gdb
	sw		AT,4(k0)							# backup at
	.set	at
	sw		v0,8(k0)							# backup v0,v1
	sw		v1,12(k0)
	sw		a0,16(k0)							# backup a0~a3
	sw		a1,20(k0)
	sw		a2,24(k0)
	sw		a3,28(k0)
	sw		t0,32(k0)							# backup t0~t7
	sw		t1,36(k0)
	sw		t2,40(k0)
	sw		t3,44(k0)
	sw		t4,48(k0)
	sw		t5,52(k0)
	sw		t6,56(k0)
	sw		t7,60(k0)
	sw		s0,64(k0)							# backup s0~s7 for stub read
	sw		s1,68(k0)
	sw		s2,72(k0)
	sw		s3,76(k0)
	sw		s4,80(k0)
	sw		s5,84(k0)
	sw		s6,88(k0)
	sw		s7,92(k0)
	sw		t8,96(k0)							# backup t8,t9
	sw		t9,100(k0)
	#skip k0,k1
	#skip gp
	#skip sp
	sw		s8,104(k0)						# backup s8
	sw		k1,108(k0)						# backup ra which is moved to k1 in vector
	
	mfc0	t0,C0_SR							# load status
	nop
	nop
	mfc0	t1,C0_EPC							# load EPC
	nop
	nop
	sw		t0,112(k0)							# backup status
	sw		t1,116(k0)							# backup EPC
	
	# save gp & sp to support gdb
	sw		gp,120(k0)
	sw		sp,124(k0)
	# setup context base for gdb to read registers
	#sw		k0,pExceptionContext
	
	j		ra									# return
	move	sp,k0
	
	.set	reorder
	.end _saveContext
	
	

#/*-------------------------------------------------------------------
#**
#** void _restoreContext(void)
#**
#**-------------------------------------------------------------------
#*/
	.text
	.globl	_restoreContext
	.ent	_restoreContext
_restoreContext:
	.set	noreorder
	.set	noat
	
	move	k0,sp
	lw		AT,4(k0)							# restore at
	lw		v0,8(k0)							# restore v0,v1
	lw		v1,12(k0)
	lw		a0,16(k0)							# restore a0~a3
	lw		a1,20(k0)
	lw		a2,24(k0)
	lw		a3,28(k0)
	lw		t0,32(k0)							# restore t0~t7
	lw		t1,36(k0)
	lw		t2,40(k0)
	lw		t3,44(k0)
	lw		t4,48(k0)
	lw		t5,52(k0)
	lw		t6,56(k0)
	lw		t7,60(k0)
	lw		s0,64(k0)							# restore s0~s7 for stub write
	lw		s1,68(k0)
	lw		s2,72(k0)
	lw		s3,76(k0)
	lw		s4,80(k0)
	lw		s5,84(k0)
	lw		s6,88(k0)
	lw		s7,92(k0)
	lw		t8,96(k0)							# restore t8,t9
	lw		t9,100(k0)
	# skip k0,k1
	# skip gp
	# skip sp
	lw		s8,104(k0)						# restore s8
	
	lw		k1,112(k0)						# restore status
	# restore gp & sp to support gdb
	lw		gp,120(k0)
	lw		sp,124(k0)
	mtc0	k1,C0_SR								# write status
	nop
	nop
	
	lw		k1,108(k0)						# restore ra
	
	j		ra
	lw		k0,116(k0)						# fetch saved EPC
	
	.set	at
	.set	reorder
	.end _restoreContext

		
	.globl	except_vec1_generic
	.ent	except_vec1_generic
except_vec1_generic:
	.set noreorder
	.set noat
	move	k1,ra								# backup ra
	la		k0,_saveContext
	jal		k0
	nop
	#la		k0,genexcpt_handler
	jal		k0
	nop
	#la		k0,_restoreContext
	jal		k0
	nop
	move	ra,k1								# restore ra
	#addiu	k0,k0,4
	j		k0									# return from exception
	rfe
	
	.set at
	.set reorder

	.end	except_vec1_generic


#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_ReadStatus(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_ReadStatus
	.ent	lx4180_ReadStatus
lx4180_ReadStatus:
	mfc0	v0,C0_SR
	jr		ra
	.end lx4180_ReadStatus



#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_WriteStatus(uint32)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_WriteStatus
	.ent	lx4180_WriteStatus
lx4180_WriteStatus:
	.globl	writeStatus
#	.aent	writeStatus
writeStatus:
	mfc0	v0,C0_SR
	mtc0	a0,C0_SR
	jr		ra	
	.end lx4180_WriteStatus



#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_ReadCause(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_ReadCause
	.ent	lx4180_ReadCause
lx4180_ReadCause:
	mfc0	v0,C0_CAUSE
	jr		ra	
	.end lx4180_ReadCause



#/*-------------------------------------------------------------------
#**
#** void lx4180_checkSp(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_checkSp
	.ent	lx4180_checkSp
lx4180_checkSp:
	srl     t0,sp,24
	li      t1,0x80
1:
	bne     t0,t1,1b
	nop
	jr		ra	
	.end lx4180_checkSp



#/*-------------------------------------------------------------------
#**
#** void lx4180_WriteCause(uint32)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_WriteCause
	.ent	lx4180_WriteCause
lx4180_WriteCause:
	mtc0	a0,C0_CAUSE
	jr		ra	
	.end lx4180_WriteCause



#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_ReadEPC(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_ReadEPC
	.ent	lx4180_ReadEPC
lx4180_ReadEPC:
	mfc0	v0,C0_EPC
	jr		ra	
	.end lx4180_ReadEPC


#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_ReadAddr(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_ReadAddr
	.ent	lx4180_ReadAddr
lx4180_ReadAddr:
	mfc0	v0,C0_BADVADDR
	jr		ra	
	.end lx4180_ReadAddr



#/*-------------------------------------------------------------------
#**
#** void lx4180_WriteCCTL(uint32)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_WriteCCTL
	.ent	lx4180_WriteCCTL
lx4180_WriteCCTL:
	mtc0	a0,C0_XCONTEXT
	jr		ra	
	.end lx4180_WriteCCTL



#/*-------------------------------------------------------------------
#**
#** uint32 lx4180_Break(void)
#**
#**-------------------------------------------------------------------
#*/
	.globl	lx4180_Break
	.ent	lx4180_Break
lx4180_Break:
	break
	jr		ra	
	.end lx4180_Break

#/*-------------------------------------------------------------------
#**
#** int32 setIlev(int32 new_ilev)
#**
#**		Setup interrupt level such that only the interrupts of higher 
#**		or equal priority level are permitted. The previous value of 
#**		interrupt level is returned.
#**
#**-------------------------------------------------------------------
#*/
	.globl	setIlev
	.ent	setIlev
setIlev:
	b		TCT_Control_Interrupts
	nop
	.end setIlev

	
	
	
#/*-------------------------------------------------------------------
#**
#** int32 getIlev(void)
#**
#**		Get the current interrupt level.
#**
#**-------------------------------------------------------------------
#*/
	.globl	getIlev
	.ent	getIlev
getIlev:
	lw		v0,_currIlev							# pickup the previous value
	nop
	nop
	jr		ra	
	nop
	.end getIlev
#endif	



