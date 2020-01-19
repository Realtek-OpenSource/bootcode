/*************************************************************
*
* 	application.c - stub application initialization routine
*/

/*
DESCRIPTION
Initialize user application code.
*/ 

#include "board.h"
#include "uart.h"
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#include "../boot/memctl/bspchip_8685.h"
#endif


/* reserve for system heap */
#define _SYSTEM_HEAP_SIZE	1024*256
char dl_heap[_SYSTEM_HEAP_SIZE];

/* Merge chip auto-detection for the correct spi base address and system clk */
#ifdef CONFIG_SPI_FLASH
extern unsigned int SFCR, SFCSR, SFDR;
#endif
unsigned int CLOCKGEN_FREQ;

#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
extern unsigned int board_LX_freq_mhz(const unsigned int cmu_ctrl_reg, 
			unsigned char use_cmu,unsigned int index);
#endif

/* Refer to Linux kernel code setup.c */
static void SYS_CLOCK_SET(void){
	unsigned int reg=0;

#ifdef CONFIG_RTL8676
	reg = (REG32(0xb8000200) & 0x01F00)>>8;
	CLOCKGEN_FREQ = (reg+2)*10000000;			// = 25*(set value+2)*1/2, for RTL8676S
#elif defined(CONFIG_RTL8676S)
	reg = (REG32(0xb8000200) & 0x01F00)>>8;
	CLOCKGEN_FREQ = (reg+2)*12500000;			// = 25*(set value+2)*1/2, for RTL8676S
#elif defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	//8685 spi-nand FPGA test~
#ifdef CONFIG_FPGA_8685S
	CLOCKGEN_FREQ = (50)*1000000;
#else
	CLOCKGEN_FREQ = (board_LX_freq_mhz(REG32(SYSREG_CMUCTLR_REG), 0,3))*1000000;
#endif

#endif

#ifdef CONFIG_SPI_FLASH
	SFCR = 0xB8001200;
    	SFCSR= 0xB8001208;
    	SFDR = 0xB800120c;

#ifdef CONFIG_RTL8685S
#ifndef CONFIG_ADV_SPIC
	if(IS_RL6405A || IS_RL6405B){
		reg = REG32(0xb8000100);

		/* Use Old SPIC, set bit[9] = 0, and set bit[7] to inverse */
		reg = reg & (~(1<<9));
		reg = reg^(1<<7);

		REG32(0xb8000100) = reg;
	}else if(IS_RLE0705){ 
		unsigned int bd_value=0x0;

		reg = REG32(0xb8000100);

		/* Must refill the bonding value to avoid werid xDSL behavior
		   Step1 : Read the bonding value from bit[25:24] of 0xb8000100
	  	   Step2 : Write the bonding value to bit[13:12] of 0xb8000100			
		 */
		bd_value = ((reg & (0x3<<24)) >> 24);
		reg = ((reg & ~(0x3<<12)) | (bd_value<<12));

		/* Use Old SPIC, set bit[10] = 0, and set bit[6] to inverse */
		reg = reg & (~(1<<10));
		reg = reg^(1<<6);
		
		REG32(0xb8000100) = reg;
	}
#endif /* CONFIG_ADV_SPIC */
#endif /* CONFIG_RTL8685S */
		
#endif /*CONFIG_SPI_FLASH */

}

void C_Entry( void )
{
	/* setup spi-flash base address and system clk */
	SYS_CLOCK_SET();

	/* initialize heap */
	dlmem_init((void*)dl_heap, sizeof(dl_heap));

	/* initialize UART for debug message*/	
#ifndef CONFIG_SPI_NAND_FLASH
	initUart( BOARD_PARAM_BAUD );
#endif

	init_timer();

	check_loader_mode(0);

}
