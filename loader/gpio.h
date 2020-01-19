/*
 * Copyright c                Realtek Semiconductor Corporation, 2002
 * All rights reserved.                                                    
 * 
 * Abstract:
 *
 *   GPIO driver header file for export include.
 *
 */

#ifndef _GPIO_H_
#define _GPIO_H_



/*port function definition*/
#define GPIO_FUNC_INPUT 	0x0001  /*data input*/
#define GPIO_FUNC_OUTPUT 	0x0002	/*data output*/
#define GPIO_FUNC_IRQ_FALL 	0x0003	/*falling edge IRQ*/
#define GPIO_FUNC_IRQ_RISE 	0x0004	/*rising edge IRQ*/
#define GPIO_FUNC_IRQ_LEVEL 	0x0005	/*level trigger IRQ*/

extern void gpioConfig (int gpio_num, int gpio_func);
extern void gpioSet(int gpio_num);
extern void gpioClear(int gpio_num);
enum GPIO_DATA {
	GPIO_ABCD = 0,
	GPIO_EFGH,
	GPIO_DATA_NUM
};
#define JTAG_UNUSABLE
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#define GPIOCR_BASE 0xB8003300
#else
#define GPIOCR_BASE 0xB8003500
#endif
/*Port A,B,C,D*/
#define GPIO_PABCD_CNR		(GPIOCR_BASE+0x00)	/*Port A,B,C,D control register*/
#define GPIO_PABCD_PTYPE	(GPIOCR_BASE+0x04)	/*Port A,B,C,D peripheral type control register*/
#define GPIO_PABCD_DIR		(GPIOCR_BASE+0x08)	/*Port A,B,C,D direction */
#define GPIO_PABCD_DAT		(GPIOCR_BASE+0x0C)	/*Port A,B,C,D data register*/
#define GPIO_PABCD_ISR		(GPIOCR_BASE+0x10)	/*Port A,B,C,D interrupt status register*/
#define GPIO_PAB_IMR		(GPIOCR_BASE+0x14)	/*Port A,B interrupt mask register*/
#define GPIO_PCD_IMR		(GPIOCR_BASE+0x18)	/*Port C,D interrupt mask register*/
#define GPIO_PEFGH_DIR		(GPIOCR_BASE+0x24)	/*Port E,F,G,H direction */
#define GPIO_PEFGH_DAT		(GPIOCR_BASE+0x28)	/*Port E,F,G,H data register*/
#define GPIO_PEFGH_ISR		(GPIOCR_BASE+0x2c)	/*Port E,F,G,H interrupt status register*/
#define GPIO_PEF_IMR		(GPIOCR_BASE+0x30)	/*Port E,F interrupt mask register*/
#define GPIO_PGH_IMR		(GPIOCR_BASE+0x34)	/*Port G,H interrupt mask register*/



enum GPIO_DEF {
	GPIO_A_0 = 0, GPIO_A_1, GPIO_A_2, GPIO_A_3, GPIO_A_4, GPIO_A_5, GPIO_A_6, GPIO_A_7, 
	GPIO_B_0 = 8, GPIO_B_1, GPIO_B_2, GPIO_B_3, GPIO_B_4, GPIO_B_5, GPIO_B_6, GPIO_B_7, 
	GPIO_C_0 = 16, GPIO_C_1, GPIO_C_2, GPIO_C_3, GPIO_C_4, GPIO_C_5, GPIO_C_6, GPIO_C_7, 
	GPIO_D_0 = 24, GPIO_D_1, GPIO_D_2, GPIO_D_3, GPIO_D_4, GPIO_D_5, GPIO_D_6, GPIO_D_7,
	GPIO_E_0 = 32, GPIO_E_1, GPIO_E_2, GPIO_E_3, GPIO_E_4, GPIO_E_5, GPIO_E_6, GPIO_E_7,
	GPIO_F_0 = 40, GPIO_F_1, GPIO_F_2, GPIO_F_3, GPIO_F_4, GPIO_F_5, GPIO_F_6, GPIO_F_7,
	GPIO_G_0 = 48, GPIO_G_1, GPIO_G_2, GPIO_G_3, GPIO_G_4, GPIO_G_5, GPIO_G_6, GPIO_G_7,
	GPIO_H_0 = 56, GPIO_H_1, GPIO_H_2, GPIO_H_3, GPIO_H_4, GPIO_H_5, GPIO_H_6, GPIO_H_7,
	GPIO_END

};

/* pci-e led definition */
#define LED_DSL			GPIO_H_3
#define LED_PPP_G		GPIO_H_1
#define LED_PPP_R		GPIO_G_1
#define LED_PWR_R		GPIO_G_0
#define LED_USB			GPIO_F_6


#ifdef CONFIG_TW_GAN99U
#define PWR_LED_1		GPIO_G_0
#define PWR_LED_2		GPIO_G_1
#define PWR_LED_3		GPIO_F_6
#endif


#endif  /* _GPIO_H_ */
