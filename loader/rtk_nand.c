/*  
 *     Realtek NAND flash driver for bootcode
 *     2010/10/14 v0.0
 *     Author: czyao
 *
 */

#include "rtk_nand.h"
#include "rtk_nand_base.h"
#include "rtk_bbt.h"
//#define DEBUG
#ifdef  DEBUG

#define DEBUG_ALWAYS   0xffffffff

static unsigned int debug = 0;
    #define DEBUG_PRINT(mask, string) \
                if ((debug & mask) || (mask == DEBUG_ALWAYS)) \
                printf string
#else
    #define DEBUG_PRINT(mask, string)
#endif
//end ccwei








//ccwei: 120229
#ifdef CONFIG_NAND_PAGE_2K
void rtk_PIO_read(int page, int offset, int length, unsigned char * buffer)
{
	int i;
	unsigned int flash_addr1, flash_addr2;
	unsigned int data_out;
	printf("%s: page %d offset %d len %d\n",__FUNCTION__, page, offset, length);
	rtk_writel(0xc00fffff, NACR);

	/* Command cycle 1*/
	rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);

	check_ready();

	flash_addr1 =  ((page & 0xff) << 16) | offset;
    flash_addr2 = (page >> 8) & 0xffffff;

	/* Give address */
	

	rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
	rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);

	/* Command cycle 2*/

	rtk_writel((CECS0|CMD_PG_READ_C2), NACMR);
	check_ready();
		
	
	check_ready();

	for(i=0; i<(length/4); i++){
		data_out = rtk_readl(NADR);
		memcpy( buffer+i*4, &data_out, 4);
	}

	check_ready();
	rtk_writel(0, NACMR);

	return;
	
}
#endif

#if 0
BOOL rtk_erase_block (int page)
{
	int addr_cycle[5], page_shift;

	//printf("!!!!! rtk_erase_block !!!!!, block = %d\n\r",page/ppb);

	if ( page & (ppb-1) ){
		printf("page %d is not block alignment !!\n", page);
		return 0;
	}
	printf(".");
	check_ready();

	rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO), NACR);
	rtk_writel((NWER|NRER|NDRS|NDWS), NASR);
	rtk_writel(0x0, NACMR);

	rtk_writel((CECS0|CMD_BLK_ERASE_C1),NACMR);
	check_ready();


	if(!NAND_ADDR_CYCLE){
		addr_cycle[0] = addr_cycle[1] =0;
		for(page_shift=0; page_shift<3; page_shift++){
			addr_cycle[page_shift+2] = (page>>(8*page_shift)) & 0xff;
		}

		rtk_writel( ((~enNextAD) & AD2EN|AD1EN|AD0EN|
				(addr_cycle[2]<<CE_ADDR0) |(addr_cycle[3]<<CE_ADDR1)|(addr_cycle[4]<<CE_ADDR2)),NAADR);
	}else{
		addr_cycle[0] = 0;
		for(page_shift=0; page_shift<4; page_shift++){
			addr_cycle[page_shift+1] = (page>>(8*page_shift)) & 0xff;
		}

		rtk_writel( ((~enNextAD) & AD2EN|AD1EN|AD0EN|
				(addr_cycle[1]<<CE_ADDR0) |(addr_cycle[2]<<CE_ADDR1)|(addr_cycle[3]<<CE_ADDR2)),NAADR);
	}



	rtk_writel((CECS0|CMD_BLK_ERASE_C2),NACMR);
	check_ready();

	rtk_writel((CECS0|CMD_BLK_ERASE_C3),NACMR);
	check_ready();

	if(rtk_readl(NADR) & 0x01){
		if( page>=0 && page < ppb)
			return SUCCESS;
		else
			return FAIL;
	}
		
	return SUCCESS;

}


