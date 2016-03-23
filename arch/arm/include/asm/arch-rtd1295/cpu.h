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
#ifdef CONFIG_RTK_TEE_SUPPORT	
#define BL31_ENTRY_ADDR		0x10120000
#endif

#ifndef __ASSEMBLY__
void bootup_slave_cpu(void);
#endif

#endif
