#include "./bspchip_8685.h"
#include "./memctl_8685.h"
#include "./memctl_8685_func.h"

/*
 * Constant definition
 */

/* typedef unsigned int uint32; */
/*  40p per tap delay, real 25p per tap.
             tDS    tDH
  DDR2 400   150p   275p
  DDR2 533   100p   225p
  DDR2 677   100p   175p
  DDR2 800   50p    125p
  DDR3 800   75p    150p

*/
#define MEMCTL_CALI_RETRY_LIMILT			(5)
#define MEMCTL_CALI_MIN_READ_WINDOW                     (7)
#define MEMCTL_CALI_MIN_WRITE_WINDOW                    (7)
#define CPU_DCACHE_SIZE                                 (0x8000)
#define MEMCTL_CALI_TARGET_LEN                          (CPU_DCACHE_SIZE * 2)
#define MEMCTL_CALI_FULL_SCAN_RESOLUTION                (2)
#define MEMCTL_CALI_WRITE_DELAY_START_TAP               (0)
#define MEMCTL_CALI_READ_DELAY_START_TAP                (0)
#define MEMCTL_DATA_PATTERN_8BIT                        (0x00FF00FF)
#define MEMCTL_DATA_PATTERN_16BIT                       (0x0000FFFF)
#define MEMCTL_CALI_TARGET_ADDR                         (0x80b00000)

/* Adds for SD5-----------------------Start--*/
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
/* Adds for SD5-----------------------End--*/

/*
 * Function Declaration
 */

/* return 32bit verify status */

#if 0
uint32 _verify_pattern(uint32 start_addr, uint32 len);
void _write_pattern(uint32 start_addr, uint32 len);
#endif
uint32 _memctl_set_phy_delay(uint32 bit_location, uint32 w_delay, uint32 r_delay);
void _memctl_set_phy_delay_all(uint32 w_delay, uint32 r_delay);
void _memctl_result_to_DQ_RW_Array(uint32 result, uint32 w_delay, uint32 r_delay, uint32 RW_array_addr[32][32]);
void _memctl_dq_rw_array_post_processing(uint32 w_start, uint32 r_start, uint32 resolution, uint32 **DQ_RW_Array);
void  _memctl_set_phy_delay_dqrf(uint32 bit_loc,uint32 max_w_seq_start,uint32 max_w_len,uint32 max_r_seq_start,uint32 max_r_len);
uint32 _memctl_find_proper_RW_dealy(uint32 resolution, uint32 w_start, uint32 r_start, uint32 DQ_RW_Array[32][32]);

/*
 * Function Implementation
 */
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
        volatile uint32 *ddcrdqr_base, *soc_id_reg;
	unsigned char r_delay_tap,w_delay_tap;
	unsigned int dram_mhz = 0;

        ddcrdqr_base = (volatile unsigned int *)DACDQR;
	soc_id_reg   = (volatile unsigned int *)SOCPNR;

        ddcrdqr_base += bit_loc;
	dram_mhz = board_DRAM_freq_mhz();

	if(memctlc_DDR_Type()==2){		
#if	0
		if(max_w_seq_start==0){
			if(max_w_len>13){
				w_delay_tap = (max_w_len/2) - ((31-max_w_len)/3);
				//w_delay_tap = (max_w_len/2);
			}else{
				w_delay_tap = 0;
					}

		}else{
			w_delay_tap = max_w_len/2;
			}
#else
		if(max_w_seq_start==0){

			if(bit_loc<8){			
				/* LDQS */
				switch(dram_mhz){
					case 400:			
						if(max_w_len/4 >= 1)
							w_delay_tap = (max_w_len/4)-1;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 375:			
					case 350:			
						w_delay_tap = max_w_len/4;
						break;
					default:	
						if(max_w_len/4 >= 2)
							w_delay_tap = (max_w_len/4)-2;
						else
							w_delay_tap = max_w_len/4;

					}
			}
			else{	
				/* HDQS */
				switch(dram_mhz){
					case 375:			
						w_delay_tap = (max_w_len/4)+2;
						break;
					case 400:			
					case 350:			
					default:		
						w_delay_tap = max_w_len/4;
					}			
			}
		}
		else{
			w_delay_tap = max_w_len/2;
		}
			
#endif		
	}
	else if(memctlc_DDR_Type()==3){
		
		if(max_w_seq_start==0){
			if(bit_loc<8){		
				
				/* LDQS */
				switch(dram_mhz){
					case 400:			
						if(max_w_len/4 >= 3)
							w_delay_tap = (max_w_len/4)-3;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 375:			
					case 350:			
					default:		
						if(max_w_len/4 >= 2)
							w_delay_tap = (max_w_len/4)-2;
						else
							w_delay_tap = max_w_len/4;

					}
			}else{		
				/* HDQS*/
				switch(dram_mhz){
					case 400:			
						if(max_w_len/4 >= 4)
							w_delay_tap = (max_w_len/4)-4;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 375:			
					case 350:			
					default:		
						if(max_w_len/4 >= 2)
							w_delay_tap = (max_w_len/4)-2;
						else
							w_delay_tap = max_w_len/4;

					}
				}
		}else{
			w_delay_tap = max_w_len/2;
		}
	}

	
	if((max_r_len-max_r_seq_start)>20){
		r_delay_tap=(max_r_seq_start+max_r_len)/2;	
	}else if(max_r_len>12 && max_r_seq_start==0){
		r_delay_tap=max_r_len-12;
	}else if(max_r_seq_start!=0){
		r_delay_tap=(max_r_seq_start+max_r_len)/2;		
	}else{
		r_delay_tap=0;
	}		

	*ddcrdqr_base = (((max_w_seq_start + w_delay_tap) & 0x1f) << 24) | \
            	        (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | \
                    	(((max_r_seq_start + (r_delay_tap)) & 0x1f) << 8) | \
                    	(((max_r_seq_start) & 0x1f) << 0);
		
       	_memctl_update_phy_param();

        return;
}

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

