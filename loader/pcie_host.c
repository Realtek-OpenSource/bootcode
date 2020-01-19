/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Abstract: [1] PCIE driver source code.
*		  [2] PCIE host applications
* ---------------------------------------------------------------
*/

#include "board.h"
#include "gpio.h"
#include "pcie_host.h"

/*
  * 	extern functions
  */
extern void delay_msec(unsigned long ms);

/*	
  * 	Function 		: HostPCIe_SetPhyMdioWrite
  *	Input	      		: regaddr, val, slot
  *	Description 	: Write PCIe PHY parameters through MDIO 
  *
  */
void HostPCIe_SetPhyMdioWrite(unsigned int regaddr, unsigned short val, unsigned int slot)
{
	unsigned int MDIO_ADDR=0;

	if(slot == 0)
		MDIO_ADDR = PCIE0_MDIO;
	else if(slot == 1)
		MDIO_ADDR = PCIE1_MDIO;
	
 	REG32(MDIO_ADDR)= ( (regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET) | ((val&0xffff)<<PCIE_MDIO_DATA_OFFSET)  | (1<<PCIE_MDIO_RDWR_OFFSET) ; 
	delay_msec(1);
}


/*	
  * 	Function 		: HostPCIe_ReadPhyMdioWrite
  *	Input	      		: regaddr, val, slot
  *	Description 	: Read PCIe PHY parameters through MDIO 
  *
  */
void HostPCIe_ReadPhyMdioWrite(unsigned int regaddr, unsigned int slot)
{
	unsigned read_data=0, MDIO_ADDR=0;

	if(slot == 0)
		MDIO_ADDR = PCIE0_MDIO;
	else if(slot == 1)
		MDIO_ADDR = PCIE1_MDIO;
	
	printf("read Slot %u(0x%08X) REG ADDR=0x%x \n\r",slot,MDIO_ADDR,regaddr);
 	REG32(MDIO_ADDR)= ( (regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET); 
	delay_msec(1);
	read_data=REG32(MDIO_ADDR);
	printf("HostPCIe PHY addr= 0x%02x, data= 0x%04x    \n\r",(read_data>>8)&0xFF,read_data>>16 );
}

void PCIe_Reset(void)
{
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	/* 1. PCIE phy mdio reset */
	REG32(0xb8000504) = 0x00900000;
	REG32(0xb8000504) = 0x01b00000;

	// 2. PCIE MAC reset
	REG32(0xb8000600) &= ~(1<<7);
	REG32(0xb8000600) |= (1<<7);

	// 5.MDIO Reset
	REG32(0xb8000504) = 0x01b00000;

	// 6. PCIE PHY Reset
	REG32(0xb8b21008) = 0x1; //bit7 PHY reset=0   bit0 Enable LTSSM=1
	REG32(0xb8b21008) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1
	delay_msec(100);
	
#else
	REG32(0xb8000300)&= ~(1<<2); //GPIO PIN MUX Control Register [n2]=JTAG_GPIO 1: JTAG pins
	REG32(0xb800030c)|= (1<<9); 				//enable PCIe module
	printf("PCIe module enable....\n\r");
	delay_msec(100);
	REG32(0xb8000400)&= ~(1<<20);				//Reset PCIe MDIO
	delay_msec(1);
	REG32(0xb8000400)|= (1<<20);				//Reset PCIe MDIO		
	printf("PCIe MDIO reset....\n\r");
	REG32(0xb8003528) |=  (1<<26);			//PCIe Hardware resest define, and set low 100m sec, H2
	delay_msec(100);
	REG32(0xb8003524) |= (1<<26);
	delay_msec(100);
	REG32(0xb8003528) &= ~(1<<26);
	delay_msec(100);
	REG32(0xb8003528) |=  (1<<26);			//PCIe Hardware resest define, and set low 100m sec
	delay_msec(100);
	printf("PCIe Hardware reset output ( GPIO H2 )....\n\r");
#endif
}

void Pcie_LBK(void)
{
													//1. PCIE phy mdio reset
	REG32(0xB8000400)=0x1D400000;delay_msec(1);		//ew	@$SYS_PCIE_PHY0	=	0x1D400000
	REG32(0xB8000400)=0x1D500000;delay_msec(1);		//ew	@$SYS_PCIE_PHY0	=	0x1D500000

													//2. PCIE MAC reset
    	REG32(0xB800030C)&=~(1 << 9);delay_msec(1);		//ew	@$Module_Enable 	=	@@$Module_Enable	&	~(1 << 9)
	REG32(0xB800030C)|=(1 << 9);delay_msec(1);		//ew	@$Module_Enable		=	@@$Module_Enable	|	(1 << 9)

	PCIe_Reset();									//3. PCIe hardware reset
	REG32(0xB8000400)=0x1D500000;delay_msec(1);		//4. MDIO Reset
	
	REG32(0xB8B01008)= 0x1;delay_msec(1);			//5. PCIE PHY Reset
	REG32(0xB8B01008)= 0x81;delay_msec(1);

	HostPCIe_SetPhyMdioWrite(0, 0x1046,0);
	HostPCIe_SetPhyMdioWrite(1, 0x0003,0);				
	HostPCIe_SetPhyMdioWrite(2, 0x2d18,0);
	HostPCIe_SetPhyMdioWrite(3, 0x6D09,0);
	HostPCIe_SetPhyMdioWrite(4, 0x5c3f,0);				
	HostPCIe_SetPhyMdioWrite(6, 0x9048,0);
	HostPCIe_SetPhyMdioWrite(5, 0x2213,0);
	HostPCIe_SetPhyMdioWrite(7, 0x31ff,0);
	HostPCIe_SetPhyMdioWrite(8, 0x18d7,0);
	HostPCIe_SetPhyMdioWrite(9, 0x539C,0);				
	HostPCIe_SetPhyMdioWrite(0xa, 0x00e8,0);
	HostPCIe_SetPhyMdioWrite(0xb, 0x0711,0);
	HostPCIe_SetPhyMdioWrite(0xc, 0x0828,0);
	HostPCIe_SetPhyMdioWrite(0xd, 0x17A6,0);
	HostPCIe_SetPhyMdioWrite(0xe, 0x98c5,0);
	HostPCIe_SetPhyMdioWrite(0xf, 0x0f0f,0);
	HostPCIe_SetPhyMdioWrite(0x10, 0x000C,0);
	HostPCIe_SetPhyMdioWrite(0x11, 0x3C00,0);
	HostPCIe_SetPhyMdioWrite(0x12, 0xFC00,0);
	HostPCIe_SetPhyMdioWrite(0x13, 0x0C81,0);
	HostPCIe_SetPhyMdioWrite(0x14, 0xDE01,0);
	HostPCIe_SetPhyMdioWrite(0x19, 0xFCE0,0);
	HostPCIe_SetPhyMdioWrite(0x1a, 0x7C00,0);
	HostPCIe_SetPhyMdioWrite(0x1b, 0xFC00,0);
	HostPCIe_SetPhyMdioWrite(0x1c, 0xFC00,0);
	HostPCIe_SetPhyMdioWrite(0x1d, 0xA0EB,0);
	HostPCIe_SetPhyMdioWrite(0x1e, 0xC280,0);
	HostPCIe_SetPhyMdioWrite(0x1f, 0x0600,0);

	// 7. PCIE Device Reset
	//PCIe_Reset();
	//8. Set BAR
	REG32(0xb8b10010)=0x18c00001;delay_msec(1);
	REG32(0xb8b10018)=0x19000004;delay_msec(1);
	REG32(0xb8b10004)=0x00180007;delay_msec(1);
	REG32(0xb8b00004)=0x00100007;delay_msec(1);
	//9. PCIE trigger BIST.......
	REG32(0xb8b01000)=0xDE011401;delay_msec(1);					//0x14=0xDE01
	REG32(0xb8b01000)=0x84971301;delay_msec(1);					//0x13=0x8497	
	REG32(0xb8b01000)=0x8C971301;delay_msec(1);					//0x13=0x8C97	
	REG32(0xb8b01000)=0xCC971301;delay_msec(1);					//0x13=0xCC97	
	
	//10.Read data from PCIE ePHY REG.......
	REG32(0xb8b01000)=0x1700;delay_msec(1);
	printf("received packets count=0x%x \n\r",REG32(0xb8b01000)&0xFF);
	REG32(0xb8b01000)=0x1800;delay_msec(1);
	printf("received error count=0x%x \n\r",(REG32(0xb8b01000)>>4)&0xFF);
}

void Pcie_D_LBK(void)
{
	//ew 0xb8b01000=0x84971301
	REG32(0xb8b01000)=0x84971301;delay_msec(1);
	//ew 0xb8b01000=0x040C1001
	REG32(0xb8b01000)=0x040C1001;delay_msec(1);
	//ew 0xb8b01000=0x34001101
	REG32(0xb8b01000)=0x34001101;delay_msec(1);
	//ew 0xb8b01000=0x8C971301		//0x8CB71301, PN 
	REG32(0xb8b01000)=0x8C971301;delay_msec(1);
	//ew 0xb8b01000=0xDE011401
	REG32(0xb8b01000)=0xDE011401;delay_msec(1);
	//ew 0xb8b01000=0xCC971301
	REG32(0xb8b01000)=0xCC971301;delay_msec(1);
	//ew 0xb8b01000=0x00001300;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001300;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));
	//ew 0xb8b01000=0x00001700;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001700;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));
	//ew 0xb8b01000=0x00001800;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001800;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));
	//ew 0xb8b01000=0x00001400;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001400;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));
	//ew 0xb8b01000=0x00000600;dw 0xb8b01000
	REG32(0xb8b01000)=0x00000600;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));	
	//ew 0xb8b01000=0x00001000;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001000;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));	
	//ew 0xb8b01000=0x00001100;dw 0xb8b01000
	REG32(0xb8b01000)=0x00001100;delay_msec(1);
	printf("REG32(0xb8b01000)=0x%x \n\r",REG32(0xb8b01000));	
}

