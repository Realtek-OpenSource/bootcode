#ifndef _RTK_NAND_BASE_H_
#define _RTK_NAND_BASE_H_

#define SUCCESS		0
#define FAIL		-1
#define NULL		0

#ifdef CONFIG_CHECK_HDR
#define LOAD_HEADER_LEN 0x40 //add 64-bytes header in front of loader image!
#define MAGIC_LOADER 0x6C6F6164 //'load'
#define MAGIC_SQUASH 0x68737173
#endif

#define BACKUP_LOADER 2
#define BACKUP_IMAGE 2

#ifdef CONFIG_NAND_PAGE_2K 
#define BOOT_SIZE	           0x200000
#else
#define BOOT_SIZE	           0x100000
#endif


#define USER_SPACE_SIZE        0x800000
#define VIMG_SPACE_SIZE        0x600000
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#define APPL_SIZE	           0x600000
#else
#define APPL_SIZE	           0x400000
#endif
//#define DRAM_DIMAGE_ADDR       0x80A00000
#define DRAM_DIMAGE_ADDR       MEM_LIMIT /*define in config.mk*/
#define DRAM_TEMP_IMAGE_ADDR   0x80300000
#define DRAM_DLOADER_ADDR      0x80100000
#define DRAM_DOOB_ADDR         0x80200000

#define IMG_START_ADDR     (BOOT_SIZE + USER_SPACE_SIZE)//0x900000 //9MB:   1MB(Boot) + 8MB(user config)
#define IMG_BACKUP_ADDR    (BOOT_SIZE + USER_SPACE_SIZE + VIMG_SPACE_SIZE)//0xF00000 //15MB: 1MB(Boot) + 8MB(user config) + 6MB(vm.img1)
//#define LOADER_BACKUP_ADDR 0x80000 //512K
#define BACKUP_BBT 3


#define DEBUG_READ     0x00000001
#define DEBUG_WRITE    0x00000002
#define DEBUG_PROGRAM  0x00000004
#define DEBUG_ERASE    0x00000008
#define DEBUG_ID       0x00000010
#define DEBUG_MAP      0x00000020
#define DEBUG_PROG16   0x00000040
#define DEBUG_ALWAYS   0xffffffff



/*Nand Flash function prototype*/
struct  rtk_nand_info_s;
typedef struct rtk_nand_info_s rtk_nand_info_t;

typedef void  (nand_read_write_page_t)(unsigned int flash_page, void *dma_addr);
//typedef void  (nand_dma_read_write_t)(void *dma_addr, u32_t dma_len, u32_t blk_pge_addr);
typedef void (nand_pio_write_read_t)(void *addr, unsigned int wr_bytes, unsigned int blk_pge_addr, unsigned int col_addr);

typedef int  (nand_read_page_ecc_t)(unsigned int flash_page, unsigned char *data_addr, unsigned char *oob_addr,
		unsigned int data_size);
typedef void  (nand_write_page_ecc_t)(unsigned int flash_page, unsigned char *data_addr, unsigned char *oob_addr,
		unsigned int data_size);
typedef int  (nand_read_ecc_t)(unsigned int from, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf);
typedef void  (nand_write_ecc_t)(unsigned int to, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf);

//typedef void  (nand_ecc_encode_t)(void *dma_addr, void *p_eccbuf);
//typedef int (nand_ecc_decode_t)(void *dma_addr, void *p_eccbuf);
typedef int (nand_erase_block_t) (unsigned int blk_pge_addr);
typedef int (nand_scan_bbt) (void);
typedef int (nand_check_bb) (unsigned int blk_pge_addr);

//typedef int (nand_chk_eccStatus_t)(void);
//typedef nand_info_t* (nand_probe_t) (void);
typedef void* (nand_probe_t) (void);


struct device_type{
    unsigned char  *name;
#ifdef 	CONFIG_SPI_NAND_FLASH
	unsigned short MAN_ID;
	unsigned short DEV_ID;
#else
    unsigned int id;
#endif
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

extern struct device_type nand_dev_info;


struct rtk_nand_info_s {
    unsigned int            man_id:16;  //Manufacture id 
    unsigned int            dev_id:16;  //Device id
    unsigned int            num_block;
    unsigned int            num_chunk_per_block; /*page per block*/
    unsigned int            chunk_size;
	unsigned int			chip_size; /*flash chip total size*/
    nand_probe_t            *_probe;
	nand_check_bb			*_nand_check_bb;
	nand_scan_bbt			*_nand_scan_bbt;
	nand_erase_block_t      *_erase_block;
    nand_pio_write_read_t   *_nand_pio_write;
    nand_pio_write_read_t   *_nand_pio_read;
    nand_read_write_page_t 	*_nand_read_page; //raw read without ecc decode
    nand_read_write_page_t 	*_nand_write_page;//raw write without ecc encode
    //nand_dma_read_write_t   *_dma_read;
    //nand_dma_read_write_t   *_dma_write;
    nand_read_page_ecc_t   	*_nand_read_page_ecc;
    nand_write_page_ecc_t  	*_nand_write_page_ecc;
    nand_read_ecc_t   		*_nand_read_ecc;
    nand_write_ecc_t  		*_nand_write_ecc;
    //nand_ecc_encode_t       *_ecc_encode;
    //nand_ecc_decode_t       *_ecc_decode;
    //nand_chk_eccStatus_t    *_chk_ecc_error_sts;
};

extern rtk_nand_info_t rtk_nand_base_info;


extern unsigned int ppb; /*page per block*/
extern unsigned int ppb_shift;/*page per block shift ex:ppb=64; ppb_shift=6 (2^6)*/
extern unsigned int chip_size;
extern unsigned int page_size;
extern unsigned int page_shift;
extern unsigned int block_size;
extern unsigned int block_shift;
extern unsigned int pagemask;
extern unsigned int oob_size;
extern unsigned int isLastPage;
extern unsigned int _block_size_;
extern unsigned int _page_size_;

#ifdef CONFIG_NAND_PAGE_2K
#define OOB_SIZE 64
#define CHUNK_SIZE (2048 + 64)
#else
#define CHUNK_SIZE (512 + 16)
#define OOB_SIZE 64
#endif

#define page_number(page) (page & (ppb-1))
#define block_number(page) (page >> ppb_shift)

extern unsigned char NfDataBuf[CHUNK_SIZE];
extern unsigned char NfSpareBuf[OOB_SIZE];

unsigned int shift_value(unsigned int x);
int rtk_block_isbad(unsigned int ofs);
int dump_mem(unsigned int addr, int len);

void delay_msec(unsigned long ms);
#ifdef CONFIG_SPI_NAND_FLASH
void rtk_spi_nand_probe(void);
#endif
#ifdef CONFIG_NAND_FLASH
void rtk_parallel_nand_probe(void);
#endif
#endif /*_RTK_NAND_BASE_H_*/
