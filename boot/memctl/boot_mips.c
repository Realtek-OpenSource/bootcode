#include "./bspchip_8685.h"
#include "./memctl_8685.h"
#include "./memctl_8685_func.h"

#ifndef BIT
#define BIT(x) (1 << x)
#endif

#if 0
//Boot DSP register. ASIC Register
#define R_AIPC_ASIC_ENABLE_DSP_CLK         	0xB8000044
#define R_AIPC_ASIC_KICK_DSP                    	0xB800408c
#define BIT_ENABLE_DSP_TAP                      	BIT(6)
#define BIT_ENABLE_DSP_CLOCK                    	BIT(5)
#define BIT_KICK_DSP                            		BIT(0)

//Reset CPU register. 
#define R_AIPC_ASIC_RESET_CPU                   	0xBB00018C
#define BIT_RESET_CPU                           		BIT(2)
#else
#define R_AIPC_ASIC_ENABLE_DSP_CLK		0xB8000308
#define BIT_ENABLE_DSP_CLOCK                    	BIT(5)
#endif



//timer register
#ifdef CONFIG_RTL8685_PERI
#define TC_BASE         0xB8003200
#define TC1DATA         (TC_BASE+0x10)
#define TC1CNT          (TC_BASE+0x14)
#define TC1CNR          (TC_BASE+0x18)
#define TC1EN           (1 << 28)
#define TC1MODE_TIMER   (1 << 24)
#define TC1_DIV_OFFSET  0
#define CLOCKGEN_FREQ	200000000
#endif

void init_timer()
{
#ifdef CONFIG_RTL8685_PERI
        /* RTL8685 timer */
        /*disable clock */
        REG32(TC1CNR) = 0x00000000;

        /* timer 1 interval as 0.5 ms  */
        REG32(TC1DATA) = (1000*4*60*60) << 0; /* 1 hour */
        REG32(TC1CNR) = (CLOCKGEN_FREQ/1000/4) << TC1_DIV_OFFSET;
        REG32(TC1CNR) |= (TC1EN|TC1MODE_TIMER);
#else /*CONFIG_RTL8685_PERI*/
#endif /*CONFIG_RTL8685_PERI*/
}

unsigned long get_sys_time(void)
{
	//tylo, 8672 fpga^M
#ifdef CONFIG_RTL8685_PERI
	return ((*((volatile unsigned long *)TC1CNT))/4);
#else /*CONFIG_RTL8685_PERI*/
#endif /*CONFIG_RTL8685_PERI*/
}

int timeout(unsigned long start, unsigned long ms)
{
	unsigned long cur_ms = get_sys_time();
	if(cur_ms>=start)
		if((cur_ms-start) > ms)
			return 1;
	if(cur_ms<start)        /* roll over */
		if((cur_ms+(1000*60*60)-start) > ms)
			return 1;
	return 0;
}

void delay_msec(unsigned long ms)
{
	unsigned long start = get_sys_time();

	while(!timeout(start,ms));
}


void boot_cpu1(void) {
	unsigned int regVal;
	int i;

#if 0
	puts("\n\rBoot CPU1...\n\r");

	regVal = REG32(R_AIPC_ASIC_ENABLE_DSP_CLK);
	//regVal &= ~BIT_ENABLE_DSP_TAP;
	regVal |= BIT_ENABLE_DSP_CLOCK;
	REG32(R_AIPC_ASIC_ENABLE_DSP_CLK) = regVal;
	
	puts("Enalbe CPU1 CLK 0xb8000044 = 0x"); 
	puthex(REG32(R_AIPC_ASIC_ENABLE_DSP_CLK));
	puts("\n\r");

	puts("Start delay 4 ms\n\r");
	init_timer();
	delay_msec(4);
	puts("After delay 4 ms\n\r");

	REG32(R_AIPC_ASIC_KICK_DSP) |= BIT_KICK_DSP;
	puts("Kick start CPU1 0xb800408c = 0x");  
	puthex(REG32(R_AIPC_ASIC_KICK_DSP));
	puts("\n\r");
#else
#if 0
	puts("\n\rSet parallel tdi\n\r");
	regVal = REG32(0xb8000214);
	regVal |= (1 << 14);
	REG32(0xb8000108) = regVal;
	
	puts("Disable one jtag\n\r");
	regVal = REG32(0xb8000108);
	regVal &= ~(1 << 2);
	REG32(0xb8000108) = regVal;
#endif
#if 1
	puts("Setup slow bits\n\r");
	regVal = REG32(0xb8000214);
	regVal |= (1 << 11);
	regVal |= (1 << 9);
	REG32(0xb8000214) = regVal;
#endif
	puts("Registers: \n\r");
	puts("0xb8000108: ");
	puthex(REG32(0xb8000108));
	puts("\n\r");
	puts("0xb8000214: ");
	puthex(REG32(0xb8000214));
	puts("\n\r");
	puts("0xb8004088: ");
	puthex(REG32(0xb8004088));
	puts("\n\r");
	puts("0xb80040fc: ");
	puthex(REG32(0xb80040fc));
	puts("\n\r");

	/* reset booting instruction register */
	/* we use this register to indicate that 5281 has finished loading loader from flash to dram */
	REG32(0xb8004100) = 0;

	puts("\n\rBoot CPU1...\n\r");
	regVal = REG32(R_AIPC_ASIC_ENABLE_DSP_CLK);	
	regVal |= BIT_ENABLE_DSP_CLOCK;	
	
	REG32(R_AIPC_ASIC_ENABLE_DSP_CLK) = regVal;		

	puts("Enalbe CPU1 CLK 0xb8000308 = 0x"); 
	puthex(REG32(R_AIPC_ASIC_ENABLE_DSP_CLK));
	puts("\n\r");

	puts("Start delay 4 ms\n\r");	
	init_timer();	
	delay_msec(4);	
	puts("After delay 4 ms\n\r");	
#endif
	//show_cpu1_msg();

	return;		
}

