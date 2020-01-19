/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Abstract: GPIO driver source code.
*
* ---------------------------------------------------------------
*/
#if 0	//shlee
#include <linux/config.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mman.h>
#include <linux/ioctl.h>
#include <linux/fd.h>
#include <linux/init.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/byteorder.h>
#endif	//#if 0

#if defined(CONFIG_RTL8670)
#include "lx4180.h"
#else // 8671
#include "lx5280.h"
#endif
#include "gpio.h"

//static int GPIOdataReg=0;
static unsigned int GPIOdataReg[GPIO_DATA_NUM]={0};


static unsigned int get_GPIOMask(void)
{
	unsigned int portMask=0xFFFFFFFF;

	return portMask;
}

/*
Config one GPIO pin. Release 1 only support output function
number and PIN location map:
Pin	num
PB7	15
:	:
PB0	8
PA7	7
:	:
PA0	0
*/
void gpioConfig (int gpio_num, int gpio_func)
{
  unsigned int mask;
  
	//printk( "<<<<<<<<<enter gpioConfig(gpio_num:%d, gpio_func:%d)\n", gpio_num, gpio_func );
	
	if ((gpio_num>=GPIO_END)||(gpio_num<0)) return;
	 if (gpio_num <= GPIO_D_7) {
		mask = 1 << gpio_num;
		if (GPIO_FUNC_INPUT == gpio_func)
			REG32(GPIO_PABCD_DIR) &= ~mask;
		else
	        	REG32(GPIO_PABCD_DIR) |= mask;
	}
	else if (gpio_num <= GPIO_H_7) {
		mask = 1 << (gpio_num - GPIO_E_0);

		if (GPIO_FUNC_INPUT == gpio_func)
			REG32(GPIO_PEFGH_DIR) &= ~mask;
		else
	        REG32(GPIO_PEFGH_DIR) |= mask;
	}
	return;
}

/*set GPIO pins on*/
void gpioSet(int gpio_num)
{
  unsigned int portMask=0;
	unsigned int pins;
 
	//printk( "<<<<<<<<<enter gpioSet( gpio_num:%d )\n", gpio_num );  
	if ((gpio_num>=GPIO_END)||(gpio_num<0)) return;
	if (gpio_num <= GPIO_D_7) {
		pins = 1 << gpio_num;
		portMask = get_GPIOMask();
		pins &= portMask;  //mask out disable pins
		if (pins == 0) return;  //no pins to set 

		GPIOdataReg[GPIO_ABCD] |= pins;  //set pins
		//write out
		REG32(GPIO_PABCD_DAT) = GPIOdataReg[GPIO_ABCD];
	}
	else if (gpio_num <= GPIO_H_7) {
		pins = 1 << (gpio_num - GPIO_E_0);
		GPIOdataReg[GPIO_EFGH] |= pins;
		REG32(GPIO_PEFGH_DAT) = GPIOdataReg[GPIO_EFGH];
	}    
	return;
}

/*set GPIO pins off*/
void gpioClear(int gpio_num)
{
  unsigned int portMask=0;
	unsigned int pins;

//	printk( "<<<<<<<<<enter gpioClear( gpio_num:%d )\n", gpio_num );      
	if ((gpio_num>=GPIO_END)||(gpio_num<0)) return;
	if (gpio_num <= GPIO_D_7) {
		pins = 1 << gpio_num;
		portMask = get_GPIOMask();
		pins &= portMask;  //mask out disable pins
		if (pins==0) return;  //no pins to reset    

		GPIOdataReg[GPIO_ABCD] &= ~pins;  //reset pins
		//write out
		REG32(GPIO_PABCD_DAT) = GPIOdataReg[GPIO_ABCD];
	}
	else if (gpio_num <= GPIO_H_7) {
		pins = 1 << (gpio_num - GPIO_E_0);
		GPIOdataReg[GPIO_EFGH] &= ~pins;
		REG32(GPIO_PEFGH_DAT) = GPIOdataReg[GPIO_EFGH];
	}
	return;
}

