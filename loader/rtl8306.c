#include "gpio.h"

#define RTL_W32(reg, value)			(*(volatile u32*)(ETHBASE+reg)) = (u32)value
#define RTL_W16(reg, value)			(*(volatile u16*)(ETHBASE+reg)) = (u16)value
#define RTL_W8(reg, value)			(*(volatile u8*)(ETHBASE+reg)) = (u8)value
#define RTL_R32(reg)				(*(volatile u32*)(ETHBASE+reg))
#define RTL_R16(reg)				(*(volatile u16*)(ETHBASE+reg))
#define RTL_R8(reg)					(*(volatile u8*)(ETHBASE+reg))

typedef unsigned long long	uint64;
typedef long long		int64;
typedef unsigned int	uint32;
typedef int			int32;
typedef unsigned short	uint16;
typedef short			int16;
typedef unsigned char	uint8;
typedef char			int8;

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED -1
#endif

#define RTL8306_PHY_NUMBER	7
#define RTL8306_REGSPERPAGE  32
#define RTL8306_REGSPERPHY	68
#define RTL8306_REG_NUMBER  ((RTL8306_REGSPERPHY)*(RTL8306_PHY_NUMBER))
#define RTL8306_PAGE_NUMBER 4
#define RTL8306_REGPAGE0		0x0
#define RTL8306_REGPAGE1		0x1
#define RTL8306_REGPAGE2		0x2
#define RTL8306_REGPAGE3		0x3
#define RTL8306_PORT0 		0x0
#define RTL8306_PORT1 		0x1
#define RTL8306_PORT2 		0x2
#define RTL8306_PORT3 		0x3
#define RTL8306_PORT4 		0x4
#define RTL8306_PORT5 		0x5
#define RTL8306_PORT_NUMBER 6
#define RTL8306_NOCPUPORT 7

#define RTL8306_UNICASTPKT		0   /*Unicast packet, but not include unknown DA unicast packet*/
#define RTL8306_BROADCASTPKT      1   /*Broadcast packet*/
#define RTL8306_MULTICASTPKT		2   /*Multicast packet*/
#define RTL8306_UDAPKT			3   /*Unknown DA unicast packet*/

#define RTL8306_S 0
#define RTL8306_SD 1
#define RTL8306_SDM 2
#define RTL8306_CHIPID 0X5988
#define RTL8306_VERNUM 0X0

typedef struct asicVersionPara_s
{
    uint16 chipid;
    uint8 vernum;
    uint8 series;
    uint8 revision;
} asicVersionPara_t;

#define REG32GPIO(reg)	*(volatile unsigned int*)(0xb8003500+reg)
#define GPIO_SHIFT(x)	(x)

#define PABCDIR		(0x08)
#define PABCDAT		(0x0c)

void _rtl8305s_smiZBit(void) {
	unsigned int i;
	//REG32(PABCDIR) = (REG32(PABCDIR)& 0x3FFFFFFF) | 0x80000000;
	REG32GPIO(PABCDIR)= (REG32GPIO(PABCDIR)& ~((1<<GPIO_SHIFT(GPIO_MDC))|(1<<GPIO_SHIFT(GPIO_MDIO))))
	 | (1<<GPIO_SHIFT(GPIO_MDC));
	//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF);
	//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF);
	gpioClear(GPIO_MDC);//GPIO_PA4
	gpioClear(GPIO_MDIO);//GPIO_PA3
	for(i=0; i<25; i++);
}

/* Generate  1 -> 0 transition and sampled at 1 to 0 transition time */
void _rtl8305s_smiReadBit(unsigned char * data) {
	unsigned int i;
	//REG32(PABCDIR) = (REG32(PABCDIR)& 0x3FFFFFFF) | 0x80000000;
	REG32GPIO(PABCDIR)= (REG32GPIO(PABCDIR)& ~((1<<GPIO_SHIFT(GPIO_MDC))|(1<<GPIO_SHIFT(GPIO_MDIO))))
	 | (1<<GPIO_SHIFT(GPIO_MDC));
	//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF) | 0x80000000;
	//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF) | 0x00100000;
	gpioSet(GPIO_MDC); //GPIO_PA4
	gpioClear(GPIO_MDIO); //GPIO_PA3
	for(i=0; i<25; i++);
	//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF);
	//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF);	
	gpioClear(GPIO_MDC);//GPIO_PA4
	gpioClear(GPIO_MDIO);//GPIO_PA3	
	//*data = (REG32(PABCDAT) & 0x40000000)?1:0;
	*data = (REG32GPIO(PABCDAT) & (1<<GPIO_SHIFT(GPIO_MDIO)))?1:0;
}

