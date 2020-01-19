/* re8670pool.c - RealTek re8670 Fast Ethernet interface header */
/* History:
 *  2007/03/06  SH  Added Dcache write invalidate for CPU LX4181
 *
*/

#include "re8670poll.h"
#include "iob.h"

#define NULL	0

unsigned char descriptor_tx[TX_DESC_NUM*16+256];
unsigned char descriptor_rx[RX_DESC_NUM*16+256];

NIC_FD_T *pTxBDPtr;
NIC_FD_T *pRxBDPtr;
unsigned int	txBDhead = 0;	// index for system to release buffer
unsigned int	txBDtail = 0;	// index for system to set buf to BD
unsigned int	rxBDtail = 0;	// index for system to set buf to BD

#ifndef CONFIG_RTL865XC
char *Lan_Receive(void)
#else
char Lan_Receive(unsigned char **data, unsigned int *len)
#endif
{
#ifdef CONFIG_RTL865XC
	int ret;
	ret=swNic_receive(data, len);

	if(ret!=0){
		return NULL;
	}
	else{
		return 1;
	}
#else
char *pBuf, *pData;

	EISR = 0xffff;  //reset RDU flag to start rx again

	if(pRxBDPtr[rxBDtail].StsLen & OWN_BIT)
		return NULL;

	pData = (char*)pRxBDPtr[rxBDtail].DataPtr;
	pBuf = IOBGetHdr(pData);
	IOB_PKT_LEN(pBuf) = (pRxBDPtr[rxBDtail].StsLen & 0xfff) - 4;

#if 0
	{
	int i;
	unsigned char *sbuf = pData;
		printf("p=%x, l=%d\n\r", sbuf, IOB_PKT_LEN(pBuf));
		for(i=0;i<IOB_PKT_LEN(pBuf); i++)
		{
			if((i%16)==0)
				printf("\n\r");
			printf("%02x ", sbuf[i]);
		}
		printf("\n\r");
	}
#endif

	pRxBDPtr[rxBDtail].StsLen &= ~0xfff;
	pRxBDPtr[rxBDtail].StsLen |= 2048;
	
	pRxBDPtr[rxBDtail].StsLen |= OWN_BIT;
	EthrntRxCPU_Des_Num = rxBDtail;
	rxBDtail++;
	rxBDtail %= RX_DESC_NUM;

	return (pBuf);
#endif
}

int Lan_Transmit(void * buff, unsigned int length)
{
#ifdef CONFIG_RTL865XC
	swNic_send(buff, length);
	swNic_txDone();
	return 0;
#else
	pTxBDPtr[txBDtail].DataPtr = (unsigned int)buff | UNCACHE_MASK;
	if(length < 60)
		length = 60;
	pTxBDPtr[txBDtail].StsLen &= ~0xfff;
	pTxBDPtr[txBDtail].StsLen |= length;

	// trigger to send
	pTxBDPtr[txBDtail].StsLen |= OWN_BIT|FS_BIT|LS_BIT|(1<<23);
	//tylo, for 8672 fpga, cache write-back
	__asm__ volatile(
		"mtc0 $0,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"li $8,512\n\t"
		"mtc0 $8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"mtc0 $0,$20\n\t"
		"nop"
		: /* no output */
		: /* no input */
			);

	IO_CMD |= (1<<0);
	
	delay_msec(1);
	while(pTxBDPtr[txBDtail].StsLen & OWN_BIT)
		delay_msec(1);

	/* advance one */
	txBDtail++;
	txBDtail %= TX_DESC_NUM;
	
	return 0;
#endif
}

int Lan_Initialed = 0;

//11/09/05' hrchen, disable NIC
void Lan_Stop(void)
{
#ifndef CONFIG_RTL865XC
   	CR = 0x01;	 /* Reset */	
	while ( CR & 0x1 );		
	CR = RX_DISALBE;	 /* receive checksum */

	/* Interrupt Register, ISR, IMR */
	EIMR = 0;
	EISR = 0xffff;
	
    Lan_Initialed = 0;
#endif
}

void rtl8672_smiWrite(unsigned char phyaddr,unsigned char regaddr,unsigned short value){
	unsigned int tmp=0;

	tmp=1<<31 | (phyaddr&0x1f)<<26 | (regaddr&0x1f)<<16 | (value&0xffff);	
	MIIAR=tmp;
	do
	{
		delay_msec(20);
	}
	while (MIIAR & 0x80000000);
}

