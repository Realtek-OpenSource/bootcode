#include "./bspchip_8685.h"
#include "./memctl_8685.h"
#include "./memctl_8685_func.h"
#if defined(CONFIG_BOOT_MIPS) || defined(CONFIG_MIPS)
#include "./ipc.h"

#define IPC_CALIBRATION
#endif

#ifdef CONFIG_FPGA_8685SB
#define MEMCTL_CALI_RETRY_LIMILT					(1)
#else
#define MEMCTL_CALI_RETRY_LIMILT					(5)
#endif
#define MEMCTL_CALI_MIN_READ_WINDOW                  (7)
#define MEMCTL_CALI_MIN_WRITE_WINDOW                 (7)
#ifdef CONFIG_MIPS
#define CPU_DCACHE_SIZE                                 		(0x1000)
#else
#define CPU_DCACHE_SIZE                                 		(0x8000)
#endif
#define MEMCTL_CALI_TARGET_LEN                          	(CPU_DCACHE_SIZE * 2)
#define MEMCTL_CALI_FULL_SCAN_RESOLUTION         (2)
#define MEMCTL_CALI_WRITE_DELAY_START_TAP       (0)
#define MEMCTL_CALI_READ_DELAY_START_TAP         (0)
#define MEMCTL_CALI_TARGET_ADDR                         	(0x80b00000)

#define _REDUCE_CALIBRATION_

/* Adds for SD5-----------------------Start--*/
#ifdef CONFIG_MIPS
#define _memctl_DCache_flush_invalidate _1004K_L1_DCache_flush
#else
void _memctl_DCache_flush_invalidate(void){
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
#endif
/* Adds for SD5-----------------------End--*/

#ifndef CONFIG_MIPS
uint32 _memctl_set_phy_delay(uint32 bit_location, uint32 w_delay, uint32 r_delay);
void _memctl_set_phy_delay_all(uint32 w_delay, uint32 r_delay);
void _memctl_result_to_DQ_RW_Array(uint32 result, uint32 w_delay, uint32 r_delay, uint32 RW_array_addr[32][32]);
void _memctl_dq_rw_array_post_processing(uint32 w_start, uint32 r_start, uint32 resolution, uint32 **DQ_RW_Array);
void  _memctl_set_phy_delay_dqrf(uint32 bit_loc,uint32 max_w_seq_start,uint32 max_w_len,uint32 max_r_seq_start,uint32 max_r_len);
uint32 _memctl_find_proper_RW_dealy(uint32 resolution, uint32 w_start, uint32 r_start, uint32 DQ_RW_Array[32][32]);
#endif

/*
 * Function Implementation
 */

uint32 pat_ary[] __attribute__ ((section(".text")))= {
 			0x00010000, 0x01234567, 0x00000000, 0x76543210,
			0xFFFFFFFF, 0x89abcdef, 0x0000FFFF, 0xfedcba98,
			0xFFFF0000, 0x00FF00FF, 0xFF00FF00, 0xF0F0F0F0,
			0x0F0F0F0F, 0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5,
			0xA5A55A5A, 0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A,
			0x5A5AA5A5, 0x5555AAAA, 0xAAAA5555, 0x5555AAAA,
			0xAAAA5555, 0xAAAA5555, 0x5555AAAA, 0xAAAA5555,
			0x5555AAAA, 0xCC3333CC, 0x33CCCC33, 0xCCCC3333
		   };



void _write_pattern_1(uint32 start_addr, uint32 len)
{
        volatile uint32 *p_start, data_tmp;
        uint32 b_len;
        uint32 ary_i;


        /* In case of write through D-Cache mechanisim, read data in DCache */
        p_start = (volatile uint32 *)start_addr;
        for(b_len = 0; b_len < len; b_len += sizeof(uint32)){
                data_tmp = *p_start;
        }

        /* Write data */
        p_start = (volatile uint32 *)start_addr;
		ary_i = 0;
        for(b_len = 0; b_len < len; b_len += sizeof(uint32)){
                *p_start = pat_ary[ary_i];
                p_start++;
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(uint32));
        }

        _memctl_DCache_flush_invalidate();

        return;
}


