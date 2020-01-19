#ifndef _RTK_BBT_H_
#define _RTK_BBT_H_

#ifdef CONFIG_NAND_PAGE_2K
#define KEEP_ORI_BBI /*keep original bad block indicator, for page size 2K*/
#endif
#ifdef KEEP_ORI_BBI
#define DATA_BBI_OFF  ((512*4)- 48)/*(0~1999)512+512+512+512-48*/
#define OOB_BBI_OFF  ((16*4) - 10 - 1) /*(0~23)6+[10]+6+[10]+6+[10]+6+[10] []:ecc bytes*/
#else
#define OOB_BBI_OFF  0 /*(0~23)6+[10]+6+[10]+6+[10]+6+[10] []:ecc bytes*/
#define DATA_BBI_OFF  0
#endif

struct Boot_Rsv{
    unsigned int num;
    unsigned int start_block;
};

struct  BB_t{
    unsigned short BB_die;
    unsigned short bad_block;
    unsigned short RB_die;
    unsigned short remap_block;
};

struct  BBT_v2r{
    unsigned int block_r;
};

#ifdef CONFIG_RTK_NORMAL_BBT
struct  BBT_normal{
    unsigned short BB_die;
    unsigned short bad_block;
    unsigned short RB_die;
    unsigned short block_info;	
};
#endif
//-------------------Reserve Block Area usage ---------------------//
#define	BB_INIT		0xFFFE
#define	RB_INIT		0xFFFD
#define	BBT_TAG		0xBB
#define 	RSV_TAG 	0xCC
#define 	BB_DIE_INIT	0xEEEE
#define 	RB_DIE_INIT	BB_DIE_INIT




#ifdef CONFIG_RTK_NORMAL_BBT
extern unsigned int bbt_num;// = 102; 102 = (32MB/16K) * 5%
extern struct BBT_normal *bbt_nor;
int nand_scan_normal_bbt(void);
int nand_checkbad_block(unsigned int flash_page);
int rtk_update_normal_bbt (struct BBT_normal *bbt_nor);
void dump_normal_BBT(void);
#endif
#ifdef CONFIG_RTK_REMAP_BBT
int rtk_scan_v2r_bbt(void);
int rtk_nand_scan_bbt(void);
void dump_BBT(void);
#endif



#endif /*_RTK_BBT_H_*/
