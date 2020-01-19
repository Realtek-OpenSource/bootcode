#include "./bspchip_8685.h"
#include "./memctl_8685.h"

/*====================================================================================== */
/* Merge from board_mem_diag.c */
/*=======================================================================================*/
unsigned int dramtype_freq_25oc[4][8]__attribute__ ((section(".text"))) = {						
		{100, 384, 225, 192, 100, 384, 225, 100},						
		{150, 350, 225, 200, 150, 350, 225, 125},						
		{192, 300, 250, 175, 192, 300, 250, 150},						
		{200, 400, 275, 166, 200, 400, 275, 200},					
};


void pll_setup(void){

#ifndef CONFIG_SOFTWARE_OVERWRITE_FREQ

	/* Don't do anything, just use default value */

#else
	unsigned int wait_loop_cnt = 0;

	/* (1) CPU CLOCK select LX PLL clock */    
	wait_loop_cnt = 700 * 10000;    
	while(wait_loop_cnt--){    
	    if(wait_loop_cnt == 3500000){    
	        REG32(SYSREG_SYSTEM_STATUS_REG) =     
	            REG32(SYSREG_SYSTEM_STATUS_REG) & (~SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK);    
	    }    
	}

	/* (2) Invoke the PLL change. */    
#if 1
	/* DSP clock must be set by new method due to the design*/
	REG32(SYSREG_SYSTEM_STATUS_REG) &= ~(SYSREG_SYSTEM_STATUS_SYS_CPU2_EN_MASK);
	REG32(SYSREG_SYSTEM_STATUS_REG) |= SYSREG_SYSTEM_STATUS_SYS_CPU2_EN_MASK;

	wait_loop_cnt = 500 * 1000;    
	while(wait_loop_cnt--)
	
	REG32(SYSREG_SYSCLK_CONTROL_REG) |= CONFIG_SYS_PLL_CTRL;   

	wait_loop_cnt = 500 * 1000;    
	while(wait_loop_cnt--)

	REG32(SYSREG_SYSTEM_STATUS_REG) &= ~(SYSREG_SYSTEM_STATUS_SYS_CPU2_EN_MASK);
#else
	wait_loop_cnt = 700 * 10000;    
	while(wait_loop_cnt--){    
	    if(wait_loop_cnt == 3500000){    
	        REG32(SYSREG_SYSCLK_CONTROL_REG) |= CONFIG_SYS_PLL_CTRL;    
	    }    
	}
#endif

	/* (3) Set MEMCLK Phase 90 to 90 */        
	REG32(SYSREG_PINMUX0_REG)|=(1<<24);
	REG32(SYSREG_PLL_CTL_REG)= (REG32(SYSREG_PLL_CTL_REG)& 0xffffff87)|(0x2<<3);		/* phase 45*/
	
	/* (4.1) Set MEMCLK Clock divison */        
	REG32(SYSREG_MCKG_FREQ_DIV_REG) = CONFIG_MCKG_FREQ_DIV;   
	REG32(SYSREG_MCKG_PHS_SEL_REG) = 0;

	/* (4.2) Set OCP/LX Clock divison */
	REG32(SYSREG_CMUIPMASK0_REG) = CONFIG_SYS_CMU_IP_MASK0;
	REG32(SYSREG_CMUIPMASK1_REG) = CONFIG_SYS_CMU_IP_MASK1;
	REG32(SYSREG_CMUSLPIDX_REG) = CONFIG_SYS_CMU_SLP_IDX;

	 /* (5) CPU CLOCK select OCP0 PLL clock */    
	 wait_loop_cnt = 700 * 10000;   
	 while(wait_loop_cnt--){    
	     if(wait_loop_cnt == 3500000){   
	         REG32(SYSREG_SYSTEM_STATUS_REG) =    
	             REG32(SYSREG_SYSTEM_STATUS_REG) | SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK;    
	     }    
	 } 
#endif

	return;

}

unsigned int sys_get_dramtype_freq_strapping(void)
{	
	volatile unsigned int *bonding_reg;	
	volatile unsigned int *strapping_reg;	
	unsigned int b_sel, s_sel;	
	
	/* !!!!!!!!!!!!!!!! FIX ME !!!!!!!!!!!!!!!!!! */
	
	bonding_reg = (volatile unsigned int *)BONDING_REG;	
	strapping_reg = (volatile unsigned int *)STRAPING_REG;	
	b_sel = (*bonding_reg & BONDING_REG_DRAMTYPE_FREQ43_MASK) >> BONDING_REG_DRAMTYPE_FREQ43_FD_S;	
	s_sel = (*strapping_reg & STRAPING_REG_DRAMTYPE_FREQ210_MASK) >> STRAPING_REG_DRAMTYPE_FREQ210_FD_S; 	

	return dramtype_freq_25oc[b_sel][s_sel];	
}


/* Function Name: 
 * 	board_CPU_freq_mhz
 * Descripton:
 *	Get the current CPU frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	CPU frequncy in MHz.
 */
unsigned int board_CPU_freq_mhz(const unsigned int cmu_ctrl_reg)
{
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
	unsigned int cpu_clk;
	unsigned int cmu_div;

	cmu_div = 1;
	if(cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_MD_MASK){
		cmu_div = 1 << ((cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_CPU0_FREQ_DIV_MASK) >>\
			   SYSREG_CMUCTLR_CMU_CPU0_FREQ_DIV_FD_S);
	}

	cpu_clk = (REG32(SYSREG_SYSCLK_CONTROL_REG) & SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S;

	cpu_clk = (cpu_clk + 20) * 25;
	/*puts("CPU Clock "); puthex((cpu_clk/cmu_div)); puts("\n");*/

	return (cpu_clk/cmu_div);
#else
	return CONFIG_CPUCLK_MHZ;
#endif
}

/* Function Name:
 *      board_DSP_freq_mhz
 * Descripton:
 *      Get the current DSP frequency in MHz.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      DSP frequncy in MHz.
 */
unsigned int board_DSP_freq_mhz(const unsigned int cmu_ctrl_reg)
{
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
		unsigned int dsp_clk;
		unsigned int cmu_div;

		cmu_div = 1;
		if(cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_MD_MASK){
			cmu_div = 1 << ((cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_CPU1_FREQ_DIV_MASK) >>\
			SYSREG_CMUCTLR_CMU_CPU1_FREQ_DIV_FD_S);
		}
		
        dsp_clk = (REG32(SYSREG_SYSCLK_CONTROL_REG) & SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK) \
                >> SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S;

	dsp_clk = (dsp_clk + 20) * 25;
        return (dsp_clk/cmu_div);
#else
        return CONFIG_DSPCLK_MHZ;
#endif
}


/* Function Name: 
 * 	board_DRAM_freq_mhz
 * Descripton:
 *	Get the current DRAM frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	DRAM Frequncy in MHz.
 */
unsigned int board_DRAM_freq_mhz(void)
{
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
	unsigned int mem_clk, clk_divison;

	clk_divison = 1 << ((REG32(SYSREG_MCKG_FREQ_DIV_REG) & SYSREG_MCKG_FREQ_DIV_MASK) \
		>> SYSREG_MCKG_FREQ_DIV_FD_S);
	mem_clk = (REG32(SYSREG_SYSCLK_CONTROL_REG) & SYSREG_SYSCLK_CONTROL_SDPLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_SDPLL_FD_S;


	mem_clk = (mem_clk + 12) * 25;
	/*puts("DRAM clock: 0x"); puthex(mem_clk); puts("\n");*/

	return mem_clk / clk_divison;
#else
	return sys_get_dramtype_freq_strapping();
#endif
}

/* Function Name: 
 * 	board_LX_freq_mhz
 * Descripton:
 *	Get the current LX bus frequency in MHz.
 * Input:
 *	cmu_ctrl_reg
 *     use_cmu : 0->always in wake up mode
 *			 1->use cmu mode
 *     index : LX index, 0->LX0, 1->LX1, 2->LX2, 3->LXP
 * Output:
 * 	None
 * Return:
 *  	LX bus frequncy in MHz.
 */
unsigned int board_LX_freq_mhz(const unsigned int cmu_ctrl_reg, unsigned char use_cmu, unsigned int index)
{
#ifdef CONFIG_FPGA_RTL8685SB
	return CONFIG_LXBUS_MHZ;
#else
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
	unsigned int lx_clk_div;
	unsigned int lx_clk_div_offset;
	unsigned int lx_freq_mhz;
	unsigned int src_oc_mhz;
	unsigned int cmu_div, cmu_mask, cmu_shift;

	cmu_shift = SYSREG_CMUCTLR_CMU_LX0_FREQ_DIV_FD_S - (index*3);
	cmu_mask = 7 << cmu_shift;
	cmu_div = 1;

#if 1
	if(((cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_MD_MASK)>> SYSREG_CMUCTLR_CMU_MD_FD_S)\
		== SYSREG_CMUCTLR_CMU_MD_MANUALLY_SEL){

		cmu_div = 1 << ((cmu_ctrl_reg & cmu_mask) >> cmu_shift);
	}
#else
	
	if(((cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_MD_MASK)>> SYSREG_CMUCTLR_CMU_MD_FD_S)\
			 == SYSREG_CMUCTLR_CMU_MD_MANUALLY_SEL){
		cmu_div = 1 << ((cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_LX0_FREQ_DIV_MASK) >>\
			   SYSREG_CMUCTLR_CMU_LX0_FREQ_DIV_FD_S);
	}
#endif

	src_oc_mhz = 25;
	lx_clk_div_offset = 6;
	
	lx_clk_div = REG32(SYSREG_LX_PLL_SEL_REG) + lx_clk_div_offset;
	lx_freq_mhz = (src_oc_mhz * (lx_clk_div + 2))/2;
	/*puts("LX clock: 0x"); puthex(lx_freq_mhz/cmu_div); puts("\n");*/
	
	return lx_freq_mhz/cmu_div;
#else
	return CONFIG_LXBUS_MHZ;
#endif
#endif //CONFIG_FPGA_RTL8685SB
}

/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
void memctlc_clk_rev_check(void)
{
	volatile unsigned int delay_loop;
	unsigned int clk_rev_ctl_reg;
	unsigned int clk_rev;
	unsigned int cpu_clk;
	unsigned int dsp_clk;
	unsigned int mem_clk;
	unsigned int lx0_clk;
	unsigned int lx1_clk;
	unsigned int lx2_clk;

	clk_rev = 0;
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
	clk_rev_ctl_reg = CONFIG_SYS_CMU_CTRL;
#else
	clk_rev_ctl_reg = REG32(SYSREG_CMUCTLR_REG);
#endif
	cpu_clk = board_CPU_freq_mhz(clk_rev_ctl_reg);
	dsp_clk = board_DSP_freq_mhz(clk_rev_ctl_reg);
	mem_clk = board_DRAM_freq_mhz();
	lx0_clk = board_LX_freq_mhz(clk_rev_ctl_reg, 1,0);
	lx1_clk = board_LX_freq_mhz(clk_rev_ctl_reg, 1,1);
	lx2_clk = board_LX_freq_mhz(clk_rev_ctl_reg, 1,2);
	

	if(cpu_clk <= mem_clk){
		clk_rev = (clk_rev | SYSREG_OCP0_SMALLER_MASK);
	}

	if(dsp_clk < mem_clk){
		clk_rev = (clk_rev | SYSREG_OCP1_SMALLER_MASK);
	}

	if(lx0_clk <= mem_clk){
		clk_rev = (clk_rev | SYSREG_LX0_SMALLER_MASK );
	}
	
	if(lx1_clk <= mem_clk){
		clk_rev = (clk_rev | SYSREG_LX1_SMALLER_MASK );
	}

	if(lx2_clk <= mem_clk){
		clk_rev = (clk_rev | SYSREG_LX2_SMALLER_MASK );
	}

	REG32(SYSREG_CMUCTLR_REG) = clk_rev_ctl_reg & ~(SYSREG_OCP0_SMALLER_MASK | SYSREG_OCP1_SMALLER_MASK |\
 			SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK)\
			| clk_rev ;

	delay_loop = 0x1000;
	while(delay_loop--);

	return;
}

/*
 * DRAM parameters APIs for initializing DDR1/2/3 SDRAM
 * We current define environment variables listed below
 * for DRAM ODT/OCD/ZQ/Delay parameters. These fuctions are called when initializing
 * DDR1/2/3 SDRAM memory.
 * zq_cali_value: ZQ auto-calibration value, we apply zq_cali_value if it exists.
 * dram_odt	: DRAM side ODT value of DDR2 SDRAM(0, 50, 75, 150)
 * dram_ocd	: DRAM side OCD value of DDR3 SDRAM(ZQ/6, ZQ/7)
 * ddrkodl	: DDRKODL register value. (Assign CLKM/TX/PHASE 90 delay.)
 * 
 */
unsigned int get_memory_delay_parameters(unsigned int *para_array)
{
	para_array[0] = CONFIG_DRAM_PREFERED_DDRKODL;
	return 1;
}

#ifdef CONFIG_DDR2_USAGE
unsigned int get_memory_ddr2_dram_odt_parameters(unsigned int *para_array)

{
	para_array[0] = (unsigned int)CONFIG_DDR2_DRAM_ODT_VALUE;
	return 1; /*fail */
}
#endif

#ifdef CONFIG_DDR3_USAGE
unsigned int get_memory_ddr3_dram_rtt_nom_parameters(unsigned int *para_array)

{
	para_array[0] = (unsigned int)CONFIG_DDR3_DRAM_RTT_NOM_VALUE;
	return 1; /*fail */
}

unsigned int get_memory_ddr3_dram_rtt_wr_parameters(unsigned int *para_array)

{
	para_array[0] = (unsigned int)CONFIG_DDR3_DRAM_RTT_WR_VALUE;
	return 1; /*fail */
}
#endif

unsigned int get_memory_dram_reduce_drv_parameters(unsigned int *para_array)
{
	if(CONFIG_PREFERED_DRAM_DRIV_STRENGTH){
		para_array[0] = 0; /*full*/
	}else{
		para_array[0] = 1;/*reduce*/
	}
	return 1;
}


