
#include "spi_nand_blr_util.h"

#include "spi_nand.h"
#include "rtk_nand_base.h"




#define GD5F1GQ4UAYIG 0xC8F1
	#define GD_MAN_ID 0xC8
	#define GD_DEV_ID 0xF1	
#define W25N01GV			0xEFAA21
#define MID_W25N01GV 		0xEF 	//MAN ID
#define DID_W25N01GV        0xAA21	//DEV ID


#define W25M02GVZEIG 			0xEFAB21
#define MID_W25M02GVZEIG  		0xEF 	//MAN ID
#define DID_W25M02GVZEIG        0xAB21	//DEV ID
struct device_type spi_nand_dev[]=
{
  	{"GD5F1GQ4UAYIG", GD_MAN_ID, GD_DEV_ID, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xFF, 0x00, 0x00, 0x00}, 
	{"W25N01GV", MID_W25N01GV, DID_W25N01GV, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xFF, 0x00, 0x00, 0x00}, 
	{"W25M02GVZEIG", MID_W25M02GVZEIG, DID_W25M02GVZEIG, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xFF, 0x00, 0x00, 0x00}, 
	{NULL,}
};

#define DEBUG_SPI
#ifdef DEBUG_SPI
static unsigned int _debug_ = (DEBUG_ID);
#define DEBUG_SPI_PRINT(mask, string) \
			if ((_debug_ & mask) || (mask == DEBUG_ALWAYS)) \
			printf string
#else
#define DEBUG_SPI_PRINT(mask, string)
#endif

//czyao
#define check_page_align(addr)					\
do {									\
	if (addr & (_plr_spi_nand_info.chunk_size- 1)) {				\
		printf (				\
			"%s: attempt access non-page-aligned data\n",	\
			__func__);					\
		printf (				\
			"%s: chunk_size = 0x%x\n",			\
			__func__,_plr_spi_nand_info.chunk_size);			\
		return -1;						\
	}								\
} while (0)

#define check_block_align(addr)					\
do {									\
	if (addr & (block_size - 1)) {				\
		printf (				\
			"%s: attempt access non-block-aligned data\n",	\
			__func__);					\
		return -1;						\
	}								\
} while (0)

u32_t get_block_page_index(u32_t flash_page)
{
	int block_index;
	int page_index;
	page_index = page_number(flash_page);
	block_index = block_number(flash_page);
	DEBUG_SPI_PRINT(DEBUG_READ,("%s-%d page_index=%d, block_index=%d\n",__func__,__LINE__,page_index,block_index));
	return ((block_index << ppb_shift)|page_index);
}

/*
*	erase only one block per call
*	flash_page : erase block location
*	   return value:
*	 -1: erase fail
*	 0: succeed.
*/
int spi_nand_block_erase(u32_t flash_page)
{
	u32_t blk_pge_addr=0;
	int ret=0;
	//DEBUG_SPI_PRINT(DEBUG_ERASE,("%s-%d flash_page=%d\t",__func__,__LINE__,flash_page));
	blk_pge_addr = get_block_page_index(flash_page);
	printf(".");
	return nasu_block_erase(blk_pge_addr);
}



#define ECSR 0xB801A614 /*ECC Controller Status Register*/
	#define CHECK_ALL_ONE (1 << 4) /*check all data content are all 0xFF*/
/*  Return value:
  *  -1: ECC decode fail
  *  0~6: Number of bits that is correctted
  *  flash_page : read form page number
  *  data_addr : buffer addr ((read from spi nand flash per chunk))
  *  oob_addr : temp spare area addr for each dma read (512+16)
  */
/*Note: 
If you want to check the spare area, 
you should check the data_addr the last 64 bytes
*/  
int spi_nand_read_page_with_ecc(unsigned int flash_page, unsigned char *data_buf, unsigned char *oob_buf, unsigned int data_size)
{
	u32_t blk_pge_addr=0;
	int ret=0;
	//DEBUG_SPI_PRINT(DEBUG_READ,("\n\rXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\r"));
	DEBUG_SPI_PRINT(DEBUG_READ,("%s-%d flash_page=%d\n",__func__,__LINE__,flash_page));
	blk_pge_addr = get_block_page_index(flash_page);
	DEBUG_SPI_PRINT(DEBUG_READ,("%s-%d blk_pge_addr=0x%x\n",__func__,__LINE__,blk_pge_addr));
	//check_page_align(blk_pge_addr);
	ret = nasu_page_read_ecc(data_buf,blk_pge_addr,oob_buf);
	//cache_flush();
	if(ret){
		if(REG32(ECSR) & CHECK_ALL_ONE){
			//printf("ALL_ONE ECC %d pass!\n",flash_page);
			return 0;
		}else{
			return -1;
		}
	}
	//dump_mem(0x20000000|(unsigned int)data_buf, CHUNK_SIZE);
	//DEBUG_SPI_PRINT(DEBUG_READ,("\n\rXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\r"));


	return ret;
}

