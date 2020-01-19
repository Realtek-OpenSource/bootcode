#ifndef _PCIE_HOST_H_
#define _PCIE_HOST_H_

#define PCIE0_MDIO 0xb8b01000
#define PCIE1_MDIO 0xb8b21000
#define PCIE_MDIO_REG_OFFSET (8)
#define PCIE_MDIO_DATA_OFFSET (16)
#define PCIE_MDIO_RDWR_OFFSET (0)

/* Depends on platforms */
#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
	#define BSP_PCIE0_D_CFG0 	0xB8B10000
	#define BSP_PCIE0_H_CFG	0xB8B00000
	#define BSP_PCIE0_H_PWRCR	0xB8B01008
	#define PCIE0_gpio_RST	11
	#define BSP_PCI_MISC		0xB8000400
	#define PCI_MISC			BSP_PCI_MISC
		#define BSP_PCI_MDIO_RESET_ASSERT 0x1d400000
		#define BSP_PCI_MDIO_RESET_RELEASE 0x1d500000
	#define BSP_MISC_PINSR	0xB8000308
		#define BSP_CLKSEL                              (1 << 23)
#elif defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	#define MISC_IP_SEL		0xB8000600
	#define BSP_PCIE0_D_CFG0 	0xB8B10000
	#define BSP_PCIE0_H_CFG	0xB8B00000
	#define BSP_PCIE0_H_PWRCR	0xB8B01008
	#if defined(CONFIG_RTL8685)
		#define PCIE0_gpio_RST	28	/* GPIO D4  for RTL8685 */
	#else
		#define PCIE0_gpio_RST	26	/* GPIO D2  for RTL8685S */
	#endif
	#define BSP_PCIE1_D_CFG0 	0xB8B30000
	#define BSP_PCIE1_H_CFG	0xB8B20000
	#define BSP_PCIE1_H_PWRCR	0xB8B20008
	#define PCIE1_gpio_RST	36	/* GPIO E4  for RTL8685, RTL8685S*/
	#define BSP_PCI_MISC 		0xb8000504
		#define BSP_PCI_MDIO_RESET_ASSERT 0x00900000
		#define BSP_PCI_MDIO_RESET_RELEASE 0x01b00000
	#define  BSP_SYS_CLK_SEL	0xb8000044
	#define BSP_MISC_PINSR 	BSP_SYS_CLK_SEL
	#define BSP_SYS_CLK_SRC_40MHZ 0  /*1:40MHz, 0:25MHz*/
	 #define BSP_CLKSEL             BSP_SYS_CLK_SRC_40MHZ
#endif

#define PCI_MISC			BSP_PCI_MISC
#define MISC_PINSR		BSP_MISC_PINSR
#define CLKSEL			BSP_CLKSEL

/* WiFi driver definitions */
#define MAX_PAYLOAD_SIZE_128B    0x00

/* PCIE phy parameters */
struct pcie_para{
	unsigned int port;
	unsigned short reg;	
	unsigned int value;
};

#define CLK35_328_8676	0
#define CLK40_8676		1
#define CLK25_8685_P0	2
#define CLK25_8685_P1	3
#define CLK25_8685S_P0	4
#define NOT_DEFINED_CLK	0xF

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAIL
#define FAIL -1
#endif

/* Functions */
extern void HostPCIe_SetPhyMdioWrite(unsigned int regaddr, unsigned short val, unsigned int slot);
extern void HostPCIe_ReadPhyMdioWrite(unsigned int regaddr, unsigned int slot);
extern void PCIe_Reset(void);
extern void Pcie_LBK(void);
extern void Pcie_D_LBK(void);
extern int PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, int mdio_reset, unsigned long conf_addr);

#endif