static int pcie_reset_done[2] = {0};

struct pcie_para ePHY[][29] = {
     {	{0, 1, 0x0003}, 	{0, 2, 0x2d18},	{0, 3, 0x4d09},	{0, 4, 0x5c3f},   
	{0, 0, 0x1046},   	{0, 6, 0x9048},	{0, 5, 0x2213},	{0, 7, 0x31ff},   
	{0, 8, 0x18d7},   	{0, 9, 0x539c},  	{0, 0xa, 0x00e8}, 	{0, 0xb, 0x0711}, 
	{0, 0xc, 0x0828}, 	{0, 0xd, 0x17a6},	{0, 0xe, 0x98c5},	{0, 0xf, 0x0f0f}, 
	{0, 0x10, 0x000c},	{0, 0x11, 0x3c00},	{0, 0x12, 0xfc00},	{0, 0x13, 0x0c81},
	{0, 0x14, 0xde01},	{0, 0x19, 0xfce0},	{0, 0x1a, 0x7c00},	{0, 0x1b, 0xfc00},
	{0, 0x1c, 0xfc00},	{0, 0x1d, 0xa0eb},	{0, 0x1e, 0xc280},	{0, 0x1f, 0x0600},
	{0xff,0xff,0xffff}}, //8676 35.328M clk
     {	{0, 1, 0x0003}, 	{0, 2, 0x2d18},	{0, 3, 0x4d09},	{0, 4, 0x5000},   
	{0, 0, 0x1047},   	{0, 6, 0x9148},	{0, 5, 0x23cb},	{0, 7, 0x31ff},   
	{0, 8, 0x18d7},   	{0, 9, 0x539c},  	{0, 0xa, 0x00e8}, 	{0, 0xb, 0x0711}, 
	{0, 0xc, 0x0828}, 	{0, 0xd, 0x17a6},	{0, 0xe, 0x98c5},	{0, 0xf, 0x0f0f}, 
	{0, 0x10, 0x000c},	{0, 0x11, 0x3c00},	{0, 0x12, 0xfc00},	{0, 0x13, 0x0c81},
	{0, 0x14, 0xde01},	{0, 0x19, 0xfce0},	{0, 0x1a, 0x7c00},	{0, 0x1b, 0xfc00},
	{0, 0x1c, 0xfc00},	{0, 0x1d, 0xa0eb},	{0, 0x1e, 0xc280},	{0, 0x1f, 0x0600},
	{0xff,0xff,0xffff}}, //8676 40M clk
     {	{0, 0, 0x404c},	{0, 1, 0x16a3},	{0, 2, 0x6340},	{0, 3, 0x370d},	
	{0, 4, 0x856a},	{0, 5, 0x8109},	{0, 6, 0x6081},	{0, 7, 0x5400},
	{0, 8, 0x9000},	{0, 9, 0x0ccc},	{0, 0xa, 0x4437},	{0, 0xb, 0x0230}, 	
	{0, 0xc, 0x0021},	{0, 0xd, 0x0000},	{0, 0xe, 0x0000},	{0, 0x1f, 0x0000}, 
	{0xff,0xff,0xffff}}, //8685 25M clk
     {	{1, 0, 0x404c},	{1, 1, 0x16a3},	{1, 2, 0x6340},	{1, 3, 0x370d},	
	{1, 4, 0x856a},	{1, 5, 0x8109},	{1, 6, 0x6081},	{1, 7, 0x5400},
	{1, 8, 0x9000},	{1, 9, 0x0ccc},	{1, 0xa, 0x4437},	{1, 0xb, 0x0230}, 	
	{1, 0xc, 0x0021},	{1, 0xd, 0x0000},	{1, 0xe, 0x0000},	{1, 0x1f, 0x0000}, 
	{0xff,0xff,0xffff}}, //8685 25M clk
      {{0, 0, 0x404c},  {0, 1, 0x16a3}, {0, 2, 0x6340}, {0, 3, 0x370d},
        {0, 4, 0x4644}, {0, 5, 0x8109}, {0, 6, 0x2081}, {0, 7, 0x5400},
        {0, 8, 0x9000}, {0, 9, 0x0ccc}, {0, 0xa, 0x4437},       {0, 0xb, 0x0230},
        {0, 0xc, 0x0021},       {0, 0xd, 0x0000},       {0, 0xe, 0x0000},       {0, 0x1f, 0x0000},
        {0xff,0xff,0xffff}} //8685S 25M clk
};

