/*#include "loader.h"*/
#include "board.h"
#include "uart.h"
#include "../boot/memctl/bspchip_8685.h"

void put_cc( char c )
{
	if(c=='\n')
		putchar('\r');
    /* While !THRE, loop */
    while ( 0 == (UART_LSR & 0x20) );

    UART_THR = c;

}

void putchar( char c )
{
	if(c=='\n')
		putchar('\r');
    /* While !THRE, loop */
    while ( 0 == (UART_LSR & 0x20) );

    UART_THR = c;

}

void puts( char *string )
{
    int index = 0;

    while ( 0 != string[ index ] )
    {
		putchar( string[ index++ ] );
    }
    return;
}

void puthex(unsigned long hex)
{
int	i;
char	c;

	for (i=0;i<8;i++)
	{
		c = hex >> 28;
		c += '0';
		if (c > '9')
			c += ('A' - '9' - 1);
		putchar(c);
		hex <<= 4;
	}
}


int getch( char *ch )
{

    if ( 1 == (UART_LSR & 0x01) )
    {
    	return UART_RBR;
    }
    else
    	return 0;
}

char poll_cc( void )
{
    return (UART_LSR & 0x01 );
}

int getchar( void)
{
	while ( 1 != (UART_LSR & 0x01) );
	
	return( UART_RBR );
}

int getcc( void)
{
	if ( 1 == (UART_LSR & 0x01) )
		return( UART_RBR );
	return 0;
}

//auto-detect chip version for system clock, loader/main.c
extern unsigned int CLOCKGEN_FREQ;
#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
void initUart(unsigned int baud_rate)
{
    unsigned short divisor;
	int i;
	
	#ifdef CONFIG_UART1
	i=REG32(0xB8000300) & (~0xc000);	//disable UART0
	i|=0x300;											//enable UART1
	REG32(0xB8000300)=i;
#else
	i=REG32(0xB8000300) & (~0x300);	//disable UART1
	i|=0xC000;											//enable UART0
	REG32(0xB8000300)=i;
	#endif
    /* DLAB Set, 8 bit word, no parity, 1 stop bit */
    UART_LCR = 0x83;

    /* Set Baud Rate to 9600 bps */
    divisor = (CLOCKGEN_FREQ / ( 16 * baud_rate )) - 1;
	
    UART_DLM = (divisor >> 8) & 0x00FF;
    UART_DLL = divisor & 0x00FF;

    /* Clear DLAB Bit */
    UART_LCR = 0x03;

	UART_FCR = 0x06;
	for(i=0;i<1000;i++);
	UART_FCR = 0x01;
    
    UART_IER = 0x00;
    UART_MCR = 0x03;
    
}

#elif defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
/* Provide the usage in boot !! */
#ifdef CONFIG_RTL8685
#define UART0_SEL				0x1000
#define UART1_SEL				0x800
#elif defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#define UART0_SEL				0x1800
#define UART1_SEL				0x200
#endif

#ifdef CONFIG_MIPS
static unsigned int get_LX_freq_mhz(void) {
	return CONFIG_LXBUS_MHZ;
}
#endif

void initUart(unsigned int baud_rate)
{
	unsigned short divisor;
	unsigned int clock_freq;
	int i;

#ifdef CONFIG_MIPS
	clock_freq = get_LX_freq_mhz()*1000000;
#else
	clock_freq = board_LX_freq_mhz(REG32(SYSREG_CMUCTLR_REG), 0, 3)*1000000;
#endif

	/* UART0/UART1 Selection 

		Use UART0 : 1. Disable UART1
					 2. Enable UART0
		Use UART1 : 1. Disable UART0
					 2. Enable UART1
	*/
#if defined(CONFIG_BOOT_MIPS) || defined(CONFIG_MIPS)
#ifdef CONFIG_UART1
	i=REG32(SYSREG_PINMUX0_REG);	
	i|=UART1_SEL;											
	REG32(SYSREG_PINMUX0_REG)=i;
#else
	i=REG32(SYSREG_PINMUX0_REG);	
	i|=UART0_SEL;											
	REG32(SYSREG_PINMUX0_REG)=i;
#endif /* CONFIG_UART1 */
#else
#ifdef CONFIG_UART1
	i=REG32(SYSREG_PINMUX0_REG) & (~UART0_SEL);	
	i|=UART1_SEL;											
	REG32(SYSREG_PINMUX0_REG)=i;
#else
	i=REG32(SYSREG_PINMUX0_REG) & (~UART1_SEL);	
	i|=UART0_SEL;											
	REG32(SYSREG_PINMUX0_REG)=i;
#endif /* CONFIG_UART1 */
#endif /* CONFIG_BOOT_MIPS or CONFIG_MIPS*/

	/* DLAB Set, 8 bit word, no parity, 1 stop bit */
	UART_LCR = 0x83;

	/* Set Baud Rate to 9600 bps */
	divisor = (clock_freq / ( 16 * baud_rate )) ;
	
	UART_DLM = (divisor >> 8) & 0x00FF;
	UART_DLL = divisor & 0x00FF;

	/* Clear DLAB Bit */
	UART_LCR = 0x03;

	UART_FCR = 0x06;
	for(i=0;i<1000;i++);
	UART_FCR = 0x01;

	UART_IER = 0x00;
	UART_MCR = 0x03;
    
}
#endif
