#include "rtk_nand.h"
#include "rtk_nand_base.h"
#include "rtk_bbt.h"
#define DEBUG_PRINT(mask, string)
#if 1
rtk_nand_info_t rtk_nand_base_info;
unsigned int chip_size = 0;
unsigned int ppb; /*page per block*/
unsigned int ppb_shift;/*page per block shift ex:ppb=64; ppb_shift=6 (2^6)*/
unsigned int page_size;
unsigned int page_shift;
unsigned int block_size;
unsigned int _block_size_;
unsigned int _page_size_;
unsigned int block_shift;
unsigned int pagemask;
unsigned int oob_size;
unsigned int isLastPage = 0;
struct device_type nand_dev_info;
#endif
#ifdef CONFIG_NAND_PAGE_2K
unsigned char NfDataBuf[CHUNK_SIZE] __attribute__((__aligned__(32)));
unsigned char NfSpareBuf[OOB_SIZE] __attribute__((__aligned__(32)));
#else
unsigned char NfDataBuf[CHUNK_SIZE] __attribute__((__aligned__(32)));
unsigned char NfSpareBuf[OOB_SIZE] __attribute__((__aligned__(32)));
#endif
unsigned char nand_id[6];
//NAND flash structure

struct device_type nand_device[]=
{
  	{"H27U2G8F2C", H27U2G8F2C, 0x10000000, 0x10000000, 2048, 64*2048, 64, 1, 1, 0x44, 0x00, 0x00, 0x00}, 
/*ccwei*/
	{"K9F1G08U0B", K9F1G08U0B, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x40, 0x00, 0x00, 0x00} ,
	{"K9F1G08U0D", K9F1G08U0D, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x40, 0x00, 0x00, 0x00} ,
	{"MX30LF1G08AA", MX30LF1G08AA, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xff, 0x00, 0x00, 0x00},
	{"MX30LF1G08AC", MX30LF1G08AC, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x02, 0x00, 0x00, 0x00},
	{"MX30LF1208AA", MX30LF1208AA, 0x4000000, 0x4000000, 2048, 64*2048, 64, 1, 0, 0xff, 0x00, 0x00, 0x00},
	{"NAND256W3A", NAND256W3A, 0x2000000, 0x2000000,  512, 32*512, 16, 1, 0, 0xff, 0x00, 0x00, 0x00},
/*end-ccwei*/
	{"MT29F1G08A", MT29F1G08A, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x02, 0x00, 0x00, 0x00} ,  	
	{"F59L1G81A", F59L1G81A, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x40, 0x00, 0x00, 0x00} ,
	{"S34ML01G1", S34ML01G1, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xff, 0x00, 0x00, 0x00} ,
	{"F59L1G81MA", F59L1G81MA, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x40, 0x00, 0x00, 0x00} ,	
	{"S34ML01G200", S34ML01G200, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0xff, 0x00, 0x00, 0x00} ,	
	{"W29N01GV", W29N01GV, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x00, 0x00, 0x00, 0x00} ,	
	{"A5U1G31AT", A5U1G31AT, 0x8000000, 0x8000000, 2048, 64*2048, 64, 1, 0, 0x40, 0x00, 0x00, 0x00} ,	
	{NULL,}
};

void cache_flush_nand(void){
	__asm__ volatile(
			".set  push\n\t"
			".set  noreorder\n\t"
			"mfc0    $8, $20\n\t"
			"ori     $8, 0x202\n\t"
			"xori    $9, $8, 0x202\n\t"
			"mtc0    $9, $20\n\t"
			"mtc0    $8, $20\n\t"
			".set pop\n"
			"nop"
			: /* no output */
			: /* no input */
				);
}


void dma_cache_writeb(void)
{
	cache_flush_nand();
}


void check_ready(void)
{
	while(1) {
		if(  ( rtk_readl(NACR) & 0x80000000) == 0x80000000 ) 
			break;
	}
}

void rtk_nand_read_id(void)
{
	int id_chain;
	//unsigned char id[5],i;

	check_ready();
	rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO|WBO), NACR);

	rtk_writel(0x0, NACMR);	
	rtk_writel( (CECS0|CMD_READ_ID) , NACMR);          //read ID command
	check_ready();

	rtk_writel( (0x0 |AD2EN|AD1EN|AD0EN) , NAADR);  //dummy address cycle
	check_ready();
	
	id_chain = rtk_readl(NADR);
	nand_id[0] = id_chain & 0xff;
	nand_id[1] = (id_chain >> 8) & 0xff;
	nand_id[2] = (id_chain >> 16) & 0xff;
	nand_id[3] = (id_chain >> 24) & 0xff;

	id_chain = rtk_readl(NADR);
	nand_id[4] = id_chain & 0xff;

	rtk_writel( 0x0, NACMR);
	rtk_writel( 0x0, NAADR);


