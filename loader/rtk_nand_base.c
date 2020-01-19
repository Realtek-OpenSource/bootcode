#include "rtk_nand_base.h"
#ifdef CONFIG_SPI_NAND_FLASH
#include "spi_nand.h"
#endif
#ifdef CONFIG_NAND_FLASH
#include "rtk_nand.h"
#endif
#if defined(CONFIG_RTK_NORMAL_BBT) || defined(CONFIG_RTK_REMAP_BBT)
#include "rtk_bbt.h"
#endif

//rtk_nand_info_t rtk_nand_base_info;
#if 0
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








int nand_erase_nand (unsigned int flash_page, unsigned int len)
{

	int page, chipnr;
	int i, old_page, block;
	int elen = 0;
	int rc = 0;
	int realpage, chipnr_remap;
	int err_chipnr = 0, err_chipnr_remap = 1;
	int numchips=1, page_offset=0;
	unsigned int addr;
#ifdef CONFIG_RTK_REMAP_BBT
{
	unsigned int real_addr;
	i = (addr >> block_shift);
	real_addr = (bbt_v2r[i].block_r << block_shift);//real block index, addr.
	addr = real_addr;
	//printf("%s: blockr:%d addr:%d\n\r",__FUNCTION__,bbt_v2r[i].block_r, addr);
}
#endif
	//printf("[%s-%d] flash_page=%d, len=%d\n",__func__,__LINE__,flash_page,len);
	addr = (flash_page << page_shift);
	chipnr = chipnr_remap = 0;
	old_page = page = flash_page & pagemask;
	page_offset = page_number(page);
	block = block_number(page);

	if ((addr + len) > chip_size) {
		printf("%s: Attempt erase beyond end of device\n\r",__FUNCTION__);
		return FAIL;
	}
#ifdef CONFIG_RTK_NORMAL_BBT
	for ( i=0; i<bbt_num; i++){
		if ( bbt_nor[i].bad_block != BB_INIT ){
			if ( block == bbt_nor[i].bad_block ){
				printf("%s: block:%d is bad!",__FUNCTION__,block);
				return -1;
			}
		}else
			break;
	}
#endif	
	
 	while (elen < len) {
#ifdef CONFIG_RTK_REMAP_BBT
		for ( i=0; i<RBA; i++){			
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
					block = bbt[i].remap_block;
				}			
			}else				
			break;		
		}
#endif
		page = block*ppb;
		printf("Ready to Erase blk %u\n\r",page/ppb);

		//rc = rtk_erase_block(page);
		rc = rtk_nand_base_info._erase_block(page);			
		if (rc) {
		    printf("%s: block erase failed at page address=0x%08x\n", __FUNCTION__, addr);
#ifdef CONFIG_RTK_REMAP_BBT
			int block_remap = 0x12345678;
            /* update BBT */
			if(check_BBT(page/ppb)==0)
			{				
                for( i=0; i<RBA; i++){
	                if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
		                err_chipnr = chipnr;
		                bbt[i].BB_die = err_chipnr;
		                bbt[i].bad_block = page/ppb;
		                err_chipnr_remap = bbt[i].RB_die;
		                block_remap = bbt[i].remap_block;
		                break;
	                }
                }

                if( block_remap == 0x12345678 ){
	                 printf("[%s] RBA do not have free remap block\n\r", __FUNCTION__);
	                 return FAIL;
                }

                dump_BBT();

                if ( rtk_update_bbt(bbt)){
	                printf("[%s] rtk_update_bbt() fails\n", __FUNCTION__);
	                return FAIL;
                }
			}
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
			 for( i=0; i<bbt_num; i++){
				 if ( bbt_nor[i].bad_block == BB_INIT){
					 bbt_nor[i].BB_die = numchips-1;
					 bbt_nor[i].bad_block = block;
					 bbt_nor[i].RB_die = numchips-1;
					 bbt_nor[i].block_info= 0xba; //bad block in process.
					 break;
				 }
			 }
			 dump_normal_BBT();
		
			 if ( rtk_update_normal_bbt (bbt_nor) ){
				 printf("[%s] rtk_update_normal_bbt() fails\n\r", __FUNCTION__);
				 return -1;
			 }
#endif
#ifndef CONFIG_SPI_NAND_FLASH
		     if(!NAND_ADDR_CYCLE)	
			     //NfSpareBuf[0] = 0x00;
		         NfSpareBuf[OOB_BBI_OFF] = 0x00;
		     else
			     NfSpareBuf[5] = 0x00;
