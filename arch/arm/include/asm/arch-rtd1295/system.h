/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 by Chuck Chen <ycchen@realtek.com>
 *
 *      =========================================
 *        Date           Who                    Comment
 *      =========================================
 *      20150812        Tom Ting              Change for phoenix 1295
 *
 */

#ifndef _ASM_MACH_SYSTEM_H_
#define _ASM_MACH_SYSTEM_H_

//RTD1295 support 3 MISC timers,TC0/TC1 for SCPU NWC,TC2 for SCPU SWC.
#define SYSTEM_TIMER            CONFIG_SYS_TIMER

#if defined (CONFIG_BOARD_FPGA)
#define TIMER_CLOCK             33000000        //FPGA
#else
#define TIMER_CLOCK             27000000
#endif
#define MAX_HWTIMER_NUM         3

#define RBUS_ADDR		0x98000000	// Used for mmu_setup
#define RBUS_SIZE		0x00050000
#define RBUS_END		(RBUS_ADDR + RBUS_SIZE)

#endif  // _ASM_MACH_SYSTEM_H_
