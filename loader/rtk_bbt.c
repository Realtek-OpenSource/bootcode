#include "rtk_bbt.h"
#include "rtk_nand_base.h"
#include "rtk_nand.h"


#ifdef CONFIG_RTK_NORMAL_BBT
struct BBT_normal *bbt_nor; /*allocate in nand_scan_normal_bbt*/
unsigned int bbt_num;// = 102; 102 = (32MB/16K) * 5%
int BBT_PERCENT = 5;
static int rtk_create_normal_bbt( int page);
#endif
#ifdef CONFIG_RTK_REMAP_BBT
struct BB_t *bbt;
struct BBT_v2r *bbt_v2r;
int RBA_PERCENT = 5; //reserve 5% area in the end for bbt
unsigned int RBA;// = 102; 102 = (32MB/16K) * 5%
int read_has_check_bbt = 0;
unsigned int read_block = 0XFFFFFFFF;
unsigned int read_remap_block = 0XFFFFFFFF;
int write_has_check_bbt = 0;
unsigned int write_block = 0XFFFFFFFF;
unsigned int write_remap_block = 0XFFFFFFFF;
unsigned int erase_block = 0XFFFFFFFF;
unsigned int erase_remap_block = 0XFFFFFFFF;
int erase_has_check_bbt = 0;
static int rtk_create_bbt( int page);
#endif

unsigned char BBTdataBuf[CHUNK_SIZE] __attribute__((__aligned__(32)));
unsigned char BBToobBuf[OOB_SIZE] __attribute__((__aligned__(32)));