/* Generate  0 -> 1 transition and put data ready during 0 to 1 whole period */
void _rtl8305s_smiWriteBit(unsigned char data) {
	unsigned int i;
	
	//REG32(PABCDIR) = REG32(PABCDIR) | 0xC0000000;
	REG32GPIO(PABCDIR) = REG32GPIO(PABCDIR) | (1<<GPIO_SHIFT(GPIO_MDC)) | (1<<GPIO_SHIFT(GPIO_MDIO));
	if(data) {/* Write 1 */
		//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF) | 0x40000000;
		//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF) | 0x00080000;
		gpioClear(GPIO_MDC);//GPIO_PA4
		gpioSet(GPIO_MDIO);//GPIO_PA3

		for(i=0; i<25; i++);
		//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF) | 0xC0000000;
		//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF) | 0x00180000;
		gpioSet(GPIO_MDC);//GPIO_PA4
		gpioSet(GPIO_MDIO);//GPIO_PA3
	} else {
		//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF);
		//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF);
		gpioClear(GPIO_MDC);//GPIO_PA4
		gpioClear(GPIO_MDIO);//GPIO_PA3
		for(i=0; i<25; i++);
		//REG32(PABCDAT) = (REG32(PABCDAT) & 0x3FFFFFFF) | 0x80000000;
		//REG32GPIO(PABCDAT) = (REG32GPIO(PABCDAT) & 0xFFe7FFFF) | 0x00100000;
		gpioSet(GPIO_MDC);//GPIO_PA4
		gpioClear(GPIO_MDIO);//GPIO_PA3
	}
}

void rtl8305s_smiRead(unsigned char phyad, unsigned char regad, unsigned short * data) {
	int i;
	unsigned char readBit;

	/* Configure port A pin 6, 7 to be GPIO and disable interrupts of these two pins */
	//REG32(PABCCNR) = REG32(PABCCNR) & 0x3FFFFFFF;
	//REG32(PABIMR) = REG32(PABIMR) & 0xFFFFFFF;
	/* 32 continuous 1 as preamble*/
	for(i=0; i<32; i++)
		_rtl8305s_smiWriteBit(1);
	/* ST: Start of Frame, <01>*/
	_rtl8305s_smiWriteBit(0);
	_rtl8305s_smiWriteBit(1);
	/* OP: Operation code, read is <10> */
	_rtl8305s_smiWriteBit(1);
	_rtl8305s_smiWriteBit(0);
	/* PHY Address */
	for(i=4; i>=0; i--) 
		_rtl8305s_smiWriteBit((phyad>>i)&0x1);
	/* Register Address */
	for(i=4; i>=0; i--) 
		_rtl8305s_smiWriteBit((regad>>i)&0x1);
	/* TA: Turnaround <z0> */
	_rtl8305s_smiZBit();
	_rtl8305s_smiReadBit(&readBit);
	/* Data */
	*data = 0;
	for(i=15; i>=0; i--) {
		_rtl8305s_smiReadBit(&readBit);
		*data = (*data<<1) | readBit;
	}
	_rtl8305s_smiZBit();
}

