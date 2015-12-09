/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 by Tom Ting <tom_ting@realtek.com>
 *
 * Time initialization.
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>


#ifdef CONFIG_RTK_SLAVE_CPU_BOOT
void bootup_slave_cpu(void)
{
	printf("Bring UP slave CPUs\n");
	/* Turn on SLAVE CPU */
	__raw_writel(0x00021000, CPU_RELEASE_ADDR);	//Set default release ADDR to uboot start text

	__raw_writel(0x00010000, 0x9801D104);
	udelay(1);
	__raw_writel(0x03FFFF00, 0x9801D530);
	udelay(1);
	__raw_writel(0x011F3BFF, 0x9801D100);
	udelay(1);
	__raw_writel(0x0F1F3BFF, 0x9801D100);
	udelay(10);

	/* Release Slave CPUs From ROM code and enter WFI in uboot entry */
	asm volatile("sev" : : : "memory");
	udelay(100);

	// clear the CPU release addr so it will stay in wfe loop until kernel fill it with correct start addr
	__raw_writel(0x00000000, CPU_RELEASE_ADDR);

}
#endif //CONFIG_RTK_SLAVE_CPU_BOOT