#if 0	
	int i;
	printf("NAND Flash ID: ");
	for(i=0;i<5;i++){
		printf("%x ",nand_id[i]);
	}
	printf("\n\r");
#endif

	return;
}

unsigned int shift_value(unsigned int x)
{
	int r = 1;

        if (!x)
                return 0;
        if (!(x & 0xffff)) {
                x >>= 16;
                r += 16;
        }
        if (!(x & 0xff)) {
                x >>= 8;
                r += 8;
        }
        if (!(x & 0xf)) {
                x >>= 4;
                r += 4;
        }
        if (!(x & 3)) {
                x >>= 2;
                r += 2;
        }
        if (!(x & 1)) {
                x >>= 1;
                r += 1;
        }
        return r;
}

#ifdef CONFIG_NAND_PAGE_512
int rtk_PIO_read_basic(int page, int offset)
{
	int i;
	unsigned int flash_addr1=0;
	unsigned int data_out;
	int rlen, real_page;
	unsigned int cmd;

	real_page = page;

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
		//printf("[%3d] 0x%08X \n",i, data_out);
		if(data_out!=0xffffffff){
			printf("[%3d] 0x%08X \n",i, data_out);
			printf("%s: page %d offset %d i %d rlen %d\n",__FUNCTION__, page, offset, i, rlen);
			return -1;
		}
	}

	rtk_writel(0, NACMR);
	check_ready();
	
	return 0;
	
}

int rtk_check_allone_512(int page)
{
	int rc=0;

	rc = rtk_PIO_read_basic(page,0);
	if(rc < 0)
		goto read_finish;

read_finish:
	return rc;
}
#else
int rtk_check_allone(int page, int offset)
{
	unsigned int flash_addr1, flash_addr2;
	unsigned int data_out;
	int real_page, i, rlen;;

	real_page = page;

	/* rlen is equal to (512 + 16) */
	rlen = 528; 
	
	rtk_writel(0xc00fffff, NACR);

	/* Command cycle 1*/
	rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);

	check_ready();

	flash_addr1 =  ((real_page & 0xff) << 16) | offset;
	flash_addr2 = (real_page >> 8) & 0xffffff;

	/* Give address */
	rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
	rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);

	/* Command cycle 2*/
	rtk_writel((CECS0|CMD_PG_READ_C2), NACMR);

	check_ready();

	for(i=0; i<(rlen/4); i++){
		data_out = rtk_readl(NADR);	
		if( data_out != 0xffffffff){
			printf("%s, page %d offset %d i %d\n",__FUNCTION__, page, offset, i);
			return -1;
		}
	}

	check_ready();
	rtk_writel(0, NACMR);

	return 0;

}
#endif

int rtk_check_pageData(int page, int offset)
{
	int rc = 0;
	int error_count,status;

	status = rtk_readl(NASR);

	if( (status & NDRS)== NDRS){		

		 if( status & NRER) {
			error_count = (status & 0xf0) >> 4;
			
			if(error_count <=4 && error_count > 0 ) {
				printf("[%s] boot: Correctable HW ECC Error at page=%u, status=0x%08X\n\r", __FUNCTION__, page,status);
				status &= 0x0f; //clear NECN
				rtk_writel(status, NASR);
				return 0;
			}else{			
#ifdef CONFIG_NAND_PAGE_512
				if( rtk_check_allone_512(page) == 0 ){
					status &= 0x0f; //clear NECN
					rtk_writel(status, NASR);
				    	//printf("[%s] boot: Page %d is all one page, bypass it !!\n\r",__func__,page);
				    	return 0;
				}
#else
				if( rtk_check_allone(page,offset) == 0 ){
					status &= 0x0f; //clear NECN
					rtk_writel(status, NASR);
				    	//printf("[%s] Page %d is all one page, bypass it !!\n\r",__func__,page);
				    	return 0;
				}
#endif			
				printf("[%s] boot: Un-Correctable HW ECC Error at page=%u, status=0x%08X error_count %d\n\r", __FUNCTION__, page,status, error_count);
				status &= 0x0f; //clear NECN
				rtk_writel(status, NASR);
				return -1;				
			}
		}
		
	}
	else if( (status & NDWS)== NDWS){
		 if( status & NWER) {
			printf("[%s] boot: NAND Flash write failed at page=%u, status=0x%08X\n\r", __FUNCTION__, page,status);
			rtk_writel(status, NASR);
			return -1;
		}
	}

	rtk_writel(status, NASR);

	return rc;
}

