
#ifndef _ASICREGS_H
#define _ASICREGS_H



#undef SWTABLE_NO_HW


#define	DIVISOR			20
#define TCD_OFFSET		8
#define TICK_FREQ	HZ	//HZ is at include/asm-mipsnommu/param.h LN41
#ifndef REG32
  #define REG32(reg)	(*(volatile unsigned int *)(reg))
#endif
#ifndef REG16
  #define REG16(reg)	(*(volatile unsigned short *)(reg))
#endif
#ifndef REG8
#define REG8(reg)	(*(volatile unsigned char *)(reg))
#endif

#define MAX_ILEV                        10
#define TICK_ILEV                        9  //->IRQ7
#define UART_ILEV 	    	             8  //->IRQ6
#define SAR_ILEV                         7  //->IRQ5
#define Ethernet_ILEV                    6  //->IRQ4
#define DMT_ILEV                         5  //->IRQ3
#define USB_ILEV                         4  //->IRQ3
#define PCI_ILEV                         3  //->IRQ3
#define GPIO_ILEV                        2  //->IRQ2
#define Time_Out_ILEV                    1  //->IRQ2

/* Global interrupt control registers 
*/
#define GICR_BASE                           0xB9C03000
#define GIMR                                (0x010 + GICR_BASE)       /* Global interrupt mask */
#define GISR                                (0x012 + GICR_BASE)       /* Global interrupt status */
#define IRR1                                (0x014 + GICR_BASE)       /* Interrupt routing 1 */
#define IRR2                                (0x018 + GICR_BASE)       /* Interrupt routing 2 */
#define ILR                                 (0x01C + GICR_BASE)       /* Interrupt level register */
/* Global interrupt mask register field definitions 
*/
#define UART_IM                             (1 << 15)       /* UART interrupt enable */
#define Timer_IM                            (1 << 14)       /* Timer interrupt enable */
#define Time_Out_IM                         (1 << 13)       /* Time out interrupt enable */
#define SAR_IM                              (1 << 12)       /* SAR interrupt enable */
#define Ethernet_IM                         (1 << 11)       /* Ethernet interrupt enable */
#define DMT_IM                              (1 << 10)       /* DMT interrupt enable */
#define UART2_IM                            (1 <<  9)       /* UART2 interrupt enable */
#define PCI_IM                              (1 <<  8)       /* PCI interrupt enable */
#define GPIO_IM                             (1 <<  7)       /* GPIO port interrupt enable */
#define NIC2_IM                             (1 <<  6)       /* NIC2 interrupt enable */
#define PCM_IM                              (1 <<  5)       /* PCM interrupt enable */
#define SPI_IM                              (1 <<  4)       /* SPI interrupt enable */
#define POWER_IM                            (1 <<  3)       /* POWER interrupt enable */
/* Global interrupt status register field definitions 
*/
#define UART_IS                             (1 << 15)       /* UART interrupt pending */
#define Timer_IS                            (1 << 14)       /* Timer/Counter interrupt pending */
#define Time_Out_IS                         (1 << 13)       /* time out interrupt pending */
#define SAR_IS                              (1 << 12)       /* SAR interrupt pending */
#define Ethernet_IS                         (1 << 11)       /* Ethernet interrupt pending */
#define DMT_IS                              (1 << 10)       /* DMT interrupt pending */
#define UART2_IS                            (1 <<  9)       /* UART2 client interrupt pending */
#define PCI_IS                              (1 <<  8)       /* PCI interrupt pending */
#define GPIO_IS                             (1 <<  7)       /* GPIO interrupt pending */
#define NIC2_IS                             (1 <<  6)       /* NIC2 interrupt pending */
#define PCM_IS                              (1 <<  5)       /* PCM interrupt pending */
#define SPI_IS                              (1 <<  4)       /* SPI interrupt pending */
#define POWER_IS                            (1 <<  3)       /* POWER interrupt pending */
/* Interrupt routing register 1 field definitions 
*/
#define UART_IPS                            28              /* UART interrupt routing select offset */
#define Timer_IPS                           24              /* Timer interrupt routing select offset */
#define Time_Out_IPS                        20              /* Time out interrupt routing select offset */
#define SAR_IPS                             16              /* SAR interrupt routing select offset */
#define Ethernet_IPS                        12              /* Ethernet interrupt routing select offset */
#define DMT_IPS                              8              /* DMT interrupt routing select offset */
#define UART2_IPS                            4              /* UART2 interrupt routing select offset */
#define PCI_IPS                              0              /* PCI interrupt routing select offset */
/* Interrupt routing register 2 field definitions 
*/
#define GPIO_IPS                            28              /* GPIO interrupt routing select offset */
#define NIC2_IPS                            24              /* NIC2 interrupt routing select offset */
#define PCM_IPS                             20              /* PCM interrupt routing select offset */
#define SPI_IPS                             16              /* SPI interrupt routing select offset */
#define POWER_IP                            12              /* POWER interrupt routing select offset */


