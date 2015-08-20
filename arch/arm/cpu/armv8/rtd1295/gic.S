/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 by Tom Ting <tom_ting@realtek.com>
 *
 * interrupt test
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#include <asm/gic.h>
#include <asm/macro.h>

ENTRY(gic_test_init)
#if defined(CONFIG_GICV2)
	stp	x4, x5, [sp,#-32]!
	stp	x9, x10, [sp,#16]

	/**** Config SGIs and PPIs as Grp1 ****/
	ldr	x4, =GICD_BASE
	mov	w9, #~0
	str	w9, [x4, GICD_IGROUPRn]

	/**** Enable All SPI interrupts ****/
	ldr	w9, [x4, GICD_TYPER]
	and	w10, w9, #0x1f          /* ITLinesNumber */
	cbz	w10, 1f                 /* No SPIs */
	mov	w9, #~0
	add	x5, x4, (GICD_ISENABLERn + 4) /* start from #32-bit, SPI */
0:	
	str	w9, [x5], #0x4
	sub	w10, w10, #0x1
	cbnz	w10, 0b
1:
	/**** Enable All SGI & PPI interrupts ****/
	str	w9, [x4, GICD_ISENABLERn]

	/**** Initialize Cpu Interface ****/
	ldr	x4, =GICC_BASE
	mov	w9, #0x1e7
	str	w9, [x4, GICC_CTLR]
	mov	w9, #0x1 << 7
	str	w9, [x4, GICC_PMR]

	ldp	x9, x10, [sp,#16]
	ldp	x4, x5, [sp],#32
#endif /* CONFIG_GICV2 */
	ret
ENDPROC(gic_test_init)

ENTRY(cpu_sgi_test)
#if defined(CONFIG_GICV2)
	stp	x4, x5, [sp,#-16]!
	ldr	x4, =GICD_BASE
	mov	w5, #0x8000
	movk	w5, #0x200, lsl #16
	str	w5, [x4, GICD_SGIR]
	ldp	x4, x5, [sp],#16
#endif
	ret
ENDPROC(cpu_sgi_test)