void rtl8672_smiRead(unsigned char phyaddr,unsigned char regaddr,unsigned short *value){
	unsigned int tmp=0;

	tmp=(phyaddr&0x1f)<<26 | (regaddr&0x1f)<<16;
	MIIAR = tmp;
	do
	{
		delay_msec(20);
	}
	while (!(MIIAR & 0x80000000));
	*value=MIIAR&0xffff;
}

void internal_select_page(unsigned char page)
{
	unsigned int tmp=0;
	unsigned short value=0;

	tmp=(1<<26) | ((31&0x1f)<<16);
	MIIAR = tmp;
	do {
		delay_msec(20);
	}
	while (!(MIIAR & 0x80000000));
	value = MIIAR&0xfffc;
	value |= page;

	tmp = (1<<31) | (1<<26) | ((31&0x1f)<<16) | (value&0xffff);
	MIIAR = tmp;
	do {
		delay_msec(20);
	}
	while (MIIAR & 0x80000000);
}

#define REG32(reg)      (*(volatile unsigned int *)(reg))

int Lan_Initialize(char *mac)
{
	if(Lan_Initialed)
		return 0;
	initIOB();
	
        #ifdef CONFIG_RTL8676S
        REG32(0xbb804004) = 0x80180310;
        #endif


#ifdef CONFIG_RTL865XC
	swInit(mac);
#else
	
	Lan_Stop();

	pTxBDPtr = (NIC_FD_T *)((((unsigned int)(descriptor_tx+0xff))& 0xffffff00)|UNCACHE_MASK);
	pRxBDPtr = (NIC_FD_T *)((((unsigned int)(descriptor_rx+0xff))& 0xffffff00)|UNCACHE_MASK);
	
	/* setup descriptor */
	RxFDP = pRxBDPtr;
	RxCDO = 0;
	TxFDP1 = pTxBDPtr;
	TxCDO1 = 0;	

	// init xmt BD
	int i;
	
	for(i=0;i<TX_DESC_NUM;i++)
	{
		pTxBDPtr[i].StsLen = 0;
		pTxBDPtr[i].VLan = 0;
		pTxBDPtr[i].DataPtr = 0;
	}
	pTxBDPtr[TX_DESC_NUM-1].StsLen |= EOR_BIT;

	for(i=0;i<RX_DESC_NUM;i++)
	{
		char *pBuf;
		if ( (pBuf = getIOB()) == 0 ) {
            return -1;
        }

		pRxBDPtr[i].StsLen = 2048+ OWN_BIT + RX_DESC_BUFFER_SIZE;
		pRxBDPtr[i].VLan = 0;
		pRxBDPtr[i].DataPtr = (unsigned long) IOB_PKT_PTR(pBuf);
	}
	pRxBDPtr[RX_DESC_NUM-1].StsLen |= EOR_BIT;
	//    
	// set MAC address
#if 1	

	{
		unsigned long mac_reg0, mac_reg1;
		//mac_reg0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
		//mac_reg1 = (mac[4] << 24) | (mac[5] << 16);
		memcpy(&mac_reg0, mac, 4);
		memcpy(&mac_reg1, &mac[4], 2);
		NIC_ID0 = mac_reg0;
		NIC_ID1 = mac_reg1;	
	}
#endif	
	/* RCR, don't accept error packet */
	RCR = NoErrAccept;
	//RCR = NoErrPromiscAccept;
	
	/* TCR: IFG, Mode */
	TCR = (unsigned long)(TCR_IFG<<TCR_IFG_OFFSET)|(TCR_NORMAL<<TCR_MODE_OFFSET);
	
	Rx_Pse_Des_Thres = RX_FC_THRESHOLD;

	/* Rx descriptor Size */	
	EthrntRxCPU_Des_Num = RX_DESC_NUM-1;

	RxRingSize = 0x00;	// 16 descriptor

	/* Flow Control */
	MSR &= ~(TXFCE | RXFCE);
 	
	/* Ethernet IO CMD */
	IO_CMD = CMD_CONFIG;

#ifdef SUPPORT_MULT_UPGRADE
	MAR0 = 0xffffffff;
	MAR4 = 0xffffffff;
#endif	
#endif

	Lan_Initialed = 1;
   	txBDtail = 0;	// index for system to set buf to BD
    rxBDtail = 0;	// index for system to set buf to BD

	return 0;
}