#ifdef CONFIG_RTK_REMAP_BBT
//####################################################################
// Function : create_v2r_remapping
// Description : Create virtual block to real good block mapping table in specific block
// Input:
//		page: the page we want to put the V2R mapping table, it must be block alignment
//         block_v2r_num: from block 0 to the specified block number 
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int create_v2r_remapping(unsigned int page, unsigned int block_v2r_num)
{
	//unsigned int block_v2r_num=0;
	unsigned char mem_page_num, page_counter=0;
	unsigned char *temp_BBT = 0;
    int rc=0;
	unsigned int offs=0, offs_real=0;
	unsigned int search_region=0, count=0;
    //offs = start_page;
	//offs_real = start_page;
#if 1
	count = 0;
	search_region = (block_v2r_num << block_shift);
	//just create [bootloader+user+rootfs+rootfs2] region remapping
    while(offs < search_region){
		if ( rtk_block_isbad(offs_real) ){
			offs_real += block_size;
		}else{
    		//bbt_v2r[count].block_v = (offs >> block_shift);
    		bbt_v2r[count].block_r = (offs_real >> block_shift);			
			offs+=block_size;
			offs_real += block_size;			
//printf("bbt_v2r[%d].block_v %d,  bbt_v2r[%d].block_r %d\n",count,bbt_v2r[count].block_v,count,bbt_v2r[count].block_r);
			count++;
		}
	}
	//printf("[%s, line %d] block_v2r_num %d\n\r",__FUNCTION__,__LINE__, block_v2r_num);
#endif
	mem_page_num = ((sizeof(struct BBT_v2r)*block_v2r_num) + page_size-1 )/page_size;
	//printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);

	temp_BBT = (unsigned char *) malloc( mem_page_num*page_size);
	if ( !temp_BBT ){
		printf("%s: Error, no enough memory for temp_BBT v2r\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT_V2R;
	}
	memset( temp_BBT, 0xff, mem_page_num*page_size);

	if ( rtk_erase_block(page)){
		printf("[%s]erase block %d failure !!\n\r", __FUNCTION__, page/ppb);
		rc =  -1;
    	if(!NAND_ADDR_CYCLE)
	    	NfSpareBuf[OOB_BBI_OFF] = 0x00;
    	else
			NfSpareBuf[5] = 0x00;
		if(isLastPage){
			rtk_write_ecc_page(page+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
			rtk_write_ecc_page(page+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
		}else{
			rtk_write_ecc_page(page,temp_BBT, &NfSpareBuf, page_size);
			rtk_write_ecc_page(page+1,temp_BBT, &NfSpareBuf, page_size);
		}		
		goto EXIT_V2R;
	}
	if(!NAND_ADDR_CYCLE)
		//NfSpareBuf[0] = BBT_TAG;
		NfSpareBuf[OOB_BBI_OFF] = BBT_TAG;
	else
		NfSpareBuf[5] = BBT_TAG;
	memcpy( temp_BBT, bbt_v2r, sizeof(struct BBT_v2r)*block_v2r_num );
	//dump_mem((unsigned int)temp_BBT,512);
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT page %d failure!!\n\r", __FUNCTION__, page+page_counter);
				rc =  -1;
				rtk_erase_block(page);
    			if(!NAND_ADDR_CYCLE)
	    			NfSpareBuf[OOB_BBI_OFF] = 0x00;
    			else
					NfSpareBuf[5] = 0x00;
				if(isLastPage){
					rtk_write_ecc_page(page+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(page+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
				}else{
					rtk_write_ecc_page(page,temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(page+1,temp_BBT, &NfSpareBuf, page_size);
				}				
				goto EXIT_V2R;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
			page_counter++;
			mem_page_num--; 	
	}
	EXIT_V2R:
	if(temp_BBT)
		free(temp_BBT);
	return rc;

}

int rtk_scan_v2r_bbt(void)
{
	unsigned int bbt_v2r_page;
    int rc=0, i=0, error_count=0;
	unsigned char isbbt=0;
	unsigned char mem_page_num=0, page_counter=0, mem_page_num_tmp=0;
	unsigned char *temp_BBT=NULL;
	unsigned int block_v2r_num=0;
	unsigned char load_bbt_error = 0, is_first_boot=1;
    bbt_v2r_page = ((BOOT_SIZE/block_size)-(2*BACKUP_BBT))*ppb;

	block_v2r_num = ((BOOT_SIZE + USER_SPACE_SIZE + VIMG_SPACE_SIZE + VIMG_SPACE_SIZE) >> block_shift);
	printf("[%s, line %d] block_v2r_num %d bbt_v2r_page %d\n\r",__FUNCTION__,__LINE__, block_v2r_num, bbt_v2r_page);

	//create virtual block to real good block remapping table!!!
	bbt_v2r = (unsigned char *) malloc(sizeof(struct BBT_v2r)*(block_v2r_num));
	if(!bbt_v2r){
		printf("%s-%d: Error, no enough memory for bbt_v2r\n",__FUNCTION__,__LINE__);
		return FAIL;
	}

	mem_page_num = ((sizeof(struct BBT_v2r)*block_v2r_num) + page_size-1 )/page_size;
	printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);


	temp_BBT =(unsigned char *)malloc( mem_page_num*page_size );
	if(!temp_BBT){
		printf("%s: Error, no enough memory for temp_BBT_v2r\n",__FUNCTION__);
		return FAIL;
	}
//test NEW method!
	for(i=0;i<BACKUP_BBT;i++){
	    rc = rtk_read_ecc_page(bbt_v2r_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
	    if(!NAND_ADDR_CYCLE)
		    //isbbt = NfSpareBuf[0];
		    isbbt = NfSpareBuf[OOB_BBI_OFF];
	    else
		    isbbt = NfSpareBuf[5];
		if(!rc){
		    if(isbbt==BBT_TAG)
				is_first_boot = 0;
		}
	}
	printf("%s: is_first_boot:%d\n\r",__FUNCTION__, is_first_boot);
	for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;load_bbt_error=0;
		rc = rtk_block_isbad((bbt_v2r_page+(i*ppb))*page_size);
		if(!rc){
			printf("load v2r bbt table:%d page:%d\n\r",i, (bbt_v2r_page+(i*ppb)));
		    rc = rtk_read_ecc_page(bbt_v2r_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
		    if(!NAND_ADDR_CYCLE)
			    //isbbt = NfSpareBuf[0];
			    isbbt = NfSpareBuf[OOB_BBI_OFF];
		    else
			    isbbt = NfSpareBuf[5];
	        if(!rc){
			    if(isbbt == BBT_TAG){
			        printf("[%s] have created bbt_v2r table:%d on block %d, just loads it !!\n\r", __FUNCTION__,i,(bbt_v2r_page/ppb)+i);
			        memcpy( temp_BBT, &NfDataBuf, page_size );
			        page_counter++;
			        mem_page_num_tmp--;
			        while( mem_page_num_tmp>0 ){
				        if( rtk_read_ecc_page((bbt_v2r_page+(i*ppb)+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					        printf("[%s] load bbt_v2r table%d error!! page %d\n\r", __FUNCTION__,i, bbt_v2r_page+(i*ppb)+page_counter);
					        //free(temp_BBT);
					        //load_bbt1 = 1;
							rtk_erase_block(bbt_v2r_page+(ppb*i));
							if(!NAND_ADDR_CYCLE)
								NfSpareBuf[OOB_BBI_OFF] = 0x00;
							else
								NfSpareBuf[5] = 0x00;
							if(isLastPage){
								rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
								rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
							}else{
								rtk_write_ecc_page(bbt_v2r_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
								rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
							}						        
					        load_bbt_error = 1;
					        //return -1;
					        //goto TRY_LOAD_BBT1;
			                error_count++;					        
					        break;
				        }
						if(!NAND_ADDR_CYCLE)
			    			isbbt = NfSpareBuf[OOB_BBI_OFF];
		    			else
			    			isbbt = NfSpareBuf[5];
						if(isbbt == BBT_TAG){//check bb tag in each page!
				        memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				        page_counter++;
				        mem_page_num_tmp--;
						}else{
                            load_bbt_error=1;
			                error_count++;
					        printf("[%s] check bbt_v2r table%d tag:0x%x fail!!\n\r", __FUNCTION__,i,isbbt);
							break;						
						}
			        }
					if(!load_bbt_error){
			            memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
					    printf("check bbt_v2r table:%d OK\n\r",i);
					    goto CHECK_V2R_BBT_OK;
					}
			    }else{
					printf("Create bbt_v2r table:%d is_first_boot %d\n\r",i,is_first_boot);
					if(is_first_boot)
					    create_v2r_remapping(bbt_v2r_page+(i*ppb),block_v2r_num);
			    }
		    }else{
				printf("read bbt_v2r table:%d page:%d error\n\r",i, bbt_v2r_page+(i*ppb));
				rtk_erase_block(bbt_v2r_page+(ppb*i));
				if(!NAND_ADDR_CYCLE)
					NfSpareBuf[OOB_BBI_OFF] = 0x00;
				else
					NfSpareBuf[5] = 0x00;
				if(isLastPage){
					rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
				}else{
					rtk_write_ecc_page(bbt_v2r_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_v2r_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
				}
		    }
		}else{
            printf("bbt_v2r table:%d block:%d page:%d is bad\n\r",i,(bbt_v2r_page/ppb)+i,bbt_v2r_page+(i*ppb));
			error_count++;
		}
	}
CHECK_V2R_BBT_OK:	
//end!
#if 0
	rc = rtk_read_ecc_page(bbt_v2r_page, &NfDataBuf, &NfSpareBuf,page_size);
    if(!NAND_ADDR_CYCLE)
	    isbbt = NfSpareBuf[0];
	else
		isbbt = NfSpareBuf[5];

    //printf("[%s, line %d] isbbt_b0 = %d rc %d\n\r",__FUNCTION__,__LINE__,isbbt, rc);

	if(!rc){
		if(isbbt == BBT_TAG){
			printf("[%s] have created bbt_v2r B0 on block %d, just loads it !!\n\r", __FUNCTION__,bbt_v2r_page/ppb);
			memcpy( temp_BBT, &NfDataBuf, page_size );
			page_counter++;
			mem_page_num--;
			while( mem_page_num>0 ){
				if( rtk_read_ecc_page((bbt_v2r_page+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					printf("[%s] load bbt_v2r B0 error!!\n\r", __FUNCTION__);
					//free(temp_BBT);
					load_bbt1 = 1;
					//return -1;
					goto TRY_LOAD_BBT1;
				}
				memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;
			}
			memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
		}else{
			printf("[%s] read bbt_v2r B0 tags fails, try to load bbt_v2r B1\n\r", __FUNCTION__);
			rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
            if(!NAND_ADDR_CYCLE)
	            isbbt = NfSpareBuf[0];
	        else
		        isbbt = NfSpareBuf[5];	
			if ( !rc ){
				if ( isbbt == BBT_TAG ){
					printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n", __FUNCTION__, (bbt_v2r_page/ppb)+1);
					memcpy( temp_BBT, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;

					while( mem_page_num>0 ){
						if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
							printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
							free(temp_BBT);
							return -1;
						}
						memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
						page_counter++;
						mem_page_num--;
					}
					memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
				}else{
					printf("[%s] read bbt_v2r B1 tags fails, nand driver will creat bbt_v2r B0 and B1\n\r", __FUNCTION__);
					create_v2r_remapping(bbt_v2r_page,block_v2r_num);
					create_v2r_remapping(bbt_v2r_page+ppb,block_v2r_num);
				} //BBT2_TAG
			}else{
				printf("[%s] read bbt_v2r B1 with HW ECC fails, nand driver will creat BBT B0\n", __FUNCTION__);
				create_v2r_remapping(bbt_v2r_page, block_v2r_num);
			}
		}// if BBT_TAG
	}else{
		printf("[%s] read bbt_v2r B0 with HW ECC error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_v2r_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy(bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
			}else{
				printf("[%s] read bbt_v2r B1 tags fails, nand driver will creat BBT B1\n\r", __FUNCTION__);
				create_v2r_remapping(bbt_v2r_page+ppb,block_v2r_num);//fix me! later
			}
		}else{
			printf("[%s-%d:] read bbt_v2r B0 and B1 with HW ECC fails\n\r", __FUNCTION__,__LINE__);
			free(temp_BBT);
			return -1;
		}
	}

TRY_LOAD_BBT1:
    if(load_bbt1){
		rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_v2r_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy(bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
			}
		}
	}
#endif
	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
        rc = -1;
		printf("%d v2r table are all bad!(T______T)\n\r", BACKUP_BBT);
	}
    return rc;
}
//####################################################################
// Function : scan_last_die_BB
// Description : Scan the Bad Block
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int scan_last_die_BB(void)
{
	unsigned int start_page;
	unsigned int block_num;
	unsigned int addr;
	unsigned int rc;
	//int remap_block[RBA];
	int block_offset=0;
	int remap_count = 0;
	int i,j, table_index=0 , numchips=1;
	//start_page = BOOT_SIZE;
	block_num = (chip_size/block_size);

	unsigned char *block_status = (unsigned char *) malloc( block_num );
	if ( !block_status ){
		printf("%s: Error, no enough memory for block_status\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT;
	}
	memset ( (unsigned int *)block_status, 0, block_num );

	int *remap_block = (unsigned int*)malloc(sizeof(int)*RBA);
	if ( !remap_block ){
		printf("%s: Error, no enough memory for remap_block\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT;
	}
	memset ( (unsigned int *)remap_block, 0, sizeof(int)*RBA );


	/*search bad block of all Nand flash area.*/
	for( addr=0; addr<chip_size; addr+=block_size ){
		int bb = addr >> block_shift;
		if ( rtk_block_isbad(addr) ){
			block_status[bb] = 0xff;
			printf("block[%d] is bad\n",bb);
		}
	}




    /*check  bad block in RBA;*/
	for ( i=0; i<RBA; i++){
		if ( block_status[(block_num-1)-i] == 0x00){
			remap_block[remap_count] = (block_num-1)-i;
			//printf("A: remap_block[%d]=%x %x\n",remap_count, remap_block[remap_count], (block_num-1)-i);
			remap_count++;
		}
	}

    /*If there are some bad blocks in RBA, the remain remap block just map to RB_INIT*/
	if (remap_count<RBA+1){
		for (j=remap_count+1; j<RBA+1; j++){
			remap_block[j-1] = RB_INIT;
			//printf("B: remap_block[%d]=%x\n",j-1, remap_block[j-1]);
		}
	}

#if 0
	//skip 1MB bootloader region
    block_offset = start_page >> block_shift;
//	for ( i=0; i<(block_num-RBA); i++){//remab bad block from start_page and before RBA
	for ( i=block_offset; i<(block_num-RBA_ori); i++){//remab bad block from start_page and before RBA

		if (block_status[i] == 0xff){
			bbt[table_index].bad_block = i;
			bbt[table_index].BB_die = numchips-1;
			bbt[table_index].remap_block = remap_block[table_index];
			bbt[table_index].RB_die = numchips-1;
printf("A:bbt[%d].bad_block = %x ",table_index, bbt[table_index].bad_block);
printf("A:bbt[%d].remap_block = %x \n",table_index, bbt[table_index].remap_block);			
			table_index++;
		}
	}
#endif
	for( i=table_index; table_index<RBA; table_index++){
		bbt[table_index].bad_block = BB_INIT;
		bbt[table_index].BB_die = BB_DIE_INIT;
		bbt[table_index].remap_block = remap_block[table_index];
		bbt[table_index].RB_die = numchips-1;
//printf("B:bbt[%d].bad_block = %x ",table_index, bbt[table_index].bad_block);
//printf("B:bbt[%d].remap_block = %x \n",table_index, bbt[table_index].remap_block);
	}
	
EXIT:
	if (rc){
		if (block_status)
			free(block_status);	
	}
	if(remap_block)
		free(remap_block);
	return 0;	
	
}

//####################################################################
// Function : rtk_create_bbt
// Description : Create Bad Block Table in specific block
// Input:
//		page: the page we want to put the BBT, it must be block alignment
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int rtk_create_bbt( int page)
{
	int rc = 0;
	unsigned char *temp_BBT = 0;
	unsigned char mem_page_num, page_counter=0;

	if ( scan_last_die_BB() ){
		printf("[%s] scan_last_die_BB() error !!\n\r", __FUNCTION__);
		return -1;
	}

	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	temp_BBT = (unsigned char *) malloc( mem_page_num*page_size);
	if ( !temp_BBT ){
		printf("%s: Error, no enough memory for temp_BBT\n\r",__FUNCTION__);
		return -1;
	}

	memset( temp_BBT, 0xff, mem_page_num*page_size);

  	if ( rtk_erase_block( page)){
		printf("[%s]erase block %d failure !!\n\r", __FUNCTION__, page/ppb);
		rc =  -1;
    	if(!NAND_ADDR_CYCLE)
	    	NfSpareBuf[OOB_BBI_OFF] = 0x00;
    	else
			NfSpareBuf[5] = 0x00;
		if(isLastPage){
			rtk_write_ecc_page(page+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
			rtk_write_ecc_page(page+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
		}else{
			rtk_write_ecc_page(page,temp_BBT, &NfSpareBuf, page_size);
			rtk_write_ecc_page(page+1,temp_BBT, &NfSpareBuf, page_size);
		}		
		goto EXIT;
	}
    if(!NAND_ADDR_CYCLE)
	    //NfSpareBuf[0] = BBT_TAG;
	    NfSpareBuf[OOB_BBI_OFF] = BBT_TAG;
    else
		NfSpareBuf[5] = BBT_TAG;
	memcpy( temp_BBT, bbt, sizeof(struct BB_t)*RBA );
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT page %d failure!!\n\r", __FUNCTION__, page+page_counter);
				rc =  -1;
			rtk_erase_block(page);
	    	if(!NAND_ADDR_CYCLE)
	    		NfSpareBuf[OOB_BBI_OFF] = 0x00;
    		else
				NfSpareBuf[5] = 0x00;			
			if(isLastPage){
				rtk_write_ecc_page(page+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
				rtk_write_ecc_page(page+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
			}else{
				rtk_write_ecc_page(page,temp_BBT, &NfSpareBuf, page_size);
				rtk_write_ecc_page(page+1,temp_BBT, &NfSpareBuf, page_size);
			}			
				goto EXIT;
		}
		page_counter++;
		mem_page_num--;		
	}

EXIT:
	if (temp_BBT)
		free(temp_BBT);
		
	return rc;	

}

//####################################################################
// Function : dump_BBT
// Description : Dump Bad Block Table
//			  [ a ] ( b, c, d, e )
//			  a: index;  b: BB_die, c : bad_block; d : RB_die; e : remap_block
//			  ex: [0] (0, 168, 0, 2047)
// Input:
//		NON
// Output:
//		NON
//####################################################################
void dump_BBT(void)
{
	int i;
	int BBs=0;

	printf("[%s] Nand BBT Content\n\r", __FUNCTION__);

	for ( i=0; i<RBA; i++){
		if ( i==0 && bbt[i].BB_die == BB_DIE_INIT && bbt[i].bad_block == BB_INIT ){
			printf("Congratulation!! No BBs in this Nand.\n\r");
			break;
		}
		if ( bbt[i].bad_block != BB_INIT ){
			printf("[%d] (%d, %u, %d, %u)\n\r", i, bbt[i].BB_die,bbt[i].bad_block, 
				bbt[i].RB_die, bbt[i].remap_block);
			BBs++;
		}
	}
	//this->BBs = BBs;
	return;
}

//####################################################################
// Function : rtk_nand_scan_bbt
// Description : Scan for bad block table
//			  bootcode @ block 0, BBT0 @ block 1, BBT1 @ block 2
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
int rtk_nand_scan_bbt(void)
{
	unsigned char *temp_BBT=0;
	unsigned char mem_page_num, page_counter=0, mem_page_num_tmp=0;
	unsigned int rc=0, i=0, error_count=0;
	unsigned char isbbt;
	unsigned char load_bbt_error = 0, is_first_boot=1;
	//unsigned char check0, check1, check2, check3;
	unsigned int bbt_page;
    unsigned int addr;
    /*czyao, reserve RESERVED_AREA bytes area for bootloader, 
      and the last 'BACKUP_BBT' blocks of RESERVED_AREA area is for bad block table*/
	bbt_page = ((BOOT_SIZE/block_size)-BACKUP_BBT)*ppb;
	
	printf("[%s, line %d] bbt_page = %d \n\r",__FUNCTION__,__LINE__,bbt_page);

	RBA = ((unsigned int)chip_size/block_size)*RBA_PERCENT / 100;

	printf("[%s, line %d] RBA = %d \n\r",__FUNCTION__,__LINE__,RBA);

	//czyao 
	bbt = (unsigned char *) malloc(sizeof(struct BB_t)*RBA);
	if(!bbt){
		printf("%s-%d: Error, no enough memory for bbt\n",__FUNCTION__,__LINE__);
		return FAIL;
	}

	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);

	temp_BBT =(unsigned char *)malloc( mem_page_num*page_size );
	if(!temp_BBT){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}

	memset( temp_BBT, 0xff, mem_page_num*page_size);
//test NEW method!
	for(i=0;i<BACKUP_BBT;i++){
	    rc = rtk_read_ecc_page(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
	    if(!NAND_ADDR_CYCLE)
		    //isbbt = NfSpareBuf[0];
		    isbbt = NfSpareBuf[OOB_BBI_OFF];
	    else
		    isbbt = NfSpareBuf[5];
		if(!rc){
		    if(isbbt==BBT_TAG)//check bbt has already created
				is_first_boot = 0;
		}
	}
	printf("%s: is_first_boot:%d\n\r",__FUNCTION__, is_first_boot);

	for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;load_bbt_error=0;
		rc = rtk_block_isbad((bbt_page+(i*ppb))*page_size);
		if(!rc){
			printf("load bbt table:%d page:%d\n\r",i, (bbt_page+(i*ppb)));
		    rc = rtk_read_ecc_page(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
		    if(!NAND_ADDR_CYCLE)
			    //isbbt = NfSpareBuf[0];
			    isbbt = NfSpareBuf[OOB_BBI_OFF];
		    else
			    isbbt = NfSpareBuf[5];
	        if(!rc){
			    if(isbbt == BBT_TAG){
			        printf("[%s] have created bbt table:%d on block %d, just loads it !!\n\r", __FUNCTION__,i,(bbt_page/ppb)+i);
			        memcpy( temp_BBT, &NfDataBuf, page_size );
			        page_counter++;
			        mem_page_num_tmp--;
			        while( mem_page_num_tmp>0 ){
				        if( rtk_read_ecc_page((bbt_page+(i*ppb)+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					        printf("[%s] load bbt table%d error!!\n\r", __FUNCTION__,i);
					        //free(temp_BBT);
					        //load_bbt1 = 1;
							rtk_erase_block(bbt_page+(ppb*i));
							if(!NAND_ADDR_CYCLE)
								NfSpareBuf[OOB_BBI_OFF] = 0x00;
							else
								NfSpareBuf[5] = 0x00;
							if(isLastPage){
								rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
								rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
							}else{
								rtk_write_ecc_page(bbt_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
								rtk_write_ecc_page(bbt_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
							}					        
                            load_bbt_error=1;
					        //return -1;
					        //goto TRY_LOAD_BBT1;
			                error_count++;					        
					        break;
				        }
						if(!NAND_ADDR_CYCLE)
			    			isbbt = NfSpareBuf[OOB_BBI_OFF];
		    			else
			    			isbbt = NfSpareBuf[5];
						if(isbbt == BBT_TAG){//check bb tag in each page!
				        memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				        page_counter++;
				        mem_page_num_tmp--;
						}else{
                            load_bbt_error=1;
			                error_count++;
					        printf("[%s] check bbt table%d tag:0x%x fail!!\n\r", __FUNCTION__,i,isbbt);
							break;						
						}						
			        }
					if(!load_bbt_error){
					    memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
					    printf("check bbt table:%d OK\n\r",i);
					    goto CHECK_BBT_OK;
					}
			    }else{
					printf("Create bbt table:%d is_first_boot:%d\n\r",i, is_first_boot);
					if(is_first_boot)
				        rtk_create_bbt(bbt_page+(i*ppb));
			    }
		    }else{
				printf("read bbt_v2r table:%d page:%d error\n\r",i, bbt_page+(i*ppb));
				rtk_erase_block(bbt_page+(ppb*i));
				if(!NAND_ADDR_CYCLE)
					NfSpareBuf[OOB_BBI_OFF] = 0x00;
				else
					NfSpareBuf[5] = 0x00;
				if(isLastPage){
					rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
				}else{
					rtk_write_ecc_page(bbt_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
				}
		    }
		}else{
            printf("bbt table:%d block:%d page:%d is bad\n\r",i,(bbt_page/ppb)+i,bbt_page+(i*ppb));
			error_count++;
		}
	}
CHECK_BBT_OK:		
#if 0
    /*check bbt0*/
	rc = rtk_read_ecc_page(bbt_page, &NfDataBuf, &NfSpareBuf,page_size);

    if(!NAND_ADDR_CYCLE)
	    isbbt = NfSpareBuf[0];
	else
		isbbt = NfSpareBuf[5];
	//check0 = NfSpareBuf[0];
	//check1 = NfSpareBuf[1];
	//check2 = NfSpareBuf[2];
	//check3 = NfSpareBuf[3];
	//printf("[%s, line %d] isbbt_b0 = %d rc %d\n\r",__FUNCTION__,__LINE__,isbbt_b0, rc);

	if(!rc){
		if(isbbt == BBT_TAG){
			printf("[%s] have created bbt B0 on block %d, just loads it !!\n\r", __FUNCTION__,bbt_page/ppb);
			memcpy( temp_BBT, &NfDataBuf, page_size );
			page_counter++;
			mem_page_num--;

			while( mem_page_num>0 ){
				if( rtk_read_ecc_page((bbt_page+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					printf("[%s] load bbt B0 error!!\n\r", __FUNCTION__);
					//free(temp_BBT);
					/*runtime error in loading bbt0, we should try to load BBT1*/
					load_bbt1 = 1;
					//return -1;
					goto TRY_LOAD_BBT1;
				}
				memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;
			}
			memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
		}else{
            /*condition: (1):in the first time, to load bbt1, and bbt0 doesn't have BBT_TAG*/
			printf("[%s] read BBT B0 tags fails, try to load BBT B1\n\r", __FUNCTION__);
			rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
            if(!NAND_ADDR_CYCLE)
	            isbbt = NfSpareBuf[0];
	        else
		        isbbt = NfSpareBuf[5];	
			if ( !rc ){
				if ( isbbt == BBT_TAG ){
					printf("[%s] have created bbt B1 on block %d, just loads it !!\n", __FUNCTION__, (bbt_page/ppb)+1);
					memcpy( temp_BBT, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;

					while( mem_page_num>0 ){
						if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
							printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
							free(temp_BBT);
			                /*condition: bbt0 and bbt1 are all bad, goodbye!!!
			                             change another flash chip!!!!*/							
							return -1;
						}
						memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
						page_counter++;
						mem_page_num--;
					}
					memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
				}else{
		            /*the only condition: (1):in the first time to create bbt */
					printf("[%s] read BBT B1 tags fails, nand driver will creat BBT B0 and B1\n\r", __FUNCTION__);
					rtk_create_bbt(bbt_page);
					rtk_create_bbt(bbt_page+ppb);
				}
			}else{
	            /*condition: in first time we found that bbt1 is bad,  try to create bbt0*/			
				printf("[%s] read BBT B1 with HW ECC fails, nand driver will creat BBT B0\n", __FUNCTION__);
				rtk_create_bbt(bbt_page);
			}
		}// if BBT_TAG
	}else{
		/*condition: bbt0 is bad*/
		printf("[%s] read BBT B0 with HW ECC error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
			            /*condition: bbt0 and bbt1 are all bad, goodbye!!!
			                      change another flash chip!!!!*/							
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy( bbt , temp_BBT, sizeof(struct BB_t)*RBA );
			}else{
				printf("[%s] read BBT B1 tags fails, nand driver will creat BBT B1\n\r", __FUNCTION__);
				rtk_create_bbt( bbt_page+ppb);
			}
		}else{
			/*condition: bbt0 and bbt1 are all bad, goodbye!!!change another flash chip!!!!*/
			printf("[%s-%d:] read BBT B0 and B1 with HW ECC fails\n\r", __FUNCTION__,__LINE__);
			free(temp_BBT);
			return -1;
		}
	}
TRY_LOAD_BBT1:
	if(load_bbt1){
		printf("[%s] read BBT B0 have runtime error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];
		if ( !rc ){
		    if ( isbbt == BBT_TAG ){
			    printf("[%s] have created bbt B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_page/ppb)+1);
			    memcpy( temp_BBT, &NfDataBuf, page_size );
			    page_counter++;
			    mem_page_num--;
			    while( mem_page_num>0 ){
				    if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					    printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
					    free(temp_BBT);
					    /*condition: bbt0 and bbt1 are all bad, goodbye!!!
					       change another flash chip!!!!*/							
					    return -1;
				    }
				    memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				    page_counter++;
				    mem_page_num--;
			    }
			    memcpy( bbt , temp_BBT, sizeof(struct BB_t)*RBA );
		    }
		}
	}
#endif	
	dump_BBT();

	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
        rc = -1;
		printf("%d bbt table are all bad!(T______T)\n\r", BACKUP_BBT);
	}		
	return rc;

}
#endif //CONFIG_RTK_NAND_BBT


#ifdef CONFIG_RTK_REMAP_BBT

static int check_BBT(unsigned int blk)
{	
	int i;	
	printf("[%s] blk:%d\n", __FUNCTION__, blk);	

	for ( i=0; i<RBA; i++)	
	{		
	    if ( bbt[i].bad_block == blk )
        {			
            printf("blk 0x%x already exist\n",blk);			
		    return -1;			
	    }	
	}	
	return 0;
}



//int rtk_update_bbt ( unsigned char *data_buf, unsigned char *oob_buf,  struct BB_t *bbt)
int rtk_update_bbt (struct BB_t *bbt)
{
	int rc = 0, i=0, error_count=0;
	unsigned char *temp_BBT = 0;
	unsigned int bbt_page;
	unsigned char mem_page_num=0, page_counter=0;
	unsigned char mem_page_num_tmp=0, page_counter_tmp=0;

	//czyao, reserve 1M bytes area for bootloader, and the last 2 blocks of 1M area is for bad block table
	bbt_page = ((BOOT_SIZE/block_size)-BACKUP_BBT)*ppb;
	
	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	printf("[%s] mem_page_num %d\n\r", __FUNCTION__, mem_page_num);
	
	temp_BBT = (unsigned char *)malloc(mem_page_num*page_size);
	if ( !(temp_BBT) ){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}	
	memset(temp_BBT, 0xff, mem_page_num*page_size);
	memcpy(temp_BBT, bbt, sizeof(struct BB_t)*RBA );
	//memcpy(data_buf, temp_BBT, page_size);
/*
	if(!NAND_ADDR_CYCLE)
	    NfSpareBuf[0] = BBT_TAG;
	else
	    NfSpareBuf[5] = BBT_TAG;
*/	    
//test new method
    for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;
		if(!NAND_ADDR_CYCLE)
			//NfSpareBuf[0] = BBT_TAG;
			NfSpareBuf[OOB_BBI_OFF] = BBT_TAG;
		else
			NfSpareBuf[5] = BBT_TAG;
		if(rtk_erase_block(bbt_page+(ppb*i))){
			printf("[%s]error: erase BBT%d page %d failure\n\r", __FUNCTION__,i, bbt_page+(ppb*i));
			/*erase fail: mean this block is bad, so do not write data!!!*/
			if(!NAND_ADDR_CYCLE)
				NfSpareBuf[OOB_BBI_OFF] = 0x00;				
			else
				NfSpareBuf[5] = 0x00;
			if(isLastPage){
				rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
				rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
			}else{
				rtk_write_ecc_page(bbt_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
				rtk_write_ecc_page(bbt_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
			}			
			mem_page_num_tmp = 0; 
			error_count++;
		}
		while( mem_page_num_tmp>0 ){
			//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
			//	this->g_oobbuf, 1) )
			if(rtk_write_ecc_page(bbt_page+(ppb*i)+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
					printf("[%s] write BBT%d page %d failure!!\n\r", __FUNCTION__,i, bbt_page+(ppb*i)+page_counter);
					//rc =  -1;
					//goto EXIT;
				rtk_erase_block(bbt_page+(ppb*i));
				if(!NAND_ADDR_CYCLE)
					NfSpareBuf[OOB_BBI_OFF] = 0x00;
				else
					NfSpareBuf[5] = 0x00;
				if(isLastPage){
					rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
				}else{
					rtk_write_ecc_page(bbt_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
					rtk_write_ecc_page(bbt_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
				}						
					error_count++;
					break;
			}
		//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
			page_counter++;
			mem_page_num_tmp--; 	
		}	 

	}
//end
#if 0	
	if ( sizeof(struct BB_t)*RBA <= page_size){
		memcpy( data_buf, bbt, sizeof(struct BB_t)*RBA );
	}else{
		temp_BBT = (unsigned char *)malloc(2*page_size);
		if ( !(temp_BBT) ){
			printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
			return FAIL;
		}
		memset(temp_BBT, 0xff, 2*page_size);
		memcpy(temp_BBT, bbt, sizeof(struct BB_t)*RBA );
		memcpy(data_buf, temp_BBT, page_size);
	}
#endif
#if 0 //test new method	
	if(rtk_erase_block(bbt_page)){
		printf("[%s]error: erase block 1 page %d failure\n\r", __FUNCTION__, bbt_page);
	}
#endif	
#if 0
	if(rtk_write_ecc_page(bbt_page, data_buf, oob_buf, page_size)){
		printf("[%s]update BBT B0 page 0 failure\n\r", __FUNCTION__);
	}else{
		if ( sizeof(struct BB_t)*RBA > page_size){
			memset(data_buf, 0xff, page_size);
			memcpy( data_buf, temp_BBT+page_size, sizeof(struct BB_t)*RBA - page_size );
			if(rtk_write_ecc_page(bbt_page+1, data_buf,  oob_buf, page_size)){
				printf("[%s]update BBT B0 page 1 failure\n\r", __FUNCTION__);
			}
		}	
	}
#endif
#if 0 //test new method	

	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(bbt_page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT1 page %d failure!!\n\r", __FUNCTION__, bbt_page+page_counter);
				rc =  -1;
				goto EXIT;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
		page_counter++;
		mem_page_num--;		
	}

	if ( rtk_erase_block(ppb+bbt_page)){
		printf("[%s]error: erase block 2, page %d failure\n\r", __FUNCTION__, ppb+bbt_page);
        rc = -1;
		goto EXIT;
	}
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(bbt_page+ppb+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT2 page %d failure!!\n\r", __FUNCTION__, bbt_page+ppb+page_counter);
				rc =  -1;
				goto EXIT;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
		page_counter++;
		mem_page_num--;		
	}
#endif	
#if 0	
	if(rtk_write_ecc_page(ppb+bbt_page, data_buf, oob_buf, page_size)){
		printf("[%s]update BBT B1 failure\n\r", __FUNCTION__);
		return FAIL;
	}else{
		if ( sizeof(struct BB_t)*RBA > page_size){
			memset(data_buf, 0xff, page_size);
			memcpy( data_buf, temp_BBT+page_size, sizeof(struct BB_t)*RBA - page_size );
			if(rtk_write_ecc_page(ppb+bbt_page+1, data_buf,  oob_buf, page_size)){
				printf("[%s]error: erase block 0 failure\n\r", __FUNCTION__);
				return FAIL;
			}
		}		
	}
#endif
EXIT:
	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
		rc = -1;
		printf("%d table are all bad!(T______T)\n\r", BACKUP_BBT);
	}
	return rc;
}
#endif //CONFIG_RTK_REMAP_BBT

#ifdef CONFIG_RTK_NORMAL_BBT
int rtk_update_normal_bbt (struct BBT_normal *bbt_nor)
{
	int rc = 0, i=0, error_count=0;
	unsigned char *temp_BBT;
	unsigned char *oob_buf = BBToobBuf;
	unsigned int bbt_page;
	unsigned char mem_page_num=0, page_counter=0;
	unsigned char mem_page_num_tmp=0, page_counter_tmp=0;
	bbt_page = ((CONFIG_NORMAL_BBT_POSITION/block_size)-BACKUP_BBT)*ppb;
	mem_page_num = (sizeof(struct BBT_normal)*bbt_num + page_size-1 )/page_size;
	printf("[%s] mem_page_num %d\n\r", __FUNCTION__, mem_page_num);
/*
	temp_BBT = (unsigned char *)malloc(mem_page_num*page_size);
	if ( !(temp_BBT) ){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}	
*/	
	temp_BBT = BBTdataBuf;
	memset(temp_BBT, 0xff, mem_page_num*CHUNK_SIZE);
	memcpy(temp_BBT, bbt_nor, sizeof(struct BBT_normal)*bbt_num);
    for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;
#ifdef CONFIG_SPI_NAND_FLASH
		temp_BBT[2048] = BBT_TAG;
#else
		if(!NAND_ADDR_CYCLE)
			oob_buf[OOB_BBI_OFF] = BBT_TAG;
		else
			oob_buf[5] = BBT_TAG;
#endif		
		if(rtk_nand_base_info._erase_block(bbt_page+(ppb*i))){
			printf("[%s]error: erase normal BBT%d page %d failure\n\r", __FUNCTION__,i, bbt_page+(ppb*i));
			mem_page_num_tmp = 0; 
			error_count++;
#ifdef CONFIG_SPI_NAND_FLASH
			temp_BBT[2048] = BBT_TAG;
#else
			if(!NAND_ADDR_CYCLE)
				oob_buf[OOB_BBI_OFF] = 0x00;				
			else
				oob_buf[5] = 0x00;
#endif			
			if(isLastPage){
				rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-1),temp_BBT, oob_buf, page_size);
				rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-2),temp_BBT, oob_buf, page_size);
			}else{
				rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i),temp_BBT, oob_buf, page_size);
				rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+1,temp_BBT, oob_buf, page_size);
			}
		}
		while( mem_page_num_tmp>0 ){
#ifdef CONFIG_SPI_NAND_FLASH
			rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+page_counter,temp_BBT+page_counter*CHUNK_SIZE, oob_buf, page_size);
#else
			rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+page_counter,temp_BBT+page_counter*page_size, oob_buf, page_size);
/*
			if(rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+page_counter,temp_BBT+page_counter*page_size, oob_buf, page_size))	{
					printf("[%s] write normal BBT%d page %d failure!!\n\r", __FUNCTION__,i, bbt_page+(ppb*i)+page_counter);
					rtk_nand_base_info._erase_block(bbt_page+(ppb*i));
					if(!NAND_ADDR_CYCLE)
						oob_buf[OOB_BBI_OFF] = 0x00;
					else
						oob_buf[5] = 0x00;
					if(isLastPage){
						rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-1),temp_BBT, oob_buf, page_size);
						rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-2),temp_BBT, oob_buf, page_size);
					}else{
						rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i),temp_BBT, oob_buf, page_size);
						rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+1,temp_BBT, oob_buf, page_size);
					}
					error_count++;
					break;
			}
*/			
#endif			
			page_counter++;
			mem_page_num_tmp--; 	
		}	 
	}
//EXIT:
	//if (temp_BBT)
	//	free(temp_BBT);
    if(error_count >= BACKUP_BBT){
		rc = -1;
		printf("%s: %d normal table are all bad!(T______T)\n\r",__FUNCTION__, BACKUP_BBT);
	}
	return rc;
}


//####################################################################
// Function : scan_normal_BB
// Description : Scan the Bad Block in first boot time!
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int scan_normal_BB(void)
{
	int table_index = 0, i=0;
	int total_page_count=0;
	int block_index=0;
#if 0
	unsigned char *block_status = (unsigned char *) malloc( block_num );
	if ( !block_status ){
		printf("%s: Error, no enough memory for block_status\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT;
	}
	memset ( (unsigned int *)block_status, 0, block_num );
#endif	
    /*total block number in this chip*/
	//block_num = (chip_size >> block_shift);
    /*skip block 0*/
	//addr = block_size;
	/*search bad block of all Nand flash area.*/
	//for( addr=0; addr<chip_size; addr+=block_size ){
	total_page_count = (chip_size >> page_shift);
printf("[%s-%d] total_page_count=%d\n",__func__,__LINE__,total_page_count);

	for( i=0; i<total_page_count; i+=ppb ){
		block_index = (i/ppb);
		if ( rtk_block_isbad(i) ){
			//block_status[block_index] = 0xff;
			printf("block[%d] is bad\n",block_index);
			bbt_nor[table_index].bad_block = block_index;
			/*bad block in manufacture time*/
			bbt_nor[table_index].block_info = 0x00;
			bbt_nor[table_index].BB_die = 0;			
			bbt_nor[table_index].RB_die = 0;
//printf("B:bbt_nor[%d].bad_block = %x ",table_index, bbt_nor[table_index].bad_block);
//printf("B:bbt_nor[%d].block_info = %x \n",table_index, bbt_nor[table_index].block_info);
			table_index++;
		}
		if(table_index >= bbt_num){
			printf("bad block number %d exceed bbt_num %d\n",table_index,bbt_num);
			return -1;
		}		
		
	}
	for( i=table_index; table_index<bbt_num; table_index++){
		bbt_nor[table_index].bad_block = BB_INIT;
		bbt_nor[table_index].BB_die = BB_DIE_INIT;
		bbt_nor[table_index].block_info = 0xff;
		bbt_nor[table_index].RB_die = BB_DIE_INIT;
//printf("B:bbt_nor[%d].bad_block = %x ",table_index, bbt_nor[table_index].bad_block);
//printf("B:bbt_nor[%d].block_info = %x \n",table_index, bbt_nor[table_index].block_info);
	}
	
	return 0;	
	
}
//####################################################################
// Function : rtk_create_normal_bbt
// Description : Create normal Bad Block Table in specific block
// Input:
//		page: the page we want to put the normal BBT, it must be block alignment
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int rtk_create_normal_bbt( int page)
{
	int rc = 0;
	unsigned char *temp_BBT = 0;
	unsigned char mem_page_num, page_counter=0;
	unsigned char *oob_buf;
	if ( scan_normal_BB() ){
		printf("[%s] scan_nor_BB() error !!\n\r", __FUNCTION__);
		return -1;
	}

	mem_page_num = (sizeof(struct BBT_normal)*bbt_num + page_size-1 )/page_size;
/*
	temp_BBT = (unsigned char *) malloc( mem_page_num*page_size);
	if ( !temp_BBT ){
		printf("%s: Error, no enough memory for temp_BBT\n\r",__FUNCTION__);
		return -1;
	}
*/
	temp_BBT = BBTdataBuf;
	oob_buf = BBToobBuf;
	memset( temp_BBT, 0xff, mem_page_num*CHUNK_SIZE);

  	if ( rtk_nand_base_info._erase_block(page)){
		printf("[%s]erase block %d failure !!\n\r", __FUNCTION__, page/ppb);
		rc =  -1;
#ifdef CONFIG_SPI_NAND_FLASH
		temp_BBT[2048] = 0x00;
#else
    	if(!NAND_ADDR_CYCLE)
	    	//NfSpareBuf[0] = 0x00;
	    	NfSpareBuf[OOB_BBI_OFF] = 0x00;
    	else
			NfSpareBuf[5] = 0x00;
#endif		
		//mark this block as bad block!
		if ( isLastPage){
			rtk_nand_base_info._nand_write_page_ecc(page+(ppb-1),temp_BBT, oob_buf, page_size);
			rtk_nand_base_info._nand_write_page_ecc(page+(ppb-2),temp_BBT, oob_buf, page_size);
		}else{
			rtk_nand_base_info._nand_write_page_ecc(page,temp_BBT, oob_buf, page_size);
			rtk_nand_base_info._nand_write_page_ecc(page+1,temp_BBT, oob_buf, page_size);
		}
		goto EXIT;
	}
#ifdef CONFIG_SPI_NAND_FLASH
		temp_BBT[2048] = BBT_TAG;
#else	
    if(!NAND_ADDR_CYCLE)
	    //NfSpareBuf[0] = BBT_TAG;
	    oob_buf[OOB_BBI_OFF] = BBT_TAG;
    else
		oob_buf[5] = BBT_TAG;
#endif	
	memcpy( temp_BBT, bbt_nor, sizeof(struct BBT_normal)*bbt_num);
	while( mem_page_num>0 ){
		printf("[%s] page_counter=%d!!\n\r", __FUNCTION__,page_counter);

#ifdef CONFIG_SPI_NAND_FLASH
		rtk_nand_base_info._nand_write_page_ecc(page+page_counter,temp_BBT+page_counter*CHUNK_SIZE, oob_buf, page_size);
#else
		rtk_nand_base_info._nand_write_page_ecc(page+page_counter,temp_BBT+page_counter*page_size, oob_buf, page_size);
/*
		if(rtk_nand_base_info._nand_write_page_ecc(page+page_counter,temp_BBT+page_counter*page_size, oob_buf, page_size))	{
			printf("[%s] write normal BBT page %d failure!!\n\r", __FUNCTION__, page+page_counter);
			rc =  -1;
			rtk_nand_base_info._erase_block(page);
    		if(!NAND_ADDR_CYCLE)
	    		//NfSpareBuf[0] = 0x00;
	    		oob_buf[OOB_BBI_OFF] = 0x00;
    		else
				oob_buf[5] = 0x00;
			//mark this block as bad block!
			if ( isLastPage){
				rtk_nand_base_info._nand_write_page_ecc(page+(ppb-1),temp_BBT, oob_buf, page_size);
				rtk_nand_base_info._nand_write_page_ecc(page+(ppb-2),temp_BBT, oob_buf, page_size);
			}else{
				rtk_nand_base_info._nand_write_page_ecc(page,temp_BBT, oob_buf, page_size);
				rtk_nand_base_info._nand_write_page_ecc(page+1,temp_BBT, oob_buf, page_size);
			}
			goto EXIT;
		}
*/		
#endif
		page_counter++;
		mem_page_num--;		

	}

EXIT:
	//if (temp_BBT)
	//	free(temp_BBT);
		
	return rc;	

}

#endif //CONFIG_RTK_NORMAL_BBT


#ifdef CONFIG_RTK_NORMAL_BBT
//####################################################################
// Function : dump_normal_BBT
// Description : Dump normal Bad Block Table
//			  [ a ] ( b, c, d, e )
//			  a: index;  b: BB_die, c : bad_block; d : RB_die; e : block_info
//			  ex: [0] (0, 168, 0, 0x00) : bad block in manufacture
//			  ex: [0] (0, 168, 0, 0xBA) : bad block in process
// Input:
//		NON
// Output:
//		NON
//####################################################################
void dump_normal_BBT(void)
{
	int i;
	int bad_block_num=0;

	printf("[%s] Nand Normal BBT Content\n\r", __FUNCTION__);
	for ( i=0; i<bbt_num; i++){
		if ( i==0 && bbt_nor[i].BB_die == BB_DIE_INIT && bbt_nor[i].bad_block == BB_INIT ){
			printf("Congratulation!! No BBs in this Nand.\n\r");
			break;
		}
		if ( bbt_nor[i].bad_block != BB_INIT ){
			printf("[%d] (%d, %d, %d, %x)\n\r", i, bbt_nor[i].BB_die,bbt_nor[i].bad_block, 
				bbt_nor[i].RB_die, bbt_nor[i].block_info);
			bad_block_num++;
		}
	}
	//this->BBs = BBs;
	printf("[%s] Nand Normal BBT has %d bad block\n\r", __FUNCTION__,bad_block_num);	
}


/*
*	nand_checkbad_block : check the block is bad or not
*	ofs : block page offset 
*
*/
int nand_checkbad_block (unsigned int flash_page)
{
	unsigned int page, block,i;
	//page = ((int) ofs) >> page_shift;
	//block = flash_page >> ppb;
	block = block_number(flash_page);
//printf("%s: block:%d\n",__FUNCTION__,block);
	for( i=0; i<bbt_num; i++){
//printf("%s: bbt_nor[%d].bad_block:%d block:%d page:%d\n",__FUNCTION__,i,bbt_nor[i].bad_block,block,page);
		if ( bbt_nor[i].bad_block != BB_INIT ){
			if ( block == bbt_nor[i].bad_block ){
				printf("%s: block:%d is bad!\n",__FUNCTION__,block);
				return -1;
			}
		}else
			break;
	}
	return 0;
}

//####################################################################
// Function : nand_scan_normal_bbt
// Description : Scan for normal bad block table
//
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
int nand_scan_normal_bbt(void)
{
	unsigned char *data_buf;
	unsigned char *oob_buf;

	unsigned char mem_page_num, page_counter=0, mem_page_num_tmp=0;
	unsigned int rc=0, i=0, bbt_check_count=0 ,j=0, error_count=0;
	unsigned char isbbt;
	unsigned char load_bbt_error = 0, is_first_boot=1;
	//unsigned char check0, check1, check2, check3;
	unsigned int bbt_page;
    unsigned int addr;
	//unsigned char bbt_check[BACKUP_BBT]={0};
	
    /*czyao, reserve RESERVED_AREA bytes area for bootloader, 
      and the last 'BACKUP_BBT' blocks of RESERVED_AREA area is for bad block table*/
	//bbt_page = ((BOOT_SIZE/block_size)-BACKUP_BBT)*ppb;
	bbt_page = ((CONFIG_NORMAL_BBT_POSITION/block_size)-BACKUP_BBT)*ppb;
	printf("[%s, line %d] bbt_page = %d \n\r",__FUNCTION__,__LINE__,bbt_page);

    bbt_num = ((chip_size >> block_shift)*BBT_PERCENT) / 100;
	printf("[%s, line %d] bbt_num = %d \n\r",__FUNCTION__,__LINE__,bbt_num);

	bbt_nor = (unsigned char *) malloc(sizeof(struct BBT_normal)*bbt_num);
	if(!bbt_nor){
		printf("%s-%d: Error, no enough memory for bbt\n",__FUNCTION__,__LINE__);
		return FAIL;
	}

	mem_page_num = (sizeof(struct BBT_normal)*bbt_num + page_size-1 )/page_size;
	printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);
/*
	temp_BBT =(unsigned char *)malloc( mem_page_num*page_size );
	if(!temp_BBT){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}
*/
	data_buf = BBTdataBuf;
	oob_buf = BBToobBuf;
	//memset( temp_BBT, 0xff, mem_page_num*CHUNK_SIZE);
//NEW method!
	for(i=0;i<BACKUP_BBT;i++){
		rc = rtk_block_isbad((bbt_page+(i*ppb)));
		if(!rc){
				//rc = rtk_read_ecc_page(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size)
				rc = rtk_nand_base_info._nand_read_page_ecc(bbt_page+(i*ppb), data_buf,oob_buf,page_size);
				//delay_msec(100);
#ifdef CONFIG_SPI_NAND_FLASH
//for(j=0;j<10;j++)
#if 0
{
printf("=============DMA-DATA===============================================\n\r");			
				dump_mem(0x20000000 | (unsigned int)data_buf, CHUNK_SIZE);
printf("=============DMA-OOB-DATA===============================================\n\r"); 		
				dump_mem(0x20000000 | (unsigned int)oob_buf, OOB_SIZE);
}
#endif
				isbbt = data_buf[2048];
printf("[%s, line %d] rc:%d isbbt=0x%x 0x%x\n\r",__func__,__LINE__,rc,isbbt,data_buf[2049]);
printf("[%s, line %d] bbt_page+(i*ppb)=%d\n\r",__func__,__LINE__,bbt_page+(i*ppb));

#else
	    		if(!NAND_ADDR_CYCLE)
		    		isbbt = oob_buf[OOB_BBI_OFF];
	    		else
		    		isbbt = oob_buf[5];
#endif				
				if(!rc){
		    		if(isbbt==BBT_TAG){//check bbt has already created
						is_first_boot = 0;
		    		}
				}
		}//check bad block
	}
	printf("[%s, line %d] is_first_boot:%d\n\r",__FUNCTION__,__LINE__,is_first_boot);

	for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;load_bbt_error=0;
		rc = rtk_block_isbad((bbt_page+(i*ppb)));
		if(!rc){
			printf("load normal bbt table:%d page:%d\n\r",i, (bbt_page+(i*ppb)));
			//bbt_check_count = 0;
		    //rc = rtk_read_ecc_page(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
			rc = rtk_nand_base_info._nand_read_page_ecc(bbt_page+(i*ppb), data_buf, oob_buf,page_size);		    
#ifdef CONFIG_SPI_NAND_FLASH
			isbbt = data_buf[2048];
#else
		    if(!NAND_ADDR_CYCLE)
			    //isbbt = NfSpareBuf[0];
			    isbbt = oob_buf[OOB_BBI_OFF];
		    else
			    isbbt = oob_buf[5];
#endif			
	        if(!rc){
			    if(isbbt == BBT_TAG){
			        printf("[%s] have created normal bbt table:%d on block %d, just loads it !!\n\r", __FUNCTION__,i,(bbt_page/ppb)+i);
			        //memcpy( data_buf, NfDataBuf, page_size );
			        page_counter++;
					bbt_check_count++;
			        mem_page_num_tmp--;
			        while( mem_page_num_tmp>0 ){
				        //if( rtk_read_ecc_page((bbt_page+(i*ppb)+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
				        if( rtk_nand_base_info._nand_read_page_ecc((bbt_page+(i*ppb)+page_counter), data_buf, oob_buf, page_size)){
					        printf("[%s] load normal bbt table%d error!! page %d\n\r", __FUNCTION__,i, bbt_page+(i*ppb)+page_counter);
							//rtk_erase_block(bbt_page+(ppb*i));
							rtk_nand_base_info._erase_block(bbt_page+(ppb*i));
							if(!NAND_ADDR_CYCLE)
								oob_buf[OOB_BBI_OFF] = 0x00;
							else
								oob_buf[5] = 0x00;
							//mark this block as bad block!
							if(isLastPage){
								//rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-1),temp_BBT, &NfSpareBuf, page_size);
								//rtk_write_ecc_page(bbt_page+(ppb*i)+(ppb-2),temp_BBT, &NfSpareBuf, page_size);
								rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-1),data_buf, oob_buf, page_size);
								rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-2),data_buf, oob_buf, page_size);
							}else{
								//rtk_write_ecc_page(bbt_page+(ppb*i),temp_BBT, &NfSpareBuf, page_size);
								//rtk_write_ecc_page(bbt_page+(ppb*i)+1,temp_BBT, &NfSpareBuf, page_size);
								rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i),data_buf, oob_buf, page_size);
								rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+1,data_buf, oob_buf, page_size);								
							}							
					        //free(temp_BBT);
					        //load_bbt1 = 1;
                            load_bbt_error=1;
					        //return -1;
					        //goto TRY_LOAD_BBT1;
			                error_count++;					        
					        break;
				        }
#ifdef CONFIG_SPI_NAND_FLASH
						isbbt = data_buf[2048];
#else
						if(!NAND_ADDR_CYCLE)
			    			//isbbt = NfSpareBuf[0];
			    			isbbt = data_buf[OOB_BBI_OFF];
		    			else
			    			isbbt = oob_buf[5];
#endif						
						if(isbbt == BBT_TAG){//check bb tag in each page!
							//memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
							page_counter++;
							mem_page_num_tmp--;
						}else{
                            load_bbt_error=1;
			                error_count++;
					        printf("[%s] check normal bbt table%d tag:0x%x fail!!\n\r", __FUNCTION__,i,isbbt);
							break;
						}
			        }
					
					if(!load_bbt_error){
					    memcpy( bbt_nor, data_buf, sizeof(struct BB_t)*bbt_num );
					    printf("check normal bbt table:%d OK\n\r",i);
					    goto CHECK_BBT_OK;
					}
			    }else{
					printf("Create normal bbt table:%d is_first_boot:%d\n\r",i, is_first_boot);
					if(is_first_boot)
				        rtk_create_normal_bbt(bbt_page+(i*ppb));
			    }
		    }else{
		        //mark as bad block table!
				printf("read normal bbt table:%d page:%d error\n\r",i, (bbt_page+(i*ppb)));
				rtk_nand_base_info._erase_block(bbt_page+(ppb*i));
#ifdef CONFIG_SPI_NAND_FLASH
				data_buf[2048] = 0;
#else
				if(!NAND_ADDR_CYCLE)
					//NfSpareBuf[0] = 0x00;
					oob_buf[OOB_BBI_OFF] = 0x00;
				else
					oob_buf[5] = 0x00;
#endif				
				//mark this block as bad block!
				if(isLastPage){
					rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-1),data_buf, oob_buf, page_size);
					rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+(ppb-2),data_buf, oob_buf, page_size);
				}else{
					rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i),data_buf, oob_buf, page_size);
					rtk_nand_base_info._nand_write_page_ecc(bbt_page+(ppb*i)+1,data_buf, oob_buf, page_size);
				}
			}
		}else{
            printf("normal bbt table:%d block:%d page:%d is bad\n\r",i,(bbt_page/ppb)+i,bbt_page+(i*ppb));
			error_count++;
		}
	}
CHECK_BBT_OK:		

	dump_normal_BBT();

	//if (temp_BBT)
	//	free(temp_BBT);
    if(error_count >= BACKUP_BBT){
        rc = -1;
		printf("normal %d bbt table are all bad!(T______T)\n\r", BACKUP_BBT);
	}		
	return rc;

}
#endif