uint32 _verify_pattern_1(uint32 start_addr, uint32 len)
{

        volatile uint32 *p_start, data_tmp;
        uint32 b_len, err_result;
        uint32 ary_i, pat_data;

        _memctl_DCache_flush_invalidate();

        err_result = 0;

        /* Read data */
		ary_i = 0;
        p_start = (volatile uint32 *)start_addr;
        for(b_len = 0; b_len < len; b_len += sizeof(uint32)){
		data_tmp = *p_start;
		pat_data = pat_ary[ary_i];
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(uint32));
		err_result = err_result | ( (pat_data | data_tmp) & ( ~(pat_data & data_tmp)));
		if(err_result == 0xffffffff)
		        return err_result;
		p_start++;
        }

        return err_result;
}


#ifndef CONFIG_MIPS
void memctl_sync_write_buf(void)
{
	*((volatile unsigned int *)0xB8001038) = 0x80000000;
	while(*((volatile unsigned int *)0xB8001038) & 0x80000000);
	return;
}

void _memctl_set_phy_delay_all(uint32 w_delay, uint32 r_delay)
{
	uint32 i_dq;
        volatile uint32 *ddcrdqr_base;

        ddcrdqr_base = (uint32 *)DACDQR;

	/*printf("%s:%d: wdelay(%d), r_delay(%d)\n", __FUNCTION__, __LINE__, w_delay, r_delay);*/
	for(i_dq = 0; i_dq < 32; i_dq++){
		*ddcrdqr_base = (w_delay << 24) | (r_delay << 8);
		ddcrdqr_base++;
	}

        _memctl_update_phy_param();

	return ;
}