#endif		
		     if ( isLastPage){
				 unsigned char *temp_buf = (unsigned char *)malloc(page_size);
				 memset(temp_buf,0xba,sizeof(char)*page_size);
			     //rtk_write_ecc_page(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
			     //rtk_write_ecc_page(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
				 rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
			     rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);

				 if(temp_buf)
                     free(temp_buf);				 
		     }else{
				 unsigned char *temp_buf = (unsigned char *)malloc(page_size);
				 memset(temp_buf,0xba,sizeof(char)*page_size);
			     //rtk_write_ecc_page(block*ppb,temp_buf ,&NfSpareBuf , page_size);
			     //rtk_write_ecc_page(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
			     rtk_nand_base_info._nand_write_page_ecc(block*ppb,temp_buf ,&NfSpareBuf , page_size);
				 rtk_nand_base_info._nand_write_page_ecc(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);

				 if(temp_buf)
					 free(temp_buf);

		     }
#ifdef CONFIG_RTK_NORMAL_BBT
			return -1;
#endif
#ifdef CONFIG_RTK_REMAP_BBT
			 //erase the remapping block!!
			 rc = rtk_erase_block(block_remap*ppb);
#endif			 
		}
		
			
		elen += block_size;

		old_page += ppb;
		
		if ( elen<len && !(old_page & pagemask)) {
			old_page &= pagemask;
		}

		block = old_page/ppb;
	}

	return rc;
}




int nand_read_ecc (unsigned int flash_page, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
	unsigned int page, realpage;
	unsigned int from;
	int data_len, oob_len;
	int rc=0;
	int i, old_page, page_offset, block;
	int chipnr, chipnr_remap;
	int numchips=1;
#ifdef CONFIG_RTK_REMAP_BBT
{
    unsigned int offset=0, aa=0;	
	i = (from >> block_shift); //virtual block index
	aa = from & ~(block_size -1);
	offset = from - aa;
	from =  (bbt_v2r[i].block_r << block_shift) + offset;//real block index, addr.
//printf("%s: blockr:%d from:%d offset %d\n\r",__FUNCTION__,bbt_v2r[i].block_r, from, offset);
}
#endif
	from = flash_page << page_shift;
	if ((from + len) > chip_size) {
		printf ("nand_read_ecc: Attempt read beyond end of device\n");
		return FAIL;
	}

	if ((from & (page_size-1)) ||(len & (page_size-1))) {
		printf("nand_read_ecc: Attempt to read not page aligned data\n");
		return FAIL;
	}

	//realpage = (int)(from >> page_shift);
	//chipnr = chipnr_remap = (int)(from >> this->chip_shift);
	old_page = page = flash_page & pagemask;
	page_offset = page_number(page);//page & (ppb-1);
	block = block_number(page);//page/ppb;


#ifdef CONFIG_RTK_REMAP_BBT
	if (numchips==1 && block != read_block ){
		read_block = block;
		read_remap_block = 0xFFFFFFFF;
		read_has_check_bbt = 0;
	}
#endif
	data_len = oob_len = 0;
#ifdef CONFIG_RTK_NORMAL_BBT
	for ( i=0; i<bbt_num; i++){
		if ( bbt_nor[i].bad_block != BB_INIT ){
			if ( block == bbt_nor[i].bad_block ){
				printf("%s: block:%d is bad!\n",__FUNCTION__,block);
				return -1;
			}
		}else
			break;
	}
#endif
	while(data_len<len){
#ifdef CONFIG_RTK_REMAP_BBT
		if( numchips==1){
			if ( (page>=block*ppb) && (page<(block+1)*ppb) && read_has_check_bbt==1 )
				goto SKIP_BBT_CHECK;
		}

		for ( i=0; i<RBA; i++){
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
					read_remap_block = block = bbt[i].remap_block;
				}
			}else
				break;
		}
		read_has_check_bbt = 1;

SKIP_BBT_CHECK:

		if (  read_has_check_bbt==1 ){
			if ( read_remap_block == 0xFFFFFFFF )
				page = block*ppb + page_offset;
			else	
				page = read_remap_block*ppb + page_offset;
		}else
			page = block*ppb + page_offset;  