/*  
  *  flash_page : write into page number
  *  data_addr : buffer addr (write into spi nand flash per chunk)
  *  oob_addr : spare area addr
  */
/*Note: 
If you want to write the spare area, 
you should modify the data_addr the last 64 bytes
*/ 
void spi_nand_write_page_with_ecc(unsigned int flash_page, unsigned char *data_buf, unsigned char *oob_buf, unsigned int data_size)
{
	u32_t blk_pge_addr=0;
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d flash_page=%d\n",__func__,__LINE__,flash_page));
	blk_pge_addr = get_block_page_index(flash_page);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d blk_pge_addr=0x%x\n",__func__,__LINE__,blk_pge_addr));
	//check_page_align(blk_pge_addr);
	nasu_page_write_ecc(data_buf,blk_pge_addr,oob_buf);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d done\n",__func__,__LINE__));
}
/*  
  *  flash_page : write into page number
  *  data_addr : buffer addr (write into spi nand flash per chunk)
*/
void spi_nand_write_page(unsigned int flash_page, unsigned char *data_buf)
{
	u32_t blk_pge_addr=0;
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d flash_page=%d\n",__func__,__LINE__,flash_page));
	blk_pge_addr = get_block_page_index(flash_page);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d blk_pge_addr=0x%x\n",__func__,__LINE__,blk_pge_addr));
	//check_page_align(blk_pge_addr);
	nasu_page_write(data_buf,blk_pge_addr);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d done\n",__func__,__LINE__));
}


/*  
  *  flash_page : read from page number
  *  data_addr : buffer addr (read from spi nand flash per chunk)
*/
void spi_nand_read_page(unsigned int flash_page, unsigned char *data_buf)
{
	u32_t blk_pge_addr=0;
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d flash_page=%d\n",__func__,__LINE__,flash_page));
	blk_pge_addr = get_block_page_index(flash_page);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d blk_pge_addr=0x%x\n",__func__,__LINE__,blk_pge_addr));
	//check_page_align(blk_pge_addr);
	nasu_page_read(data_buf,blk_pge_addr);
	DEBUG_SPI_PRINT(DEBUG_WRITE,("%s-%d done\n",__func__,__LINE__));
}


/*
*	data_buf : buffer (read from spi nand flash indisde a chunk)
*      wr_bytes : read length
*	flash_page : read from which page location
*      col_addr : pio read starting addr inside a page
*/
void spi_nand_pio_read(u32_t flash_page, void *data_buf, u32_t wr_bytes, u32_t col_addr)
{
	u32_t blk_pge_addr=0;
	blk_pge_addr = get_block_page_index(flash_page);
	//check_page_align(blk_pge_addr);
	nasu_pio_read(data_buf,wr_bytes,blk_pge_addr,col_addr);
}


/*
*	data_buf : buffer (read from spi nand flash indisde a chunk)
*      wr_bytes : read length
*	flash_page : read from which page location
*      col_addr : pio read starting addr inside a page
*/
void spi_nand_pio_write(u32_t flash_page, void *data_buf, u32_t wr_bytes, u32_t col_addr)
{
	u32_t blk_pge_addr=0;
	blk_pge_addr = get_block_page_index(flash_page);
	//check_page_align(blk_pge_addr);
	nasu_pio_write(data_buf,wr_bytes,blk_pge_addr,col_addr);
}

