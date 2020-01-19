#include "./bspchip_8685.h"
#include "./memctl_8685.h"
#include "./memctl_8685_func.h"

#ifdef CONFIG_DDR1_USAGE
#define DDR1_USAGE
#else
#undef DDR1_USAGE
#endif

#ifdef CONFIG_DDR2_USAGE
#define DDR2_USAGE
#else
#undef DDR2_USAGE
#endif

#ifdef CONFIG_DDR3_USAGE
#define DDR3_USAGE
#else
#undef DDR3_USAGE
#endif


/* Prototype*/
extern unsigned int memctlc_DDR_Type(void);

#ifdef DDR1_USAGE
void memctlc_ddr1_dll_reset(void);
extern void _DTR_DDR1_MRS_setting(unsigned int *mr);
#endif


#ifdef DDR2_USAGE
void memctlc_ddr2_dll_reset(void);
extern void _DTR_DDR2_MRS_setting(unsigned int *mr);
#endif


#ifdef DDR3_USAGE
void memctlc_ddr3_dll_reset(void);
extern void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
#endif

unsigned int _DCR_get_buswidth(void);
void memctlc_dram_phy_reset(void);
void memctlc_clk_rev_check(void);
void memctlc_lxbus_check(void);


/* Definitions */
#define printf puts

/* Function Name: 
 * 	sys_watchdog_enable
 * Descripton:
 *	Implement system watchdog function
 * Input:
 *	ph1 -> phase 1 threshold, ph2 -> phase 2 thershold
 * Output:
 * 	None
 * Return:
 *	None
 */
void sys_watchdog_enable(unsigned int ph1, unsigned int ph2)
{
	REG32(SYSREG_WDCNTRR) |= SYSREG_WDT_KICK;
	REG32(SYSREG_WDTCTRLR) = ((SYSREG_WDT_E) |\
								((ph1 << SYSREG_PH1_TO_S) & SYSREG_PH1_TO_MASK) |\
								((ph2 << SYSREG_PH2_TO_S) & SYSREG_PH2_TO_MASK));
	return;
}

void sys_watchdog_disable(void)
{
	REG32(SYSREG_WDCNTRR) = 0x0;
	REG32(SYSREG_WDTCTRLR) = 0x0;

	return;
}

/*====================================================================================== */
/* Merge from memctl_dram.c */
/*=======================================================================================*/
void _memctl_delay_clkm_cycles(unsigned int delay_cycles)
{
	volatile unsigned int *mcr, read_tmp;

	mcr = (unsigned int *)MCR;

	while(delay_cycles--){
		read_tmp = *mcr;
	}

	return;
}

void _DRAM_PLL_CLK_power_switch(unsigned char power_on)
{
	volatile unsigned int *dpcpw;
	unsigned int	delay_tmp;
	dpcpw = (unsigned int *)0xB8000204;

	if(power_on)
		*dpcpw &=  ~(1<<4);
	else
		*dpcpw |=  (1<<4);

	delay_tmp=0x1fff;
	while(delay_tmp--);
}

void _periodic_DRAM_refresh(unsigned char enable)
{
	volatile unsigned int *dmcr;
	dmcr = (unsigned int *)DMCR;

	if(enable){
		/* Enable DRAM periodic DRAM refresh operation. */
		*dmcr &=  ~(1<<24);
	}else{
		/* Disable DRAM periodic DRAM refresh operation */
		*dmcr |=  (1<<24);
	}
	
	_memctl_delay_clkm_cycles(10);
	while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);
}

void _check_DMCR_done_bit(unsigned char dqm_tap)
{
	volatile unsigned int *dmcr;
	dmcr = (unsigned int *)DMCR;
	if((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0){
		puts("_check_MCR_done_bit, done bit not clear...dqm_tap(");	
		puthex(dqm_tap);
		puts(")\n\r");
	}
}

void _memctl_update_phy_param(void)
{
	volatile unsigned int *dmcr;
	volatile unsigned int *dcr;
	volatile unsigned int *dacr;
	volatile unsigned int dacr_tmp1, dacr_tmp2;
	volatile unsigned int dmcr_tmp;

	dmcr = (unsigned int *)DMCR;
	dcr = (unsigned int *)DCR;
	dacr = (unsigned int *)DACCR;
	
	/* Write DMCR register to sync the parameters to phy control. */
	dmcr_tmp = *dmcr;
	*dmcr = dmcr_tmp;
	_memctl_delay_clkm_cycles(10);	
	/* Waiting for the completion of the update procedure. */
	while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);

	__asm__ __volatile__("": : :"memory");

	/* reset phy buffer pointer */
	dacr_tmp1 = *dacr;
	dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
	dacr_tmp2 = dacr_tmp1 | (0x10);
	*dacr = dacr_tmp1 ;

	_memctl_delay_clkm_cycles(10);	
	__asm__ __volatile__("": : :"memory");
	*dacr = dacr_tmp2 ;

	return;
}

void plat_memctl_ZQ_force_config(void)
{
	    unsigned int zq_force_value0, zq_force_value1, zq_force_value2;
	    volatile unsigned int *zq_pad_ctl_reg;
	    zq_force_value0 = 0x0022b49f; /*OCD 60, ODT 50*/
	    zq_force_value1 = 0x00570057;
	    zq_force_value2 = 0x58282809;
	 
	    zq_pad_ctl_reg = (volatile unsigned int *)0xB8000118;
	 
	    *zq_pad_ctl_reg     = zq_force_value0;
	    *(zq_pad_ctl_reg+1) = zq_force_value0;
	    *(zq_pad_ctl_reg+2) = zq_force_value0;
	    *(zq_pad_ctl_reg+3) = zq_force_value0;
	    *(zq_pad_ctl_reg+6) = zq_force_value0;
	    *(zq_pad_ctl_reg+7) = zq_force_value0;
	    *(zq_pad_ctl_reg+8) = zq_force_value0;
	    *(zq_pad_ctl_reg+9) = zq_force_value0;

	    *(zq_pad_ctl_reg+4) = zq_force_value1;
	    *(zq_pad_ctl_reg+5) = zq_force_value1;
	    *(zq_pad_ctl_reg+10)= zq_force_value1;
	 
	    *(zq_pad_ctl_reg+11)= zq_force_value2;
	 
	    return; 
}