/* Timer control registers 
*/
#define GPIOCR_BASE 0xB9C01000
#define TC0DATA                             (0x020 + GPIOCR_BASE)       /* Timer/Counter 0 data */
#define TC1DATA                             (0x024 + GPIOCR_BASE)       /* Timer/Counter 1 data */
#define TC0CNT                              (0x028 + GPIOCR_BASE)       /* Timer/Counter 0 count */
#define TC1CNT                              (0x02C + GPIOCR_BASE)       /* Timer/Counter 1 count */
#define TCCNR                               (0x030 + GPIOCR_BASE)       /* Timer/Counter control */
#define TCIR                                (0x034 + GPIOCR_BASE)       /* Timer/Counter intertupt */
#define CDBR                                (0x038 + GPIOCR_BASE)       /* Clock division base */
#define WDTCNR                              (0x03C + GPIOCR_BASE)       /* Watchdog timer control */
/* Timer/Counter data register field definitions 
*/
#define TCD_OFFSET                          8
/* Timer/Counter control register field defintions 
*/
#define TC0EN                               (1 << 31)       /* Timer/Counter 0 enable */
#define TC0MODE_COUNTER                     0               /* Timer/Counter 0 counter mode */
#define TC0MODE_TIMER                       (1 << 30)       /* Timer/Counter 0 timer mode */
#define TC1EN                               (1 << 29)       /* Timer/Counter 1 enable */
#define TC1MODE_COUNTER                     0               /* Timer/Counter 1 counter mode */
#define TC1MODE_TIMER                       (1 << 28)       /* Timer/Counter 1 timer mode */
/* Timer/Counter interrupt register field definitions 
*/
#define TCIR_TC0IE                          (1 << 31)       /* Timer/Counter 0 interrupt enable */
#define TCIR_TC1IE                          (1 << 30)       /* Timer/Counter 1 interrupt enable */
#define TCIR_TC0IP                          (1 << 29)       /* Timer/Counter 0 interrupt pending */
#define TCIR_TC1IP                          (1 << 28)       /* Timer/Counter 1 interrupt pending */
/* Clock division base register field definitions 
*/
#define DIVF_OFFSET                         16
/* Watchdog control register field definitions 
*/
#define WDTE_OFFSET                         24              /* Watchdog enable */
#define WDSTOP_PATTERN                      0xA5            /* Watchdog stop pattern */
#define WDTCLR                              (1 << 23)       /* Watchdog timer clear */
#define OVSEL_13                            0               /* Overflow select count 2^13 */
#define OVSEL_14                            (1 << 21)       /* Overflow select count 2^14 */
#define OVSEL_15                            (2 << 21)       /* Overflow select count 2^15 */
#define OVSEL_16                            (3 << 21)       /* Overflow select count 2^16 */

/*Bus time out register
*/
#define BSTMOUT_OFFSET                       29              
#define BSTMOUT_20us                         0               /* bus time out interval 20 us*/
#define BSTMOUT_30us                         (1 << BSTMOUT_OFFSET)       /* bus time out interval 30 us*/
#define BSTMOUT_40us                         (2 << BSTMOUT_OFFSET)       /* bus time out interval 40 us*/
#define BSTMOUT_50us                         (3 << BSTMOUT_OFFSET)       /* bus time out interval 50 us*/


/* GPIO control registers 
*/
/*Port A*/
#define GPIO_PADIR	(GPIOCR_BASE+0x00L)	/*Port A direction register*/
#define GPIO_PADAT	(GPIOCR_BASE+0x04L)	/*Port A data register*/
#define GPIO_PAISR	(GPIOCR_BASE+0x08L)	/*Port A interrupt status register*/
#define GPIO_PAIMR	(GPIOCR_BASE+0x0CL)	/*Port A interrupt mask register*/
/*Port B*/
#define GPIO_PBDIR	(GPIOCR_BASE+0x10L)	/*Port B direction register*/
#define GPIO_PBDAT	(GPIOCR_BASE+0x14L)	/*Port B data register*/
#define GPIO_PBISR	(GPIOCR_BASE+0x18L)	/*Port B interrupt status register*/
#define GPIO_PBIMR	(GPIOCR_BASE+0x1CL)	/*Port B interrupt mask register*/


#define RT8670_SICR_BASE	0xB9C04000	//tylo,0xBD800000	/*System interface Configuration Register*/
#define RT8670_SICR_SPI 	0x00000040	/*SPI interface,  1:use SPI, 0:use SPI as GPIOA0~2,B7*/
#define RT8670_SICR_JTAG	0x00000010	/*JTAG interface, 1:use JTAG, 0:use JTAG as GPIOA3~7*/
#define RT8670_SICR_UART	0x00000008	/*UART interface, 1:use UART, 0:use UART as GPIOB3~6*/

#endif   /* _ASICREGS_H */
