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
	int count  = 0;
	printf("Bring UP slave CPUs\n");
	/* Turn on SLAVE CPU */
	__raw_writel(0x00021000, CPU_RELEASE_ADDR);	//Set default release ADDR to uboot start text
	asm volatile ("ISB SY" : : : "memory");
	sync();	

	__raw_writel(0x00010000, 0x9801D104);
	asm volatile ("ISB SY" : : : "memory");
	sync();
	
	__raw_writel(0x03FFFF00, 0x9801D530);
	asm volatile ("ISB SY" : : : "memory");
	sync();	
	
	__raw_writel(0x011F3BFF, 0x9801D100);
	asm volatile ("ISB SY" : : : "memory");
	sync();	

	__raw_writel(0x0F1F3BFF, 0x9801D100);
	asm volatile ("ISB SY" : : : "memory");
	sync();

	//wait slave cores enter wfe
	while(__raw_readl(0x98000454) != 0x1 || __raw_readl(0x98000458) != 0x2  || __raw_readl(0x9800045C) != 0x3){
		count++;
		//printf("dummy1 = 0x%08x\n", __raw_readl(0x98000454));
		//printf("dummy2 = 0x%08x\n", __raw_readl(0x98000458));
		//printf("dummy3 = 0x%08x\n", __raw_readl(0x9800045c));
		if (count > 10000) break;
		udelay(100);
	}
	//clear flag
	__raw_writel(0x00000000, 0x98000454);
	__raw_writel(0x00000000, 0x98000458);	
	__raw_writel(0x00000000, 0x9800045C);
	asm volatile ("ISB SY" : : : "memory");
	sync();

	/* Release Slave CPUs From ROM code and enter WFI in uboot entry */
	asm volatile("sev" : : : "memory");
	
   	//wait slave cores get release address 0x21000
	count = 0;
	while(__raw_readl(0x98000454) != 0x1 || __raw_readl(0x98000458) != 0x2  || __raw_readl(0x9800045C) != 0x3){
		count++;
		//printf("dummy1 = 0x%08x\n", __raw_readl(0x98000454));
		//printf("dummy2 = 0x%08x\n", __raw_readl(0x98000458));
		//printf("dummy3 = 0x%08x\n", __raw_readl(0x9800045c));
		if (count > 10000) break;
		udelay(100);
	}
	//clear flag
	__raw_writel(0x00000000, 0x98000454);
	__raw_writel(0x00000000, 0x98000458);	
	__raw_writel(0x00000000, 0x9800045C);
	asm volatile ("ISB SY" : : : "memory");
	sync();
	 // clear the CPU release addr so it will stay in wfe loop until kernel fill it with correct start addr
	__raw_writel(0x00000000, CPU_RELEASE_ADDR);
	asm volatile ("ISB SY" : : : "memory");
	sync();
	
}
#endif //CONFIG_RTK_SLAVE_CPU_BOOT:
