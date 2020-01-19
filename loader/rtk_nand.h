#ifndef _NAND_FLASH_H_
#define _NAND_FLASH_H_

//ccwei 111107
#include "global.h"
//end
#define rtk_readb(offset)         (*(volatile unsigned char *)(offset))
#define rtk_readw(offset)         (*(volatile unsigned short *)(offset))
#define rtk_readl(offset)         (*(volatile unsigned long *)(offset))

#define rtk_writeb(val, offset)    (*(volatile unsigned char *)(offset) = val)
#define rtk_writew(val, offset)    (*(volatile unsigned short *)(offset) = val)
#define rtk_writel(val, offset)    (*(volatile unsigned long *)(offset) = val)


/*
 * NAND flash controller address
 *  czyao , nand flash address
 */
#define NAND_CTRL_BASE  0xB801A000
#define NACFR  (NAND_CTRL_BASE + 0x0)
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	#define NAFC_NF 	(1<<27)
	#define WP_B		(1<<23)
	#define BCH_12T_en  (1<<22)
	#define BCH_24T_en	(1<<21)
#else
	#define NAFC_NF		(1<<28)
#endif	
#define NACR    (NAND_CTRL_BASE + 0x04)
	#define flash_READY  	(1<<31)
	#define ECC_enable    	(1<<30)
	#define RBO		     	(1<<29)
	#define WBO		     	(1<<28)
#define NACMR    (NAND_CTRL_BASE + 0x08)
	#define CECS1 		    (1<<31)
	#define CECS0	     	(1<<30)
	#define Chip_Seletc_Base	30
#define NAADR    (NAND_CTRL_BASE + 0x0C)
	#define enNextAD		(1<<27)
	#define AD2EN		    (1<<26)
	#define AD1EN		    (1<<25)
	#define AD0EN		    (1<<24)
	#define CE_ADDR2		16
	#define CE_ADDR1		8
	#define CE_ADDR0		0
#define NADCRR   (NAND_CTRL_BASE + 0x10)
	#define TAG_DIS		    (1<<6)
	#define DESC1		    (1<<5)
	#define DESC0		    (1<<4)
	#define DMARE		    (1<<3)
	#define DMAWE		    (1<<2)
	#define LBC_128		    3
	#define LBC_64		    2
	#define LBC_32		    1
	#define LBC_16		    0
#define NADR      (NAND_CTRL_BASE + 0x14)
#define NADFSAR   (NAND_CTRL_BASE + 0x18)
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685s)
#define NADFSAR2  (NAND_CTRL_BASE + 0x1C)
#define NADRSAR   (NAND_CTRL_BASE + 0x20)
#define NADTSAR   (NAND_CTRL_BASE + 0x24)
#define NASR      (NAND_CTRL_BASE + 0x28)
#define NECN			    (0xf<<4)
#else
#define NADRSAR   (NAND_CTRL_BASE + 0x1C)
#define NASR      (NAND_CTRL_BASE + 0x20)
#define NADTSAR	  (NAND_CTRL_BASE + 0x54)
#define NECN			    (1<<4)
#endif
#define NRER			    (1<<3)
#define NWER			    (1<<2)
#define NDRS			    (1<<1)
#define NDWS		        (1<<0)
#define M_mask		0xe0000000


/* NAND Flash Command Sets */
#define CMD_READ_ID				0x90
#define CMD_READ_STATUS		0x70

#define CMD_PG_READ_C1		0x00
#define CMD_PG_READ_C2		0x30
#define CMD_PG_READ_C3		CMD_READ_STATUS

#define CMD_PG_READ_A		0x00
#define CMD_PG_READ_B		0x01
#define CMD_PG_READ_C		0x50

#define CMD_PG_WRITE_C1		0x80
#define CMD_PG_WRITE_C2		0x10
#define CMD_PG_WRITE_C3		CMD_READ_STATUS

#define CMD_BLK_ERASE_C1		0x60	
#define CMD_BLK_ERASE_C2		0xd0	
#define CMD_BLK_ERASE_C3		CMD_READ_STATUS	

#define CMD_RESET                 		0xff

#if 0

struct device_type{
    unsigned char  *name;
    unsigned int id;
    unsigned int size;		
    unsigned int chipsize;		
    unsigned short PageSize;
    unsigned int BlockSize;
    unsigned short OobSize;
    unsigned char num_chips;
    unsigned char isLastPage;	
    unsigned char CycleID5th; 
    unsigned char T1;
    unsigned char T2;
    unsigned char T3;
};




#endif
//-------------------Reserve Block Area usage ---------------------//

#ifdef CONFIG_NAND_PAGE_512
#define 	LOADER_IMAGE_SIZE (64 * 1024)
#else
#define 	LOADER_IMAGE_SIZE (128 * 1024)
#endif



//--------------------- Supported Nand Flash ---------------------//
#define H27U2G8F2C		0xADDA9095	//SLC, 256 MB, 1 dies, 2K page

/*ccwei*/
#define K9F1G08U0B		0xECF10095	//SLC, 128 MB, 1 dies, 2K page
#define K9F1G08U0D		0xECF10015	//SLC, 128 MB, 1 dies, 2K page
#define MX30LF1G08AA	0xC2F1801D	//SLC, 128MB,  1 dies, 2K page
#define MX30LF1G08AC	0xC2F18095	//SLC, 128MB,  1 dies, 2K page
#define MX30LF1208AA	0xC2F0801D	//SLC, 64MB,  1 dies, 2K page
#define NAND256W3A		0x20752075	//SLC, 32MB,    1dies, 512 Bytes page
#define K9G8G08U0A      0xECD314A5  //5195
#define MT29F1G08A		0x2CF18095	//Micron, SLC, 128MB,  1 dies, 2K page
#define F59L1G81A 		0x92F18095	//SLC, 128MB, 1dies, 2K page
#define S34ML01G1		0x01F1001D
#define S34ML01G200		0x01F1801D
#define W29N01GV		0xEFF18095
#define F59L1G81MA		0xC8D18095
#define A5U1G31AT		0x92F18095	//Zentel, SLC, 128MB, 1 dies, 2K page

#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#define NAND_ADDR_MASK	(3<<28)
#define NAND_ADDR_CYCLE	(((*(volatile unsigned int *)((NACFR)) & NAND_ADDR_MASK) == 0) ? 1:0)
#else
#define Boot_Select			0xB8000304
#define NAND_ADDR_MASK		(3<<7)
/*NAND_ADDR_CYCLE = 1, address cycle=3, NAND_ADDR_CYCLE=0, address cycle=4 or 5*/
#define NAND_ADDR_CYCLE	(((*(volatile unsigned int *)((Boot_Select)) & NAND_ADDR_MASK) == 0) ? 1:0)
#endif

#ifdef CONFIG_RTK_REMAP_BBT
extern struct BBT_v2r *bbt_v2r;
/*
    #ifdef CONFIG_NAND_PAGE_512
    #define RESERVED_AREA      0x100000  //reserved 0x10_0000 1M Bytes area for bootloader
    #else
    #define RESERVED_AREA      0x200000  //reserved 0x20_0000 2M Bytes area for bootloader
    #endif
*/    
#endif





//--------------------- Function ---------------------//
void check_ready(void);
void rtk_nand_read_id(void);
int rtk_block_isbad(unsigned int flash_page);
int rtk_check_pageData(int page, int offset);
BOOL rtk_read_ecc_page (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr,unsigned int image_size);
BOOL rtk_write_ecc_page (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr,unsigned int image_size);

//ccwei: 120229
#ifdef CONFIG_NAND_PAGE_2K
void rtk_PIO_read(int page, int offset, int length, unsigned char * buffer);
#endif

#ifdef CONFIG_NAND_PAGE_512
int rtk_PIO_read_512(int page, unsigned char * buffer);
void rtk_PIO_write_512(int page, int offset, int length, unsigned char * buffer);
#endif

int nand_read_ecc (unsigned int flash_page, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf);
int nand_write_ecc (unsigned int flash_page, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf);
int nand_erase_nand (unsigned int flash_page, unsigned int len);

BOOL rtk_erase_block (int page);
int dump_mem(unsigned int addr, int len);

//--------------------- extern value ---------------------//
extern unsigned char nand_id[6];
#endif
