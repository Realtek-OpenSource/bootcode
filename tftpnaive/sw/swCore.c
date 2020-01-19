/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core driver source code.
*
* $Author: kehsieh $
*
* ---------------------------------------------------------------
*/
#include "board.h"
#include <rtl_types.h>
#include <rtl_errno.h>
#include <rtl8650/loader.h>  //wei edit
#include <rtl8650/asicregs.h>
#include <rtl8650/swCore.h>
#include <rtl8650/phy.h>

//#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
//#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
//#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

extern char eth0_mac[6];

#if CONFIG_RTL865XC
static uint8 fidHashTable[]={0x00,0x0f,0xf0,0xff};

/*#define rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + 0x10000 * (type)) */
#define		RTL8651_ASICTABLE_BASE_OF_ALL_TABLES		0xBB000000
#define		rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + ((type)<<16) )
#define 		RTL865X_FAST_ASIC_ACCESS
#define		RTL865XC_ASIC_WRITE_PROTECTION				/* Enable/Disable ASIC write protection */
#define		RTL8651_ASICTABLE_ENTRY_LENGTH (8 * sizeof(uint32))
#define		RTL865X_TLU_BUG_FIXED		1


#ifdef RTL865X_FAST_ASIC_ACCESS
static uint32 _rtl8651_asicTableSize[] =
{
	2 /*TYPE_L2_SWITCH_TABLE*/,
	1 /*TYPE_ARP_TABLE*/,
    2 /*TYPE_L3_ROUTING_TABLE*/,
	3 /*TYPE_MULTICAST_TABLE*/,
	1 /*TYPE_PROTOCOL_TRAP_TABLE*/,
	5 /*TYPE_VLAN_TABLE*/,
	3 /*TYPE_EXT_INT_IP_TABLE*/,
    1 /*TYPE_ALG_TABLE*/,
    4 /*TYPE_SERVER_PORT_TABLE*/,
    3 /*TYPE_L4_TCP_UDP_TABLE*/,
    3 /*TYPE_L4_ICMP_TABLE*/,
    1 /*TYPE_PPPOE_TABLE*/,
    8 /*TYPE_ACL_RULE_TABLE*/,
    1 /*TYPE_NEXT_HOP_TABLE*/,
    3 /*TYPE_RATE_LIMIT_TABLE*/,
};
#endif

static void _rtl8651_asicTableAccessForward(uint32 tableType, uint32 eidx, void *entryContent_P) {
	ASSERT_CSP(entryContent_P);


	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

#ifdef RTL865X_FAST_ASIC_ACCESS

	{
		register uint32 index;

		for( index = 0; index < _rtl8651_asicTableSize[tableType]; index++ )
		{
			WRITE_MEM32(TCR0+(index<<2), *((uint32 *)entryContent_P + index));
		}

	}
#else
	WRITE_MEM32(TCR0, *((uint32 *)entryContent_P + 0));
	WRITE_MEM32(TCR1, *((uint32 *)entryContent_P + 1));
	WRITE_MEM32(TCR2, *((uint32 *)entryContent_P + 2));
	WRITE_MEM32(TCR3, *((uint32 *)entryContent_P + 3));
	WRITE_MEM32(TCR4, *((uint32 *)entryContent_P + 4));
	WRITE_MEM32(TCR5, *((uint32 *)entryContent_P + 5));
	WRITE_MEM32(TCR6, *((uint32 *)entryContent_P + 6));
	WRITE_MEM32(TCR7, *((uint32 *)entryContent_P + 7));
#endif	
	WRITE_MEM32(SWTAA, ((uint32) rtl8651_asicTableAccessAddrBase(tableType) + eidx * RTL8651_ASICTABLE_ENTRY_LENGTH));//Fill address
}

static int32 _rtl8651_forceAddAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P) {

	#ifdef RTL865XC_ASIC_WRITE_PROTECTION
	if (RTL865X_TLU_BUG_FIXED)	/* No need to stop HW table lookup process */
	{	/* No need to stop HW table lookup process */
		WRITE_MEM32(SWTCR0,EN_STOP_TLU|READ_MEM32(SWTCR0));
		while ( (READ_MEM32(SWTCR0) & STOP_TLU_READY)==0);
	}
	#endif

	_rtl8651_asicTableAccessForward(tableType, eidx, entryContent_P);

 	WRITE_MEM32(SWTACR, ACTION_START | CMD_FORCE);//Activate add command
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

	#ifdef RTL865XC_ASIC_WRITE_PROTECTION
	if (RTL865X_TLU_BUG_FIXED)	/* No need to stop HW table lookup process */
	{
		WRITE_MEM32(SWTCR0,~EN_STOP_TLU&READ_MEM32(SWTCR0));
	}
	#endif

	return SUCCESS;
}

uint32 rtl8651_filterDbIndex(ether_addr_t * macAddr,uint16 fid) {
    return ( macAddr->octet[0] ^ macAddr->octet[1] ^
                    macAddr->octet[2] ^ macAddr->octet[3] ^
                    macAddr->octet[4] ^ macAddr->octet[5] ^fidHashTable[fid]) & 0xFF;
}

static int32 rtl8651_setAsicL2Table(ether_addr_t	*mac, uint32 column)
{
	rtl865xc_tblAsic_l2Table_t entry;
	uint32	row;

	row = rtl8651_filterDbIndex(mac, 0);
	if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN))
		return FAILED;
	if(mac->octet[5] != ((row^(fidHashTable[0])^ mac->octet[0] ^ mac->octet[1] ^ mac->octet[2] ^ mac->octet[3] ^ mac->octet[4] ) & 0xff))
		return FAILED;

	memset(&entry, 0,sizeof(entry));
	entry.mac47_40 = mac->octet[0];
	entry.mac39_24 = (mac->octet[1] << 8) | mac->octet[2];
	entry.mac23_8 = (mac->octet[3] << 8) | mac->octet[4];

//	entry.extMemberPort = 0;   
	entry.memberPort = 7;
	entry.toCPU = 1;
	entry.isStatic = 1;
//	entry.nxtHostFlag = 1;

	/* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec. */
	entry.agingTime = 0x03;
	
//	entry.srcBlock = 0;
	entry.fid=0;
	entry.auth=1;

	return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
}
#endif

//------------------------------------------------------------------------
static void _rtl8651_clearSpecifiedAsicTable(uint32 type, uint32 count) 
{
	extern int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
	struct { uint32 _content[8]; } entry;
	uint32 idx;
	
	bzero(&entry, sizeof(entry));
	for (idx=0; idx<count; idx++)// Write into hardware
		swTable_addEntry(type, idx, &entry);
}

void FullAndSemiReset( void )
{
	/* Perform full-reset for sw-core. */ 
	REG32(SIRR) |= FULL_RST;
	delay_msec(50);

	/* Enable TRXRDY */
	REG32(SIRR) |= TRXRDY;
}

int32 rtl865xC_setAsicEthernetMIIMode(uint32 port, uint32 mode)
{
	if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
		return FAILED;
	if ( mode != LINK_RGMII && mode != LINK_MII_MAC && mode != LINK_MII_PHY )
		return FAILED;

	if ( port == 0 )
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P0GMIICR, ( READ_MEM32( P0GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	else
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P5GMIICR, ( READ_MEM32( P5GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	return SUCCESS;

}

int32 rtl865xC_setAsicEthernetRGMIITiming(uint32 port, uint32 Tcomp, uint32 Rcomp)
{
	if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
		return FAILED;
	if ( Tcomp < RGMII_TCOMP_0NS || Tcomp > RGMII_TCOMP_2NS || Rcomp < RGMII_RCOMP_0NS || Rcomp > RGMII_RCOMP_3DOT5NS )
		return FAILED;
	
	if ( port == 0 )
	{
		WRITE_MEM32(P0GMIICR, ( ( ( READ_MEM32(P0GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
	}
	else
	{
		WRITE_MEM32(P5GMIICR, ( ( ( READ_MEM32(P5GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
	}

	WRITE_MEM32(P0GMIICR, READ_MEM32(P0GMIICR)|Conf_done);

	return SUCCESS;
}


int32 rtl8651_getAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 *rData)
{
	uint32 status;

	WRITE_MEM32( MDCIOCR, COMMAND_READ | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) );

#ifdef RTL865X_TEST
	status = READ_MEM32( MDCIOSR );
#else
#if defined(CONFIG_RTL8196C)
	delay_msec(10);   //wei add, for 8196C_test chip patch. mdio data read will delay 1 mdc clock.
#endif
	do { status = READ_MEM32( MDCIOSR ); } while ( ( status & STATUS ) != 0 );
#endif

	status &= 0xffff;
	*rData = status;

	return SUCCESS;
}

int32 rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData)
{
	WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) | wData );

#ifdef RTL865X_TEST
#else
	while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );		/* wait until command complete */
#endif

	return SUCCESS;
}

int32 rtl8651_restartAsicEthernetPHYNway(uint32 port, uint32 phyid)
{
	uint32 statCtrlReg0;

	/* read current PHY reg 0 */
	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	/* enable 'restart Nway' bit */
	statCtrlReg0 |= RESTART_AUTONEGO;

	/* write PHY reg 0 */
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

	return SUCCESS;
}

//====================================================================
#ifdef CONFIG_EXTS_RTL8367B
int RTL8367B_probe(unsigned int phyid)
{
	extern int RL6000_write(unsigned int addr, unsigned int data);
	extern int RL6000_read(unsigned int addr, unsigned int *data);
	extern void RL6000_RGMII(void);
	extern void RL6000_cpu_tag(int enable);

	unsigned int id = 0;

	//set port0 RGMII mode
	rtl865xC_setAsicEthernetMIIMode(0, LINK_RGMII);
	//set Port 0 Interface Type Configuration to GMII/MII/RGMII interface 
	WRITE_MEM32(PITCR, (READ_MEM32(PITCR)&0xfffffffc)|Port0_TypeCfg_GMII_MII_RGMII);
	//set external phyid
	WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ExtPHYID_MASK))|(phyid << ExtPHYID_OFFSET));

	//chip reset, must wait more than 0.9 sec
	mdelay(10);
	RL6000_write(0x1322, 0x1);
	mdelay(1000);	

	RL6000_write(0x13C2, 0x0249);
	RL6000_read(0x1300, &id);

	if (!id || (0xffff == (id & 0xffff)))
		goto probe_fail;

	rtl865xC_setAsicEthernetRGMIITiming(0, RGMII_TCOMP_2NS, RGMII_RCOMP_2NS);
	WRITE_MEM32(MACCR,(READ_MEM32(MACCR)&0xffffcfff)|0x01<<12);//select 100Mhz system clk
	
	//disable auto-polling
	WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&~PollLinkStatus));
	WRITE_MEM32(PCRP1, (READ_MEM32(PCRP1)&~PollLinkStatus));
	WRITE_MEM32(PCRP2, (READ_MEM32(PCRP2)&~PollLinkStatus));
	WRITE_MEM32(PCRP3, (READ_MEM32(PCRP3)&~PollLinkStatus));
	WRITE_MEM32(PCRP4, (READ_MEM32(PCRP4)&~PollLinkStatus));

	//set port0 force mode
	WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ForceSpeedMask)&(~ExtPHYID_MASK))|ForceSpeed1000M|EnForceMode);
	mdelay(10);

	//port0 force link
	WRITE_MEM32(PCRP0, READ_MEM32(PCRP0)|ForceLink);

	rtk_switch_init();
	RL6000_write(0x1b03, 0x0222);	//LED setting, LED0/LED1/LED2:Link/Act
	RL6000_RGMII();
	RL6000_cpu_tag(0);

	printf("ExtSwitch is detected! \n\r");
	return SUCCESS;

probe_fail:
	FullAndSemiReset();
	rtl8651_setAsicEthernetPHYReg(0, 31, 0);	//select back to page 0
	return FAILED;
}
#endif /* CONFIG_EXTS_RTL8367B */