void rtl8305s_smiWrite(unsigned char phyad, unsigned char regad, unsigned short data) {
	int i;

	/* Configure port A pin 6, 7 to be GPIO and disable interrupts of these two pins */
	//REG32(PABCCNR) = REG32(PABCCNR) & 0x3FFFFFFF;
	//REG32(PABIMR) = REG32(PABIMR) & 0xFFFFFFF;
	/* 32 continuous 1 as preamble*/
	for(i=0; i<32; i++)
		_rtl8305s_smiWriteBit(1);
	/* ST: Start of Frame, <01>*/
	_rtl8305s_smiWriteBit(0);
	_rtl8305s_smiWriteBit(1);
	/* OP: Operation code, write is <01> */
	_rtl8305s_smiWriteBit(0);
	_rtl8305s_smiWriteBit(1);
	/* PHY Address */
	for(i=4; i>=0; i--) 
		_rtl8305s_smiWriteBit((phyad>>i)&0x1);
	/* Register Address */
	for(i=4; i>=0; i--) 
		_rtl8305s_smiWriteBit((regad>>i)&0x1);
	/* TA: Turnaround <10> */
	_rtl8305s_smiWriteBit(1);
	_rtl8305s_smiWriteBit(0);
	/* Data */
	for(i=15; i>=0; i--) 
		_rtl8305s_smiWriteBit((data>>i)&0x1);
	_rtl8305s_smiZBit();
}



void miiar_write(unsigned char phyaddr,unsigned char regaddr,unsigned short value){	
	rtl8305s_smiWrite(phyaddr,regaddr,value);
}

void miiar_read(unsigned char phyaddr,unsigned char regaddr,unsigned short *value){
	
	rtl8305s_smiRead(phyaddr,regaddr,value);
}

int32 smiRead(uint32 phyad, uint32 regad, uint32 * data) 
{
	uint16 mydata;
	miiar_read(phyad , (unsigned char)regad ,(unsigned short*)&mydata); 
	*data = (uint32)mydata;
	return	SUCCESS;
}

int32 smiWrite(uint32 phyad, uint32 regad, uint32 data)
{
	miiar_write(phyad , (unsigned char)regad , (unsigned short)data);
	return 	SUCCESS;	
}

int32 _rtl8306_phyReg_set(uint32 phyad, uint32 regad, uint32 npage, uint32 value) 
{
    uint32 rdata;
    uint32 regval; 

    if ((phyad >= 7) || (npage > 0x5))
        return FAILED;

    /*read/write pcs register*/
    smiRead(5, 16, &rdata); 
    smiWrite(5, 16, rdata|0x0001);

    /*Select PHY Register Page*/
    smiRead(phyad, 31, &regval);
    regval &= ~0xFF;
    regval |= npage;
    smiWrite(phyad, 31, regval);

    smiWrite(phyad, regad, value);

    smiWrite(5, 16, rdata & (~0x0001));
    
    return SUCCESS;
}

int32 rtl8306_setAsicPhyReg(uint32 phyad, uint32 regad, uint32 npage, uint32 value) {
	uint32 rdata; 

	if ((phyad >= RTL8306_PHY_NUMBER) || (regad >= RTL8306_REGSPERPAGE) ||
		(npage >= RTL8306_PAGE_NUMBER))	
		return FAILED;
	/* Select PHY Register Page through configuring PHY 0 Register 16 [bit1 bit15] */
	value = value & 0xFFFF;
	smiRead(0, 16, &rdata); 
	switch (npage) {
	case RTL8306_REGPAGE0:
		smiWrite(0, 16, (rdata & 0x7FFF) | 0x0002);
		break;
	case RTL8306_REGPAGE1:
		smiWrite(0, 16, rdata | 0x8002 );
		break;
	case RTL8306_REGPAGE2:
		smiWrite(0, 16, rdata & 0x7FFD);
		break;
	case RTL8306_REGPAGE3:
		smiWrite(0, 16, (rdata & 0xFFFD) | 0x8000);
		break;
	default:
		return FAILED;
	}
	
	smiWrite(phyad, regad, value);
	return SUCCESS;
}