void  _memctl_set_phy_delay_dqrf(uint32 bit_loc,uint32 max_w_seq_start,uint32 max_w_len,uint32 max_r_seq_start,uint32 max_r_len)
{
	volatile uint32 *ddcrdqr_base;
	volatile uint32 *mcr;
	unsigned char r_delay_tap;

	ddcrdqr_base = (volatile uint32 *)DACDQR;
	mcr = (volatile uint32 *)MCR; 

	ddcrdqr_base += bit_loc;
	
	/* For DDR2, DDR3 read delay tap*/
	if((max_r_len-max_r_seq_start)>20){
		r_delay_tap=(max_r_seq_start+max_r_len)/2;
	}else if(max_r_len>12 && max_r_seq_start==0){
		r_delay_tap=max_r_len-12;
	}else if(max_r_seq_start!=0){
		r_delay_tap=(max_r_seq_start+max_r_len)/2;		
	}else{
		r_delay_tap=0;
	}

	/* We currently don't set write DQ delay taps in new mem-controller */
	*ddcrdqr_base = (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | \
					(((max_r_seq_start + (r_delay_tap)) & 0x1f) << 8) | \
					(((max_r_seq_start) & 0x1f) << 0);

	_memctl_update_phy_param();

	return;
}

#ifndef _REDUCE_CALIBRATION_

void _memctl_result_to_DQ_RW_Array(uint32 result, uint32 w_delay, uint32 r_delay, uint32 RW_array_addr[32][32])
{
        /*
         * RW_array_addr [32]   [32] : [Rising 0~15, Falling 0~15] [w_delay]
         *              32bit  W_delay
         */

        uint32 bit_loc, correct_bit;

        /* We mark correct bit */
 	result = ~result;

	if(REG32(DCR) & 0x0F000000 == 0){ /* 8bit mode*/
		result = ((result & 0xFF000000) >> 24) | ((result & 0x00FF0000)) |\
					((result & 0x0000FF00) >> 8) | ((result & 0x000000FF) << 16);
	}else { /* 16bit mode*/
        	result = ((result & 0xFFFF0000) >> 16) | ((result & 0x0000FFFF) << 16);
	}

        for(bit_loc=0; bit_loc < 32; bit_loc++){
                correct_bit = (result >> bit_loc) & 0x1;
                RW_array_addr[bit_loc][w_delay] |= (correct_bit << r_delay);
		//printf("correct_bit(%d), RW_array_addr[%d][%d] = 0x%08x, bit_loc(%d)\n", correct_bit, bit_loc, w_delay, RW_array_addr[bit_loc][w_delay], bit_loc);
        }

        return;
}

uint32 _memctl_find_proper_RW_dealy(uint32 resolution, uint32 w_start, uint32 r_start, uint32 DQ_RW_Array[32][32])
{
        uint32 max_r_seq_start, max_r_len, r_delay, r_seq_start, r_len;
        uint32 max_w_seq_start, max_w_len, w_delay, w_seq_start, w_len, search_seq_start, bit_loc;
        uint32 bit_fail, mode_16bit;


        if(REG32(DCR) & 0x0F000000){
                mode_16bit = 1;
        }else{
                mode_16bit = 0;
        }

        bit_fail = 0;
        for(bit_loc = 0; bit_loc < 32; bit_loc++){

		if(mode_16bit == 0){
	            if((bit_loc > 7) && (bit_loc < 16))
	                    continue;
	            if((bit_loc > 23) && (bit_loc < 32))
	                    continue;
		}

	        max_r_len = 0;
	        max_r_seq_start = 0;
       		max_w_len = 0;
	        max_w_seq_start = 0;
		//printf("bit(%d):\n", bit_loc);
		//puts("Bit:"); puthex(bit_loc); puts(" \n");
		
                /* Searching for the max. sequetial read window. */
                for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
                        r_len = 0;
                        r_seq_start = 0;
                        search_seq_start = 1;
			//printf("   w(%d) %08x\n", w_delay, DQ_RW_Array[bit_loc][w_delay]);
			//puts("w("); puthex(w_delay); puts(") ");
			//puthex(DQ_RW_Array[bit_loc][w_delay]); puts(" \n");
			

			for(r_delay = r_start; r_delay < 32; r_delay+=resolution){
                                if(search_seq_start == 1){
                                        if( (DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1 ){
                                                r_seq_start = r_delay;
                                                search_seq_start = 0;
                                        }
                                        if( (r_delay+resolution) >= 31 ){
                                                r_len = 1;
                                                if(r_len > max_r_len){
                                                        max_r_len = r_len;
                                                        max_r_seq_start = r_seq_start;
                                                        r_len = 0;
                                                        r_seq_start = r_delay + resolution;
                                                }
                                        }

                                }else{
                                        if( 0 == ((DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1) ){
                                                r_len = r_delay - r_seq_start - resolution + 1;
                                                if(r_len > max_r_len){
                                                        max_r_len = r_len;
                                                        max_r_seq_start = r_seq_start;
                                                        r_len = 0;
                                                        r_seq_start = r_delay + resolution;
                                                }
                                                search_seq_start = 1;
                                        }else{
                                                if((r_delay+resolution)  >= 31){
                                                        r_len = r_delay - r_seq_start + 1;
                                                        if(r_len > max_r_len){
                                                                max_r_len = r_len;
                                                                max_r_seq_start = r_seq_start;
                                                                r_len = 0;
                                                                r_seq_start = r_delay + resolution;
                                                        }
                                                }
                                        }
                                }
                        }
                }


                w_len = 0;
                w_seq_start = 0;
                search_seq_start = 1;
                /* Searching for the max. write delay window basing on max. read delay window. */
                for(r_delay = max_r_seq_start ; r_delay < (max_r_seq_start + max_r_len) ; r_delay += resolution){
                        w_len = 0;
                        w_seq_start = 0;
                        search_seq_start = 1;
                        for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
                                if(search_seq_start == 1){
                                        if( (DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1 ){
                                                w_seq_start = w_delay;
                                                search_seq_start = 0;
                                        }
                                        if( (w_delay+resolution) >= 31 ){
                                                w_len = 1;
                                                if(w_len > max_w_len){
                                                        max_w_len = w_len;
                                                        max_w_seq_start = w_seq_start;
                                                        w_len = 0;
                                                        w_seq_start = w_delay + resolution;
                                                }
                                        }

                                }else{
                                        if( 0 == ((DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1) ){
                                                w_len = w_delay - w_seq_start - resolution + 1;
                                                if(w_len > max_w_len){
                                                        max_w_len = w_len;
                                                        max_w_seq_start = w_seq_start;
                                                        w_len = 0;
                                                        w_seq_start = w_delay + resolution;
                                                }
                                                search_seq_start = 1;
                                        }else{
                                                if((w_delay+resolution)  >= 31){
                                                        w_len = w_delay - w_seq_start + 1;
                                                        if(w_len > max_w_len){
                                                                max_w_len = w_len;
                                                                max_w_seq_start = w_seq_start;
                                                                w_len = 0;
                                                                w_seq_start = w_delay + resolution;
                                                        }
                                                }
                                        }
                                }
                        }


                }

		//printf("bit:%02d, max_r_s(%d), max_r_l(%d), max_w_s(%d), max_w_len(%d)\n", bit_loc, max_r_seq_start, max_r_len,  max_w_seq_start, max_w_len);

		puts("Bit:"); puthex(bit_loc); puts(" ");
			puts("max_r_s:"); puthex(max_r_seq_start); puts(" ");
			puts("max_r_l:"); puthex(max_r_len); puts(" ");
			puts("max_w_s:"); puthex(max_w_seq_start); puts(" ");	
			puts("max_w_len:"); puthex(max_w_len); puts(" \r");	
		
		_memctl_set_phy_delay_dqrf(bit_loc, max_w_seq_start, max_w_len, max_r_seq_start, max_r_len);

                if((max_w_len <= MEMCTL_CALI_MIN_WRITE_WINDOW) || (max_r_len <= MEMCTL_CALI_MIN_READ_WINDOW)){
                        bit_fail |= (1 << bit_loc);
                }

        }

        return bit_fail;
}

uint32 _DDR_Calibration_Full_Scan(uint32 target_addr, uint32 len, uint32 resolution, uint32 w_start, uint32 r_start)
{
        uint32 w_delay, r_delay, WR_Result;
        uint32 DQ_RW_Array[32][32];

        /* Initialize DQ_RW_Array */
        for(w_delay = 0; w_delay < 32; w_delay++){
                for(r_delay = 0; r_delay < 32; r_delay++){
                        DQ_RW_Array[w_delay][r_delay] = 0;
                }
        }


        /* Fully scan whole delay tap. */
        for(w_delay = w_start; w_delay < 32; w_delay += resolution){
                for(r_delay = r_start; r_delay < 32; r_delay += resolution){
			_memctl_set_phy_delay_all(w_delay, r_delay);
			_write_pattern_1(target_addr, len);

			memctl_sync_write_buf();

			WR_Result = _verify_pattern_1(target_addr, len);

			/*	puts("w_delay: "); puthex(w_delay);
				puts(" r_delay: "); puthex(r_delay);
				puts(" WR_result: "); puthex(WR_Result);
				puts("\r");*/

			_memctl_result_to_DQ_RW_Array(WR_Result, w_delay, r_delay, &DQ_RW_Array[0][0]);
                }
        }

        /* All scan result is in DQ_RW_Array, choose a proper delay tap setting. */
        if( 0 == _memctl_find_proper_RW_dealy( resolution, w_start, r_start, &DQ_RW_Array[0][0])){
                return 0;/* Pass */
        }else{
                return 1;/* Fali */
        }

}

#else /* _REDUCE_CALIBRATION_ */

#ifdef IPC_CALIBRATION
extern void ipc_write_pattern(unsigned int addr, unsigned int len);
extern unsigned int ipc_verify_pattern(unsigned int addr, unsigned int len) ;
#endif

uint32 _DDR_Calibration_Full_Scan_read_window(uint32 target_addr, uint32 len, uint32 resolution, uint32 w_start, uint32 r_start)
{
	uint8 DQ_WR_array[32][4];
	uint8 w_delay, r_delay, bit_loc; 
	uint8 is_this_bit_correct;
	uint8 mode_16bit;
	uint8 max_r_seq_start, max_r_len;
	uint8 r_seq_start, r_len;
	uint8 search_seq_start;
	uint32 WR_Result = 0, bit_fail = 0;

#define MAX_R_SEQ_START  (0)
#define MAX_R_LEN        (1)
#define R_SEQ_START      (2)
#define SEARCH_SEQ_START (3)

	for(bit_loc = 0; bit_loc < 32; bit_loc++){
		DQ_WR_array[bit_loc][MAX_R_SEQ_START] = 0;
		DQ_WR_array[bit_loc][MAX_R_LEN]       = 0;
	}

	if(REG32(DCR) & 0x0F000000){
		mode_16bit = 1;
	}else{
		mode_16bit = 0;
	}

	for(w_delay = w_start; w_delay < 32; w_delay += resolution){
		//For each w_delay, it is a new search for each w_delay, so that reset the relative information to initial state.
		for(bit_loc = 0; bit_loc < 32; bit_loc++){
			    DQ_WR_array[bit_loc][R_SEQ_START]     = 0;
			    DQ_WR_array[bit_loc][SEARCH_SEQ_START]= 1;
		}

		for(r_delay = r_start; r_delay < 32; r_delay += resolution){

			_memctl_set_phy_delay_all(w_delay, r_delay);
#ifdef IPC_CALIBRATION
			ipc_write_pattern(target_addr, len);
#else	
			_write_pattern_1(target_addr, len);
#endif
			memctl_sync_write_buf();

#ifdef IPC_CALIBRATION
			WR_Result = ipc_verify_pattern(target_addr, len);
			puts("[RLX5281] ipc has finished verify_pattern, result=0x"); puthex(WR_Result); puts("\n");
#else
			WR_Result = _verify_pattern_1(target_addr, len);
#endif			 
			/* We mark correct bit */
			WR_Result = ~WR_Result;

			if( mode_16bit == 0){ /* 8bit mode */
				WR_Result = ((WR_Result & 0xFF000000) >> 24) | ((WR_Result & 0x00FF0000)) | ((WR_Result & 0x0000FF00) >> 8) | ((WR_Result & 0x000000FF) << 16);
			}else{ /* 16bit mode */
				WR_Result = ((WR_Result & 0xFFFF0000) >> 16) | ((WR_Result & 0x0000FFFF) << 16);
			}
            
			for(bit_loc = 0; bit_loc < 32; bit_loc++){

				if(mode_16bit == 0){
					if((bit_loc > 7) && (bit_loc < 16))
						continue;
					if((bit_loc > 23) && (bit_loc < 32))
						continue;
				}              

				max_r_seq_start  	= DQ_WR_array[bit_loc][MAX_R_SEQ_START];
				max_r_len        	= DQ_WR_array[bit_loc][MAX_R_LEN];
				r_seq_start      	= DQ_WR_array[bit_loc][R_SEQ_START];
				search_seq_start	= DQ_WR_array[bit_loc][SEARCH_SEQ_START];

				is_this_bit_correct = ((WR_Result>>bit_loc) & 0x1);

				if(search_seq_start == 1){
					if(is_this_bit_correct == 1){
						r_seq_start = r_delay;
						search_seq_start = 0;
					}
					if( (r_delay+resolution) >= 31 ){
						r_len = 1;
						if(r_len > max_r_len){
							max_r_len = r_len;
							max_r_seq_start = r_seq_start;
							r_len = 0;
							r_seq_start = r_delay + resolution;
						}
					}
				}else{
					if(is_this_bit_correct == 0){
						r_len = r_delay - r_seq_start - resolution + 1;
						if(r_len > max_r_len){
							max_r_len = r_len;
							max_r_seq_start = r_seq_start;
							r_len = 0;
							r_seq_start = r_delay + resolution;
						}
						search_seq_start = 1;
					}else{
						if((r_delay+resolution) >= 31){
							r_len = r_delay - r_seq_start + 1;
							if(r_len > max_r_len){
								max_r_len = r_len;
								max_r_seq_start = r_seq_start;
								r_len = 0;
								r_seq_start = r_delay + resolution;
							}
						}
					}
				}

				if((max_r_len >= DQ_WR_array[bit_loc][MAX_R_LEN])&&(max_r_seq_start >= DQ_WR_array[bit_loc][MAX_R_SEQ_START])){
					DQ_WR_array[bit_loc][MAX_R_SEQ_START] 	= max_r_seq_start;
					DQ_WR_array[bit_loc][MAX_R_LEN]       		= max_r_len;
				}
				DQ_WR_array[bit_loc][R_SEQ_START]     = r_seq_start;
				DQ_WR_array[bit_loc][SEARCH_SEQ_START]= search_seq_start;
			
			}
            
		}         
	}   

	for(bit_loc = 0; bit_loc < 32; bit_loc++){
		//printf("bit:%02d, max_r_s(%d), max_r_len(%d): ", 
		//	bit_loc, DQ_WR_array[bit_loc][MAX_R_SEQ_START], DQ_WR_array[bit_loc][MAX_R_LEN]); 
      
#ifndef CONFIG_FPGA_RTL8685SB
		puts("Bit:"); puthex(bit_loc); puts(" ");
			puts("max_r_s:"); puthex(DQ_WR_array[bit_loc][MAX_R_SEQ_START]); puts(" ");
			puts("max_r_l:"); puthex(DQ_WR_array[bit_loc][MAX_R_LEN]); puts(" \r");
#endif
		if((DQ_WR_array[bit_loc][MAX_R_LEN] <= MEMCTL_CALI_MIN_READ_WINDOW)){
                        bit_fail |= (1 << bit_loc);
               }

		//write back the calibrated delay taps
		_memctl_set_phy_delay_dqrf(bit_loc, 0, 0, DQ_WR_array[bit_loc][MAX_R_SEQ_START], DQ_WR_array[bit_loc][MAX_R_LEN]);        
	}
	
	return bit_fail;
}

#endif /* _REDUCE_CALIBRATION_ */

#ifdef CONFIG_FPGA_RTL8685SB
void _memctl_set_default_window(void)
{
	int bit_loc=0;

	for(bit_loc=0; bit_loc<32; bit_loc++){
		//write back the calibrated delay taps
		_memctl_set_phy_delay_dqrf(bit_loc, 0, 0, 0, 1); 
	}

	return;
}
#endif

uint32 DDR_Calibration(unsigned char full_scan)
{
        uint32 target_addr, len, resolution, w_start, r_start;
	 uint32 retry_limit;
        target_addr 	= MEMCTL_CALI_TARGET_ADDR;
        len         		= MEMCTL_CALI_TARGET_LEN;
        resolution  	= MEMCTL_CALI_FULL_SCAN_RESOLUTION;
        w_start     	= MEMCTL_CALI_WRITE_DELAY_START_TAP;
        r_start     	= MEMCTL_CALI_READ_DELAY_START_TAP;
	retry_limit = 0;

		/* Enable dynamic PHY FIFO Reset */
		REG32(DACCR)= REG32(DACCR) | 0x20;

        /* Do a fully scan to choose a proper point. */
	if(full_scan){
#ifdef _REDUCE_CALIBRATION_
		 while( 0 != _DDR_Calibration_Full_Scan_read_window(target_addr, len, resolution, w_start, r_start)){
#else
	     while( 0 != _DDR_Calibration_Full_Scan(target_addr, len, resolution, w_start, r_start)){
#endif

			 /* Do watchdog reset while calibration is fail */
#ifndef CONFIG_RTL8685SB
			 sys_watchdog_enable(0,0);
#endif				
	                /* Base on the proper point, we do a one dimension scan for the reliabilties. */
			retry_limit++;
			if(retry_limit > MEMCTL_CALI_RETRY_LIMILT){
#ifdef CONFIG_FPGA_RTL8685SB
					_memctl_set_default_window();
#endif				
	               	return 1; /* Fail, need to define failure status. */
			}
		}
	}
	return 0;
}

#endif /* #ifndef CONFIG_MIPS */