#ifdef CONFIG_EXTS_RTL8211E
int RTL8211E_probe(unsigned int phyid)
{
	unsigned int uid, tmp;

	//set port0 RGMII mode
	rtl865xC_setAsicEthernetMIIMode(0, LINK_RGMII);
	//set Port 0 Interface Type Configuration to GMII/MII/RGMII interface 
	WRITE_MEM32(PITCR, (READ_MEM32(PITCR)&0xfffffffc)|Port0_TypeCfg_GMII_MII_RGMII);
	//set external phyid
	WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ExtPHYID_MASK))|(phyid << ExtPHYID_OFFSET));
	
	rtl8651_getAsicEthernetPHYReg(phyid, 2, &tmp);
	uid = tmp << 16;
	rtl8651_getAsicEthernetPHYReg(phyid, 3, &tmp);
	uid = uid | tmp;

	if (uid != 0x001cc915)
		goto probe_fail;
	
	/* LED setting */
	//select page 7
	rtl8651_setAsicEthernetPHYReg(phyid, 31, 7);
	//extension page 44
	rtl8651_setAsicEthernetPHYReg(phyid, 30, 0x2c);
	//Register 28 bit[2:0] = 111 (set LED 0 On when 10/100/1000Link)
	rtl8651_getAsicEthernetPHYReg(phyid, 28, &tmp);
	tmp |= 0x7;
	rtl8651_setAsicEthernetPHYReg(phyid, 28, tmp);
	//Register 26 bit[4] = 1 (set LED 0 blinking when Activity)
	rtl8651_getAsicEthernetPHYReg(phyid, 26, &tmp);
	tmp |= 0x10;
	rtl8651_setAsicEthernetPHYReg(phyid, 26, tmp);
	//select page 0
	rtl8651_setAsicEthernetPHYReg(phyid, 31, 0);

	rtl865xC_setAsicEthernetRGMIITiming(0, RGMII_TCOMP_2NS, RGMII_RCOMP_1NS);
	WRITE_MEM32(MACCR,(READ_MEM32(MACCR)&0xffffcfff)|0x01<<12);//select 100Mhz system clk

	printf("RTL8211E is detected! \n\r");
	return SUCCESS;

probe_fail:
	FullAndSemiReset();
	return FAILED;
}
#endif

#ifdef CONFIG_EXTS_RTL8214
typedef struct RTL8214C_confcode_prv_t{
        uint32 phy_id;
	uint32 reg_id;
	uint32 data;	       	
}RTL8214C_confcode_prv_s;