//####################################################################
// Function : rtk_block_isbad
// Description : check bad block
// Input:
//		flash_page: page offset
// Output:
//		BOOL: 0=>OK, 
//			 -1=>This block is bad, 
//			   1=>Read oob area fail,
//####################################################################
int rtk_block_isbad(unsigned int flash_page)
{
	unsigned int page, block, page_offset;
	unsigned char block_status_p1;
	int i;
	unsigned char *oob_buf = NfSpareBuf;
	unsigned char *data_buf = NfDataBuf;
	page = flash_page;
	page_offset = page & (ppb-1);
	block = page/ppb;
	if ( isLastPage ){
		page = block*ppb + (ppb-1);	
		if(rtk_nand_base_info._nand_read_page_ecc(page, data_buf, oob_buf, page_size)){
			printf("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}
#ifdef CONFIG_SPI_NAND_FLASH
			block_status_p1 = data_buf[2048];
#else
        if(!NAND_ADDR_CYCLE)		
		    block_status_p1 = oob_buf[OOB_BBI_OFF];
		else
			block_status_p1 = oob_buf[5];
#endif		
	}else{	
		if(rtk_nand_base_info._nand_read_page_ecc(page, data_buf, oob_buf, page_size)){
			printf ("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}
#ifdef CONFIG_SPI_NAND_FLASH
		block_status_p1 = data_buf[2048];
#else
	    if(!NAND_ADDR_CYCLE)		
			block_status_p1 = oob_buf[OOB_BBI_OFF];
	    else
		    block_status_p1 = oob_buf[5];
#endif		
	}
	if( block_status_p1 == BBT_TAG){
		printf("Reserved area for BBT: block=%d, block_status_p1=0x%x\n\r",block,block_status_p1);
		return 0;
	}else 
	if ( block_status_p1 != 0xff){		
		printf ("WARNING: Die 0: block=%d is bad, block_status_p1=0x%x\n\r", block, block_status_p1);
		return -1;
	}
	return 0;
}
#if 1
BOOL rtk_erase_block (int page)
{
	int addr_cycle[5], page_shift;
	//printf("!!!!! rtk_erase_block !!!!!, block = %d\n\r",page/ppb);
	if ( page & (ppb-1) ){
		printf("page %d is not block alignment !!\n", page);
		return 0;
	}
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
	unsigned char switch_bbi=0, write_bbi=0;;
	unsigned char *data_ptr;
	//printf("!!!!! rtk_write_ecc_page !!!!!,page=%d block = %d\n\r",flash_page,flash_page/ppb);
	rtk_writel(0xc00fffff, NACR);
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
		dma_counter = page_size >> 9;
		dma_cache_writeb();
		if(oob_addr){
			dma_cache_writeb();
		}
		while(dma_counter >0) {  //Move 1 page
			check_ready();
			rtk_writel( (rtk_readl(NACR) |ECC_enable & (~RBO) & (~WBO)), NACR);
			//set DMA RAM start address
			dram_sa = ((unsigned int)data_ptr+buf_pos*512) & (~M_mask);
			rtk_writel( dram_sa, NADRSAR);
			//set DMA OOB start address
			oob_sa = ((unsigned int) oob_addr+buf_pos*16) & (~M_mask);
			rtk_writel( oob_sa, NADTSAR);
			//set DMA flash start address
			rtk_writel( flash_addr_t, NADFSAR);
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
	return SUCCESS;
}
#endif
// Function : rtk_read_ecc_page
// Description : Read image from NAND flash to DRAM
// Input:
//		flash_address : address of flash page
//		image_addr     : address of image, in dram address
//		oob_addr		: address of oob, in dram address
//		image_size	: the length of image
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
BOOL rtk_read_ecc_page (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr,
		unsigned int image_size)
{
	
	int dram_sa, oob_sa;	
	int dma_counter = page_size >> 9;	//Move unit=512Byte 
	int dma_size=0;
	int buf_pos=0;
	int page_counter=0;
	int page_num[3], page_shift=0, page_start;
	unsigned char * oob_buf;
	unsigned long flash_addr_t=0, flash_addr_t1;
	unsigned char switch_bbi=0, read_bbi=0;

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
	
	while( dma_size>0 ){

		dma_counter = page_size >> 9;

		if(oob_addr){
			dma_cache_writeb();
		}

		while(dma_counter >0) {  //Move 1 page

			check_ready();
			rtk_writel( (rtk_readl(NACR) |ECC_enable & (~RBO) & (~WBO)), NACR);

			//set DMA RAM start address
			dram_sa = ((unsigned int)image_addr+buf_pos*512) & (~M_mask);
			rtk_writel( dram_sa, NADRSAR);
			//printf("SDRAM address: 0x%08X ",dram_sa);

			//set DMA oob start address
			//oob_sa = ((unsigned int)oob_buf+buf_pos*16) & (~M_mask);
			oob_sa = ((unsigned int)oob_addr+buf_pos*16) & (~M_mask);
			rtk_writel( oob_sa, NADTSAR);

			//set DMA flash start address
			rtk_writel( flash_addr_t, NADFSAR);
			//printf("Flash address: 0x%08X \n\r",flash_addr_t);
			flash_addr_t +=528;
			
			//DMA read
			rtk_writel((~TAG_DIS) & (DESC0|DMARE|LBC_128),NADCRR);	
			check_ready();

			if(FAIL== rtk_check_pageData((page_start+page_counter), buf_pos*(512+16))) {
				dma_cache_writeb();
				return FAIL;
			}
			
			dma_counter--;
			buf_pos++;

		}

		page_counter +=1;

		if(!NAND_ADDR_CYCLE){
			flash_addr_t1 +=page_counter*0x1000;
		}else{
			flash_addr_t1 +=page_counter*0x200;
		}
		
		flash_addr_t = flash_addr_t1;
		dma_size -= page_size;
		
	}
	if(isLastPage){
		read_bbi = flash_page & (ppb-1);
		if((read_bbi == (ppb-2)) || (read_bbi ==(ppb-1))){
			switch_bbi = 1;
		}
	}else{
		read_bbi = flash_page & (ppb-1);
		if((read_bbi == 0) || (read_bbi ==1)){
			switch_bbi = 1;
		}
	}	
	if(!NAND_ADDR_CYCLE)
	{
		unsigned char temp_val=0;
		if(switch_bbi && (flash_page > ppb-1)){
			temp_val = image_addr[DATA_BBI_OFF];
			image_addr[DATA_BBI_OFF] = oob_addr[OOB_BBI_OFF];
			oob_addr[OOB_BBI_OFF] = temp_val;
		}
	}
	dma_cache_writeb();
	//free(oob_buf);
	return SUCCESS;
}


#if 0
//####################################################################
// Function : scan_last_die_BB
// Description : Scan the Bad Block
// Input:
//		start_page : 
// Output:
//		NON
//####################################################################
void rtk_check_nand_space(unsigned int start_page, unsigned int loader_size, struct Boot_Rsv *boot_info, int backup)
{
	unsigned int re_size = BOOT_SIZE;  				 //reserved size = 1MBytes
	unsigned int re_block = (re_size/block_size -3);  //the first block is used for boot!!!, the last two block is for bad block table
	unsigned int flash_addr_t = 0, count=0;
	int i,j;
	unsigned int re_area = 0;
	struct Boot_Rsv *boot_tmp = boot_info;

	//boot_tmp = (struct Boot_Rsv *)malloc(backup*sizeof(struct Boot_Rsv));

	//page translates to mapping
	if( (start_page % ppb) ==0 ){
		flash_addr_t = start_page*page_size;
	}

	for(j=0; j<backup; j++){
		count =0;
		re_area = 0;
		
		for(i=0;i<re_block;i++){
	
			if(!rtk_block_isbad(flash_addr_t)){  // good block or  reserved for loader
				count++;
				re_area += block_size;			
			}else{
				count=0;
				re_area =0;
			}
			
			flash_addr_t += block_size;

			if((re_area >= loader_size) && (count!=0) ){  // reserved loader area is enough & block count is not equal to zero
				boot_tmp[j].num = count;
				boot_tmp[j].start_block = (flash_addr_t)/(block_size)-count;			
				break;
			}
			else if( (i==re_block-1) && (re_area< loader_size)){
				//printf("Not enough blocks for area [%d]\n\r",j);
				break;
			}

		}

		re_block -=count;

	}
	return;
}
#endif
#ifdef CONFIG_NAND_FLASH
//####################################################################
// Function : rtk_parallel_nand_probe
// Description : probe NAND flash basic information
//			  maker_id, device_id, chip_size, page_size, block_size, oob_size, isLastPage
// Input:
//		NON
// Output:
//		NON
//####################################################################
void rtk_parallel_nand_probe(void)
{
	int i=0;
	unsigned char maker_code, device_code;	
	unsigned char B5th, B6th;
	unsigned int nand_type_id;
	unsigned int num_chips_probe = 1;
	//unsigned int rc;
	
	DEBUG_PRINT(DEBUG_ALWAYS, ("%s in\n\r",__func__));

	memset(&nand_dev_info, 0, sizeof(struct device_type));

	rtk_nand_read_id();
	rtk_nand_base_info._nand_read_page_ecc = rtk_read_ecc_page; 
	rtk_nand_base_info._nand_write_page_ecc = rtk_write_ecc_page;		
	rtk_nand_base_info._erase_block = rtk_erase_block;

	rtk_writel(0xC00FFFFF, NACR);     //Enable ECC	
	rtk_writel(0x0000000F, NASR);     //clear NAND flash status register

	maker_code = nand_id[0];
	device_code = nand_id[1];
	nand_type_id = maker_code<<24 | device_code<<16 | nand_id[2]<<8 |nand_id[3];
	B5th = nand_id[4];
	B6th = nand_id[5];

	DEBUG_PRINT(DEBUG_ALWAYS,("[%s, line %d] maker_code = %X, device_code = %X, nand_type_id = %X, B5th = %X, B6th = %X\n\r"
		,__FUNCTION__,__LINE__,maker_code,device_code, nand_type_id,B5th, B6th));
	
	for(i=0;nand_device[i].name;i++){
		if( nand_device[i].id == nand_type_id &&
			((nand_device[i].CycleID5th==0xff)?1:(nand_device[i].CycleID5th==B5th)) ){

			if (nand_device[i].size == num_chips_probe * nand_device[i].chipsize){
				if ( num_chips_probe == nand_device[i].num_chips ){
					DEBUG_PRINT(DEBUG_ALWAYS,("One %s chip has %d die(s) on board\n\r", nand_device[i].name, nand_device[i].num_chips));
					//mtd->PartNum = nand_device[i].name;
					//device_size = nand_device[i].size;
					chip_size = nand_device[i].chipsize;	
					page_size = nand_device[i].PageSize;
					block_size = nand_device[i].BlockSize;
					_block_size_ = block_size;
					oob_size = nand_device[i].OobSize;
					//num_chips = nand_device[i].num_chips;
					isLastPage = nand_device[i].isLastPage;
					//rtk_lookup_table_flag = 1;

					memcpy(&nand_dev_info, &nand_device[i], sizeof(struct device_type));
					
					break;
				}
			}
		}

	}

	DEBUG_PRINT(DEBUG_ALWAYS,("nand part=%s, id=%x, device_size=%u, chip_size=%u, num_chips=%d, isLastPage=%d\n\r", 
			nand_dev_info.name, nand_dev_info.id, nand_dev_info.size, nand_dev_info.chipsize, 
			nand_dev_info.num_chips, nand_dev_info.isLastPage));
	
	ppb = block_size/page_size;
	page_shift = shift_value(page_size) -1;
	block_shift = shift_value(block_size) -1 ;
	pagemask = (chip_size/page_size) -1;
	ppb_shift = shift_value(ppb)-1;
	DEBUG_PRINT(DEBUG_ALWAYS,("page_shift = %d, block_shift = %d, pagemask = 0x%X\n\r",page_shift,block_shift,pagemask));

	//rc = rtk_nand_scan_bbt();
	DEBUG_PRINT(DEBUG_ALWAYS, ("%s out\n\r",__func__));

	return;

}
#endif
void rtk_nand_base_probe(void)
{
	//printf("%s-%d\n",__func__,__LINE__);
	#ifdef CONFIG_SPI_NAND_FLASH
		rtk_nand_base_info._probe = rtk_spi_nand_probe;
	#else
		rtk_nand_base_info._probe = rtk_parallel_nand_probe;	
	#endif
	//printf("%s-%d\n",__func__,__LINE__);
	rtk_nand_base_info._probe();
	//printf("%s-%d\n",__func__,__LINE__);
}