//####################################################################
// Function : rtk_write_ecc_page
// Description : Write image from DRAM to NAND flash
// Input:
//		flash_page	: address of flash page
//		image_addr     : address of image, in dram address
//		oob_addr		: address of oob, in dram address
//		image_size	: the length of image
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
BOOL rtk_write_ecc_page (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr,
		unsigned int image_size)
{
	
	int dram_sa, oob_sa;	
	int dma_counter = page_size >> 9;	//Move unit=512Byte 
	int dma_size;
	int buf_pos=0;
	int page_counter=0;
	int page_num[3], page_shift=0, page_start;
	unsigned long flash_addr_t=0, flash_addr_t1;
	//unsigned char *oob_buf;
	unsigned char switch_bbi=0, write_bbi=0;;
	unsigned char *data_ptr;

	rtk_writel(0xc00fffff, NACR);


	/*if ( (flash_address % (page_size))!=0 ){
		printf("\n\rflash_address must be 2KB aligned!!");
		return FAIL;
	}*/

	//Page size alignment

	dma_size = (image_size+page_size-1)&(~(page_size-1));	

	//Translate nand flash address
	page_start = flash_page;   
	
	for(page_shift=0;page_shift<3; page_shift++) {
		 page_num[page_shift] = ((page_start>>(8*page_shift)) & 0xff);
		 if(!NAND_ADDR_CYCLE)
		 	 flash_addr_t |= (page_num[page_shift] << (12+8*page_shift));
		 else
		 	 flash_addr_t |= (page_num[page_shift] << (9+8*page_shift));
    	}

	//Setting default value of flash_addr_t1
	flash_addr_t1 = flash_addr_t;

	if(isLastPage){
		write_bbi = flash_page & (ppb-1);
		if((write_bbi == (ppb-2)) || (write_bbi ==(ppb-1))){
			switch_bbi = 1;
		}
	}else{
		write_bbi = flash_page & (ppb-1);
		if((write_bbi == 0) || (write_bbi ==1)){
			switch_bbi = 1;
		}
	}
	if(!NAND_ADDR_CYCLE)
	{
		unsigned char temp_val;
		if(switch_bbi && (flash_page > ppb-1)){
			memcpy(NfDataBuf,image_addr,page_size);
			temp_val = NfDataBuf[DATA_BBI_OFF];
			NfDataBuf[DATA_BBI_OFF] = oob_addr[OOB_BBI_OFF];
			oob_addr[OOB_BBI_OFF] = temp_val;
			data_ptr = NfDataBuf;
		}
		else
			data_ptr = image_addr;			
	}
	else
		data_ptr = image_addr;			
	while( dma_size>0 ){

		if( (dma_size%(ppb*page_size)) == 0)
			printf(".");

		dma_counter = page_size >> 9;

		dma_cache_writeb();

		if(oob_addr){
			dma_cache_writeb();
		}

		while(dma_counter >0) {  //Move 1 page

			check_ready();
			rtk_writel( (rtk_readl(NACR) |ECC_enable & (~RBO) & (~WBO)), NACR);

			//set DMA RAM start address
			//dram_sa = ((unsigned int)image_addr+buf_pos*512) & (~M_mask);
			dram_sa = ((unsigned int)data_ptr+buf_pos*512) & (~M_mask);
			rtk_writel( dram_sa, NADRSAR);
			//printf("NADRSAR : 0x%08X ",rtk_readl(NADRSAR));

			//set DMA OOB start address
			oob_sa = ((unsigned int) oob_addr+buf_pos*16) & (~M_mask);
			rtk_writel( oob_sa, NADTSAR);

			//set DMA flash start address
			rtk_writel( flash_addr_t, NADFSAR);
			//printf("NADFSAR : 0x%08X \n\r",rtk_readl(NADFSAR));
			flash_addr_t +=528;

			//set OOB address
			rtk_writel(oob_sa, NADTSAR);
			
			//DMA write
			rtk_writel( (~TAG_DIS) & (DESC0|DMAWE|LBC_128),NADCRR);	
			check_ready();

			if(FAIL== rtk_check_pageData((page_start+page_counter), buf_pos*(512+16)))
				return FAIL;
			
			dma_counter--;
			buf_pos++;

		}

		page_counter +=1;

		if(!NAND_ADDR_CYCLE)
			flash_addr_t1 =page_counter*0x1000;
		else
			flash_addr_t1 =page_counter*0x200;
		
		flash_addr_t = flash_addr_t1;
		dma_size -= page_size;

	}

	//free(oob_buf);
	return SUCCESS;
}
#endif

#ifdef CONFIG_NAND_PAGE_2K
void rtk_PIO_write(int page, int offset, int length, unsigned char * buffer)
{
	int i;
	unsigned int flash_addr1, flash_addr2;
	unsigned int write_data;
	
	rtk_writel(0xc00fffff, NACR);
	
	/* Command cycle 1*/
	rtk_writel((CECS0|CMD_PG_WRITE_C1), NACMR);

	check_ready();

	flash_addr1 =  ((page & 0xff) << 16) | offset;
	flash_addr2 = (page >> 8) & 0xffffff;
	
	/* Give address */
	rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
	rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);

	for(i=0; i<(length/4); i++){
		memcpy(&write_data, (buffer+4*i), 4);
		rtk_writel(write_data ,NADR);
	}

	/* Command cycle 2*/
	rtk_writel((CECS0|CMD_PG_WRITE_C2), NACMR);
	check_ready();

	rtk_writel(0, NACMR);
	check_ready();
	
}
#endif


#ifdef CONFIG_NAND_PAGE_512 // PIOR

void rtk_PIO_write_512(int page, int offset, int length, unsigned char * buffer)

{
	int i;
	unsigned int flash_addr1;
	unsigned int write_data;
	unsigned int data_out;
    printf("pio write page:%d offset:%d len:%d \n",page, offset, length);
	for(i=0;i<16;i++){
        printf("%x ", buffer[i]);
	}
    printf("%s%d:\n",__FUNCTION__,__LINE__);
	
	rtk_writel(0xc00fffff, NACR);
	
	/* Command cycle 1*/
	rtk_writel((CECS0|CMD_PG_WRITE_C1), NACMR);

	check_ready();

	flash_addr1 |= ((page & 0xffffff) << 8);
    printf("%s%d:\n",__FUNCTION__,__LINE__);	
	/* Give address */
	rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);

	for(i=0; i<(length/4); i++){
		memcpy(&write_data, (buffer+4*i), 4);
		rtk_writel(write_data ,NADR);
	}
    printf("%s%d:\n",__FUNCTION__,__LINE__);

	/* Command cycle 2*/
	rtk_writel((CECS0|CMD_PG_WRITE_C2), NACMR);
	check_ready();
//check status
	rtk_writel((CECS0|CMD_PG_WRITE_C3), NACMR);

	check_ready();
    printf("%s%d:\n",__FUNCTION__,__LINE__);

	data_out = rtk_readl(NADR);
    printf("read write status 0x%x\n", data_out);
	if(data_out & 0x1)
		printf("write error!!!\n");
	check_ready();

	rtk_writel(0, NACMR);
	check_ready();

	return;
	
}

int rtk_PIO_read_basic_data(int page, int offset, unsigned char * buffer)
{
	int i;
	unsigned int flash_addr1=0;
	unsigned int data_out;
	int rlen, real_page;
	unsigned int cmd;

	real_page = page;

/*
	if(offset==0 || offset==256){
		rlen = page_size/2;
		
		if(offset==0)
			cmd = CMD_PG_READ_A;
		else
			cmd = CMD_PG_READ_B;
		
	}else{
		rlen = oob_size;
		cmd = CMD_PG_READ_C;
	}
*/
	rlen = page_size + oob_size;
	cmd = CMD_PG_READ_A;

	rtk_writel(0xc00fffff, NACR);

	/* Command cycle 1*/
	rtk_writel((CECS0|cmd), NACMR);

	check_ready();

	flash_addr1 |= ((real_page & 0xffffff) << 8) ;

	/* Give address */
	rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);

	check_ready();

	for(i=0; i<(rlen/4); i++){
		data_out = rtk_readl(NADR);
		//printf("[%d] data_out = 0x%08X\n", i, data_out);
		memcpy( buffer+i*4, &data_out, 4);
	}

	check_ready();
	rtk_writel(0, NACMR);
	
	return 0;
	
}

int rtk_PIO_read_512(int page, unsigned char * buffer)
{
	int rc;
	/* Call rtk_PIO_read_basic */

	/* First 256 bytes*/
	rc = rtk_PIO_read_basic_data(page, 0, buffer);
#if 0
	/* Last 256 bytes*/
	rc = rtk_PIO_read_basic_data(page, 256, buffer+256);

	/* OOB area */	
	rc = rtk_PIO_read_basic_data(page, 512, buffer+512);
#endif
	return rc;
}
#endif //CONFIG_NAND_PAGE_512