RTL8214C_confcode_prv_s rtl8214C_serdes_perchip[] = {\
	{0, 0x1d, 0x1d11}, {0, 0x1e, 0x1506},  {0, 0x1d, 0x6602},  {0, 0x1e, 0x84d7}, \
	{0, 0x1d, 0x6601}, {0, 0x1e, 0x0540}, {0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, \
	{0, 0x1d, 0x6602}, {0, 0x1e, 0xf994}, {0, 0x1d, 0x6601}, {0, 0x1e, 0x0541}, \
	{0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, {0, 0x1d, 0x6602}, {0, 0x1e, 0x2da3}, \
	{0, 0x1d, 0x6601}, {0, 0x1e, 0x0542}, {0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, \
	{0, 0x1d, 0x6602}, {0, 0x1e, 0x3960}, {0, 0x1d, 0x6601}, {0, 0x1e, 0x0543}, \
	{0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, {0, 0x1d, 0x6602}, {0, 0x1e, 0x9728}, \
	{0, 0x1d, 0x6601}, {0, 0x1e, 0x0544}, {0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, \
	{0, 0x1d, 0x6602}, {0, 0x1e, 0xf83f}, {0, 0x1d, 0x6601}, {0, 0x1e, 0x0545}, \
	{0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, \
	{0, 0x1d, 0x6602}, {0, 0x1e, 0x9d85}, {0, 0x1d, 0x6601}, {0, 0x1e, 0x0423}, \
	{0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, {0, 0x1d, 0x6602}, {0, 0x1e, 0xd810}, \
	{0, 0x1d, 0x6601}, {0, 0x1e, 0x0424}, {0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0}, \
	{0, 0x1d, 0x6602}, {0, 0x1e, 0x8334}, {0, 0x1d, 0x6601}, {0, 0x1e, 0x002e}, \
	{0, 0x1d, 0x6600}, {0, 0x1e, 0x00c0} \
};

#define IS_RTL8214B		0xC941
#define IS_RTL8214C		0xCA60
#define IS_RTL8214C_MP	0xC942

uint32 Serdes_Version(void)
{
	uint32 ModelNumber = 0;
	uint32 SwitchPCRP=PCRP5, PHY_value=0;

	PHY_value = (MacSwReset |STP_PortST_FORWARDING |\ 
				PauseFlowControlEtxErx | ForceDuplex | ForceSpeed1000M |ForceLink );

	WRITE_MEM32(SwitchPCRP, (PHY_value & (~ExtPHYID_MASK) | (5 << ExtPHYID_OFFSET)));
	rtl8651_getAsicEthernetPHYReg(0x5,0x3,&ModelNumber);
	
	if(ModelNumber==IS_RTL8214B)
		printf("VerB...\n");
	else if((ModelNumber==IS_RTL8214C) || (ModelNumber==IS_RTL8214C_MP))
		printf("VerC...\n");
	else	
		printf("Unknown version = 0x%X\n",ModelNumber);

	return ModelNumber;
}

void Serdes_Enable(void)
{
	uint32 version=0;

	version = Serdes_Version();

	/* SoC Serdes registers */
	if(IS_RL6318B){
	
		WRITE_MEM32(SERDES_ADDR0+0x0, 0x0000);
		WRITE_MEM32(SERDES_ADDR0+0x4, 0xf09b);
		WRITE_MEM32(SERDES_ADDR0+0x8, 0xe38e);
		WRITE_MEM32(SERDES_ADDR0+0xc, 0x0a4b);

		WRITE_MEM32(SERDES_ADDR1+0x0, 0x7211);
		WRITE_MEM32(SERDES_ADDR1+0x4, 0x4209);
		WRITE_MEM32(SERDES_ADDR1+0x8, 0xc1f5);
		WRITE_MEM32(SERDES_ADDR1+0xc, 0x81cd);

		WRITE_MEM32(SERDES_ADDR2+0x0, 0x8c67);
		WRITE_MEM32(SERDES_ADDR2+0x4, 0x4ccc);
		WRITE_MEM32(SERDES_ADDR2+0x8, 0x0000);
		WRITE_MEM32(SERDES_ADDR2+0xc, 0x1400);

		WRITE_MEM32(SERDES_ADDR3+0x0, 0x0003);
		WRITE_MEM32(SERDES_ADDR3+0x4, 0x79ab);
		WRITE_MEM32(SERDES_ADDR3+0x8, 0x0c5c);
		WRITE_MEM32(SERDES_ADDR3+0xc, 0x7ec7);
		
		WRITE_MEM32(SERDES_ADDR4+0x0, 0x9313);
		WRITE_MEM32(SERDES_ADDR4+0x4, 0xc4aa);
		WRITE_MEM32(SERDES_ADDR4+0x8, 0x9308);
		WRITE_MEM32(SERDES_ADDR4+0xc, 0xfff2);

		WRITE_MEM32(SERDES_ADDR5+0x0, 0x04aa);
		WRITE_MEM32(SERDES_ADDR5+0x4, 0x0008);
		WRITE_MEM32(SERDES_ADDR5+0x8, 0x0000);

		WRITE_MEM32(SERDES_BASE+0x0, 0x1400);
		WRITE_MEM32(SERDES_BASE+0x0, 0x1403);
		
	}else if(IS_RL6318C){
	
		WRITE_MEM32(SERDES_ADDR0+0x0, 0x0000);
		WRITE_MEM32(SERDES_ADDR0+0x4, 0xb7c9);
		WRITE_MEM32(SERDES_ADDR0+0x8, 0x838e);
		WRITE_MEM32(SERDES_ADDR0+0xc, 0x2a4b);
		
		WRITE_MEM32(SERDES_ADDR1+0x0, 0x7211);
		WRITE_MEM32(SERDES_ADDR1+0x4, 0x4208);
		WRITE_MEM32(SERDES_ADDR1+0x8, 0xc208);
		WRITE_MEM32(SERDES_ADDR1+0xc, 0x84fa);
		
		WRITE_MEM32(SERDES_ADDR2+0x0, 0xf46f);
		WRITE_MEM32(SERDES_ADDR2+0x4, 0x5ccc);
		WRITE_MEM32(SERDES_ADDR2+0x8, 0x0000);
		WRITE_MEM32(SERDES_ADDR2+0xc, 0x1418);
		
		WRITE_MEM32(SERDES_ADDR3+0x0, 0x0003);
		WRITE_MEM32(SERDES_ADDR3+0x4, 0x79aa);
		WRITE_MEM32(SERDES_ADDR3+0x8, 0x8c20);
		WRITE_MEM32(SERDES_ADDR3+0xc, 0x0e47);
		
		WRITE_MEM32(SERDES_ADDR4+0x0, 0x0482);
		WRITE_MEM32(SERDES_ADDR4+0x4, 0x14aa);
		WRITE_MEM32(SERDES_ADDR4+0x8, 0x0303);
		WRITE_MEM32(SERDES_ADDR4+0xc, 0xa662);
		
		WRITE_MEM32(SERDES_ADDR5+0x0, 0x04bf);

		WRITE_MEM32(SERDES_BASE+0x0, 0x1400);
		WRITE_MEM32(SERDES_BASE+0x0, 0x1403);
	}


	if((version == IS_RTL8214C) || (version == IS_RTL8214C_MP)){

		/* 
		  * Handle RTL8214C : 
		  * Using MDC/MDIO to config RTL8214C if there is no value in Efuse
		  */
		int i=0,port=5;
		int sds_mode_start = 0; 
		uint32 SwitchPCRP=0, PHY_value=0;

#if 0
		if( (READ_MEM32(0xb8000068) & (1<<14)) == (1<<14)){

			/* QSGMII Serdes mode */
			sds_mode_start = 0;
		}else{
			/* RSGMII+ Serdes mode */
			sds_mode_start = 2;
		}
#else
		printf("Serdes QSGMII mode\n\r");
		WRITE_MEM32(0xb8000068, (READ_MEM32(0xb8000068)|(1<<14)));
		sds_mode_start = 0;	
#endif


		for(i=sds_mode_start; i<(sizeof(rtl8214C_serdes_perchip)/sizeof(RTL8214C_confcode_prv_s)); i++){
			rtl8651_setAsicEthernetPHYReg(port,rtl8214C_serdes_perchip[i].reg_id, \
				rtl8214C_serdes_perchip[i].data);
		}	


		/* Enable PHY after setting */
		SwitchPCRP = PCRP5; 
		PHY_value = (MacSwReset |STP_PortST_FORWARDING |\ 
			PauseFlowControlEtxErx | ForceDuplex | ForceSpeed1000M |ForceLink );

		port=5;			
		WRITE_MEM32(SwitchPCRP, (PHY_value & (~ExtPHYID_MASK) | (1 << ExtPHYID_OFFSET)));
		rtl8651_setAsicEthernetPHYReg(port, 0, 0x1140);

		port=6;
		WRITE_MEM32(SwitchPCRP, (PHY_value & (~ExtPHYID_MASK) | (2 << ExtPHYID_OFFSET)));
		rtl8651_setAsicEthernetPHYReg(port, 0, 0x1140);

		port=7;
		WRITE_MEM32(SwitchPCRP, (PHY_value & (~ExtPHYID_MASK) | (3 << ExtPHYID_OFFSET)));
		rtl8651_setAsicEthernetPHYReg(port, 0, 0x1140);

		port=8;
		WRITE_MEM32(SwitchPCRP, (PHY_value & (~ExtPHYID_MASK) | (0 << ExtPHYID_OFFSET)));
		rtl8651_setAsicEthernetPHYReg(port, 0, 0x1140);
	

	}

	return;
}

int LinkCheck[4];
SerdesStatus_t CurrentSerdes[4];

void rtl8685P_getSerdesStatus(SerdesStatus_t* PortStatus, int Port)
{
	unsigned short status;
	unsigned int SerdesAddr=0;

	if(Port==0 || Port==1)
		SerdesAddr = SDSP0_Status;
	else if(Port==2 || Port==3)
		SerdesAddr = SDSP2_Status;

	status = ((REG32(SerdesAddr)) >> (16*(Port%2))) & 0xFFFF;
	/*printf("[%s] Debug, status = 0x%04X\n",__func__,status);*/

	PortStatus->en_eee_giga= (status & EN_EEE_GIGA)?1:0;
	PortStatus->en_eee_100 = (status & EN_EEE_100)?1:0;
	PortStatus->nway_fault = (status & NWAY_FAULT)?1:0;
	PortStatus->mastar_mode = (status & MASTER_MODE)?1:0;
	PortStatus->en_nway = (status & EN_NWAY)?1:0;
	PortStatus->en_txflow= (status & EN_TXFLOW)?1:0;
	PortStatus->en_rxflow= (status & EN_RXFLOW)?1:0;
	PortStatus->link_ok = (status & LINK_OK)?1:0;
	PortStatus->fiber_mode = (status & FIBER_MODE)?1:0;
	PortStatus->full_duplex = (status & FULL_DUPLEX)?1:0;
	PortStatus->spd = status & SPD_MASK;

	/*printf("[%s] Debug, PortStatus->link_ok = %d\n",__func__,(PortStatus->link_ok ));
	printf("[%s] Debug, PortStatus->fiber_mode = %d\n",__func__,(PortStatus->fiber_mode ));
	printf("[%s] Debug, PortStatus->full_duplex = %d\n",__func__,(PortStatus->full_duplex ));
	printf("[%s] Debug, PortStatus->spd = %d\n",__func__,(PortStatus->spd ));*/

	return;
}


static void RTL8214_port_check_task(void) {
	static SerdesStatus_t SwPortStatus[4];
	SerdesStatus_t PortStatus;
	uint8 port_num;
	uint32 SwitchPCRP;
	uint32 LinkMode;
	uint32 LinkChange = 0;
	const char *strLinkStatus, *strLinkSpeed;
	for(port_num=0; port_num<4; port_num++){

		SwitchPCRP = PCRP0 + (0x4)*port_num;

		memset(&PortStatus, 0x0, sizeof(PortStatus));
		rtl8685P_getSerdesStatus(&PortStatus, port_num);
		
		if(LinkCheck[port_num])
		{
			if( memcmp(&CurrentSerdes[port_num], &PortStatus, sizeof(SerdesStatus_t) )==0 )
				LinkChange=0;
			else{
				memcpy( &CurrentSerdes[port_num], &PortStatus, sizeof(SerdesStatus_t) );
				LinkChange=1;
			}
		}else{

			/* First time checking */
			memcpy( &CurrentSerdes[port_num], &PortStatus, sizeof(SerdesStatus_t) );
			LinkCheck[port_num]=1;
			LinkChange=1;
		}
		
		if(PortStatus.link_ok){
			
			if(LinkChange){
			
				/* Set port X, X=0,1,2,3 */
				WRITE_MEM32(SwitchPCRP, (READ_MEM32(SwitchPCRP)&~PollLinkStatus));

				if( ((PortStatus.spd) & SPD_MASK) == SPD_10M) {
					LinkMode = ForceSpeed10M|EnForceMode;
					strLinkSpeed  = "(10M)";
				} else if( ((PortStatus.spd) & SPD_MASK) == SPD_100M) {
					LinkMode = ForceSpeed100M|EnForceMode;
					strLinkSpeed  = "(100M)";
				} else if( ((PortStatus.spd) & SPD_MASK) == SPD_1000M) {
					LinkMode = ForceSpeed1000M|EnForceMode;
					strLinkSpeed  = "(1G)";
				} else {
					strLinkSpeed  = "";
				}

				if(PortStatus.full_duplex)
					LinkMode |= ForceDuplex;
					
				WRITE_MEM32(SwitchPCRP, (READ_MEM32(SwitchPCRP)&(~AutoNegoSts_MASK))|LinkMode);
				WRITE_MEM32(SwitchPCRP, (READ_MEM32(SwitchPCRP)&(~PauseFlowControl_MASK))|PauseFlowControlEtxErx);
				mdelay(10);
		
				/* Set force mode in default */
				WRITE_MEM32(SwitchPCRP, READ_MEM32(SwitchPCRP)|ForceLink);
				strLinkStatus = "UP";
			}
			
		} else {
		
			if(LinkChange){
				strLinkStatus = "DOWN";
				strLinkSpeed  = "";
			}
		}

		if (PortStatus.link_ok != SwPortStatus[port_num].link_ok) {
			printf("Port%d link changed to %s%s\n", port_num, strLinkStatus,strLinkSpeed);			
			SwPortStatus[port_num] = PortStatus;
		}
	}
	
}


int RTL8214_probe(unsigned int phyid)
{
	if(IS_SDS_IF){
		printf("RTL8214 Probe.........");

		/* Initialize status */
		memset( LinkCheck, 0, sizeof(LinkCheck)  );
		memset( CurrentSerdes, 0, sizeof(CurrentSerdes)  );
		
		Serdes_Enable();
		mdelay(3000);
		register_nonblocking_task(RTL8214_port_check_task);
		return SUCCESS;

	}	
	return FAILED;	
}

#endif /* CONFIG_EXTS_RTL8214 */

//cathy, interface with phyid 0 (ex: RTL8367B) should be probed at the last in the list !!!!
struct external_giga extGigaList[] = {
#ifdef CONFIG_EXTS_RTL8211E
	{RTL8211E, RTL8211E_PHYID, RTL8211E_probe},
#endif /* CONFIG_EXTS_RTL8211E */
#ifdef CONFIG_EXTS_RTL8367B
	{RTL8367B, RTL8367B_PHYID, RTL8367B_probe},
#endif /* CONFIG_EXTS_RTL8367B*/
#ifdef CONFIG_EXTS_RTL8214 
	{RTL8214B, RTL8214B_PHYID, RTL8214_probe},
#endif /* CONFIG_EXTS_RTL8214 */
	{-1, -1, NULL}
};

int probeExtGiga(void)
{
	struct external_giga *extGiga;
	int extInf = EXT_NONE;
	
	for (extGiga = &extGigaList[0]; extGiga->probeFn != NULL; extGiga++) {
		if (SUCCESS == extGiga->probeFn(extGiga->extPhyId)) {
			extInf = extGiga->inf;
			break;
		}
	}

	return extInf;
}

void SwCore_DisableNIC(void) {
	REG32(PCRP0) &= ~EnablePHYIf;
	REG32(PCRP1) &= ~EnablePHYIf;
	REG32(PCRP2) &= ~EnablePHYIf;
	REG32(PCRP3) &= ~EnablePHYIf;
	REG32(PCRP4) &= ~EnablePHYIf;
	REG32(PCRP5) &= ~EnablePHYIf;
}

int32 swCore_init()
{
	int port = 0;
	int extInf;

	/* Full reset and semreset */
	FullAndSemiReset();
	
	extInf = probeExtGiga();
	if (EXT_NONE != extInf)
		port = 1;
	else 
		printf("Ext. phy is not found. \n\r");

	/* rtl8651_clearAsicAllTable */
	REG32(0xbb804234) = 0;
	REG32(0xbb804234) = 0x7f;	//initialize all hwnat tables excluding multicast and net interface table

	_rtl8651_clearSpecifiedAsicTable(TYPE_MULTICAST_TABLE, RTL8651_IPMULTICASTTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NETINTERFACE_TABLE, RTL865XC_NETINTERFACE_NUMBER);

	REG32(PCRP0) |= AcptMaxLen_16K | EnablePHYIf | MacSwReset;
	REG32(PCRP1) |= (1 << ExtPHYID_OFFSET) | AcptMaxLen_16K | EnablePHYIf | MacSwReset;
	REG32(PCRP2) |= (2 << ExtPHYID_OFFSET) | AcptMaxLen_16K | EnablePHYIf | MacSwReset;
	REG32(PCRP3) |= (3 << ExtPHYID_OFFSET) | AcptMaxLen_16K | EnablePHYIf | MacSwReset;
	REG32(PCRP4) |= (4 << ExtPHYID_OFFSET) | AcptMaxLen_16K | EnablePHYIf | MacSwReset;


	/* Set PVID of all ports to 8 */
	REG32(PVCR0) = (0x8 << 16) | 0x8;
	REG32(PVCR1) = (0x8 << 16) | 0x8;
	REG32(PVCR2) = (0x8 << 16) | 0x8;
	REG32(PVCR3) = (0x8 << 16) | 0x8;

	
	/* Enable L2 lookup engine and spanning tree functionality */
	// REG32(MSCR) = EN_L2 | EN_L3 | EN_L4 | EN_IN_ACL;
	REG32(MSCR) = EN_L2;
	REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

	/* Start normal TX and RX */
	REG32(SIRR) |= TRXRDY;
	
	/*PHY FlowControl. Default enable*/
	for (; port<MAX_PORT_NUMBER; port++)
	{
		/* Set Flow Control capability. */

		rtl8651_restartAsicEthernetPHYNway(port+1, port);

		#ifdef CONFIG_RTL8676
		rtl8651_setAsicEthernetPHYReg(port, 22, 0x5bd5);

		//select page 1
		rtl8651_setAsicEthernetPHYReg(port, 31, 1);
		//ethernet 100Mhz output voltage to 1.0(v)
		rtl8651_setAsicEthernetPHYReg(port, 18, 0x9004);
		//adjust AOI waveform
		rtl8651_setAsicEthernetPHYReg(port, 19, 0x5400);
		//return to page 0
		rtl8651_setAsicEthernetPHYReg(port, 31, 0);
		#endif

		#ifdef CONFIG_RTL8676S
		rtl8651_setAsicEthernetPHYReg(port, 22, 0x5bd5);

		if(IS_6333)
		{
			//select page 1
			rtl8651_setAsicEthernetPHYReg(port, 31, 1);
			//adjust AOI waveform
			rtl8651_setAsicEthernetPHYReg(port, 16, 0xa8dc);
			//return to page 0
			rtl8651_setAsicEthernetPHYReg(port, 31, 0);
		}
		#endif	
	}	
	
	#ifdef CONFIG_RTL8676S
        // add for port4 connection
		if(IS_0562) rtl8651_setAsicEthernetPHYReg(0, 24, 0x0310);
    #endif
	rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac), 0);

	REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU; // rx broadcast and unicast packet

	return 0;
}

//tylo, remove for bootloader size
#if 0
int enable_10M_power_saving(int phyid , int regnum,int data)
{
   
    unsigned int uid,tmp;  
     rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
     uid=tmp;
     //uid |=data;
     uid =data;
     printf("\nDefault uid =%x",tmp);
     printf("\nSet uid =%x\n",uid);
    //dprintf("\nSet enable_10M_power_saving00!\n");
    rtl8651_setAsicEthernetPHYReg( phyid, regnum, uid );
    //dprintf("\nSet enable_10M_power_saving01!\n");
    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
    //dprintf("\nSet enable_10M_power_saving02!\n");
    uid=tmp;
    printf("After setting,PHYID=0x%x ,regID=0x%x, Find PHY Chip! UID=0x%x\r\n", phyid, regnum, uid);
}

int32 rtl8651_returnAsicCounter(uint32 offset) 
{
	if(offset & 0x3)
		return 0;
	return  READ_MEM32(MIB_COUNTER_BASE + offset);
}

int32 rtl865xC_dumpAsicCounter(void)
{
	uint32 i;
	uint32 addrOffset_fromP0;

	for ( i = 0; i <= RTL8651_PORT_NUMBER; i++ )
	{
#ifdef CONFIG_RTL8196D // FPGA
		if (i==0 || i==3 || i==4 || i==5)
			continue;
#endif
		addrOffset_fromP0 = i * MIB_ADDROFFSETBYPORT;

		if ( i == RTL8651_PORT_NUMBER )
			printf("<CPU port>\n");
		else
			printf("<Port: %d>\n", i);
        
		printf("Rx %u * 2^32 + %u Bytes, Rx %u:%u(mc):%u(bc) Pkts, Drop %u pkts, Rx Pause %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_IFINOCTETS_P0 + addrOffset_fromP0 + 4),
			rtl8651_returnAsicCounter( OFFSET_IFINOCTETS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_IFINUCASTPKTS_P0 + addrOffset_fromP0 ),
				rtl8651_returnAsicCounter( OFFSET_ETHERSTATSMULTICASTPKTS_P0 + addrOffset_fromP0 ),
				rtl8651_returnAsicCounter( OFFSET_ETHERSTATSBROADCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT1DTPPORTINDISCARDS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSUNDERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSFRAGMEMTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSOVERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSJABBERS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSDROPEVENTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSFCSERRORS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSSYMBOLERRORS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3INPAUSEFRAMES_P0 + addrOffset_fromP0 ) );
		
		printf("Tx %u * 2^32 + %u Bytes, Tx %u:%u(mc):%u(bc) Pkts Tx Pause %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0 + 4),
			rtl8651_returnAsicCounter( OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_IFOUTUCASTPKTS_P0 + addrOffset_fromP0 ),
				rtl8651_returnAsicCounter( OFFSET_IFOUTMULTICASTPKTS_P0 + addrOffset_fromP0 ),
				rtl8651_returnAsicCounter( OFFSET_IFOUTBROADCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT3OUTPAUSEFRAMES_P0 + addrOffset_fromP0 ) );
		
	}

	return SUCCESS;
}




#define rtlglue_printf printf
#define _rtl8651_readAsicEntry    swTable_readEntry
static int rtl8651_totalExtPortNum=3;
uint32 _hw_mc_idx = 0;




int32 rtl8651_getAsicL2Table(uint32 row, uint32 column, rtl865x_tblAsicDrv_l2Param_t *l2p) {
	rtl865xc_tblAsic_l2Table_t   entry;
 
	if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN) || (l2p == NULL))
		return FAILED;

	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);

	if(entry.agingTime == 0 && entry.isStatic == 0 &&entry.auth==0)
		return FAILED;
	l2p->macAddr.octet[0] = entry.mac47_40;
	l2p->macAddr.octet[1] = entry.mac39_24 >> 8;
	l2p->macAddr.octet[2] = entry.mac39_24 & 0xff;
	l2p->macAddr.octet[3] = entry.mac23_8 >> 8;
	l2p->macAddr.octet[4] = entry.mac23_8 & 0xff;
	l2p->macAddr.octet[5] = row ^ l2p->macAddr.octet[0] ^ l2p->macAddr.octet[1] ^ l2p->macAddr.octet[2] ^ l2p->macAddr.octet[3] ^ l2p->macAddr.octet[4]  ^(fidHashTable[entry.fid]);
	l2p->cpu = entry.toCPU==1? TRUE: FALSE;
	l2p->srcBlk = entry.srcBlock==1? TRUE: FALSE;
	l2p->nhFlag = entry.nxtHostFlag==1? TRUE: FALSE;
	l2p->isStatic = entry.isStatic==1? TRUE: FALSE;
	l2p->memberPortMask = (entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;

	/* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec. */
	l2p->ageSec = entry.agingTime * 150;

	l2p->fid=entry.fid;
	l2p->auth=entry.auth;
	return SUCCESS;
}

void rtl865xC_dump_l2(void)
{
		rtl865x_tblAsicDrv_l2Param_t asic_l2;
 		uint32 row, col, port, m=0;

		rtlglue_printf(">>ASIC L2 Table:\n");

		for(row=0x0; row<RTL8651_L2TBL_ROW; row++)
		{
			for(col=0; col<RTL8651_L2TBL_COLUMN; col++)
			{
				memset((void*)&asic_l2, 0, sizeof(asic_l2));
				if (rtl8651_getAsicL2Table(row, col, &asic_l2) == FAILED)
				{
					continue;
				}

				if (asic_l2.isStatic && asic_l2.ageSec==0 && asic_l2.cpu && asic_l2.memberPortMask == 0 &&asic_l2.auth==0)
				{
					continue;
				}

				rtlglue_printf("%4d.[%3d,%d] %02x:%02x:%02x:%02x:%02x:%02x FID:%x mbr(",m, row, col, 
						asic_l2.macAddr.octet[0], asic_l2.macAddr.octet[1], asic_l2.macAddr.octet[2], 
						asic_l2.macAddr.octet[3], asic_l2.macAddr.octet[4], asic_l2.macAddr.octet[5],asic_l2.fid
				);

				m++;

				for (port = 0 ; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum ; port ++)
				{
					if (asic_l2.memberPortMask & (1<<port))
					{
						rtlglue_printf("%d ", port);
					}
				}

				rtlglue_printf(")");
				rtlglue_printf("%s %s %s %s age:%d ",asic_l2.cpu?"CPU":"FWD", asic_l2.isStatic?"STA":"DYN",  asic_l2.srcBlk?"BLK":"", asic_l2.nhFlag?"NH":"", asic_l2.ageSec);

				if (asic_l2.auth)
				{
					rtlglue_printf("AUTH:%d",asic_l2.auth);
				}
				rtlglue_printf("\n");
			}
		}

}

#include <rtl8650/vlanTable.h>

int32 rtl8651_getAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp) {	
	vlan_table_t entry;
	if(vlanp == NULL||vid>=RTL865XC_VLAN_NUMBER)
		return FAILED;        

	_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, vid, &entry);	
	if((entry.extMemberPort | entry.memberPort) == 0)
	{
		return FAILED;
	}
	vlanp->memberPortMask = (entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;
	vlanp->untagPortMask = (entry.extEgressUntag<<RTL8651_PORT_NUMBER) |entry.egressUntag;
	vlanp->fid=entry.fid;
#ifdef CONFIG_RTL8196D	
	vlanp->vid=entry.vid;
#endif
	return SUCCESS;
}

void rtl865xC_dump_vlan(void)
{
		int i, j;

		rtlglue_printf(">>ASIC VLAN Table:\n\n");
		for ( i = 0; i < RTL865XC_VLAN_NUMBER; i++ )
		{
			rtl865x_tblAsicDrv_vlanParam_t vlan;

			if ( rtl8651_getAsicVlan( i, &vlan ) == FAILED )
				continue;
			
#ifdef CONFIG_RTL8196D	
			rtlglue_printf("  entry# [%d], VID [%d] ", i, vlan.vid);
#else
			rtlglue_printf("  VID[%d] ", i);
#endif
			rtlglue_printf("\n\tmember ports:");

			for( j = 0; j < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; j++ )
			{
				if ( vlan.memberPortMask & ( 1 << j ) )
					rtlglue_printf("%d ", j);
			}

			rtlglue_printf("\n\tUntag member ports:");				

			for( j = 0; j < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; j++ )
			{
				if( vlan.untagPortMask & ( 1 << j ) )
					rtlglue_printf("%d ", j);
			}

			rtlglue_printf("\n\tFID:\t%d\n",vlan.fid);
		}
}


int32 rtl8651_getAsicNetInterface( uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp )
{
	netif_table_t entry;
	uint32 i;

	if(intfp == NULL)
		return FAILED;

	intfp->valid=0;

	if ( idx == RTL865XC_NETIFTBL_SIZE )
	{
		/* idx is not specified, we search whole interface table. */
		for( i = 0; i < RTL865XC_NETIFTBL_SIZE; i++ )
		{

			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if ( entry.valid && entry.vid==intfp->vid ){
				goto found;
			}
		}

		/* intfp.vid is not found. */
		return FAILED;
	}
	else
	{
		/* idx is specified, read from ASIC directly. */
		_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, idx, &entry);
	}

found:
	intfp->valid=entry.valid;
	intfp->vid=entry.vid;
	intfp->macAddr.octet[0] = entry.mac47_19>>21;
	intfp->macAddr.octet[1] = (entry.mac47_19 >>13)&0xff;
	intfp->macAddr.octet[2] = (entry.mac47_19 >>5)&0xff;
	intfp->macAddr.octet[3] = ((entry.mac47_19 &0x3f) <<3) | (entry.mac18_0 >>16);
	intfp->macAddr.octet[4] = (entry.mac18_0 >> 8)&0xff;
	intfp->macAddr.octet[5] = entry.mac18_0 & 0xff;
	intfp->inAclEnd = entry.inACLEnd;
	intfp->inAclStart= (entry.inACLStartH<<2)|entry.inACLStartL;
	intfp->outAclStart = entry.outACLStart;
	intfp->outAclEnd = entry.outACLEnd;
	intfp->enableRoute = entry.enHWRoute==1? TRUE: FALSE;

	switch(entry.macMask)
	{
		case 0:
			intfp->macAddrNumber =8;
			break;
		case 6:
			intfp->macAddrNumber =2;
			break;
		case 4:
			intfp->macAddrNumber =4;
			break;
		case 7:
			intfp->macAddrNumber =1;
			break;			
	}
	intfp->mtu = (entry.mtuH <<3)|entry.mtuL;

	return SUCCESS;
}

void rtl865xC_dump_netif(void)
{
	int8	*pst[] = { "DIS/BLK",  "LIS", "LRN", "FWD" };
	uint8 *mac;
	int32 i, j;

	rtlglue_printf(">>ASIC Netif Table:\n\n");
	for ( i = 0; i < RTL865XC_NETIFTBL_SIZE; i++ )
	{
		rtl865x_tblAsicDrv_intfParam_t intf;
		rtl865x_tblAsicDrv_vlanParam_t vlan;

		if ( rtl8651_getAsicNetInterface( i, &intf ) == FAILED )
			continue;
		
		if ( intf.valid )
		{
			mac = (uint8 *)&intf.macAddr.octet[0];
			rtlglue_printf("[%d]  VID[%d] %x:%x:%x:%x:%x:%x", 
				i, intf.vid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			rtlglue_printf("  Routing %s \n",
				intf.enableRoute==TRUE? "enabled": "disabled" );

			rtlglue_printf("      ingress ");

			if ( RTL8651_ACLTBL_DROP_ALL <= intf.inAclStart )
			{
				if ( intf.inAclStart == RTL8651_ACLTBL_PERMIT_ALL )
					rtlglue_printf("permit all,");
				if ( intf.inAclStart == RTL8651_ACLTBL_ALL_TO_CPU )
					rtlglue_printf("all to cpu,");
				if ( intf.inAclStart == RTL8651_ACLTBL_DROP_ALL )
					rtlglue_printf("drop all,");
			}
			else
				rtlglue_printf("ACL %d-%d, ", intf.inAclStart, intf.inAclEnd);

			rtlglue_printf("  egress ");

			if ( RTL8651_ACLTBL_DROP_ALL <= intf.outAclStart )
			{
				if ( intf.outAclStart == RTL8651_ACLTBL_PERMIT_ALL )
					rtlglue_printf("permit all,");
				if ( intf.outAclStart==RTL8651_ACLTBL_ALL_TO_CPU )
					rtlglue_printf("all to cpu,");
				if ( intf.outAclStart==RTL8651_ACLTBL_DROP_ALL )
					rtlglue_printf("drop all,");
			}
			else
				rtlglue_printf("ACL %d-%d, ", intf.outAclStart, intf.outAclEnd);

			rtlglue_printf("\n      %d MAC Addresses, MTU %d Bytes\n", intf.macAddrNumber, intf.mtu);

			rtl8651_getAsicVlan( intf.vid, &vlan );

			rtlglue_printf("\n      Untag member ports:");

			for ( j = 0; j < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; j++ )
			{
				if ( vlan.untagPortMask & ( 1 << j ) )
					rtlglue_printf("%d ", j);
			}
			rtlglue_printf("\n      Active member ports:");

			for ( j = 0; j < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; j++ )
			{
				if ( vlan.memberPortMask & ( 1 << j ) )
					rtlglue_printf("%d ", j);
			}
			
			rtlglue_printf("\n      Port state(");

			for ( j = 0; j < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; j++ )
			{
				if ( ( vlan.memberPortMask & ( 1 << j ) ) == 0 )
					continue;
				if ((( READ_MEM32( PCRP0 + j * 4 ) & STP_PortST_MASK) >> STP_PortST_OFFSET ) > 4 )
					rtlglue_printf("--- ");
				else
					rtlglue_printf("%d:%s ", j, pst[(( READ_MEM32( PCRP0 + j * 4 ) & STP_PortST_MASK) >> STP_PortST_OFFSET )]);

			}
			rtlglue_printf(")\n\n");
		}

	}
}










int32 rtl8651_getAsicAclRule(uint32 index, rtl_acl_param_t *rule) {
	rtl865xc_tblAsic_aclTable_t    entry;

	if(index >= RTL8651_ACLTBL_SIZE || rule == NULL)
		return FAILED;
	_rtl8651_readAsicEntry(TYPE_ACL_RULE_TABLE, index, &entry);
	bzero(rule, sizeof(rtl_acl_param_t));

	switch(entry.ruleType) {

	case 0x00: /* Ethernet rule type */
		 rule->dstMac_.octet[0]     = entry.is.ETHERNET.dMacP47_32 >> 8;
		 rule->dstMac_.octet[1]     = entry.is.ETHERNET.dMacP47_32 & 0xff;
		 rule->dstMac_.octet[2]     = entry.is.ETHERNET.dMacP31_16 >> 8;
	 	 rule->dstMac_.octet[3]     = entry.is.ETHERNET.dMacP31_16 & 0xff;
		 rule->dstMac_.octet[4]     = entry.is.ETHERNET.dMacP15_0 >> 8;
		 rule->dstMac_.octet[5]     = entry.is.ETHERNET.dMacP15_0 & 0xff;
		 rule->dstMacMask_.octet[0] = entry.is.ETHERNET.dMacM47_32 >> 8;
		 rule->dstMacMask_.octet[1] = entry.is.ETHERNET.dMacM47_32 & 0xff;
		 rule->dstMacMask_.octet[2] = entry.is.ETHERNET.dMacM31_16 >> 8;
		 rule->dstMacMask_.octet[3] = entry.is.ETHERNET.dMacM31_16 & 0xff;
	 	 rule->dstMacMask_.octet[4] = entry.is.ETHERNET.dMacM15_0 >> 8;
		 rule->dstMacMask_.octet[5] = entry.is.ETHERNET.dMacM15_0 & 0xff;
	 	 rule->srcMac_.octet[0]     = entry.is.ETHERNET.sMacP47_32 >> 8;
		 rule->srcMac_.octet[1]     = entry.is.ETHERNET.sMacP47_32 & 0xff;
		 rule->srcMac_.octet[2]     = entry.is.ETHERNET.sMacP31_16 >> 8;
		 rule->srcMac_.octet[3]     = entry.is.ETHERNET.sMacP31_16 & 0xff;
		 rule->srcMac_.octet[4]     = entry.is.ETHERNET.sMacP15_0 >> 8;
		 rule->srcMac_.octet[5]     = entry.is.ETHERNET.sMacP15_0 & 0xff;
		 rule->srcMacMask_.octet[0] = entry.is.ETHERNET.sMacM47_32 >> 8;
		 rule->srcMacMask_.octet[1] = entry.is.ETHERNET.sMacM47_32 & 0xff;
		 rule->srcMacMask_.octet[2] = entry.is.ETHERNET.sMacM31_16 >> 8;
		 rule->srcMacMask_.octet[3] = entry.is.ETHERNET.sMacM31_16 & 0xff;
		 rule->srcMacMask_.octet[4] = entry.is.ETHERNET.sMacM15_0 >> 8;
		 rule->srcMacMask_.octet[5] = entry.is.ETHERNET.sMacM15_0 & 0xff;
		 rule->typeLen_             = entry.is.ETHERNET.ethTypeP;
		 rule->typeLenMask_         = entry.is.ETHERNET.ethTypeM;
		 rule->ruleType_            = RTL8651_ACL_MAC;
		 break;

	case 0x02: /* IP mask rule type */
	case 0x0A: /* IP range rule type*/
		 rule->tos_         = entry.is.L3L4.is.IP.IPTOSP;
		 rule->tosMask_     = entry.is.L3L4.is.IP.IPTOSM;
		 rule->ipProto_     = entry.is.L3L4.is.IP.IPProtoP;
		 rule->ipProtoMask_ = entry.is.L3L4.is.IP.IPProtoM;
		 rule->ipFlag_      = entry.is.L3L4.is.IP.IPFlagP;
		 rule->ipFlagMask_  = entry.is.L3L4.is.IP.IPFlagM;
 		 rule->ipFOP_ = entry.is.L3L4.is.IP.FOP;
		 rule->ipFOM_ = entry.is.L3L4.is.IP.FOM;
		 rule->ipHttpFilterM_ = entry.is.L3L4.is.IP.HTTPM;
		 rule->ipHttpFilter_  = entry.is.L3L4.is.IP.HTTPP;
		 rule->ipIdentSrcDstIp_ = entry.is.L3L4.is.IP.identSDIPM;
		 if (entry.ruleType==0x2)
	 		rule->ruleType_=RTL8651_ACL_IP;
		 else 
			rule->ruleType_=RTL8652_ACL_IP_RANGE;
		 goto l3l4_shared;

	     

	case 0x06: /* TCP rule type */
	case 0x0E:
		 rule->tos_ = entry.is.L3L4.is.TCP.IPTOSP;
		 rule->tosMask_ = entry.is.L3L4.is.TCP.IPTOSM;
		 rule->tcpFlag_ = entry.is.L3L4.is.TCP.TCPFlagP;
		 rule->tcpFlagMask_ = entry.is.L3L4.is.TCP.TCPFlagM;
		 rule->tcpSrcPortUB_ = entry.is.L3L4.is.TCP.TCPSPUB;
		 rule->tcpSrcPortLB_ = entry.is.L3L4.is.TCP.TCPSPLB;
		 rule->tcpDstPortUB_ = entry.is.L3L4.is.TCP.TCPDPUB;
		 rule->tcpDstPortLB_ = entry.is.L3L4.is.TCP.TCPDPLB;
	 	 if (entry.ruleType==0x6)
		 	rule->ruleType_ = RTL8651_ACL_TCP;
		 else
		 	rule->ruleType_ = RTL8652_ACL_TCP_IPRANGE;		 
         goto l3l4_shared;

	case 0x07: /* UDP rule type */
	case 0x0F:
		 rule->tos_ = entry.is.L3L4.is.UDP.IPTOSP;
		 rule->tosMask_ = entry.is.L3L4.is.UDP.IPTOSM;
		 rule->udpSrcPortUB_ = entry.is.L3L4.is.UDP.UDPSPUB;
		 rule->udpSrcPortLB_ = entry.is.L3L4.is.UDP.UDPSPLB;
		 rule->udpDstPortUB_ = entry.is.L3L4.is.UDP.UDPDPUB;
		 rule->udpDstPortLB_ = entry.is.L3L4.is.UDP.UDPDPLB;
		 if (entry.ruleType==0x7)
			 rule->ruleType_ = RTL8651_ACL_UDP;
		 else
			 rule->ruleType_ = RTL8652_ACL_UDP_IPRANGE;
l3l4_shared:
		rule->srcIpAddr_ = entry.is.L3L4.sIPP;
		rule->srcIpAddrMask_ = entry.is.L3L4.sIPM;
		rule->dstIpAddr_ = entry.is.L3L4.dIPP;
		rule->dstIpAddrMask_ = entry.is.L3L4.dIPM;
		break;

		 
	default: return FAILED; /* Unknown rule type */

	}

	rule->aclIdx = index;

	switch(entry.actionType) {
	
	case 0x00: /* Permit. Stop matching another rules */
			rule->actionType_ = RTL8651_ACL_PERMIT;
		 	goto _common_action;
		 		

	case 0x03: /* Trap to CPU. Stop matching antoher rules */
			rule->actionType_ = RTL8651_ACL_CPU;
			goto _common_action;

	case 0x02: /* Silently Drop. Stop matching another rules */
	case 0x04: /* Silently Drop. Stop matching another rules */	
			rule->actionType_ = RTL8651_ACL_DROP;
		 	goto _common_action;
		 	
	case 0x05: /* Drop to CPU for logging. Stop matching another rules */
			rule->actionType_ = RTL8651_ACL_DROP_NOTIFY;
			goto _common_action;
	


	case 0x0f: /* RESERVED */

	default: return FAILED;

_common_action:
		/* handle pktOpApp */
		if ( entry.pktOpApp == 0x7 )
			rule->pktOpApp = RTL865XC_ACLTBL_ALL_LAYER;
		else if ( entry.pktOpApp == 0 )
			rule->pktOpApp = RTL8651_ACLTBL_NOOP;
		else
 			rule->pktOpApp = entry.pktOpApp;
 		break;
	}
	return SUCCESS;
}

void rtl865xC_dump_acl(void)
{
		int8 *actionT[] = { "", "permit", "drop", "cpu", "drop log", 
						"drop notify", 	"redirect to ethernet","redirect to pppoe", "mirror", "mirro keep match", 
						"drop rate exceed pps", "log rate exceed pps", "drop rate exceed bps", "log rate exceed bps","policy ",
						"priority selection"};
		rtl_acl_param_t asic_acl;
		rtl865x_tblAsicDrv_intfParam_t asic_intf;
		uint32 start, end;

		uint16 vid;
		int8 outRule;


				
		rtlglue_printf(">>ASIC ACL Table:\n\n");
		for(vid=0; vid<8; vid++ ) {
			/* Read VLAN Table */
			if (rtl8651_getAsicNetInterface(vid, &asic_intf) == FAILED)
				continue;
			if (asic_intf.valid==FALSE)
				continue;

			outRule = FALSE;
			start = asic_intf.inAclStart; end = asic_intf.inAclEnd;
	again:
			if (outRule == FALSE)
				rtlglue_printf("\n<<Ingress Rule for Netif  %d: (VID %d)>>\n", vid,asic_intf.vid);
			else rtlglue_printf("\n<<Egress Rule for Netif %d (VID %d)>>:\n", vid,asic_intf.vid);
			for( ; start<=end; start++) {
				if (rtl8651_getAsicAclRule(start, &asic_acl) == FAILED)
				//	assert(0);
					continue;
				switch(asic_acl.ruleType_)
				{
				case RTL8651_ACL_MAC:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Ethernet", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tether type: %x   ether type mask: %x\n", asic_acl.typeLen_, asic_acl.typeLenMask_);
					rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n",
						asic_acl.dstMac_.octet[0], asic_acl.dstMac_.octet[1], asic_acl.dstMac_.octet[2],
						asic_acl.dstMac_.octet[3], asic_acl.dstMac_.octet[4], asic_acl.dstMac_.octet[5],
						asic_acl.dstMacMask_.octet[0], asic_acl.dstMacMask_.octet[1], asic_acl.dstMacMask_.octet[2],
						asic_acl.dstMacMask_.octet[3], asic_acl.dstMacMask_.octet[4], asic_acl.dstMacMask_.octet[5]
					);
					rtlglue_printf("\tSMAC: %x:%x:%x:%x:%x:%x  SMACM: %x:%x:%x:%x:%x:%x\n",
						asic_acl.srcMac_.octet[0], asic_acl.srcMac_.octet[1], asic_acl.srcMac_.octet[2],
						asic_acl.srcMac_.octet[3], asic_acl.srcMac_.octet[4], asic_acl.srcMac_.octet[5],
						asic_acl.srcMacMask_.octet[0], asic_acl.srcMacMask_.octet[1], asic_acl.srcMacMask_.octet[2],
						asic_acl.srcMacMask_.octet[3], asic_acl.srcMacMask_.octet[4], asic_acl.srcMacMask_.octet[5]
					);
					break;

				case RTL8651_ACL_IP:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IP", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos: %x   TosM: %x   ipProto: %x   ipProtoM: %x   ipFlag: %x   ipFlagM: %x\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.ipProto_, asic_acl.ipProtoMask_, asic_acl.ipFlag_, asic_acl.ipFlagMask_
					);
					rtlglue_printf("\t<FOP:%x> <FOM:%x> <http:%x> <httpM:%x> <IdentSdip:%x> <IdentSdipM:%x> \n",
						asic_acl.ipFOP_, asic_acl.ipFOM_, asic_acl.ipHttpFilter_, asic_acl.ipHttpFilterM_, asic_acl.ipIdentSrcDstIp_,
						asic_acl.ipIdentSrcDstIpM_
					);
					rtlglue_printf("\t<DF:%x> <MF:%x>\n", asic_acl.ipDF_, asic_acl.ipMF_); 
					break;			
				case RTL8652_ACL_IP_RANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IP Range", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos: %x   TosM: %x   ipProto: %x   ipProtoM: %x   ipFlag: %x   ipFlagM: %x\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.ipProto_, asic_acl.ipProtoMask_, asic_acl.ipFlag_, asic_acl.ipFlagMask_
					);
					rtlglue_printf("\t<FOP:%x> <FOM:%x> <http:%x> <httpM:%x> <IdentSdip:%x> <IdentSdipM:%x> \n",
						asic_acl.ipFOP_, asic_acl.ipFOM_, asic_acl.ipHttpFilter_, asic_acl.ipHttpFilterM_, asic_acl.ipIdentSrcDstIp_,
						asic_acl.ipIdentSrcDstIpM_
					);
					rtlglue_printf("\t<DF:%x> <MF:%x>\n", asic_acl.ipDF_, asic_acl.ipMF_); 
					break;			



				case RTL8651_ACL_TCP:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "TCP", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.tcpSrcPortLB_, asic_acl.tcpSrcPortUB_,
						asic_acl.tcpDstPortLB_, asic_acl.tcpDstPortUB_
					);
					rtlglue_printf("\tflag: %x  flagM: %x  <URG:%x> <ACK:%x> <PSH:%x> <RST:%x> <SYN:%x> <FIN:%x>\n",
						asic_acl.tcpFlag_, asic_acl.tcpFlagMask_, asic_acl.tcpURG_, asic_acl.tcpACK_,
						asic_acl.tcpPSH_, asic_acl.tcpRST_, asic_acl.tcpSYN_, asic_acl.tcpFIN_
					);
					break;

				case RTL8652_ACL_TCP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "TCP IP RANGE", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.tcpSrcPortLB_, asic_acl.tcpSrcPortUB_,
						asic_acl.tcpDstPortLB_, asic_acl.tcpDstPortUB_
					);
					rtlglue_printf("\tflag: %x  flagM: %x  <URG:%x> <ACK:%x> <PSH:%x> <RST:%x> <SYN:%x> <FIN:%x>\n",
						asic_acl.tcpFlag_, asic_acl.tcpFlagMask_, asic_acl.tcpURG_, asic_acl.tcpACK_,
						asic_acl.tcpPSH_, asic_acl.tcpRST_, asic_acl.tcpSYN_, asic_acl.tcpFIN_
					);
					break;

				case RTL8651_ACL_UDP:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start,"UDP", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.udpSrcPortLB_, asic_acl.udpSrcPortUB_,
						asic_acl.udpDstPortLB_, asic_acl.udpDstPortUB_
					);
					break;				


				case RTL8652_ACL_UDP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP IP RANGE", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
						((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
					);
					rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
						asic_acl.tos_, asic_acl.tosMask_, asic_acl.udpSrcPortLB_, asic_acl.udpSrcPortUB_,
						asic_acl.udpDstPortLB_, asic_acl.udpDstPortUB_
					);
					break;				


				case RTL8652_ACL_SRCFILTER_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Source Filter(IP RANGE)", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tSMAC: %x:%x:%x:%x:%x:%x  SMACM: %x:%x:%x:%x:%x:%x\n", 
						asic_acl.srcFilterMac_.octet[0], asic_acl.srcFilterMac_.octet[1], asic_acl.srcFilterMac_.octet[2], 
						asic_acl.srcFilterMac_.octet[3], asic_acl.srcFilterMac_.octet[4], asic_acl.srcFilterMac_.octet[5],
						asic_acl.srcFilterMacMask_.octet[0], asic_acl.srcFilterMacMask_.octet[1], asic_acl.srcFilterMacMask_.octet[2],
						asic_acl.srcFilterMacMask_.octet[3], asic_acl.srcFilterMacMask_.octet[4], asic_acl.srcFilterMacMask_.octet[5]
					);
					rtlglue_printf("\tsvidx: %d   svidxM: %x   sport: %d   sportM: %x   ProtoType: %x\n",
						asic_acl.srcFilterVlanIdx_, asic_acl.srcFilterVlanIdxMask_, asic_acl.srcFilterPort_, asic_acl.srcFilterPortMask_,
						(asic_acl.srcFilterIgnoreL3L4_==TRUE? 2: (asic_acl.srcFilterIgnoreL4_ == 1? 1: 0))
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d   sipL: %d.%d.%d.%d\n", (asic_acl.srcFilterIpAddr_>>24),
						((asic_acl.srcFilterIpAddr_&0x00ff0000)>>16), ((asic_acl.srcFilterIpAddr_&0x0000ff00)>>8),
						(asic_acl.srcFilterIpAddr_&0xff), (asic_acl.srcFilterIpAddrMask_>>24),
						((asic_acl.srcFilterIpAddrMask_&0x00ff0000)>>16), ((asic_acl.srcFilterIpAddrMask_&0x0000ff00)>>8),
						(asic_acl.srcFilterIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsportL: %d   sportU: %d\n", asic_acl.srcFilterPortLowerBound_, asic_acl.srcFilterPortUpperBound_);
					break;

				case RTL8652_ACL_DSTFILTER_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Deatination Filter(IP Range)", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n", 
						asic_acl.dstFilterMac_.octet[0], asic_acl.dstFilterMac_.octet[1], asic_acl.dstFilterMac_.octet[2], 
						asic_acl.dstFilterMac_.octet[3], asic_acl.dstFilterMac_.octet[4], asic_acl.dstFilterMac_.octet[5],
						asic_acl.dstFilterMacMask_.octet[0], asic_acl.dstFilterMacMask_.octet[1], asic_acl.dstFilterMacMask_.octet[2],
						asic_acl.dstFilterMacMask_.octet[3], asic_acl.dstFilterMacMask_.octet[4], asic_acl.dstFilterMacMask_.octet[5]
					);
					rtlglue_printf("\tdvidx: %d   dvidxM: %x  ProtoType: %x   dportL: %d   dportU: %d\n",
						asic_acl.dstFilterVlanIdx_, asic_acl.dstFilterVlanIdxMask_, 
						(asic_acl.dstFilterIgnoreL3L4_==TRUE? 2: (asic_acl.dstFilterIgnoreL4_ == 1? 1: 0)), 
						asic_acl.dstFilterPortLowerBound_, asic_acl.dstFilterPortUpperBound_
					);
					rtlglue_printf("\tdipU: %d.%d.%d.%d   dipL: %d.%d.%d.%d\n", (asic_acl.dstFilterIpAddr_>>24),
						((asic_acl.dstFilterIpAddr_&0x00ff0000)>>16), ((asic_acl.dstFilterIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstFilterIpAddr_&0xff), (asic_acl.dstFilterIpAddrMask_>>24),
						((asic_acl.dstFilterIpAddrMask_&0x00ff0000)>>16), ((asic_acl.dstFilterIpAddrMask_&0x0000ff00)>>8),
						(asic_acl.dstFilterIpAddrMask_&0xff)
					);
					break;


				//default: assert(0);

				}


				/* Action type */
			switch (asic_acl.actionType_) {

			case RTL8651_ACL_PERMIT: /* 0x00 */
			case RTL8651_ACL_CPU: /* 0x03 */
			case RTL8651_ACL_DROP: /* 0x02, 0x04 */
			case RTL8651_ACL_DROP_LOG: /* 0x05 */
				rtlglue_printf("\tdvidx: %d   hp: %d   pppoeIdx: %d   nxtHop:%d  ", asic_acl.dvid_, asic_acl.priority_,
						asic_acl.pppoeIdx_, asic_acl.nextHop_);
				break;

			//default: assert(0);
			
			}
			rtlglue_printf("pktOpApp: %d\n", asic_acl.pktOpApp);
			
			}
	}
}