int32 rtl8306_getAsicPhyReg(uint32 phyad, uint32 regad, uint32 npage, uint32 *pvalue) {
	uint32 rdata;

	if ((phyad >= RTL8306_PHY_NUMBER) || (regad >= RTL8306_REGSPERPAGE) ||
		(npage >= RTL8306_PAGE_NUMBER))	
		return FAILED;

	/* Select PHY Register Page through configuring PHY 0 Register 16 [bit1 bit15] */
	smiRead(0, 16, &rdata); 
	switch (npage) {
	case RTL8306_REGPAGE0:
		smiWrite(0, 16, (rdata & 0x7FFF) | 0x0002);
		break;
	case RTL8306_REGPAGE1:
		smiWrite(0, 16, rdata | 0x8002 );
		break;
	case RTL8306_REGPAGE2:
		smiWrite(0, 16, rdata & 0x7FFD);
		break;
	case RTL8306_REGPAGE3:
		smiWrite(0, 16, (rdata & 0xFFFD) | 0x8000);
		break;
	default:
		return FAILED;
	}

	smiRead(phyad, regad, pvalue);
	*pvalue = *pvalue & 0xFFFF;
	return SUCCESS;
	
}

int32 _rtl8306_phyReg_get(uint32 phyad, uint32 regad, uint32 npage, uint32 *pvalue)
{
    uint32 rdata;
    uint32 regval; 

    if ((phyad >= 7) || (npage > 0x5))
        return FAILED;

    /*read/write pcs register*/
    smiRead(5, 16, &rdata);
    smiWrite(5, 16, rdata|0x0001);

    /*Select PHY Register Page*/
    smiRead(phyad, 31, &regval);
    regval &= ~0xFF;
    regval |= npage;
    smiWrite(phyad, 31, regval);
    
    smiRead(phyad, regad, pvalue);

    *pvalue = *pvalue & 0xFFFF;

    smiWrite(5, 16, rdata & (~0x0001));
    
    return SUCCESS;
}

int32 rtl8306_setAsicPhyRegBit(uint32 phyad, uint32 regad, uint32 bit, uint32 npage,  uint32 value) {
	uint32 rdata;
	if ((phyad >= RTL8306_PHY_NUMBER) || (regad >= RTL8306_REGSPERPAGE) || 
		(npage >= RTL8306_PAGE_NUMBER) || (bit > 15) || (value >1))
		return FAILED;
	rtl8306_getAsicPhyReg(phyad, regad,  npage, &rdata);
	if (value) 
		rtl8306_setAsicPhyReg(phyad, regad, npage, rdata | (1 << bit));
	else
		rtl8306_setAsicPhyReg(phyad, regad, npage, rdata & (~(1 << bit)));
	return SUCCESS;
}

int32 rtl8306_getAsicVersionInfo(asicVersionPara_t *pAsicVer)
{
    uint32 regval;

    /*get chip id*/
    rtl8306_getAsicPhyReg(4, 30, 0, &regval );
    pAsicVer->chipid = (uint16)regval;
    /*get version number*/
    rtl8306_getAsicPhyReg(4, 31, 0, &regval);
    pAsicVer->vernum = (uint8)(regval & 0xFF);
    /* bit[8:9]*/
    rtl8306_getAsicPhyReg(4, 31, 0, &regval);
    regval = (regval & 0x300) >> 8;
    if (regval  == 0 || regval ==2)
        pAsicVer->series = RTL8306_S;
    else if (regval == 1)
        pAsicVer->series = RTL8306_SD;
    else if (regval == 3)
        pAsicVer->series = RTL8306_SDM;
    else 
        pAsicVer->series = 0xFF;
        
    rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 1);
    rtl8306_getAsicPhyReg(4, 26, 0, &regval);
    pAsicVer->revision = (regval & 0xE000) >> 13;
    rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 0);

    return SUCCESS;
}