int memctlc_ZQ_calibration(unsigned int auto_cali_value)
{
#ifdef CONFIG_ZQ_AUTO_CALI
	volatile unsigned int *dmcr, *zq_cali_reg;
	volatile unsigned int *zq_cali_status_reg;
	unsigned int polling_limit, zqc_cnt;
	unsigned int reg_v, odtn, odtp, ocdn, ocdp;

	dmcr = (volatile unsigned int *)DMCR;
	zq_cali_reg = (volatile unsigned int *)0xB8001094;
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;

	/* Disable DRAM refresh operation */
	*dmcr = ((*dmcr | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK));

	zqc_cnt = 0;
 zq_cali_start:
	/* Trigger the calibration */
	*zq_cali_reg = auto_cali_value | 0x80000000;

	/* Polling to ready */
	polling_limit = 0x10000;
	while(*zq_cali_reg & 0x80000000){
		polling_limit--;
		if(polling_limit == 0){
			puts("Error, ZQ auto-calibration ready polling timeout!\n");
			plat_memctl_ZQ_force_config(); /*Use static ZQ setting*/
			goto static_zq_setting_done;
		}
	}

	/* Patch code for IO PAD */    
	/* plat_memctl_IO_PAD_patch();  */
	reg_v = *zq_cali_status_reg;   
	if(reg_v & 0x20000000) {          
		if ((zqc_cnt++) < 8) {
			_memctl_delay_clkm_cycles(1000);
			goto zq_cali_start;
		}
		odtp = ((reg_v >> 27) & 0x3);        
		odtn = ((reg_v >> 25) & 0x3);        
		ocdp = ((reg_v >> 23) & 0x3);        
		ocdn = ((reg_v >> 21) & 0x3);        
		puts("Result of ODTP/ODTN/OCDP/OCDN="); 
			puthex(odtp); printf("/");
			puthex(odtn); printf("/");
			puthex(ocdp); printf("/");
			puthex(ocdn); printf("\n");         

		if((ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */
		   (ocdn == 1))   /* OCDN must NOT be overflow (may tolerate code underflow error) */  {
			puts("ZQ Calibration Failed\n\r");
			return MEMCTL_ZQ_CALI_FAIL; /* Error, calibration fail. */
		} else {
			puts("ZQ Calibration Relaxed Pass\n\r");
			goto static_zq_setting_done;
		}
	}

	if (zqc_cnt == 0) {
		puts("ZQ Calibration Passed\n\r");
	} else {
		puts("ZQ Calibration Retried Passed\n\r");
	}
static_zq_setting_done:
	/* Enable DRAM refresh operation */
	*dmcr = *dmcr &  (~DMCR_DIS_DRAM_REF_MASK) ;

	return MEMCTL_ZQ_CALI_PASS;

#else
	volatile unsigned int *zq_pad_ctl_reg;
	
	zq_pad_ctl_reg = (volatile unsigned int *)0xB8000118;

	*zq_pad_ctl_reg     = CONFIG_ZQ_PAD_CTL0;
	*(zq_pad_ctl_reg+1) = CONFIG_ZQ_PAD_CTL1;
	*(zq_pad_ctl_reg+2) = CONFIG_ZQ_PAD_CTL2;
	*(zq_pad_ctl_reg+3) = CONFIG_ZQ_PAD_CTL3;
	*(zq_pad_ctl_reg+4) = CONFIG_ZQ_PAD_CTL4;
	*(zq_pad_ctl_reg+5) = CONFIG_ZQ_PAD_CTL5;
	*(zq_pad_ctl_reg+6) = CONFIG_ZQ_PAD_CTL6;
	*(zq_pad_ctl_reg+7) = CONFIG_ZQ_PAD_CTL7;
	*(zq_pad_ctl_reg+8) = CONFIG_ZQ_PAD_CTL8;
	*(zq_pad_ctl_reg+9) = CONFIG_ZQ_PAD_CTL9;
	*(zq_pad_ctl_reg+10)= CONFIG_ZQ_PAD_CTL10;
	*(zq_pad_ctl_reg+11)= CONFIG_ZQ_PAD_CTL11;

	return 0;
#endif

}

#ifdef CONFIG_RTL8685
/* DQM calibration is used for RTL8685 memory controller */
unsigned int memctl_dqm_pattern(unsigned int start_addr, unsigned int size, unsigned char data)
{
	unsigned int src_len = size;
	unsigned char c_data = data;
	volatile unsigned char *src_addr;
	unsigned int result=0;

	src_addr = (volatile unsigned char *)start_addr;

	/* Write patterns */
	while(src_len){
		*src_addr++ = c_data++;
		src_len--;		
	}

	src_len = size;
	c_data = data;
	src_addr = (volatile unsigned char *)start_addr;

	/* Verify patterns */
	while(src_len){		
		if(*src_addr != c_data){
			result = 1;
			break;
		}				
		*src_addr = 0x5a;
		src_addr+=2;
		c_data+=2;
		src_len-=2;
	}

	return result;
}



void memctlc_set_dqm_delay(unsigned int mem_clk_mhz)
{
	volatile unsigned int *dacdqr;
	volatile unsigned int *dcdqmr;
	volatile unsigned int *ddrckodl;
	unsigned short dqmX_delay;
	unsigned int dqmX_window[2];
	unsigned short dqmX_mid[2];
	unsigned int temp32;
	unsigned char clk_clkm_edge=0, clk_clkm_edge1=0;
	unsigned short clk_delay=0,clkm_delay=0,clkm90_delay=0,clkm_org,i;
	unsigned short R_flag=0,L_flag=31,fail_bit_count=0, fail_count=0,dqm_done_bit=0;

	dacdqr = (volatile unsigned int *)DACDQR;
	dcdqmr = (volatile unsigned int *)DCDQMR;
	ddrckodl = (volatile unsigned int *)SYSREG_DDRCKODL_REG;

	if(memctlc_DDR_Type()==IS_DDR2_SDRAM){		
		if(mem_clk_mhz==350){
			/*Etron => CLK lead DQS0 377ps, CLK lead DQS1 ps @350MHz*/
			clk_clkm_edge=clk_clkm_edge1=8; 		

			/*Etron => CLK lead DQS0 233ps, CLK lead DQS1 ps @350MHz, boot fail.*/
			/* clk_clkm_edge=clk_clkm_edge1=12;	*/
		}else{
			/*Etron => CLK lead DQS0 330ps, CLK lead DQS1 355ps @300MHz*/
			clk_clkm_edge=clk_clkm_edge1=10;		
		}
	}else if(memctlc_DDR_Type()==IS_DDR3_SDRAM){	
		if(mem_clk_mhz==350){
			/*Etron => CLK lead DQS0 188ps, CLK lead DQS1 122ps @350MHz*/
			clk_clkm_edge=clk_clkm_edge1=0;		
		}else if(mem_clk_mhz==400){
			/*Etron fail. RX DQ0 ~ 7 falling window too small.*/
			clk_clkm_edge=clk_clkm_edge1=0;		
		}else{
			/*Etron => CLK lead DQS0 270ps, CLK lead DQS1 177ps @300MHz*/
			clk_clkm_edge=clk_clkm_edge1=2;
		}
	}else{
		clk_clkm_edge=clk_clkm_edge1=0;
	}

	clkm_org = (*ddrckodl & SYSREG_DDRCKODL_DDRCLM_TAP_MASK)
					>>SYSREG_DDRCKODL_DDRCLM_TAP_FD_S;

	for(clkm_delay=clkm_org;clkm_delay<32;clkm_delay++)
	{
		if((clk_clkm_edge+clkm_delay) > 31)
			clk_clkm_edge=31;
		else
			clk_delay=clkm_delay+clk_clkm_edge;	

		/* Disable DRAM periodic DRAM refresh operation.*/
		_periodic_DRAM_refresh(0);			

		*ddrckodl=((*ddrckodl)& ~(SYSREG_DDRCKODL_DDRCLM_TAP_MASK |SYSREG_DDRCKODL_DDRCK_PHS_MASK))|
			(clkm_delay<<SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)|clk_delay;

		/* Enable DRAM periodic DRAM refresh operation.*/
		_periodic_DRAM_refresh(1);		

		/**********************************
		**	Calibrate DQM0, DQM1 windows first   **
		***********************************/
		for(i=0;i<2; i++){

			/* Reset DQM0, DQM1 windows */
			dqmX_window[i] = 0;

			/* Find appropriate windows */
			for(dqmX_delay=0;dqmX_delay<32;dqmX_delay++){
				*dcdqmr = (*dcdqmr & ~(DCDQMR_DQM1_PHASE_SHIFT_90_MASK << (1-i)*8)) | 
					(dqmX_delay << (DCDQMR_DQM1_PHASE_SHIFT_90_FD_S + (1-i)*8));
				
				_check_DMCR_done_bit(dqmX_delay);
	
				if(memctl_dqm_pattern((0xa0200001+i), 0x400, 0)==0){
					dqmX_window[i]|=(1<<dqmX_delay);
				}
			}
			/*puthex(clkm_delay);puts("==>dqmX_window=");puthex(dqmX_window[i]);puts("\n");*/
		}

		/***************************************************************
		*	Check dqm0_window & dqm1_window max value of window. Analysis window *
		****************************************************************/
		for(i=0;i<2;i++){

			/*puthex(clkm_delay);puts("==>dqm_window=");puthex(dqmX_window[i]);puts("\n");*/

			/* Analysis windows */
			if( dqmX_window[i] == 0xFFFFFFFF){
				
				/* Best case: 0xFFFFFFFF */
				dqm_done_bit=1;
				
			}else if( dqmX_window[i] == 0x0){

				/* Worst case : 0x00000000*/
				if(clkm_delay<(clkm_org+5))
					fail_count++;
				else
					dqm_done_bit=1;				
			}else{

				L_flag = 31;
				while(((dqmX_window[i]>>L_flag)&0x1)==0){
					L_flag--;
				}

				 R_flag = 0;
				while(((dqmX_window[i]>>R_flag)&0x1)==0){
					R_flag++;						
				}

				fail_bit_count=0;
				for(temp32=R_flag;temp32<=L_flag;temp32++){
					if(((dqmX_window[i]>>temp32)& 0x1)==0)
						fail_bit_count++;
				}
			}

			/* Special case 1 : there are more than 1 zero slot between R_flag & L_flag */
			if(fail_bit_count>1)
				dqm_done_bit=2;

			/* Special case 2 : the window is too small */
			if((L_flag-R_flag)<2){
				if(clkm_delay<(clkm_org+5))
					fail_count++;
				else
					dqm_done_bit=4;
			}

			/* Special case 3 : R_flag is rising */
			if(R_flag>0)
				dqm_done_bit=8;


			/*puts("dqm_done_bit:");puthex(dqm_done_bit);puts("\n");*/
			
			if(dqm_done_bit > 0){
				temp32=0;
				temp32 |= ((dqmX_mid[0]>24) ? (10 <<DCDQMR_DQM0_PHASE_SHIFT_90_FD_S) :0) |\
						((dqmX_mid[1]>24) ? (10 <<DCDQMR_DQM1_PHASE_SHIFT_90_FD_S) :0);
				
				
				clkm_delay=(clkm_delay*3)/4;
				if((clk_clkm_edge1+clkm_delay) > 31)
					clk_clkm_edge1=31;
				else
					clk_delay=clkm_delay+clk_clkm_edge1;

				/* DDR PLL_CLK power down */
				_DRAM_PLL_CLK_power_switch(0);		

				*ddrckodl=((*ddrckodl)& ~(SYSREG_DDRCKODL_DDRCK_PHS_MASK |SYSREG_DDRCKODL_DDRCLM_TAP_MASK |
					SYSREG_DDRCKODL_DDRCLM90_TAP_MASK) )|
					(clkm90_delay << SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S) |
					(clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)|
					(clk_delay << SYSREG_DDRCKODL_DDRCK_PHS_FD_S);

				/* DDR PLL_CLK power on */
				_DRAM_PLL_CLK_power_switch(1);		

				*dcdqmr = temp32;						
				_memctl_update_phy_param();
				return ;
			}

			dqmX_mid[i] = L_flag-R_flag;
		}
	
	}

}
#endif /* CONFIG_RTL8685 */

void memctlc_config_DataFullMask(void)
{
	volatile unsigned int *mcr;

	mcr = (volatile unsigned int *)MCR;
	*mcr |= MCR_RBF_MAS; 

	return;
}

#ifdef CONFIG_DRAM_AUTO_SIZE_DETECTION

unsigned int _dram_MCR_setting[6][5] __attribute__ ((section(".text")))=		
{ 	{	0x10110000/* 8MB_DDR1_08b */, 
		0x10120000/* 16MB_DDR1_08b */, 
		0x10220000/* 32MB_DDR1_08b */, 
		0x10230000/* 64MB_DDR1_08b */, 
		0x10330000/* 128MB_DDR1_08b */	
	},		
	{	0x11100000/* 8MB_DDR1_16b */, 
		0x11110000/* 16MB_DDR1_16b */, 
		0x11210000/* 32MB_DDR1_16b */, 
		0x11220000/* 64MB_DDR1_16b */, 
		0x11320000/* 128MB_DDR1_16b */
	},	
	{	0x10120000/* 16MB_DDR2_08b */, 
		0x10220000/* 32MB_DDR2_08b */, 
		0x10320000/* 64MB_DDR2_08b */, 
		0x20320000/* 128MB_DDR2_08b */, 
		0x20420000/* 256MB_DDR2_08b */	
	},		
	{	0x11110000/* 16MB_DDR2_16b */, 
		0x11210000/* 32MB_DDR2_16b */, 
		0x11220000/* 64MB_DDR2_16b */, 
		0x21220000/* 128MB_DDR2_16b */, 
		0x21320000/* 256MB_DDR2_16b */
	},	 	
	{	0x00000000/* 16MB_DDR3_08b */, 
		0x00000000/* 32MB_DDR3_08b */, 
		0x20220000/* 64MB_DDR3_08b */, 
		0x20320000/* 128MB_DDR3_08b */, 
		0x20420000/* 256MB_DDR3_08b */
	},		
	{	0x00000000/* 16MB_DDR3_16b */, 
		0x00000000/* 32MB_DDR3_16b */, 
		0x21120000/* 64MB_DDR3_16b */, 
		0x21220000/* 128MB_DDR3_16b */, 
		0x21320000/* 256MB_DDR3_16b */}
};

unsigned int _dram_type_setting[6][5] __attribute__ ((section(".text")))=		//format: n15: 1=16bit, n8..n4=tRFC, n2..n0=DRAM_type, n31..n16=DRAM_size	
{ 	{	0x008004B2/* 8MB_DDR1_08b */, 
		0x010004B2/* 16MB_DDR1_08b */, 
		0x02000692/* 32MB_DDR1_08b */, 
		0x04000802/* 64MB_DDR1_08b */, 
		0x08000C62/* 128MB_DDR1_08b */
	},		
	{	0x008084B2/* 8MB_DDR1_16b */, 
		0x010084B2/* 16MB_DDR1_16b */, 
		0x02008692/* 32MB_DDR1_16b */, 
		0x04008802/* 64MB_DDR1_16b */, 
		0x08008C62/* 128MB_DDR1_16b */
	},	
	{	0x010004B2/* 16MB_DDR2_08b */, 
		0x020004B2/* 32MB_DDR2_08b */, 
		0x04000692/* 64MB_DDR2_08b */, 
		0x08000802/* 128MB_DDR2_08b */, 
		0x10000C62/* 256MB_DDR2_08b */
	},		
	{	0x010084B2/* 16MB_DDR2_16b */, 
		0x020084B2/* 32MB_DDR2_16b */, 
		0x04008692/* 64MB_DDR2_16b */, 
		0x08008802/* 128MB_DDR2_16b */, 
		0x10008C62/* 256MB_DDR2_16b */
	},		
	{	0x010004B3/* 16MB_DDR3_08b */, 
		0x020005A3/* 32MB_DDR3_08b */, 
		0x040005A3/* 64MB_DDR3_08b */, 
		0x080006E3/* 128MB_DDR3_08b */, 
		0x10000A03/* 256MB_DDR3_08b */
	},		
	{	0x010084B3/* 16MB_DDR3_16b */, 
		0x020085A3/* 32MB_DDR3_16b */, 
		0x040085A3/* 64MB_DDR3_16b */, 
		0x080086E3/* 128MB_DDR3_16b */, 
		0x10008A03/* 256MB_DDR3_16b */}	
};

unsigned int _dram_detection_addr[6][5] __attribute__ ((section(".text")))=		
{ 	{	0xA63809A4/* 8MB_DDR1_08b */, 
		0xA6380BA4/* 16MB_DDR1_08b */, 
		0xA6780BA4/* 32MB_DDR1_08b */, 
		0xA0000000/* 64MB_DDR1_08b */, 
		0xA0000000/* 128MB_DDR1_08b */
	},		
	{	0xA6701148/* 8MB_DDR1_16b */, 
		0xA6701348/* 16MB_DDR1_16b */, 
		0xA6F01348/* 32MB_DDR1_16b */, 
		0xA6F01748/* 64MB_DDR1_16b */, 
		0xA7F01748/* 128MB_DDR1_16b */
	},		
	{	0xA6380BA4/* 16MB_DDR2_08b */, 
		0xA6780BA4/* 32MB_DDR2_08b */, 
		0xA6F80BA4/* 64MB_DDR2_08b */, 
		0xAEF80BA4/* 128MB_DDR2_08b */, 
		0xAFF80BA4/* 256MB_DDR2_08b */
	},		
	{	0xA6701348/* 16MB_DDR2_16b */, 
		0xA6F01348/* 32MB_DDR2_16b */, 
		0xA6F01748/* 64MB_DDR2_16b */, 
		0xAEF01748/* 128MB_DDR2_16b */, 
		0xAFF01748/* 256MB_DDR2_16b */
	},		
	{	0xA0000000/* 16MB_DDR3_08b */, 
		0xA0000000/* 32MB_DDR3_08b */, 
		0xAE780BA4/* 64MB_DDR3_08b */, 
		0xAEF80BA4/* 128MB_DDR3_08b */, 
		0xAFF80BA4/* 256MB_DDR3_08b */
	},		
	{	0xA0000000/* 16MB_DDR3_16b */, 
		0xA0000000/* 32MB_DDR3_16b */, 
		0xAE701548/* 64MB_DDR3_16b */, 
		0xAEF01548/* 128MB_DDR3_16b */, 
		0xAFF01548/* 256MB_DDR3_16b */}
};

#else
unsigned int ddr2_8bit_size[] __attribute__ ((section(".text")))=  
	{  0x10120000/*16MB*/    , 0x10220000/* 32MB */, 0x10320000/* 64MB */, 
	   0x20320000/* 128MB */, 0x20420000/* 256MB */, 0x20520000/* 512MB */};
unsigned int ddr2_16bit_size[] __attribute__ ((section(".text")))= 
	{ 0x11110000/*16MB*/,     0x11210000/* 32MB */, 0x11220000/* 64MB */, 
	  0x21220000/* 128MB */, 0x21320000/* 256MB */, 0x21420000/* 512MB */, 
	  0x21520000/*1GB*/};
#ifdef CONFIG_DRAM_AUTO_SIZE_DETECTION
unsigned int dram_test_addr[] __attribute__((section(".text")))=
	{ 0xa7f01354/* 32MB*/, 0xa7f01754/* 64MB */, 0xaef01754/* 128MB */,
	   0xadf01754/* 256MB*/, 0xabf01754/* 512MB */, 0xaff01754/* 1GB */};
#endif
#endif

unsigned int memctlc_config_DRAM_size(void)
{
	volatile unsigned int *dcr;
	unsigned int *size_arry;
	unsigned int dcr_value=0, dram_size=0x2000000;
#ifdef CONFIG_DRAM_AUTO_SIZE_DETECTION
	volatile unsigned int *dram_addr;
	unsigned int i;
	unsigned int DDR_para_index=0, DDR_width=16, loc=0;
#endif
	
	dcr = (volatile unsigned int *)DCR;

#ifdef CONFIG_DRAM_AUTO_SIZE_DETECTION

	if(memctlc_DDR_Type()==IS_DDR3_SDRAM)
		DDR_para_index = 4;
	else if(memctlc_DDR_Type()==IS_DDR2_SDRAM)
		DDR_para_index = 2;
	else
		DDR_para_index = 0;

	DDR_width =  8 << ((REG32(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S) ;

	loc=(DDR_width/8-1) + DDR_para_index;		
	size_arry =  &_dram_MCR_setting[loc][0];

	*dcr = size_arry[4];
	_memctl_update_phy_param();
	
	dram_addr = (volatile unsigned int *)_dram_detection_addr[loc][4];
	*dram_addr = 0x5A0FF0A5;

	/* DCache flush is necessary ? */
	_memctl_DCache_flush_invalidate();

	/* Assign 64MBytes DRAM parameters as default value */
	dcr_value = _dram_MCR_setting[loc][2];		
	dram_size = _dram_type_setting[loc][2];

#if 0	/* Unknown issue : DRAM size detection must be set from small size to big size.*/

	for(i=(sizeof(_dram_detection_addr[loc])/sizeof(unsigned int)); i>0; i--){
		if( REG32(_dram_detection_addr[loc][i-1]) != 0x5A0FF0A5 ){
			dcr_value = _dram_MCR_setting[loc][i];			
			#ifdef CONFIG_RTL8685
			/* Enable RTL8685 memory controller jitter tolerance*/
			dcr_value |= (1<<31);
			#endif /* CONFIG_RTL8685 */
			dram_size = ((_dram_type_setting[loc][i]) & 0xFFFF0000);
			break;
		}
	}
#else
	for(i=0; i<(sizeof(_dram_detection_addr[loc])/sizeof(unsigned int)); i++){
		if( REG32(_dram_detection_addr[loc][i]) == 0x5A0FF0A5 ){
			dcr_value = _dram_MCR_setting[loc][i];			
			#ifdef CONFIG_RTL8685
			/* Enable RTL8685 memory controller jitter tolerance*/
			dcr_value |= (1<<31);
			#endif /* CONFIG_RTL8685 */
			dram_size = ((_dram_type_setting[loc][i]) & 0xFFFF0000);
			break;
		}
	}
#endif
	

	*dcr = dcr_value;
	_memctl_update_phy_param();

#else /* CONFIG_DRAM_AUTO_SIZE_DETECTION */

	dram_size = CONFIG_ONE_DRAM_CHIP_SIZE;
	#ifdef CONFIG_DRAM_BUS_WIDTH_8BIT
		size_arry = &ddr2_8bit_size[0];
	#else
		size_arry = &ddr2_16bit_size[0];
	#endif
	
	switch (dram_size){
		case 0x1000000: /* 16MB */
			dcr_value = size_arry[0];
			break;
		case 0x4000000: /* 64MB */
			dcr_value = size_arry[2];
			break;
		case 0x8000000: /* 128MB */
			dcr_value = size_arry[3];
			break;
		case 0x10000000: /* 256MB */
			dcr_value = size_arry[4];
			break;
		case 0x20000000: /* 512MB */
			dcr_value = size_arry[5];
			break;
		default: /* 32MB */
			dcr_value = size_arry[1];
			break;
	}

	#ifdef CONFIG_DRAM_CHIP_NUM_TWO
	dcr_value = dcr_value | 0x8000;
	#endif

	#ifdef CONFIG_RTL8685
	/* Enable RTL8685 memory controller jitter tolerance*/
	dcr_value |= (1<<31);
	#endif /* CONFIG_RTL8685 */
	
	*dcr = dcr_value;
#endif

	return dram_size;

}


#ifdef DDR1_USAGE

enum DDR1_FREQ_SEL {
	DDR1_250=0,
	DDR1_200,
	DDR1_150,
	DDR1_END
};

unsigned int tRFC_Spec_DDR1[] __attribute__ ((section(".text")))= {
	70, 	/*64Mbit*/
	70, 	/*128Mbit*/
	70, 	/*256Mbit*/
	70, /*512Mbit*/
	120, /*1Gbit*/
};


#if defined(CONFIG_RTL8685S) || defined(RTL8685SB)
unsigned int DDR1_DTR[DDR1_END][3] __attribute__ ((section(".text")))={
	{	/* RTL8685S, DDR1, 250MHz*/
		0x23021720,
		0x0303020B,	
		0x00008000},		
	{	/* RTL8685S, DDR1, 200MHz*/
		0x22021520,
		0x0202010B,	
		0x00007000},		
	{	/* RTL8685S, DDR1, 150MHz*/
		0x22021320,
		0x0202010B,	
		0x00006000},		
};
#else
unsigned int DDR1_DTR[3][3] __attribute__ ((section(".text")))={
	{	/* RTL8685, DDR1, 250MHz*/
		0x23021720,
		0x0303020B,	
		0x00008000},		
	{	/* RTL8685, DDR1, 200MHz*/
		0x22021520,
		0x0202010B,	
		0x00007000},		
	{	/* RTL8685, DDR1, 150MHz*/
		0x22021320,
		0x0202010B,	
		0x00006000},		
};
#endif

void memctlc_config_DDR1_DTR(unsigned int default_instruction, unsigned int dram_size)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;	
	unsigned int dram_freq_mhz=0;
	unsigned int DRAM_capacity_index=0, DRAM_freq_index=0, tRFC_extend=1, dtr2_temp=0; 
	unsigned int *tRFC;

	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	dram_freq_mhz = board_DRAM_freq_mhz();

	if(default_instruction == 1){

		/* Default instruction, set the DRAM as the maximun size */
		DRAM_capacity_index = 4;
		
	}else{

		/* Search from DDR1 base size 0x800000 => 8M Bytes */
		for(DRAM_capacity_index=0; DRAM_capacity_index<6; DRAM_capacity_index++){
			if(dram_size == ((0x800000) << DRAM_capacity_index)){
				break;
			}
		}		
	}

	/* Set as default value */
	tRFC = &tRFC_Spec_DDR1[0];	
	switch(dram_freq_mhz){
		case 250:
			DRAM_freq_index = DDR1_250;
			break;
		case 200:
			DRAM_freq_index = DDR1_200;
			break;
		default:
			DRAM_freq_index = DDR1_150;
			break;
	}

	*dtr0 = DDR1_DTR[DRAM_freq_index][0];
	*dtr1 = DDR1_DTR[DRAM_freq_index][1];
	dtr2_temp = DDR1_DTR[DRAM_freq_index][2];

	*dtr2=dtr2_temp | 
		((((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+tRFC_extend)<<DTR2_RFC_FD_S);

	return;

}


#endif /* DDR1_USAGE */

#ifdef DDR2_USAGE

enum DDR2_FREQ_SEL {
	DDR2_500 = 0,
	DDR2_400,
	DDR2_350,
	DDR2_300,
	DDR2_200,
	DDR2_END
};

unsigned int tRFC_Spec_DDR2[] __attribute__ ((section(".text")))= {
	75, 	/*128Mbit*/
	75, 	/*256Mbit*/
	105, /*512Mbit*/
	128, /*1Gbit*/
	198, /*2Gbit*/
	328, /*4Mbit*/
};


#if defined(CONFIG_RTL8685S) || defined(RTL8685SB)
unsigned int DDR2_DTR[DDR2_END][3] __attribute__ ((section(".text")))={
	{	/* RTL8685S, DDR2, 500MHz*/
		0x67544626,		/* DTR0 */
		0x06060516,		/* DTR1 */
		0x00016000},	/* DTR2 temp */
	{	/* RTL8685S, DDR2, 400MHz*/
		0x55433625,
		0x05050311,	
		0x00012000},
	{	/* RTL8685S, DDR2, 350MHz*/
		0x55433525,
		0x0505030E,	
		0x00010000},
	{	/* RTL8685S, DDR2, 300MHz*/
		0x54422815,
		0x0505030D,	
		0x0000E000},
	{	/* RTL8685S, DDR2, 200MHz*/
		0x43311234,
		0x04040108,	
		0x00009000},
};
#else
unsigned int DDR2_DTR[5][3] __attribute__ ((section(".text")))={
	{	/* RTL8685, DDR2, 500MHz, not support */
		0x00000000,
		0x00000000,	
		0x00000000},
	{	/* RTL8685, DDR2, 400MHz*/
		0x56433630,
		0x05050313,	
		0x00012000},
	{	/* RTL8685, DDR2, 350MHz*/
		0x55422530,
		0x05050311,	
		0x00010000},
	{	/* RTL8685, DDR2, 300MHz*/
		0x54422820,
		0x0404030f,	
		0x0000E000},
	{	/* RTL8685, DDR2, 200MHz*/
		0x54422520,
		0x0404030f,	
		0x0000A000},
};
#endif

void memctlc_config_DDR2_DTR(unsigned int default_instruction, unsigned int dram_size)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;	
	unsigned int dram_freq_mhz=0;
	unsigned int DRAM_capacity_index=0, DRAM_freq_index=0, tRFC_extend=3, dtr2_temp=0; 
	unsigned int *tRFC;

	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	dram_freq_mhz = board_DRAM_freq_mhz();

	if(default_instruction == 1){

		/* Default instruction, set the DRAM as the maximun size */
		DRAM_capacity_index = 5;
		
	}else{

		/* Search from DDR1 base size 0x1000000 => 16M Bytes */
		for(DRAM_capacity_index=0; DRAM_capacity_index<6; DRAM_capacity_index++){
			if(dram_size == ((0x1000000) << DRAM_capacity_index)){
				break;
			}
		}		
	}

	/* Set as default value */
	tRFC = &tRFC_Spec_DDR2[0];		
	switch(dram_freq_mhz){
		case 500:
			DRAM_freq_index = DDR2_500;
			break;
		case 400:
			DRAM_freq_index = DDR2_400;
			break;
		case 350:
			DRAM_freq_index = DDR2_350;
			break;
		case 300:
			DRAM_freq_index = DDR2_300;
			break;
		case 200:
			DRAM_freq_index = DDR2_200;
			break;
		default:
			DRAM_freq_index = DDR2_300;
			break;
	}

	*dtr0 = DDR2_DTR[DRAM_freq_index][0];
	*dtr1 = DDR2_DTR[DRAM_freq_index][1];
	dtr2_temp = DDR2_DTR[DRAM_freq_index][2];

	*dtr2=dtr2_temp | 
		((((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+tRFC_extend)<<DTR2_RFC_FD_S);

	return;

}


#endif /* DDR2_USAGE */

#ifdef DDR3_USAGE

enum DDR3_FREQ_SEL {
	DDR3_400=0,
	DDR3_350,
	DDR3_300,
	DDR3_END
};

unsigned int tRFC_Spec_DDR3[] __attribute__ ((section(".text")))= {
	90, /*512Mbit*/
	110, /*1Gbit*/
	160, /*2Gbit*/
	260, /*4Gbit*/
	350, /*8Gbit*/
};

#if defined(CONFIG_RTL8685S) || defined(RTL8685SB)
unsigned int DDR3_DTR[DDR3_END][3] __attribute__ ((section(".text")))={
	{	/* RTL8685S, DDR3, 400MHz*/
		0x56444B15,
		0x05050414,	
		0x00011000},
	{	/* RTL8685S, DDR3, 350MHz*/
		0x55433915,
		0x05050412,	
		0x0000F000},
	{	/* RTL8685S, DDR3, 300MHz*/
		0x54433815,
		0x0404030F,	
		0x0000D000},
};
#else
unsigned int DDR3_DTR[3][3] __attribute__ ((section(".text")))={
	{	/* RTL8685, DDR3, 400MHz*/
		0x56444B20,
		0x07070516,	
		0x00011000},
	{	/* RTL8685, DDR3, 350MHz*/
		0x55433920,
		0x05050412,	
		0x0000F000},
	{	/* RTL8685, DDR3, 300MHz*/
		0x54433820,
		0x0404030F,	
		0x0000D000},
};
#endif

void memctlc_config_DDR3_DTR(unsigned int default_instruction, unsigned int dram_size)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;	
	unsigned int dram_freq_mhz=0;
	unsigned int DRAM_capacity_index=0, DRAM_freq_index=0, tRFC_extend=3, dtr2_temp=0; 
	unsigned int *tRFC;

	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	dram_freq_mhz = board_DRAM_freq_mhz();

	if(default_instruction == 1){

		/* Default instruction, set the DRAM as the maximun size */
		DRAM_capacity_index = 4;
		
	}else{
	
		/* Search from DDR3 base size 0x1000000 => 64M Bytes */
		for(DRAM_capacity_index=0; DRAM_capacity_index<6; DRAM_capacity_index++){
			if(dram_size == ((0x4000000) << DRAM_capacity_index)){
				break;
			}
		}		
	}

	/* Set as default value */
	tRFC = &tRFC_Spec_DDR3[0];	
	switch(dram_freq_mhz){
		case 400:
			DRAM_freq_index = DDR3_400;
			break;
		case 350:
			DRAM_freq_index = DDR3_350;
			break;
		default:
			DRAM_freq_index = DDR3_300;
			break;
	}

	*dtr0 = DDR3_DTR[DRAM_freq_index][0];
	*dtr1 = DDR3_DTR[DRAM_freq_index][1];
	dtr2_temp = DDR3_DTR[DRAM_freq_index][2];

	*dtr2=dtr2_temp | 
		((((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+tRFC_extend)<<DTR2_RFC_FD_S);

	return;

}

#endif /* DDR3_USAGE */

#ifdef  CONFIG_DRAM_AUTO_TIMING_SETTING
void memctlc_config_DTR(unsigned int default_instruction, unsigned int dram_size)
#else
void memctlc_config_DTR(void)
#endif
{	
#ifndef	CONFIG_DRAM_AUTO_TIMING_SETTING
	volatile unsigned int *dtr0, *dtr1, *dtr2;	

	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	*dtr0 = CONFIG_DRAM_DTR0;
	*dtr1 = CONFIG_DRAM_DTR1;
	*dtr2 = CONFIG_DRAM_DTR2;
#else /* CONFIG_DRAM_AUTO_TIMING_SETTING */	

#ifdef DDR1_USAGE
	if(memctlc_DDR_Type()==IS_DDR_SDRAM){
		memctlc_config_DDR1_DTR(default_instruction, dram_size);
	}else
#endif
#ifdef DDR2_USAGE
	if(memctlc_DDR_Type()==IS_DDR2_SDRAM){
		memctlc_config_DDR2_DTR(default_instruction, dram_size);
	}else
#endif
#ifdef DDR3_USAGE
	if(memctlc_DDR_Type()==IS_DDR3_SDRAM){
		memctlc_config_DDR3_DTR(default_instruction, dram_size);
	}else
#endif
	{
		puts("Unknown DRAM type\n\r");
		while(1);
	}
	
#endif /* CONFIG_DRAM_AUTO_TIMING_SETTING */	

	return;

}

#if defined(CONFIG_RTL8685S) || defined(RTL8685SB)
void memctlc_config_delay_line(unsigned int dram_freq_mhz){

	switch(dram_freq_mhz){
		case 400:
			REG32(DDCR) = REG32(DDCR) | 0x1f1f0000;
			REG32(DACCR) |= ((0x0<< 16) |(0x0<< 8));	/* Assign DQS0 and DQS1 group delay */
			REG32(SYSREG_DDRCKODL_REG)=0;
			REG32(DDR_DELAY_CTRL_REG0)=0;
			REG32(DDR_DELAY_CTRL_REG1)=0;
			REG32(DDR_DELAY_CTRL_REG2)=0;
			REG32(DDR_DELAY_CTRL_REG3)=0;
			break;
		case 500:
		case 300:
		default:
			REG32(DDCR) = REG32(DDCR) | 0x1f1f0000;
			REG32(DACCR) |= ((0x0<< 16) |(0x0<< 8));	/* Assign DQS0 and DQS1 group delay */
			REG32(SYSREG_DDRCKODL_REG)=0;
			REG32(DDR_DELAY_CTRL_REG0)=0;
			REG32(DDR_DELAY_CTRL_REG1)=0;
			REG32(DDR_DELAY_CTRL_REG2)=0;
			REG32(DDR_DELAY_CTRL_REG3)=0;					
		}
	_memctl_update_phy_param();

	return;
}
#endif


/* Function Name: 
 * 	memctlc_init_dram
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
unsigned int auto_cali_value[] __attribute__ ((section(".text")))= { 
					CONFIG_DRAM_PREFERED_ZQ_PROGRAM
				 };

#ifdef CONFIG_BOOT_MIPS
extern int ipc_end_session(void);
#endif
#ifdef CONFIG_RTL8685			 
extern void memctlc_clk_rev_check(void);
#endif /* CONFIG_RTL8685 */
void memctlc_init_dram_8685(void)
{
	/* unsigned int dram_type, i; */
	unsigned int i;
	unsigned int target_ddrkodl_value, is_zq_fail;
	unsigned int mem_clk_mhz, dram_size=0;
	volatile unsigned int delay_loop;
	volatile unsigned int *sysreg_dram_clk_en_reg;
	volatile unsigned int *ddrkodl_reg;


	/* Runtime deterministic DRAM intialization.
	 * Enter DRAM initialization if it is enabled.
	 * Determine whether we run in DRAM.
	 */
#ifndef CONFIG_FPGA_RTL8685SB
	puts("\n\rStart to set DDR parameters \n\r");
#endif

#ifndef CONFIG_RTL8685SB
	sys_watchdog_enable(20, 1);
#endif

	mem_clk_mhz = board_DRAM_freq_mhz();

	/* Delay a little bit for waiting for system to enter stable state.*/
	delay_loop = 0x1000;
	while(delay_loop--);

	/* Enable DRAM clock */
#ifndef CONFIG_FPGA_RTL8685SB
	sysreg_dram_clk_en_reg = (volatile unsigned int *)SYSREG_DRAM_CLK_EN_REG;
	while(*sysreg_dram_clk_en_reg != SYSREG_DRAM_CLK_EN_MASK ){
		*sysreg_dram_clk_en_reg = SYSREG_DRAM_CLK_EN_MASK;
	}
#endif

	/* Delay a little bit for waiting for more stable of the DRAM clock.*/
	delay_loop = 0x1000;
	while(delay_loop--);

#ifdef CONFIG_DATA_BUF_MASK
	memctlc_config_DataFullMask();
#endif

	/* Configure DRAM timing parameters */
#ifdef CONFIG_DRAM_AUTO_TIMING_SETTING
	memctlc_config_DTR(1, dram_size);
#else
	memctlc_config_DTR();
#endif

	/* Configure ZQ */
	is_zq_fail = 1;

	if(is_zq_fail){//user-defined value fail, try other predefine value
		for(i=0; i< (sizeof(auto_cali_value)/sizeof(unsigned int));i++){
			if(0 == memctlc_ZQ_calibration(auto_cali_value[i])){
				/* We found one .*/
				break;
			}
		}
		if(i >= (sizeof(auto_cali_value)/sizeof(unsigned int)) ){
			printf("ZQ calibration failed\n");
		}
	}

#if defined(CONFIG_RTL8685S) || defined(RTL8685SB)
	{
		volatile unsigned int *anadll_ctrl0_reg;
		volatile unsigned int *anadll_ctrl1_reg;
		unsigned int delay_tmp = 0x1fff;

		anadll_ctrl0_reg = (volatile unsigned int *)SYSREG_ANA_DLL_CTRL0;
		anadll_ctrl1_reg = (volatile unsigned int *)SYSREG_ANA_DLL_CTRL1;

#ifdef CONFIG_ANALOG_DLL_DELAY_LINE
		*anadll_ctrl0_reg = 0x24148a5e;	
		while(delay_tmp--);

		*anadll_ctrl0_reg = 0x24048a5e;	
#else
		*anadll_ctrl0_reg = 0xa4148a5e;	
#endif		
		*anadll_ctrl1_reg = 0x00148a5e;	
	}

	memctlc_config_delay_line(mem_clk_mhz);

#elif defined(CONFIG_RTL8685)

	ddrkodl_reg = (volatile unsigned int *)SYSREG_DDRCKODL_REG;

#ifdef CONFIG_DRAM_AUTO_PARAMETERS

	*ddrkodl_reg = 0x00000208;
#else
	if(get_memory_delay_parameters(&target_ddrkodl_value)){ 
		/* get the value */
	}else{ 
		/* no predefined value */
          	target_ddrkodl_value =  0;
	}		
	*ddrkodl_reg = target_ddrkodl_value;	
#endif
#endif /* CONFIG_RTL8685S  or CONFIG_RTL8685SB*/

	/* Reset DRAM DLL */
	if(memctlc_DDR_Type()==IS_DDR_SDRAM){
#ifdef DDR1_USAGE
		memctlc_ddr1_dll_reset();
#endif
	}else if(memctlc_DDR_Type()==IS_DDR2_SDRAM){
#ifdef DDR2_USAGE
		memctlc_ddr2_dll_reset();
#endif
	}else if(memctlc_DDR_Type()==IS_DDR3_SDRAM){
#ifdef DDR3_USAGE
		memctlc_ddr3_dll_reset();
#endif
	}else{
		puts("Error, Unknown DRAM type!\n\r");	
		while(1);
	}
	//puts("Finish Reset DRAM DLL\n\r");

	puts("Start DDR_Calibration...\n\r");

#ifdef CONFIG_RTL8685
	/* RTL8685 must do DQM calibration first for DQ/DQS calibration */

	if(mem_clk_mhz > 200){
		/* While the DRAM frequency is larger than 200MZ, 
			do DQM calibration to adjust DRAM timing. 
		*/
		DDR_Calibration(0);	
		memctlc_set_dqm_delay(mem_clk_mhz);
	}
	DDR_Calibration(1);
	
#elif defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	/* RTL8685S just do DQ/DQS calibration */
	DDR_Calibration(1);
#endif /* CONFIG_RTL8685 */

	memctlc_dram_phy_reset();
	//puts("Finish memctlc_dram_phy_reset\n\r");

	/* Configure DRAM size */
	dram_size = memctlc_config_DRAM_size();
#ifdef CONFIG_DRAM_AUTO_TIMING_SETTING
	memctlc_config_DTR(0, dram_size);
#endif
	//puts("Finish memctlc_config_DRAM_size\n\r");

#ifdef CONFIG_RTL8685
	/* Clock reverse configuration */
	memctlc_clk_rev_check();
	//puts("Finish memctlc_clk_rev_check\n\r");
#endif /* CONFIG_RTL8685 */

	memctlc_lxbus_check();
	//puts("Finish memctlc_lxbus_check\n\r");

	sys_watchdog_disable();

#ifdef CONFIG_BOOT_MIPS
	ipc_end_session();
#endif
	return;
}

#ifdef DDR1_USAGE
void memctlc_ddr1_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;

	_DTR_DDR1_MRS_setting(mr);

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR1_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR1_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR1_MR_OP_RST_DLL ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Normal mode, avoid to reset DLL when updating phy params */
	*dmcr = mr[0];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}
#endif

#ifdef DDR2_USAGE
void memctlc_ddr2_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;

	_DTR_DDR2_MRS_setting(mr);

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR2_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	//puts("Finish Disable DLL\n\r");

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR2_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);

	//puts("Finish Ensable DLL\n\r");

	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR2_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	//puts("Finish Reset DLL\n\r");

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	//puts("Finish Set EMR2\n\r");

	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	//puts("Finish reset phy fifo\n\r");

	return;
}
#endif

#ifdef DDR3_USAGE
void memctlc_ddr3_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	
	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

	_DTR_DDR3_MRS_setting(dtr, mr);

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR3_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR3_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR3_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. Set EMR3 */
	*dmcr = mr[3];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 7. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}
#endif

void memctlc_dram_phy_reset(void)
{
	volatile unsigned int *phy_ctl;

	phy_ctl = (volatile unsigned int *)DACCR;
	*phy_ctl = *phy_ctl & ((unsigned int) 0xFFFFFFEF);
	*phy_ctl = *phy_ctl | ((unsigned int) 0x10);
	//_memctl_debug_printf("memctlc_dram_phy_reset: 0x%08p(0x%08x)\n", phy_ctl, *phy_ctl);

	return;
}


void memctlc_lxbus_check(void)
{
	volatile unsigned int *socpnr, *reg1, *reg2, *reg3;

	socpnr = (volatile unsigned int *)SOCPNR;
	reg1 = (volatile unsigned int *)0xb8001074;
	reg2 = (volatile unsigned int *)0xb8000108;
	reg3 = (volatile unsigned int *)0xb80010b0;

	//puts("IC RL6318B Checking\n\r");

	if((*socpnr == 0x000c0561) || (*socpnr == 0x00080561)){

		/* OCP bus async problem */
		*reg1 = *reg1 |0x80000000;
		/* Choose delay gnt */
		*reg2 = *reg2 |0x00800000;
		
	}else if( (*socpnr == 0x00000705) || ((*socpnr & 0xffff)== 0x00006405)){

		/* Enable jitter tolerance */
		*reg3 = *reg3 |0x1111;

	}

	return;

}

