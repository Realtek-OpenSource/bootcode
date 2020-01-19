#include "./memctl/bspchip_8685.h"
#include "./memctl/memctl_8685.h"


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
#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
	unsigned int lx_clk_div;
	unsigned int lx_clk_div_offset;
	unsigned int lx_freq_mhz;
	unsigned int src_oc_mhz;
	unsigned int cmu_div, cmu_mask, cmu_shift;

	cmu_shift = SYSREG_CMUCTLR_CMU_LX0_FREQ_DIV_FD_S - (index*3);
	cmu_mask = 7 << cmu_shift;

	cmu_div = 1;
	if(cmu_ctrl_reg & SYSREG_CMUCTLR_CMU_MD_MASK){
		if(use_cmu && (cmu_ctrl_reg & cmu_mask) ){
			cmu_div = 1 << ((cmu_ctrl_reg & cmu_mask) >> cmu_shift);
		}
	}

	src_oc_mhz = 25;
	lx_clk_div_offset = 6;
	
	lx_clk_div = REG32(SYSREG_LX_PLL_SEL_REG) + lx_clk_div_offset;
	lx_freq_mhz = (src_oc_mhz * (lx_clk_div + 2))/2;
	/*puts("LX clock: 0x"); puthex(lx_freq_mhz/cmu_div); puts("\n");*/
	
	return lx_freq_mhz/cmu_div;
#else
	return CONFIG_LXBUS_MHZ;
#endif
}