int32 rtl8306_setAsicCPUPort(uint32 port, uint32 enTag) {
	uint32 regValue;
	
	if (port > RTL8306_NOCPUPORT)
		return FAILED;

      if (port < RTL8306_PORT_NUMBER) {
            /*Enable CPU port Function */
        	rtl8306_setAsicPhyRegBit(2, 21, 15, 3, 0);	
        	/*Whether enable inserting CPU tag*/
        	rtl8306_setAsicPhyRegBit(2, 21, 12, 3, enTag == TRUE ? 1 : 0);
        	/*Enable the ability to check cpu tag*/
        	//rtl8306_setAsicPhyRegBit(4, 21, 7, 0, enTag == TRUE ? 1 : 0);
        	rtl8306_setAsicPhyRegBit(4, 21, 7, 0, 1);
        	/*Enable removing CPU tag*/
        	rtl8306_setAsicPhyRegBit(2, 21, 11, 3, 1);
        	rtl8306_getAsicPhyReg(4, 21, 0, &regValue);
        	regValue = (regValue & 0xFFF8) | port;
        	rtl8306_setAsicPhyReg(4, 21, 0, regValue);
        	/*Disable IEEE802.1x function of CPU Port*/	
        	if (port < RTL8306_PORT5) {
	        	rtl8306_setAsicPhyRegBit(port, 17, 9, 2, 0);
        		rtl8306_setAsicPhyRegBit(port, 17, 8, 2, 0);
        	} else {
	        	rtl8306_setAsicPhyRegBit(6, 17, 9, 2, 0);
        		rtl8306_setAsicPhyRegBit(6, 17, 8, 2, 0);
        	}
        	/*Port 5 should be enabled especially*/
        	if (port == RTL8306_PORT5)
	        	rtl8306_setAsicPhyRegBit(6, 22, 15, 0, TRUE);
        }
        else {
            /*Disable CPU port Function */
        	rtl8306_setAsicPhyRegBit(2, 21, 15, 3, 1);	
        	rtl8306_getAsicPhyReg(4, 21, 0, &regValue);
        	regValue = (regValue & 0xFFF8) | port;
        	rtl8306_setAsicPhyReg(4, 21, 0, regValue);
        }   
		
	return SUCCESS;	
}

int32 rtl8306_setAsicStormFilterEnable(uint32 type, uint32 enabled) {

	switch(type) {
	case RTL8306_BROADCASTPKT:
		rtl8306_setAsicPhyRegBit(0, 18, 2, 0, enabled == TRUE ? 0:1);
		break;
	case RTL8306_MULTICASTPKT:
		rtl8306_setAsicPhyRegBit(2, 23, 9, 3, enabled == TRUE ? 0:1);		
		break;
	case RTL8306_UDAPKT:
		rtl8306_setAsicPhyRegBit(2, 23, 8, 3, enabled == TRUE ? 0:1);				
		break;
	default:
		return FAILED;		
	}		
	return SUCCESS;
}