/*	
  * 	Function 		: PCIE_reset_procedure
  *	Input	      		: portnum, Use_External_PCIE_CLK, mdio_reset, conf_addr
  *	Description 	: Merge from linux kernel, reset procedure of PCIE interface
  *
  */
int PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, 
							int mdio_reset, unsigned long conf_addr)
{
	int PCIE_gpio_RST, i, idx;
	unsigned int PCIE_D_CFG0, PCIE_H_CFG, PCIE_H_PWRCR;
	unsigned int ENABLE_PCIE = (1<<8);

	if (portnum==0) {
		PCIE_D_CFG0 = BSP_PCIE0_D_CFG0;
		PCIE_H_CFG = BSP_PCIE0_H_CFG;
		PCIE_H_PWRCR = BSP_PCIE0_H_PWRCR;
		PCIE_gpio_RST = PCIE0_gpio_RST;
	}
#ifdef CONFIG_USE_PCIE_SLOT_1
	else if(portnum==1) {
		PCIE_D_CFG0 = BSP_PCIE1_D_CFG0;
		PCIE_H_CFG = BSP_PCIE1_H_CFG;
		PCIE_H_PWRCR = BSP_PCIE1_H_PWRCR;
		ENABLE_PCIE = (1<<7);
		PCIE_gpio_RST = PCIE1_gpio_RST; 
	}
#endif
	else {
		printf("Error: portnum=%d\n", portnum);
		return FAIL;
	}

	if (pcie_reset_done[portnum]) 
		goto SET_BAR;

	// 0. Assert PCIE Device Reset
	gpioClear(PCIE_gpio_RST);
	gpioConfig(PCIE_gpio_RST, GPIO_FUNC_OUTPUT);
	delay_msec(10);

	// 1. PCIE phy mdio reset
#if defined(CONFIG_USE_PCIE_SLOT_0) && defined(CONFIG_USE_PCIE_SLOT_1)
	if(pcie_reset_done[0]==0 && pcie_reset_done[1]==0){
#endif
	
	#ifndef CONFIG_RTL8676S
	REG32(PCI_MISC) = BSP_PCI_MDIO_RESET_ASSERT;
	REG32(PCI_MISC) = BSP_PCI_MDIO_RESET_RELEASE;
	#endif
#if defined(CONFIG_USE_PCIE_SLOT_0) && defined(CONFIG_USE_PCIE_SLOT_1)
	}
#endif


	// 2. PCIE MAC reset
	REG32(MISC_IP_SEL) &= ~ENABLE_PCIE;
	REG32(MISC_IP_SEL) |= ENABLE_PCIE;

#if defined(CONFIG_USE_PCIE_SLOT_0) && defined(CONFIG_USE_PCIE_SLOT_1)
	if(pcie_reset_done[0]==0 && pcie_reset_done[1]==0){
#endif
	if(mdio_reset)
	{
		//printk("Do MDIO_RESET\n");
		// 5.MDIO Reset
		#ifndef CONFIG_RTL8676S
		REG32(PCI_MISC) = BSP_PCI_MDIO_RESET_RELEASE;
		#endif
	}
#if defined(CONFIG_USE_PCIE_SLOT_0) && defined(CONFIG_USE_PCIE_SLOT_1)
	}
#endif
	
	// 6. PCIE PHY Reset
	REG32(PCIE_H_PWRCR) = 0x1; //bit7 PHY reset=0   bit0 Enable LTSSM=1
	REG32(PCIE_H_PWRCR) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1
	delay_msec(100);

	//----------------------------------------
	if (mdio_reset)
	{
		if (IS_RTL8676 && !(REG32(MISC_PINSR) & CLKSEL))
			idx = CLK35_328_8676;
		else if (IS_RTL8676 && (REG32(MISC_PINSR) & CLKSEL))
			idx = CLK40_8676;
		else if (IS_RTL8685){
			if (portnum==0){
				printf("8685 pcie port 0\n\r");
				idx = CLK25_8685_P0;
			}
			else{
				printf("8685 pcie port 1\n");
				idx = CLK25_8685_P1;				
			}
		}
                else if(IS_RTL8685S){
                        printf("8685s pcie port 0\n\r");
                        idx = CLK25_8685S_P0;
                }
		else
			idx = NOT_DEFINED_CLK;

		for (i = 0; NOT_DEFINED_CLK != idx; ) {
			if(ePHY[idx][i].port != 0xff){
				if(portnum == ePHY[idx][i].port){
					HostPCIe_SetPhyMdioWrite( ePHY[idx][i].reg, ePHY[idx][i].value, ePHY[idx][i].port);
				}

				i++;
			}
			else{
				break;
			}
		}
	}

	// 7. PCIE Device Reset
	gpioSet(PCIE_gpio_RST);

	// wait for LinkUP
	i = 100;
	while(--i)
	{
		if((REG32(PCIE_H_CFG + 0x0728)&0x1f)==0x11)
			break;
		delay_msec(10);
	}

	if (i == 0)
	{
		printf("Warning!! Port %d WLan device PCIE Link Failed, State=0x%x\n", portnum, REG32(PCIE_H_CFG + 0x0728));
		printf("Reset PCIE Host PHY and try again...\n");
	    	// 4. PCIE PHY Reset
	    	REG32(PCIE_H_PWRCR) = 0x1; //bit7 PHY reset=0   bit0 Enable LTSSM=1
	    	REG32(PCIE_H_PWRCR) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1	
    
		// wait for LinkUP
		i = 100;
		while(--i)
		{
			if( (REG32(PCIE_H_CFG + 0x0728)&0x1f)==0x11)
			    break;
			delay_msec(100);
		}
		
		if (i == 0){
			printf("%s[%d]: Error!! Port %d WLan device PCIE Link failed, State=0x%x\n\r", __FUNCTION__, __LINE__, portnum, REG32(PCIE_H_CFG + 0x0728));
			//check pcie link fail~
			return FAIL;
		}
	}
	delay_msec(100);

	// 8. Set BAR
	REG32(PCIE_D_CFG0 + 0x10) = 0x18c00001;
	REG32(PCIE_D_CFG0 + 0x18) = 0x19000004;
	REG32(PCIE_D_CFG0 + 0x04) = 0x00180007;
	REG32(PCIE_H_CFG + 0x04) = 0x00100007;

	printf("Find Port_num=%d, Vender_Device_ID=0x%X\n\r", portnum, REG32(PCIE_D_CFG0 + 0x00) );

	if(i==0)
	{
		printf("Cannot LinkUP (0x%08X)\n\r", REG32(PCIE_H_CFG + 0x0728));
		return FAIL;
	}

SET_BAR:
	// Enable PCIE host
	if (pcie_reset_done[portnum] == 0) {
		WRITE_MEM32(PCIE_H_CFG + 0x04, 0x00100007);
		WRITE_MEM8(PCIE_H_CFG + 0x78, (READ_MEM8(conf_addr + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B
		pcie_reset_done[portnum] = 1;
	}
	return SUCCESS;
}