uint32 rtl8651_ipMulticastTableIndex(ipaddr_t srcAddr, ipaddr_t dstAddr) {
	uint32 idx;
	uint32 sip[32],dip[32];
	uint32 hash[7];
	uint32 i;

	for(i=0; i<7; i++) {
		hash[i]=0;
	}

	for(i=0; i<32; i++)	{
		if((srcAddr & (1<<i))!=0) {
			sip[i]=1;
		}
		else 	{
			sip[i]=0;
		}

		if((dstAddr & (1<<i))!=0) {
			dip[i]=1;
		}
		else {
			dip[i]=0;
		}			
	}

	hash[0] = sip[0] ^ sip[7]   ^ sip[14] ^ sip[21] ^ sip[28] ^ dip[1] ^ dip[8]   ^ dip[15] ^ dip[22] ^ dip[29];
	hash[1] = sip[1] ^ sip[8]   ^ sip[15] ^ sip[22] ^ sip[29] ^ dip[2] ^ dip[9]   ^ dip[16] ^ dip[23] ^ dip[30];
	hash[2] = sip[2] ^ sip[9]   ^ sip[16] ^ sip[23] ^ sip[30] ^ dip[3] ^ dip[10] ^ dip[17] ^ dip[24] ^ dip[31];
	hash[3] = sip[3] ^ sip[10] ^ sip[17] ^ sip[24] ^ sip[31] ^ dip[4] ^ dip[11] ^ dip[18] ^ dip[25];
	hash[4] = sip[4] ^ sip[11] ^ sip[18] ^ sip[25]               ^ dip[5] ^ dip[12] ^ dip[19] ^ dip[26];
	hash[5] = sip[5] ^ sip[12] ^ sip[19] ^ sip[26]               ^ dip[6] ^ dip[13] ^ dip[20] ^ dip[27];
	hash[6] = sip[6] ^ sip[13] ^ sip[20] ^ sip[27]   ^ dip[0] ^ dip[7] ^ dip[14] ^ dip[21] ^ dip[28];

	for(i=0; i<7; i++) {
		hash[i]=hash[i] & (0x01);
	}

	idx=0;
	for(i=0; i<7; i++) {
		idx=idx+(hash[i]<<i);
	}
	
	return idx;
}