#else //CONFIG_RTK_REMAP_BBT
		page = block*ppb + page_offset;  
#endif
printf("$");
//printf("[%s-%d] read page %u, data_len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, data_len,data_buf+data_len);
		//if((page % ppb)==0)  printf("$");
		//rc = rtk_read_ecc_page(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
		rc = rtk_nand_base_info._nand_read_page_ecc(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
		if (rc < 0) {
			if(rc==-1){
				printf("%s: page %d Un-correctable HW ECC\n\r", __FUNCTION__, page);
#ifdef CONFIG_RTK_REMAP_BBT
				//update BBT
				if(check_BBT(page/ppb)==0){
				    for( i=0; i<RBA; i++){
					    if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
						    bbt[i].BB_die = numchips-1;
						    bbt[i].bad_block = page/ppb;
						    break;
					    }
				    }
					dump_BBT();
					
					//if ( rtk_update_bbt ( &NfDataBuf, &NfSpareBuf, bbt) ){
					if ( rtk_update_bbt (bbt) ){
							printf("[%s] rtk_update_bbt() fails\n\r", __FUNCTION__);
							return -1;
					}

			    }//check_BBT
#endif			    
#ifdef CONFIG_RTK_NORMAL_BBT
				for( i=0; i<bbt_num; i++){
					if ( bbt_nor[i].bad_block == BB_INIT){
						bbt_nor[i].BB_die = numchips-1;
						bbt_nor[i].bad_block = block;
						bbt_nor[i].RB_die = numchips-1;						
						bbt_nor[i].block_info= 0xba; //bad block in process.
						break;
					}
				}
			    
				dump_normal_BBT();
				if ( rtk_update_normal_bbt (bbt_nor) ){
						printf("[%s] rtk_update_normal_bbt() fails\n\r", __FUNCTION__);
						return -1;
				}
#endif
#ifndef CONFIG_SPI_NAND_FLASH
			    if(!NAND_ADDR_CYCLE)	
			        NfSpareBuf[OOB_BBI_OFF] = 0x00;
			    else
				    NfSpareBuf[5] = 0x00;
#endif
				block = page/ppb;

			    if ( isLastPage){
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					//rtk_erase_block(block*ppb);
					rtk_nand_base_info._erase_block(block*ppb);
				    //rtk_write_ecc_page(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
				    //rtk_write_ecc_page(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
					if(temp_buf)
					    free(temp_buf);
			    }else{
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					//rtk_erase_block(block*ppb);
					rtk_nand_base_info._erase_block(block*ppb);
				    //rtk_write_ecc_page(block*ppb,temp_buf ,&NfSpareBuf , page_size);
				    //rtk_write_ecc_page(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
			     	rtk_nand_base_info._nand_write_page_ecc(block*ppb,temp_buf ,&NfSpareBuf , page_size);
				 	rtk_nand_base_info._nand_write_page_ecc(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);				    
					if(temp_buf)
					    free(temp_buf);
			    }
			    printf("%s: Un-correctable HW ECC Error at page=%d\n\r",__FUNCTION__, page);
#ifdef CONFIG_RTK_NORMAL_BBT
				return -1;
#endif
#ifdef CONFIG_RTK_REMAP_BBT
				rc = 0;
#endif
		    }else{
				printf("%s: page %d failed\n", __FUNCTION__, page);
				return -1;
			}
		}
	
		if(data_buf)//add by alexchang 0524-2010
		data_len += page_size;
#ifndef CONFIG_SPI_NAND_FLASH
		if(oob_buf)//add by alexchang 0524-2010
		oob_len += oob_size;
#endif
		old_page++;
		page_offset = page_number(old_page);;//old_page & (ppb-1);
		if ( data_len<len && !(old_page &  pagemask)) {
			old_page &= pagemask;
		}
		block = block_number(old_page);//old_page/ppb;

	}
	return rc;

}
#if 0
static void reverse_to_Yaffs2Tags(unsigned char *r_oobbuf)
{
	int k;
	for ( k=0; k<16; k++ ){
		r_oobbuf[k]  = r_oobbuf[1+k];
	}
}
#endif



