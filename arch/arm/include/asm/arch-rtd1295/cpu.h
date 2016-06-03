/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 by Tom Ting <tom_ting@realtek.com>
 *
 * Time initialization.
 */
#ifndef __RTK_CPU_H__
#define __RTK_CPU_H__


#define CPU_RELEASE_ADDR	0x9801aa44
#define SB2_CHIP_INFO       0x9801a204
#define BL31_ENTRY_ADDR		0x10120000
#define TEE_MEM_START_ADDR	0x10100000ULL
#define TEE_MEM_SIZE		0x00F00000ULL
#define RTD129x_CHIP_REVISION_A00 0x00000000
#define RTD129x_CHIP_REVISION_A01 0x00010000
#define RTD129x_CHIP_REVISION_B00 0x00020000

#ifndef __ASSEMBLY__

#include <asm/io.h>

void bootup_slave_cpu(void);
static inline u32 get_rtd129x_cpu_revision(void) {
	u32 val = __raw_readl(SB2_CHIP_INFO);
	return val; 
}

#endif


#endif