int add_ip_multicast_tbl(uint32 src_ip, uint32 dst_ip, int src_vlan, uint8 src_port, uint8 mem_port)
{
	rtl865x_tblAsicDrv_multiCastParam_t tbl;

	memset(&tbl, '\0', sizeof(tbl));
	tbl.mbr = (uint32)mem_port;
	tbl.port = (uint16)src_port;
	tbl.sip = src_ip;
	tbl.dip = dst_ip;
	tbl.svid = src_vlan;
	
	if (rtl8651_setAsicIpMulticastTable(&tbl, 1) != SUCCESS) {
		printf("rtl8651_setAsicIpMulticastTable() failed!\n");
		return -1;
	}
	
	return ((int)rtl8651_ipMulticastTableIndex(src_ip, dst_ip));
}

int update_ip_multicast_tbl(uint32  index, uint8 mem_port, int valid)
{
	static rtl865x_tblAsicDrv_multiCastParam_t tbl;

	if(index < RTL8651_IPMULTICASTTBL_SIZE &&  index >= 0 ){
		if (rtl8651_getAsicIpMulticastTable(index, &tbl) == SUCCESS) {
			tbl.mbr = (uint32)mem_port;	
			if (rtl8651_setAsicIpMulticastTable(&tbl, valid) != SUCCESS) {
				printf("rtl8651_setAsicIpMulticastTable() failed!\n");
				return -1;
			}		
		}
		else {
			printf("rtl8651_getAsicIpMulticastTable() failed!\n");
			return -1;		
		}
	}else{
		return -1;		
	}
	
	return SUCCESS;
}

