#ifndef _REALTEK_CPU_H_
#define _REALTEK_CPU_H_

#ifndef __ASSEMBLY__
#include <asm/io.h>

#define EFUSE_ADDR		(0x980171d8)
#define CHIP_INFO1 		(0x98007028)
#define RTK1295_CPU_ID  	0x00000000
#define RTK1294_CPU_ID  	0x00000001
#define RTK1296_CPU_ID  	0x00000002

#define RTK129x_CPU_MASK 	0x00000003
#define RTK1296_CPU_MASK 	0x00000800
#define SOC_REV_A 		(0x0)
#define SOC_REV_B 		(0x1)
#define SOC_REV_C 		(0x2)

static inline int get_cpu_id() {
	int cpu_id = RTK1295_CPU_ID;
	if ((__raw_readl((volatile u32*)(EFUSE_ADDR)) & RTK129x_CPU_MASK) == RTK1294_CPU_ID) {
		cpu_id = RTK1294_CPU_ID;
	} else if ((__raw_readl((volatile u32*)(CHIP_INFO1)) & RTK1296_CPU_MASK)) {
		cpu_id = RTK1296_CPU_ID;
	}
	return cpu_id;
}
//#define get_cpu_revision() 	 __raw_readl((volatile u32*)(SB2_IO_ADDR(CHIP_REV)))



#endif
#endif /* _REALTEK_CPU_H_ */
