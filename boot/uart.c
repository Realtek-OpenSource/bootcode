/*#include "loader.h"*/
#include "board.h"
#include "uart.h"
#include "./memctl/bspchip_8685.h"

static int CLOCKGEN_FREQ = 25000000;
extern unsigned int board_LX_freq_mhz(const unsigned int, unsigned char, unsigned int index);

void initUart(unsigned int baud_rate)
{
#ifndef UART_NOT_CONNECTED
    unsigned short divisor;
	int i;
    /* DLAB Set, 8 bit word, no parity, 1 stop bit */
    UART_LCR = 0x83;

    /* Set Baud Rate to 9600 bps */
	CLOCKGEN_FREQ = (board_LX_freq_mhz(REG32(SYSREG_CMUCTLR_REG), 0, 3))*1000000;
    divisor = (CLOCKGEN_FREQ / ( 16 * baud_rate )) - 1;
    UART_DLM = (divisor >> 8) & 0x00FF;
    UART_DLL = divisor & 0x00FF;

    /* Clear DLAB Bit */
#if 0	
    *((char *)UART_LCR) = 0x03;
    
    *((char *)UART_MCR) = 0x00;
#else
	UART_LCR = 0x03;
	UART_FCR = 0x06;
	for(i=0;i<1000;i++);
	UART_FCR = 0x01;
	UART_IER = 0x00;
	UART_MCR = 0x03;
#endif
#endif
}

void putc( char c )
{
#ifndef UART_NOT_CONNECTED
	if(c=='\n')
		putc('\r');
    /* While !THRE, loop */
    while ( 0 == (UART_LSR & 0x20) );

    UART_THR = c;


#endif
}

void puts( char *string )
{
#ifndef UART_NOT_CONNECTED
    int index = 0;

    while ( 0 != string[ index ] )
    {
		putc( string[ index++ ] );
    }
    return;
#endif
}

void puthex(unsigned long hex)
{
int	i;
char	c;

	initUart(115200);

	for (i=0;i<8;i++)
	{
		c = hex >> 28;
		c += '0';
		if (c > '9')
			c += ('A' - '9' - 1);
		putc(c);
		hex <<= 4;
	}
	putc('\n');
}


char getc( void )
{
#ifndef UART_NOT_CONNECTED
    if ( 1 == (UART_LSR & 0x01) )
    {
		return UART_RBR;
    }
    else
#endif
    {
		return 0;
    }
}

char pollcc( void )
{
    return (UART_LSR & 0x01 );
}

char getcc( void )
{
	while ( 1 != (UART_LSR & 0x01) );
	return UART_RBR;
}