int nand_write_ecc (unsigned int flash_page, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
	unsigned int page, to;
	int data_len, oob_len;
	int rc;
	int i, old_page, page_offset, block;
	int chipnr, chipnr_remap, err_chipnr = 0, err_chipnr_remap = 1;
	int numchips=1;
	int backup_offset;
	unsigned int rsv_block = 0;
	unsigned char switch_bbi=0, write_bbi=0;;

#ifdef CONFIG_RTK_REMAP_BBT
	//printf("%s-%d: to %d, len %d oob:%x\n\r",__FUNCTION__,__LINE__,to,len,oob_buf);
{
        unsigned int offset=0, aa=0;
		i = (to >> block_shift);//virtual block index
        aa = to & ~(block_size - 1);
        offset = to - aa;		
		to = (bbt_v2r[i].block_r << block_shift) + offset;//real block index, addr.
		//printf("%s: blockr:%d to:%d offset %d\n\r",__FUNCTION__,bbt_v2r[i].block_r, to, offset);
}
#endif
    to = (flash_page << page_shift);
//	printf("%s-%d: to %d, len %d stat_page:%d data_buf=0x%x\n\r",__FUNCTION__,__LINE__,to,len,flash_page,data_buf);
printf("*");

	if ((to + len) > chip_size) {
		printf("nand_write_ecc: Attempt write beyond end of device addr=0x%x\n\r",to);
		return FAIL;
	}

	if ((to & (page_size-1)) ||(len & (page_size-1))) {
		printf("nand_write_ecc: Attempt to write not page aligned data, to = 0x%08X, len = %d\n",to,len);
		return FAIL;
	}

	//realpage = (int)(to >> page_shift);
	chipnr = chipnr_remap = 0;
	old_page = page = flash_page & pagemask;
	page_offset = page_number(page);//page & (ppb-1);
	block = block_number(page);//page/ppb;

	//printf("[%s-%d] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, len , data_buf);
//ccwei 111116
//	rsv_block = RESERVED_AREA/block_size;

#ifdef CONFIG_RTK_REMAP_BBT
	//CMYu, 20091030
	if ( numchips == 1 && block != write_block ){
		write_block = block;
		write_remap_block = 0xFFFFFFFF;
		write_has_check_bbt = 0;
	}
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
		for ( i=0; i<bbt_num; i++){
			if ( bbt_nor[i].bad_block != BB_INIT ){
				if ( block == bbt_nor[i].bad_block ){
					printf("%s: block:%d is bad!\n",__FUNCTION__,block);
					return -1;
				}
			}else
				break;
		}
#endif	
	data_len = oob_len = 0;

	while ( data_len < len) {
//ccwei 111116
/*
		if(block < (rsv_block-2)){
			//printf("[%s] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
			goto SKIP_BBT_CHECK;
		}
*/		
#ifdef CONFIG_RTK_REMAP_BBT
		if (numchips == 1){
			if ( (page>=block*ppb) && (page<(block+1)*ppb) && write_has_check_bbt==1 )
				goto SKIP_BBT_CHECK;
		}

		for ( i=0; i<RBA; i++){
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
						write_remap_block = block = bbt[i].remap_block;
				}
			}else
				break;
		}
		write_has_check_bbt = 1;
SKIP_BBT_CHECK:
		if ( numchips == 1 && write_has_check_bbt==1 ){
				if ( write_remap_block == 0xFFFFFFFF )
					page = block*ppb + page_offset;
				else	
					page = write_remap_block*ppb + page_offset;
		}else
				page = block*ppb + page_offset;
#else
		page = block*ppb + page_offset;
#endif
		//if( (page % ppb) == 0) printf(".");

		//rc = rtk_write_ecc_page ( page, &data_buf[data_len], &oob_buf[oob_len], page_size);
#ifdef CONFIG_SPI_NAND_FLASH
		memcpy(NfDataBuf,&data_buf[data_len],page_size);
		rtk_nand_base_info._nand_write_page_ecc(page, NfDataBuf, oob_buf, page_size);