int32 rtl8306_init(void) {

    asicVersionPara_t AsicVer;
    uint32 regval;
    uint32 regval2;
    uint32 regval3;
    uint32 phy;
    

    rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 1);
    rtl8306_setAsicPhyRegBit(4, 23, 5, 0, 1); 
    rtl8306_getAsicPhyReg(4, 30, 0, &regval);
    rtl8306_getAsicPhyReg(4, 26, 0, &regval2);
    rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 0);
    rtl8306_setAsicPhyRegBit(4, 23, 5, 0, 0); 
    if (0x6167 == regval)
    {
    	printf("6167 ");
		switch ((regval2>>13)&0x7)
		{
		case 0:
			printf("A\n");
			break;
		case 1:
			printf("B\n");
			break;
		case 2:
			printf("C\n");
			break;
		default:
			printf("\n");
			break;
		}
        if ((regval2 & (0x7<<13)) >> 13 == 1)
        {
            rtl8306_setAsicPhyRegBit(0, 19, 2, 0, 0);
            rtl8306_setAsicPhyRegBit(4, 23, 8, 0, 1);
            rtl8306_getAsicPhyReg(0, 31, 3, &regval3);
            regval3 &= ~0x3;
            regval3 |= 0x2;
            rtl8306_setAsicPhyReg(0, 31, 3, regval3);
            rtl8306_getAsicPhyReg(1, 31, 3, &regval3);
            regval3 &= ~0x3;
            regval3 |= 0x2;
            rtl8306_setAsicPhyReg(1, 31, 3, regval3);
            rtl8306_getAsicPhyReg(2, 31, 3, &regval3);
            regval3 &= ~0x3;
            regval3 |= 0x2;
            rtl8306_setAsicPhyReg(2, 31, 3, regval3);
            rtl8306_getAsicPhyReg(3, 31, 3, &regval3);
            regval3 &= ~0x3;
            regval3 |= 0x2;
            rtl8306_setAsicPhyReg(3, 31, 3, regval3);
            rtl8306_getAsicPhyReg(5, 31, 3, &regval3);
            regval3 &= ~0x3;
            regval3 |= 0x2;
            rtl8306_setAsicPhyReg(5, 31, 3, regval3);
            rtl8306_setAsicPhyRegBit(4, 23, 8, 0, 0);

            _rtl8306_phyReg_get(0, 25, 4, &regval3);
            regval3 &= ~(0x7 << 4);
            regval3 |= (0x6 << 4);
            _rtl8306_phyReg_set(0, 25, 4, regval3);

            _rtl8306_phyReg_get(0, 28, 4, &regval3);
            regval3 &= ~(0x1 << 8);
            regval3 |= (0x1 << 8);
            _rtl8306_phyReg_set(0, 28, 4, regval3);

            _rtl8306_phyReg_set(0, 29, 4, 0x5000);
            _rtl8306_phyReg_set(0, 29, 4, 0x6000);
            _rtl8306_phyReg_set(0, 29, 4, 0x7000);
            _rtl8306_phyReg_set(0, 29, 4, 0x4000);
            _rtl8306_phyReg_set(0, 29, 4, 0x8700);
            _rtl8306_phyReg_set(0, 29, 4, 0xD36C);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFFF);
            _rtl8306_phyReg_set(0, 29, 4, 0xCA6C);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFFD);
            _rtl8306_phyReg_set(0, 29, 4, 0x5060);
            _rtl8306_phyReg_set(0, 29, 4, 0x61C5);
            _rtl8306_phyReg_set(0, 29, 4, 0x7010);
            _rtl8306_phyReg_set(0, 29, 4, 0x4001);
            _rtl8306_phyReg_set(0, 29, 4, 0x5061);
            _rtl8306_phyReg_set(0, 29, 4, 0x4001);
            _rtl8306_phyReg_set(0, 29, 4, 0x87F1);
            _rtl8306_phyReg_set(0, 29, 4, 0xCE60);
            _rtl8306_phyReg_set(0, 29, 4, 0x0026);
            _rtl8306_phyReg_set(0, 29, 4, 0x8E03);
            _rtl8306_phyReg_set(0, 29, 4, 0xA021);
            _rtl8306_phyReg_set(0, 29, 4, 0x300F);
            _rtl8306_phyReg_set(0, 29, 4, 0x58A0);
            _rtl8306_phyReg_set(0, 29, 4, 0x629C);
            _rtl8306_phyReg_set(0, 29, 4, 0x7010);
            _rtl8306_phyReg_set(0, 29, 4, 0x4002);
            _rtl8306_phyReg_set(0, 29, 4, 0x58A1);
            _rtl8306_phyReg_set(0, 29, 4, 0x87E6);
            _rtl8306_phyReg_set(0, 29, 4, 0xAE25);
            _rtl8306_phyReg_set(0, 29, 4, 0xA018);
            _rtl8306_phyReg_set(0, 29, 4, 0x301A);
            _rtl8306_phyReg_set(0, 29, 4, 0x6E94);
            _rtl8306_phyReg_set(0, 29, 4, 0x6694);
            _rtl8306_phyReg_set(0, 29, 4, 0x5123);
            _rtl8306_phyReg_set(0, 29, 4, 0x63C2);
            _rtl8306_phyReg_set(0, 29, 4, 0x5127);
            _rtl8306_phyReg_set(0, 29, 4, 0x4003);
            _rtl8306_phyReg_set(0, 29, 4, 0x87DC);
            _rtl8306_phyReg_set(0, 29, 4, 0x8EF3);
            _rtl8306_phyReg_set(0, 29, 4, 0xA10E);
            _rtl8306_phyReg_set(0, 29, 4, 0xCC40);
            _rtl8306_phyReg_set(0, 29, 4, 0x0007);
            _rtl8306_phyReg_set(0, 29, 4, 0xCA40);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFDF);
            _rtl8306_phyReg_set(0, 29, 4, 0xA202);
            _rtl8306_phyReg_set(0, 29, 4, 0x3024);
            _rtl8306_phyReg_set(0, 29, 4, 0x7018);
            _rtl8306_phyReg_set(0, 29, 4, 0x3024);
            _rtl8306_phyReg_set(0, 29, 4, 0xCC44);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFF4);
            _rtl8306_phyReg_set(0, 29, 4, 0xCC44);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFF2);
            _rtl8306_phyReg_set(0, 29, 4, 0x3000);
            _rtl8306_phyReg_set(0, 29, 4, 0x5220);
            _rtl8306_phyReg_set(0, 29, 4, 0x4004);
            _rtl8306_phyReg_set(0, 29, 4, 0x3000);
            _rtl8306_phyReg_set(0, 29, 4, 0x64A0);
            _rtl8306_phyReg_set(0, 29, 4, 0x5429);
            _rtl8306_phyReg_set(0, 29, 4, 0x4005);
            _rtl8306_phyReg_set(0, 29, 4, 0x87C6);
            _rtl8306_phyReg_set(0, 29, 4, 0xCE18);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFC4);
            _rtl8306_phyReg_set(0, 29, 4, 0xCE64);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFCF);
            _rtl8306_phyReg_set(0, 29, 4, 0x303A);
            _rtl8306_phyReg_set(0, 29, 4, 0x65C0);
            _rtl8306_phyReg_set(0, 29, 4, 0x50A9);
            _rtl8306_phyReg_set(0, 29, 4, 0x4006);
            _rtl8306_phyReg_set(0, 29, 4, 0xA3DB);
            _rtl8306_phyReg_set(0, 29, 4, 0x3043);
            _rtl8306_phyReg_set(0, 29, 4, 0x5000);
            _rtl8306_phyReg_set(0, 29, 4, 0x60F3);
            _rtl8306_phyReg_set(0, 29, 4, 0x5008);
            _rtl8306_phyReg_set(0, 29, 4, 0x7010);
            _rtl8306_phyReg_set(0, 29, 4, 0x4000);
            _rtl8306_phyReg_set(0, 29, 4, 0x87B6);
            _rtl8306_phyReg_set(0, 29, 4, 0xA3B5);
            _rtl8306_phyReg_set(0, 29, 4, 0xD36C);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFFD);
            _rtl8306_phyReg_set(0, 29, 4, 0xCA68);
            _rtl8306_phyReg_set(0, 29, 4, 0xFFBA);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);
            _rtl8306_phyReg_set(0, 29, 4, 0x0000);

            _rtl8306_phyReg_get(0, 28, 4, &regval3);
            regval3 &= ~(0x1 << 8);
            _rtl8306_phyReg_set(0, 28, 4, regval3);

            _rtl8306_phyReg_get(0, 25, 4, &regval3);
            regval3 &= ~(0x7 << 4);
            regval3 |= (0x3 << 4);
            _rtl8306_phyReg_set(0, 25, 4, regval3);

            for (phy = 0; phy <= 4; phy++)
            {
                _rtl8306_phyReg_get(phy, 24, 4, &regval3);
                regval3 &= ~(0xFF);
                regval3 |= 0xF3;
                _rtl8306_phyReg_set(phy, 24, 4, regval3);
            }
            
            /*RTCT*/
            for (phy = 0; phy <= 4; phy++)
            {
                _rtl8306_phyReg_get(phy, 16, 2, &regval3);
                regval3 &= ~(0x3FF);
                regval3 |= 0xFA;
                _rtl8306_phyReg_set(phy, 16, 2, regval3);
                _rtl8306_phyReg_get(phy, 22, 2, &regval3);
                regval3 &= ~(0x7FF);
                regval3 |= 0x12C;
                _rtl8306_phyReg_set(phy, 22, 2, regval3);
                _rtl8306_phyReg_get(phy, 23, 2, &regval3);
                regval3 &= ~(0x3FF);
                regval3 |= 0xC8;
                _rtl8306_phyReg_set(phy, 23, 2, regval3);
                _rtl8306_phyReg_get(phy, 24, 2, &regval3);
                regval3 &= ~(0x1FF);
                regval3 |= 0x32;
                _rtl8306_phyReg_set(phy, 24, 2, regval3);
                _rtl8306_phyReg_get(phy, 19, 2, &regval3);
                regval3 &= ~(0xF << 12);
                regval3 |= (0x4 << 12);
                _rtl8306_phyReg_set(phy, 19, 2, regval3);
                _rtl8306_phyReg_get(phy, 18, 2, &regval3);
                regval3 &= ~(0x1F << 10);
                regval3 |= (0x5 << 10);
                _rtl8306_phyReg_set(phy, 18, 2, regval3);
                _rtl8306_phyReg_get(phy, 25, 2, &regval3);
                regval3 &= ~(0xFF << 8);
                regval3 |= (0x4 << 8);
                _rtl8306_phyReg_set(phy, 25, 2, regval3);
                _rtl8306_phyReg_get(phy, 25, 2, &regval3);
                regval3 &= ~0xFF;
                regval3 |= 0x40;
                _rtl8306_phyReg_set(phy, 25, 2, regval3);
                
            }
        
        }
        else if ((regval2 & (0x7<<13)) >> 13 == 2)
        {
            for (phy = 0; phy <= 4; phy++)
            {
                _rtl8306_phyReg_get(phy, 22, 0, &regval3);
                regval3 &= ~(0x1 << 14);
                _rtl8306_phyReg_set(phy, 22, 0, regval3);        
            }
        }
    }
    else
    {
        /*Fix EQC problem in Version B of RTL8306 series*/
        rtl8306_getAsicVersionInfo(&AsicVer);
        if ((AsicVer.chipid == RTL8306_CHIPID) && 
            (AsicVer.vernum == RTL8306_VERNUM) && 
            (AsicVer.revision == 0x0)  )
        {
            rtl8306_setAsicPhyReg(2, 26, 0, 0x0056);
        }

        /*green featue for Version E*/
        if ((AsicVer.chipid == RTL8306_CHIPID) && 
            (AsicVer.vernum == RTL8306_VERNUM) && 
            (AsicVer.revision == 0x3))
        {
            rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 1);  

            rtl8306_getAsicPhyReg(0, 26, 0, &regval);  
            regval &= ~0x7007;
            regval |= 0x3003;
            rtl8306_setAsicPhyReg(0, 26, 0, regval);
            
            rtl8306_getAsicPhyReg(1, 29, 0, &regval);  
            regval &= ~0xFF;
            regval |= 0xC4;
            rtl8306_setAsicPhyReg(1, 29, 0, regval);

            rtl8306_setAsicPhyRegBit(0, 16, 11, 0, 0);  
        }
    }

    rtl8306_setAsicCPUPort(RTL8306_NOCPUPORT, FALSE);
    rtl8306_setAsicStormFilterEnable(RTL8306_BROADCASTPKT, TRUE);
    
    return SUCCESS;
}


#define MISCCR_BASE	0xB8003300
#define MISC_PINMUX	(MISCCR_BASE+0x00)

void init_lanswitch(void)
{
	unsigned short val=0;
	
	(*(volatile unsigned int *)MISC_PINMUX) &= 0x7fffffff;

	rtl8306_init();
	
	(*(volatile unsigned int *)MISC_PINMUX) |= 0x80000000;
/*	
	miiar_read(0, 16, &val);
	miiar_write(0, 16, (val & 0x7FFD)); //Set Page 0
	miiar_read(0, 19, &val);
	val &= ~(1<<2);
	miiar_write(0, 19, val);

	miiar_read(0, 19, &val);
*/
	//enable jtag
	//(*(volatile unsigned int *)MISC_PINMUX) |= 0x80000000;
}
/*
 * Enable_8306: Using GPIO to simulate MDC/MDIO to config rtl8306
 */              
void Enable_8306(void)
{		
    unsigned short val=0;    
    
    miiar_read(6, 22, &val);    
    val|= (1<< 15);  // Link up port 5
    miiar_write(6, 22, val);
}	