void _memctl_result_to_DQ_RW_Array(uint32 result, uint32 w_delay, uint32 r_delay, uint32 RW_array_addr[32][32])
{
        /*
         * RW_array_addr [32]   [32] : [Rising 0~15, Falling 0~15] [w_delay]
         *              32bit  W_delay
         */

        uint32 bit_loc, correct_bit;


        /* We mark correct bit */
        result = ~result;
        result = ((result & 0xFFFF0000) >> 16) | ((result & 0x0000FFFF) << 16);



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

	//printf("DQ_RW_Array: 0x%08p\n", &DQ_RW_Array[0][0]);

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

			//puts("w_delay: "); puthex(w_delay);
			//puts(" r_delay: "); puthex(r_delay);
			//puts(" WR_result: "); puthex(WR_Result);
			//puts("\n");

			_memctl_result_to_DQ_RW_Array(WR_Result, w_delay, r_delay, &DQ_RW_Array[0][0]);
                }
        }

#if 0
        /* Resolution post processing. */
        if(resolution > 1){
                _memctl_dq_rw_array_post_processing(w_start, r_start, resolution, &DQ_RW_Array[0][0]);
        }
#endif

        /* All scan result is in DQ_RW_Array, choose a proper delay tap setting. */
        if( 0 == _memctl_find_proper_RW_dealy( resolution, w_start, r_start, &DQ_RW_Array[0][0])){
                return 0;/* Pass */
        }else{
                return 1;/* Fali */
        }

}

uint32 DDR_Calibration(unsigned char full_scan)
{
        uint32 target_addr, len, resolution, w_start, r_start;
	uint32 retry_limit;
        target_addr = MEMCTL_CALI_TARGET_ADDR;
        len         = MEMCTL_CALI_TARGET_LEN;
        resolution  = MEMCTL_CALI_FULL_SCAN_RESOLUTION;
        w_start     = MEMCTL_CALI_WRITE_DELAY_START_TAP;
        r_start     = MEMCTL_CALI_READ_DELAY_START_TAP;

	/* Configure Calibation mode: Digital delay line or Analog DLL */
#ifdef CONFIG_ANALOG_DLL_DELAY_LINE
	REG32(DACCR) = REG32(DACCR) & (~(1<<31)) & 0xBFFFFFFF;
#else
	REG32(DACCR) = (REG32(DACCR) | (1<<31)) & 0xBFFFFFFF;
#endif

	/* Enable dynamic PHY FIFO Reset */
	REG32(DACCR)= REG32(DACCR) | 0x20;

	if(board_DRAM_freq_mhz() > 300){
		/* Enable DQS half clock cycle */	
		REG32(DDCR) = REG32(DDCR) | 0x80800000;
	}

#ifdef CONFIG_HALF_CLK_ENABLE
	REG32(DDCR) = REG32(DDCR) | 0x80800000;
#else
	REG32(DDCR) = REG32(DDCR) & (~(0x80800000)) ;
#endif

	retry_limit = 0;

        /* Do a fully scan to choose a proper point. */
	if(full_scan){
	        while( 0 != _DDR_Calibration_Full_Scan(target_addr, len, resolution, w_start, r_start)){

			 /* Do watchdog reset while calibration is fail */
			 sys_watchdog_enable(0,0);

	                /* Base on the proper point, we do a one dimension scan for the reliabilties. */
	                //return _DDR_Calibration_One_Dimension();
			retry_limit++;
			if(retry_limit > MEMCTL_CALI_RETRY_LIMILT){
	               		return 1; /* Fail, need to define failure status. */
			}
	        }
	}else{
		for(retry_limit=0;retry_limit<32;retry_limit++)
			_memctl_set_phy_delay_dqrf(retry_limit,0,0,0,0x14);
	}
	return 0;
}