#else
		rtk_nand_base_info._nand_write_page_ecc(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
#endif
#if 0
		rc = rtk_nand_base_info._nand_write_page_ecc(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
		if(rc<0){
/*			
			if(block<(rsv_block-2)){
				printf("[%s] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
				return -1;		
			}
*/			
			if(rc == -1){
				printf ("%s: write_ecc_page:  write failed\n\r", __FUNCTION__);
#ifdef CONFIG_RTK_REMAP_BBT
				int block_remap = 0x12345678;
				/* update BBT */
			    if(check_BBT(page/ppb)==0)
			    {				
				    for( i=0; i<RBA; i++){
					    if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
						    err_chipnr = chipnr;
						    bbt[i].BB_die = err_chipnr;
						    bbt[i].bad_block = page/ppb;
						    err_chipnr_remap = bbt[i].RB_die;
						    block_remap = bbt[i].remap_block;
						    break;
					    }
				    }

				if ( block_remap == 0x12345678 ){
					printf("[%s] RBA do not have free remap block\n\r", __FUNCTION__);
					return FAIL;
				}
			
				dump_BBT();

				    if ( rtk_update_bbt(bbt)){
					    printf("[%s] rtk_update_bbt() fails\n", __FUNCTION__);
					    return FAIL;
				    }
			    }

				backup_offset = page&(ppb-1);
				rtk_erase_block(block_remap*ppb);
				printf("[%s] Start to Backup old_page from %d to %d\n\r", __FUNCTION__, block*ppb, block*ppb+backup_offset-1);

				for ( i=0; i<backup_offset; i++){
					rtk_read_ecc_page(block*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
#if 0
					if ( NfSpareBuf )
						reverse_to_Yaffs2Tags(&NfSpareBuf); //czyao
#endif
#ifndef CONFIG_SPI_NAND_FLASH
					if(!NAND_ADDR_CYCLE)	
						//NfSpareBuf[0] = 0xff;
				        NfSpareBuf[OOB_BBI_OFF] = 0xff;
					else
						NfSpareBuf[5] = 0xff;
#endif
					rtk_write_ecc_page(block_remap*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
				}
				//Write the written failed page to new block
				rtk_write_ecc_page ( block_remap*ppb+backup_offset, &data_buf[data_len], &oob_buf[oob_len], page_size);
				printf("[%s] write failure page = %d to %d\n", __FUNCTION__, page, block_remap*ppb+backup_offset);
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
				for( i=0; i<bbt_num; i++){
					if ( bbt_nor[i].bad_block == BB_INIT){
						bbt_nor[i].BB_die = numchips-1;
						bbt_nor[i].bad_block = block;
						bbt_nor[i].RB_die = numchips-1;
						bbt_nor[i].block_info= 0xba; //bad block in process.
						break;
					}
				}
				dump_normal_BBT();
				if ( rtk_update_normal_bbt (bbt_nor) ){
					printf("[%s] rtk_update_normal_bbt() fails\n\r", __FUNCTION__);
					return -1;
				}
#endif
#ifndef CONFIG_SPI_NAND_FLASH
				if(!NAND_ADDR_CYCLE)	
					//NfSpareBuf[0] = 0x00;
					NfSpareBuf[OOB_BBI_OFF] = 0x00;
				else
					NfSpareBuf[5] = 0x00;
#endif
				block = page/ppb;

				if ( isLastPage){
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					#if 0
					rtk_erase_block(block*ppb);
					rtk_write_ecc_page(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
					#endif
					rtk_nand_base_info._erase_block(block*ppb);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);

                    if(temp_buf)
 					    free(temp_buf);
				}else{
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					#if 0
					rtk_erase_block(block*ppb);
					rtk_write_ecc_page(block*ppb,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
					#endif
					rtk_nand_base_info._erase_block(block*ppb);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
                    if(temp_buf)
 					    free(temp_buf);
				}
#ifdef CONFIG_RTK_NORMAL_BBT
				return -1;
#endif				
			}else{
				printf("%s: write_ecc_page:  rc=%d\n\r", __FUNCTION__, rc);
				return -1;
			}

		}
#endif		
//#endif /*CONFIG_SPI_NAND_FLASH*/


		if(data_buf)//add by alexchang 0524-2010
			data_len += page_size;
//		if(oob_buf) //add by alexchang 0524-2010
//			oob_len += oob_size;
//printf("[%s-%d] wirte page %u, data_len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, data_len,data_buf+data_len);

		old_page++;
		page_offset = old_page & (ppb-1);
		if ( data_len<len && !(old_page & pagemask)) {
			old_page &= pagemask;
		}
		block = old_page/ppb;

	}

	return 0;

}

int nand_write_page (unsigned int flash_page, unsigned int len, unsigned char *data_buf)
{
	unsigned int page, to;
	int data_len, oob_len;
	int rc;
	int i, old_page, page_offset, block;
	int chipnr, chipnr_remap, err_chipnr = 0, err_chipnr_remap = 1;
	int numchips=1;
	int backup_offset;
	unsigned int rsv_block = 0;
	unsigned char switch_bbi=0, write_bbi=0;;

#ifdef CONFIG_RTK_REMAP_BBT
	//printf("%s-%d: to %d, len %d oob:%x\n\r",__FUNCTION__,__LINE__,to,len,oob_buf);
{
        unsigned int offset=0, aa=0;
		i = (to >> block_shift);//virtual block index
        aa = to & ~(block_size - 1);
        offset = to - aa;		
		to = (bbt_v2r[i].block_r << block_shift) + offset;//real block index, addr.
		//printf("%s: blockr:%d to:%d offset %d\n\r",__FUNCTION__,bbt_v2r[i].block_r, to, offset);
}
#endif
    to = (flash_page << page_shift);
//	printf("%s-%d: to %d, len %d stat_page:%d data_buf=0x%x\n\r",__FUNCTION__,__LINE__,to,len,flash_page,data_buf);
printf("*");

	if ((to + len) > chip_size) {
		printf("nand_write_ecc: Attempt write beyond end of device addr=0x%x\n\r",to);
		return FAIL;
	}

	if ((to & (page_size-1)) ||(len & (page_size-1))) {
		printf("nand_write_ecc: Attempt to write not page aligned data, to = 0x%08X, len = %d\n",to,len);
		return FAIL;
	}

	//realpage = (int)(to >> page_shift);
	chipnr = chipnr_remap = 0;
	old_page = page = flash_page & pagemask;
	page_offset = page_number(page);//page & (ppb-1);
	block = block_number(page);//page/ppb;

	//printf("[%s-%d] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, len , data_buf);
//ccwei 111116
//	rsv_block = RESERVED_AREA/block_size;

#ifdef CONFIG_RTK_REMAP_BBT
	//CMYu, 20091030
	if ( numchips == 1 && block != write_block ){
		write_block = block;
		write_remap_block = 0xFFFFFFFF;
		write_has_check_bbt = 0;
	}
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
		for ( i=0; i<bbt_num; i++){
			if ( bbt_nor[i].bad_block != BB_INIT ){
				if ( block == bbt_nor[i].bad_block ){
					printf("%s: block:%d is bad!\n",__FUNCTION__,block);
					return -1;
				}
			}else
				break;
		}
#endif	
	data_len = oob_len = 0;

	while ( data_len < len) {
//ccwei 111116
/*
		if(block < (rsv_block-2)){
			//printf("[%s] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
			goto SKIP_BBT_CHECK;
		}
*/		
#ifdef CONFIG_RTK_REMAP_BBT
		if (numchips == 1){
			if ( (page>=block*ppb) && (page<(block+1)*ppb) && write_has_check_bbt==1 )
				goto SKIP_BBT_CHECK;
		}

		for ( i=0; i<RBA; i++){
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
						write_remap_block = block = bbt[i].remap_block;
				}
			}else
				break;
		}
		write_has_check_bbt = 1;
SKIP_BBT_CHECK:
		if ( numchips == 1 && write_has_check_bbt==1 ){
				if ( write_remap_block == 0xFFFFFFFF )
					page = block*ppb + page_offset;
				else	
					page = write_remap_block*ppb + page_offset;
		}else
				page = block*ppb + page_offset;
#else
		page = block*ppb + page_offset;
#endif
		//if( (page % ppb) == 0) printf(".");

		//rc = rtk_write_ecc_page ( page, &data_buf[data_len], &oob_buf[oob_len], page_size);
//#ifdef CONFIG_SPI_NAND_FLASH
		rtk_nand_base_info._nand_write_page(page, &data_buf[data_len]);
//#else
#if 0
		rc = rtk_nand_base_info._nand_write_page_ecc(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
		if(rc<0){
/*			
			if(block<(rsv_block-2)){
				printf("[%s] wirte page %u, len %u, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
				return -1;		
			}
*/			
			if(rc == -1){
				printf ("%s: write_ecc_page:  write failed\n\r", __FUNCTION__);
#ifdef CONFIG_RTK_REMAP_BBT
				int block_remap = 0x12345678;
				/* update BBT */
			    if(check_BBT(page/ppb)==0)
			    {				
				    for( i=0; i<RBA; i++){
					    if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
						    err_chipnr = chipnr;
						    bbt[i].BB_die = err_chipnr;
						    bbt[i].bad_block = page/ppb;
						    err_chipnr_remap = bbt[i].RB_die;
						    block_remap = bbt[i].remap_block;
						    break;
					    }
				    }

				if ( block_remap == 0x12345678 ){
					printf("[%s] RBA do not have free remap block\n\r", __FUNCTION__);
					return FAIL;
				}
			
				dump_BBT();

				    if ( rtk_update_bbt(bbt)){
					    printf("[%s] rtk_update_bbt() fails\n", __FUNCTION__);
					    return FAIL;
				    }
			    }

				backup_offset = page&(ppb-1);
				rtk_erase_block(block_remap*ppb);
				printf("[%s] Start to Backup old_page from %d to %d\n\r", __FUNCTION__, block*ppb, block*ppb+backup_offset-1);

				for ( i=0; i<backup_offset; i++){
					rtk_read_ecc_page(block*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
#if 0
					if ( NfSpareBuf )
						reverse_to_Yaffs2Tags(&NfSpareBuf); //czyao
#endif
#ifndef CONFIG_SPI_NAND_FLASH
					if(!NAND_ADDR_CYCLE)	
						//NfSpareBuf[0] = 0xff;
				        NfSpareBuf[OOB_BBI_OFF] = 0xff;
					else
						NfSpareBuf[5] = 0xff;
#endif
					rtk_write_ecc_page(block_remap*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
				}
				//Write the written failed page to new block
				rtk_write_ecc_page ( block_remap*ppb+backup_offset, &data_buf[data_len], &oob_buf[oob_len], page_size);
				printf("[%s] write failure page = %d to %d\n", __FUNCTION__, page, block_remap*ppb+backup_offset);
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
				for( i=0; i<bbt_num; i++){
					if ( bbt_nor[i].bad_block == BB_INIT){
						bbt_nor[i].BB_die = numchips-1;
						bbt_nor[i].bad_block = block;
						bbt_nor[i].RB_die = numchips-1;
						bbt_nor[i].block_info= 0xba; //bad block in process.
						break;
					}
				}
				dump_normal_BBT();
				if ( rtk_update_normal_bbt (bbt_nor) ){
					printf("[%s] rtk_update_normal_bbt() fails\n\r", __FUNCTION__);
					return -1;
				}
#endif
#ifndef CONFIG_SPI_NAND_FLASH
				if(!NAND_ADDR_CYCLE)	
					//NfSpareBuf[0] = 0x00;
					NfSpareBuf[OOB_BBI_OFF] = 0x00;
				else
					NfSpareBuf[5] = 0x00;
#endif
				block = page/ppb;

				if ( isLastPage){
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					#if 0
					rtk_erase_block(block*ppb);
					rtk_write_ecc_page(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
					#endif
					rtk_nand_base_info._erase_block(block*ppb);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);

                    if(temp_buf)
 					    free(temp_buf);
				}else{
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					#if 0
					rtk_erase_block(block*ppb);
					rtk_write_ecc_page(block*ppb,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
					#endif
					rtk_nand_base_info._erase_block(block*ppb);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb,temp_buf ,&NfSpareBuf , page_size);
					rtk_nand_base_info._nand_write_page_ecc(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
                    if(temp_buf)
 					    free(temp_buf);
				}
#ifdef CONFIG_RTK_NORMAL_BBT
				return -1;
#endif				
			}else{
				printf("%s: write_ecc_page:  rc=%d\n\r", __FUNCTION__, rc);
				return -1;
			}

		}
#endif		
//#endif /*CONFIG_SPI_NAND_FLASH*/

#ifdef CONFIG_SPI_NAND_FLASH
		data_len += CHUNK_SIZE;
//printf("[%s-%d] wirte page %u, data_len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, data_len,data_buf+data_len);
#else
		if(data_buf)//add by alexchang 0524-2010
			data_len += page_size;
		//if(oob_buf) //add by alexchang 0524-2010
		//	oob_len += oob_size;
//printf("[%s-%d] wirte page %u, data_len %u, data_buf = 0x%p\n\r",__func__,__LINE__, page, data_len,data_buf+data_len);
#endif		
		old_page++;
		page_offset = old_page & (ppb-1);
		if ( data_len<len && !(old_page & pagemask)) {
			old_page &= pagemask;
		}
		block = old_page/ppb;

	}

	return 0;

}