int32 rtl8651_setAsicIpMulticastTable(rtl865x_tblAsicDrv_multiCastParam_t *mCast_t, int is_valid) {
	uint32 idx;
 	rtl865xc_tblAsic_ipMulticastTable_t entry;
	int16 age;

	if(mCast_t->dip >>28 != 0xe || mCast_t->port >= RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum)
		return FAILED;//Non-IP multicast destination address
	bzero(&entry, sizeof(entry));
	entry.srcIPAddr 		= mCast_t->sip;
	entry.destIPAddrLsbs 	= mCast_t->dip & 0xfffffff;

#if 0
	idx = rtl8651_ipMulticastTableIndex(mCast_t->sip, mCast_t->dip);
#else

	if (_hw_mc_idx) {
		//dprintf("  ==> rtl8651_setAsicIpMulticastTable(): _hw_mc_idx= %d.\n", _hw_mc_idx);
		idx = _hw_mc_idx & 0xff;
	}
	else 
		idx = rtl8651_ipMulticastTableIndex(mCast_t->sip, mCast_t->dip);
		
#endif

	// fix the multicast storm issue. do not create the entry if source port is equal to member port.
	if (mCast_t->mbr ==  BIT(mCast_t->port))
		return FAILED;		

	mCast_t->mbr &= ~(BIT(mCast_t->port));

	entry.srcPort 			= mCast_t->port;
	entry.portList 			= mCast_t->mbr;


	entry.toCPU 			= 0;
	entry.valid 			= is_valid;
	entry.extIPIndex 		= mCast_t->extIdx;

	entry.ageTime			= 0;
	age = (int16)mCast_t->age;
	while ( age > 0 ) {
		if ( (++entry.ageTime) == 7)
			break;
		age -= 5;
	}
	entry.ageTime = 7;
	
	return _rtl8651_forceAddAsicEntry(TYPE_MULTICAST_TABLE, idx, &entry);
}

