#include "mips_cache_ops.h"

void _1004K_L1_DCache_flush(void){
        unsigned long config1;
        unsigned int lsize;
        unsigned long dcache_size ;
        unsigned long linesz,sets,ways;
        int i;

        config1 = read_c0_config1();

        /* D-Cache */
        lsize = (config1 >> 10) & 7;
        linesz = lsize ? 2 << lsize : 0;
        sets = 32 << (((config1 >> 13) + 1) & 7);
        ways = 1 + ((config1 >> 7) & 7);
        dcache_size = sets *  ways * linesz;

        for(i=CKSEG0;  i < (CKSEG0 + dcache_size); i +=  linesz){
                cache_op(Index_Writeback_Inv_D,i);
        }

        _mips_sync();
}

void _1004K_L1_ICache_flush(void){
	unsigned long config1;
	unsigned int lsize;
	unsigned long icache_size ;
	unsigned long linesz,sets,ways;
	int i;

	config1 = read_c0_config1();

	/* I-Cache */
	lsize = (config1 >> 19) & 7;//4->32B Line Size
	linesz = lsize ? 2 << lsize : 0;//lineSize = 32B
	sets = 32 << (((config1 >> 22) + 1) & 7);
	ways = 1 + ((config1 >> 16) & 7);
	icache_size = sets * ways * linesz;

	for(i=CKSEG0; i < (CKSEG0 + icache_size);  i +=  linesz) {
		cache_op(Index_Invalidate_I,i);
	}

        _mips_sync();

}

void flush_l1cache(void) {
	unsigned long config1;
	unsigned int lsize;
	unsigned long icache_size;
	unsigned long dcache_size ;
	unsigned long linesz,sets,ways;
	int i;

	config1 = read_c0_config1();

	/* I-Cache */
	lsize = (config1 >> 19) & 7;//4->32B Line Size
	linesz = lsize ? 2 << lsize : 0;//lineSize = 32B
	sets = 32 << (((config1 >> 22) + 1) & 7);
	ways = 1 + ((config1 >> 16) & 7);
	icache_size = sets * ways * linesz;

	for(i=CKSEG0; i < (CKSEG0 + icache_size);  i +=  linesz) {
		cache_op(Index_Invalidate_I,i);
	}


	/* D-Cache */
	lsize = (config1 >> 10) & 7;
	linesz = lsize ? 2 << lsize : 0;
	sets = 32 << (((config1 >> 13) + 1) & 7);
	ways = 1 + ((config1 >> 7) & 7);
	dcache_size = sets *  ways * linesz;

	for(i=CKSEG0;  i < (CKSEG0 + dcache_size); i +=  linesz){
		cache_op(Index_Writeback_Inv_D,i);
	}

	_mips_sync();

}

void flush_l2cache(void){
    unsigned long cache_size ;
    unsigned long linesz,sets,ways;
    int i;
    unsigned int config2;
    unsigned int tmp;

    config2 = read_c0_config2();

        /* check L2 bypass */
    if (config2 & (1 << 12))
        return 0;

    /* detect L2-Cache */
    tmp = (config2 >> 4) & 0x0f;
    if (0 < tmp && tmp <= 7)
        linesz = 2 << tmp;
    else
        return 0;

    tmp = (config2 >> 8) & 0x0f;
    if (0 <= tmp && tmp <= 7)
        sets = 64 << tmp;
    else
        return 0;

    tmp = (config2 >> 0) & 0x0f;
    if (0 <= tmp && tmp <= 7)
        ways = tmp + 1;
    else
        return 0;

    cache_size = sets *  ways * linesz;

        //printk("L2cache LineSize=%lu, Sets=%lu, Ways=%lu, CacheSize=%lu\n", linesz, sets, ways, cache_size);

        /* flush L2 cache*/
    for (i = CKSEG0;  i < (CKSEG0 + cache_size); i += linesz) {
      cache_op(Index_Writeback_Inv_SD,i);
    }

    _mips_sync();
}

void mips_cache_flush(void) {
	flush_l1cache();
	flush_l2cache();
}