void rtk_spi_nand_probe(void)
{
	//int ret;
	unsigned short maker_code, device_code;
	unsigned int spi_nand_id;
	int i=0;

/*
	chip_size = blr_spi_nand_info.num_block \
				* blr_spi_nand_info.num_chunk_per_block \
				* blr_spi_nand_info.chunk_size;
	block_size = blr_spi_nand_info.num_chunk_per_block * blr_spi_nand_info.chunk_size;
*/	
//	page_size = blr_spi_nand_info.chunk_size;
	rtk_nand_base_info._nand_read_page_ecc = spi_nand_read_page_with_ecc; 
	rtk_nand_base_info._nand_write_page_ecc = spi_nand_write_page_with_ecc;		
	rtk_nand_base_info._nand_read_page = spi_nand_read_page;
	rtk_nand_base_info._nand_write_page = spi_nand_write_page;
	rtk_nand_base_info._erase_block = spi_nand_block_erase;
//	isLastPage = 0;
	memset(&nand_dev_info, 0, sizeof(struct device_type));
	maker_code = _plr_spi_nand_info->man_id;
	device_code = _plr_spi_nand_info->dev_id;

//DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d maker_code=%x, device_code=%x spi_nand_id=%x\n",__func__,__LINE__,maker_code,device_code,spi_nand_id)); 		
	
	for(i=0;spi_nand_dev[i].name;i++){
		if(spi_nand_dev[i].MAN_ID == maker_code && spi_nand_dev[i].DEV_ID == device_code){
DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d spi nand %s probe\n",__func__,__LINE__,spi_nand_dev[i].name));			
//DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d spi nand ID:0x%x\n",__func__,__LINE__,spi_nand_dev[i].id));
			chip_size = spi_nand_dev[i].chipsize;	
			page_size = spi_nand_dev[i].PageSize;
			block_size = spi_nand_dev[i].BlockSize;
			oob_size = spi_nand_dev[i].OobSize;
			//num_chips = nand_device[i].num_chips;
			isLastPage = spi_nand_dev[i].isLastPage;
			ppb = 32*(_plr_spi_nand_info->_num_page_per_block+1);
			ppb_shift = shift_value(ppb)-1;
			memcpy(&nand_dev_info, &spi_nand_dev[i], sizeof(struct device_type));			
			break;
		}else{
			printf("%s-%d spi nand probe fail!\n",__func__,__LINE__);
		}
	}
	page_shift = shift_value(page_size) -1;
	block_shift = shift_value(block_size) -1 ;
	pagemask = (chip_size/page_size) -1;
	_page_size_ = (page_size + oob_size);//include oob size
	_block_size_ = (_page_size_ << ppb_shift);//include oob size
	//blr_spi_info.nand_spi_probe_t();
	//DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d _soc=%p\n",__func__,__LINE__,_soc));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d chip_size=0x%x\n",__func__,__LINE__,chip_size));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d block_size=0x%x\n",__func__,__LINE__,block_size));	
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d _block_size_=0x%x\n",__func__,__LINE__,_block_size_));	
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d _page_size_=0x%x\n",__func__,__LINE__,_page_size_));			
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d ppb=0x%x\n",__func__,__LINE__,ppb));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d ppb_shift=0x%x\n",__func__,__LINE__,ppb_shift));	
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d page_size=0x%x\n",__func__,__LINE__,page_size));	
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d oob_size=0x%x\n",__func__,__LINE__,oob_size));

	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d page_shift=0x%x\n",__func__,__LINE__,page_shift));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d block_shift=0x%x\n",__func__,__LINE__,block_shift));	
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d pagemask=0x%x\n",__func__,__LINE__,pagemask));


	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d man_id=0x%x\n",__func__,__LINE__,maker_code));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d dev_id=0x%x\n",__func__,__LINE__,device_code));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d num_block=0x%x\n",__func__,__LINE__,512*(_plr_spi_nand_info->_num_block+1)));
	DEBUG_SPI_PRINT(DEBUG_ID,("%s-%d chunk_size=0x%x\n",__func__,__LINE__,1024*(_plr_spi_nand_info->_page_size+1)));	
	//return ret;
}

/*
void spi_nand_pio_read(void *addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr)
{
	blr_spi_info._pio_read(addr,wr_bytes,blk_pge_addr,col_addr);
}

int spi_nand_pio_write()
{

	return 0;
}
*/
#if 0
//read data more than one chunk
//form is page alligment
int spi_nand_read_with_ecc(unsigned int from, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
	nand_spi_info_t *blr_spi_info;
	blr_spi_info = &spi_nand_info.blr_spi_info;
	
	if ((from + len) > spi_nand_info.chip_size) {		
		printf ("%s-%d: Attempt read beyond end of device\n",__func__,__LINE__);
		return FAIL;	
	}	
	blr_spi_info._chunk_read_ecc();
}

//write data more than one chunk
//to is page alligment
int spi_nand_write_with_ecc(unsigned int to, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
	nand_spi_info_t *blr_spi_info;
	blr_spi_info = &spi_nand_info.blr_spi_info;
	return blr_spi_info._chunk_read_ecc;
}
#endif




#if 0
/*
*	erase len per call
*	flash_page : erase block location
*      return value:
*	 -1: erase fail
*	 0: succeed.
*/
int spi_nand_erase(u32_t flash_page, unsigned int len)
{
	u32_t blk_pge_addr=0;
	int ret=0;
	blk_pge_addr = flash_page * blr_spi_nand_info.chunk_size;
	check_block_align(blk_pge_addr);
	printf(".");
	return blr_spi_nand_info._erase_block(blk_pge_addr);
}
#endif