int32 rtl8651_delAsicIpMulticastTable(uint32 index) {
	rtl865xc_tblAsic_ipMulticastTable_t entry;

	bzero(&entry, sizeof(entry));
	return _rtl8651_forceAddAsicEntry(TYPE_MULTICAST_TABLE, index, &entry);
}

int32 rtl8651_getAsicIpMulticastTable(uint32 index, rtl865x_tblAsicDrv_multiCastParam_t *mCast_t) {	
	static rtl865xc_tblAsic_ipMulticastTable_t entry;
	
	if (mCast_t == NULL)
		return FAILED;
   	_rtl8651_readAsicEntry(TYPE_MULTICAST_TABLE, index, &entry);

	
 	mCast_t->sip	= entry.srcIPAddr;
 	mCast_t->dip	= entry.destIPAddrLsbs | 0xe0000000;

	mCast_t->svid = 0;
	mCast_t->port = entry.srcPort;
	mCast_t->mbr = entry.portList;

	mCast_t->extIdx = entry.extIPIndex ;
	mCast_t->age	= entry.ageTime * 5;
	mCast_t->cpu = entry.toCPU;
	return SUCCESS;
}

void dump_multicast_table(void)
{
		int32	i;
		rtl865x_tblAsicDrv_multiCastParam_t asic_mc;

		printf(">> Multicast Table:\n");
		for(i=0; i < (RTL8651_IPMULTICASTTBL_SIZE + 32); i++) {
			if (rtl8651_getAsicIpMulticastTable(i,  &asic_mc) == FAILED)
				continue;

			if (asic_mc.sip != 0)
			  printf("\t[%d]  %d.%d.%d.%d/%d.%d.%d.%d; %d, port:%d,mbr:0x%02x, idx:%d, %d, %d\n", i, (asic_mc.sip>>24),
				((asic_mc.sip&0x00ff0000)>>16), ((asic_mc.sip&0x0000ff00)>>8), (asic_mc.sip&0xff),
				(asic_mc.dip>>24),((asic_mc.dip&0x00ff0000)>>16), 
				((asic_mc.dip&0x0000ff00)>>8), (asic_mc.dip&0xff),
				asic_mc.svid, asic_mc.port, asic_mc.mbr, 
				asic_mc.extIdx, asic_mc.age, asic_mc.cpu
				);
		}	
}






