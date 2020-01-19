
#ifndef _UART_H
#define _UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* include files */


//tylo, for 8672 fpga
#ifdef CONFIG_UART1
#define  UART_BASE_ADDRESS 0xb8002100
#else
#define  UART_BASE_ADDRESS 0xb8002000
#endif
//#define  UART_BASE_ADDRESS    0xb9c00000

#define UART_RBR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x00)
#define UART_THR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x00)    /* Tx Holding */
#define UART_DLL     *((volatile unsigned char *)UART_BASE_ADDRESS+0x00)
#define UART_IER     *((volatile unsigned char *)UART_BASE_ADDRESS+0x04)    /* Interrupt Enable */
#define UART_DLM     *((volatile unsigned char *)UART_BASE_ADDRESS+0x04)
#define UART_IIR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x08)
#define UART_FCR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x08)    /* FIFO Control */
#define UART_LCR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x0c)    /* Line Control */
#define UART_MCR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x10)    /* Modem Control */
#define UART_LSR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x14)    /* Line Status */
#define UART_MSR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x18)    /* Modem Status */
#define UART_SCR     *((volatile unsigned char *)UART_BASE_ADDRESS+0x1c)    /* Scratchpad */

#ifdef CONFIG_BOOT_MIPS
#define UART1_BASE_ADDRESS 0xb8002100
#define UART1_RBR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x00)
#define UART1_THR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x00)    /* Tx Holding */
#define UART1_DLL     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x00)
#define UART1_IER     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x04)    /* Interrupt Enable */
#define UART1_DLM     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x04)
#define UART1_IIR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x08)
#define UART1_FCR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x08)    /* FIFO Control */
#define UART1_LCR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x0c)    /* Line Control */
#define UART1_MCR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x10)    /* Modem Control */
#define UART1_LSR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x14)    /* Line Status */
#define UART1_MSR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x18)    /* Modem Status */
#define UART1_SCR     *((volatile unsigned char *)UART1_BASE_ADDRESS+0x1c)    /* Scratchpad */
#endif

#if 0   //never use
#define  UART_BAUD_2400       (CLOCKGEN_FREQ/16/2400)
#define  UART_BAUD_4800       (CLOCKGEN_FREQ/16/4800)
#define  UART_BAUD_9600       (CLOCKGEN_FREQ/16/9600)
#define  UART_BAUD_19200      (CLOCKGEN_FREQ/16/19200)
#define  UART_BAUD_38400      (CLOCKGEN_FREQ/16/38400)
#define  UART_BAUD_57600      (CLOCKGEN_FREQ/16/57600)
#define  UART_BAUD_115200     (CLOCKGEN_FREQ/16/115200)
#endif


#ifdef __cplusplus
}
#endif   

void initUart( unsigned int );
void puts( char * );
void putc( char );
void puthex(unsigned long);
char getc( void );
char pollcc( void );
int getcc( void );

#endif /* for the #define uart.h */					    
