/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/boot0412/loader/tick.c,v 1.4 2011/12/05 07:42:43 yachang Exp $
*
* Abstract: Tick timer driver.
*
* $Author: yachang $
*
* $Log: tick.c,v $
* Revision 1.4  2011/12/05 07:42:43  yachang
* add patch from Kevin.Chung 2011/12/02
*
* Revision 1.3  2011/09/22 14:33:40  cathy
* use volatile to access register
*
* Revision 1.2  2011/06/10 09:46:07  yachang
* patch 0412 PIO bug
*
* Revision 1.1.1.1  2011/06/10 08:06:31  yachang
*
*
* Revision 1.2  2010/09/27 08:54:58  yachang
* merge sh.lee's 00.00.13g
*
* Revision 1.1.1.1  2008/03/08 09:10:37  eric
*
*  8672 Bootloader
*  combo and e8b merged
*
* Revision 1.1.1.1  2006/06/07 05:27:50  kaohj
* initial import into CVS
*
* no message
*
* ---------------------------------------------------------------
*/

#include "board.h"


//tylo, for 8672 fpga
#ifdef RTL8672
#ifdef CONFIG_RTL8685_PERI
/*RTL8685 timer*/
#define TC_BASE		0xB8003200
#define TC0DATA		(TC_BASE+0x00)
#define TC0CNT		(TC_BASE+0x04)
#define TC0CNR		(TC_BASE+0x08)
	#define TC0EN           		(1 << 28)   
	#define TC0MODE_TIMER   	(1 << 24)   
	#define TC0_DIV_OFFSET	0
#define TC0IR			(TC_BASE+0x0C)
	#define TC0IE			(1 << 20)   
	#define TC0IP			(1 << 16)

#define TC1DATA		(TC_BASE+0x10)
#define TC1CNT		(TC_BASE+0x14)
#define TC1CNR		(TC_BASE+0x18)
	#define TC1EN           		(1 << 28)   
	#define TC1MODE_TIMER   	(1 << 24)   
	#define TC1_DIV_OFFSET	0
#define TC1IR			(TC_BASE+0x1C)
	#define TC1IE			(1 << 20)   
	#define TC1IP			(1 << 16)

#else

#ifdef CONFIG_RTL8685
/* This is a template version, timer base address is 0xb8003200 */
#define TC_BASE		0xB8003200
#else
/* This is used in RTL8676 and RTL8676S */
#define TC_BASE		0xB8003100
#endif
#define TC0DATA		(TC_BASE+0x00)
#define TC1DATA		(TC_BASE+0x04)
#define TC0CNT		(TC_BASE+0x08)
#define TC1CNT		(TC_BASE+0x0C)
#define TCCNR		(TC_BASE+0x10)
#define TCIR		(TC_BASE+0x14)
#define CDBR		(TC_BASE+0x18)
#define WDTCNT		(TC_BASE+0x1C)
#define BSTMOUT		(TC_BASE+0x20)
#endif

#else
#define TC_BASE		0xb9c01000
#define TC0DATA		(TC_BASE+0x20)
#define TC1DATA		(TC_BASE+0x24)
#define TC0CNT		(TC_BASE+0x28)
#define TC1CNT		(TC_BASE+0x2C)
#define TCCNR		(TC_BASE+0x30)
#define TCIR		(TC_BASE+0x34)
#define CDBR		(TC_BASE+0x38)
#define WDTCNT		(TC_BASE+0x3C)
#define BSTMOUT		(TC_BASE+0x40)
#endif

//auto-detect chip version for system clock, loader/main.c
extern unsigned int CLOCKGEN_FREQ;

void init_timer(void)
{
#ifdef CONFIG_RTL8685_PERI 
	/* RTL8685 timer */
	/*disable clock */
	REG32(TC0CNR) = 0x00000000;
	REG32(TC1CNR) = 0x00000000;

	/* timer 1 interval as 0.5 ms  */
	REG32(TC1DATA) = (1000*4*60*60) << 0; /* 1 hour */
	REG32(TC1CNR) = (CLOCKGEN_FREQ/1000/4) << TC1_DIV_OFFSET;
	REG32(TC1CNR) |= (TC1EN|TC1MODE_TIMER);
#else /*CONFIG_RTL8685_PERI*/
	/* disable clock */
	REG32(CDBR) = 0x00000000;
	REG32(TCCNR) = 0x00000000;
	/* timer 1 interval as 0.5 ms  */
	REG32(TC1DATA) = (1000*4*60*60) << 8; /* 1 hour */
	REG32(CDBR) = (CLOCKGEN_FREQ/1000/4) << 16;
	REG32(TCCNR) = (1<<28)|(1<<29);
#endif /*CONFIG_RTL8685_PERI*/
}
#ifdef CONFIG_GDMA_SCAN
unsigned long get_tc1cnt(void)
{
#ifdef CONFIG_RTL8685_PERI
	return ((*((unsigned long *)TC1CNT)));
#else /*CONFIG_RTL8685_PERI*/
	//return (((*((unsigned long *)TC1CNT)>>8))& 0xff);
	return (((*((unsigned long *)TC1CNT)>>8))& 0xffff);
#endif /*CONFIG_RTL8685_PERI*/
}

unsigned long get_tc1cnt_dword(void)
{
#ifdef CONFIG_RTL8685_PERI
	return ((*((unsigned long *)TC1CNT)));
#else /*CONFIG_RTL8685_PERI*/
	return (((*((unsigned long *)TC1CNT)>>8))& 0xffff);
#endif /*CONFIG_RTL8685_PERI*/
}

#endif

unsigned long get_sys_time(void)
{
	//tylo, 8672 fpga
#ifdef CONFIG_RTL8685_PERI
	return ((*((volatile unsigned long *)TC1CNT))/4);
#else /*CONFIG_RTL8685_PERI*/
	return (((*((volatile unsigned long *)TC1CNT)>>8))/4);
	//return (((1000*4*60*60) - (*((unsigned long *)TC1CNT)>>8))/4);
#endif /*CONFIG_RTL8685_PERI*/
}

int timeout(unsigned long start, unsigned long ms)
{
unsigned long cur_ms = get_sys_time();
	if(cur_ms>=start)
		if((cur_ms-start) > ms)
			return 1;
	if(cur_ms<start)	/* roll over */
		if((cur_ms+(1000*60*60)-start) > ms)
			return 1;
	return 0;
}

void delay_msec(unsigned long ms)
{
unsigned long start = get_sys_time();

	while(!timeout(start,ms));
	
}

unsigned long tick_getSysClkRate(void)
{
    return CLOCKGEN_FREQ;
}

static unsigned int inst_per_tick;
void calibrate_delay(void) {
	unsigned int x;
	//printf("calibrate delay...");
	inst_per_tick = 0;
	
	x = REG32(TC1CNT);
	while (x == REG32(TC1CNT))
		;
	x = REG32(TC1CNT);
	while (x == REG32(TC1CNT))
	{
			inst_per_tick++;
	}
	//printf("%u \r\n", inst_per_tick);
}

void delay_usec(unsigned int x) {
	volatile unsigned c;
	
	c = ((x * inst_per_tick) / 1000) + 1;
	while (c--)
		;
}