void test_netif(void)
{
	int val;
	rtl_vlan_param_t vp;
	int ret;
       rtl_netif_param_t np;
       rtl_acl_param_t ap;
	int i;
	

	for (i=0;i<8;i++) {
       	 bzero((void *) &np, sizeof(rtl_netif_param_t));
	        np.vid = 8+i;
	        np.valid = 1;
	        np.enableRoute = 0;
	        np.inAclEnd = i;
	        np.inAclStart = 0;
	        np.outAclEnd = 0;
	        np.outAclStart = 0;
	        memcpy(&np.gMac, &eth0_mac[0], 6);
	        np.macAddrNumber = 1;
	        np.mtu = 1500;
	        ret = swCore_netifCreate(i, &np); 
	}
	return;
}


#define ACL_MC_A314FC_TO_CPU 	101		//specific MAC
static void EasyACLRule(int index, int action);

void test_acl(void)
{
	
  	REG32(MSCR) |= EN_IN_ACL;

	EasyACLRule(0, ACL_MC_A314FC_TO_CPU);	

//	EasyACLRule(0, RTL8651_ACL_CPU);	
	EasyACLRule(1, RTL8651_ACL_PERMIT);	

	return;
}

void EasyACLRule(int index, int action)
{
        int ret;
        rtl_acl_param_t ap;			
		
        bzero((void *) &ap, sizeof(rtl_acl_param_t));		
 
	switch(action)
	{

		case ACL_MC_A314FC_TO_CPU:
			ap.actionType_ = RTL8651_ACL_CPU;
			ap.ruleType_ = RTL8651_ACL_MAC;

			ap.un_ty.MAC._dstMac.octet[0]=0x00;
			ap.un_ty.MAC._dstMac.octet[1]=0x1d;		
			ap.un_ty.MAC._dstMac.octet[2]=0x60;
			ap.un_ty.MAC._dstMac.octet[3]=0xa3;
			ap.un_ty.MAC._dstMac.octet[4]=0x14;
			ap.un_ty.MAC._dstMac.octet[5]=0xfc;
			ap.un_ty.MAC._dstMacMask.octet[0]=0xFF;
			ap.un_ty.MAC._dstMacMask.octet[1]=0xFF;
			ap.un_ty.MAC._dstMacMask.octet[2]=0xFF;
			ap.un_ty.MAC._dstMacMask.octet[3]=0xFF;
			ap.un_ty.MAC._dstMacMask.octet[4]=0xFF;
			ap.un_ty.MAC._dstMacMask.octet[5]=0xFF;
			break;

	
		default:
			ap.actionType_ = action;
			break;
	}
 				
	ap.pktOpApp = RTL865XC_ACLTBL_ALL_LAYER;		
	ret = swCore_aclCreate(index, &ap);
        
	if ( ret != 0 ) { 
		printf("EasyACLRule: swCore_aclCreate() failed:%d\n", ret );
	} 	
}
#endif

