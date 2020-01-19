#include "board.h"
#ifdef CONFIG_NAND_FLASH
#include "../loader/rtk_nand.h"
#include "../loader/rtk_nand_base.h"
#endif

extern unsigned char _binArrayStart[];
extern unsigned char _binArrayEnd;
unsigned int decompress_file(unsigned long Source, unsigned long Dest, unsigned long Length);
unsigned char *uncompressedStart = LOADER3_START;

/* compilier will be happy.. */
#ifndef memcpy
void memcpy(dest, source, len)
    unsigned char* dest;
    const unsigned char* source;
    unsigned int  len;
{
	if (len == 0) return;
	do {
	    *dest++ = *source++; /* ??? to be unrolled */
	} while (--len != 0);
}
#endif

#ifndef memcmp
int memcmp(s1, s2, len)
    const unsigned char* s1;
    const unsigned char* s2;
    unsigned int  len;
{
	unsigned int j;

	for (j = 0; j < len; j++) {
	    if (s1[j] != s2[j]) return 2*(s1[j] > s2[j])-1;
	}
	return 0;
}
#endif

#ifndef memset
void memset(dest, val, len)
    unsigned char* dest;
    int val;
    unsigned int  len;
{
	if (len == 0) return;
	do {
	    *dest++ = val;  /* ??? to be unrolled */
	} while (--len != 0);
}
#endif
//const char strmsg[]="  (unsigned int)_binArrayStart:";

static void unmap_sram(void){
	*((unsigned long*)0xb8001300)=0x0;
	*((unsigned long*)0xb8001310)=0x0;
	*((unsigned long*)0xb8001320)=0x0;
	*((unsigned long*)0xb8001330)=0x0;	
	*((unsigned long*)0xb8004000)=0x0;
	*((unsigned long*)0xb8004010)=0x0;
	*((unsigned long*)0xb8004020)=0x0;
	*((unsigned long*)0xb8004030)=0x0;	
}

#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
unsigned int CLOCKGEN_FREQ;
static void sys_clock_init(void) {
	unsigned int reg=0;
	reg = (*(volatile unsigned int*)0xb8000200 & 0x01F00)>>8;
#ifdef CONFIG_RTL8676S
	CLOCKGEN_FREQ = (reg+2)*12500000;			// = 25*(set value+2)*1/2, for RTL8676S
#else
	CLOCKGEN_FREQ = (reg+2)*10000000;			// = 20*(set value+2)*1/2
#endif
}
#endif

static void cache_flush(void){
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
//ccwei 
#ifdef CONFIG_NAND_FLASH
/* FOR RTL8676 NAND =====> */
//void dma_cache_writeb(void)
//{
//	cache_flush();
//}
int printf(char *fmt, ...) {}
/* <===== FOR RTL8676 NAND */
#endif

#if 0
static void dump_mem(unsigned char *start, unsigned int len) {
	unsigned char *p;
	for (p = start; p < (start + len); p++) {
		if (0==((unsigned int)p & 0xf)) {
			printf("\r\n%08x  ", (unsigned int)p);
		}
		printf("%02x ", *p);
	}
	printf("\r\n");
}
#endif 

#ifdef  CONFIG_NAND_FLASH
void RunNandLoader(void)
{
	int ret=1;
	int retry=0;
	int uncompressedLength;
	unsigned int start_page=0;
#ifdef CONFIG_CHECK_HDR	
	int find_load=0;
#endif

	rtk_nand_base_probe();

#ifdef CONFIG_RTL8685
#ifdef CONFIG_NAND_PAGE_512
	do{
		start_page+=ppb;
	}while(rtk_block_isbad(start_page));
#endif
#endif

	for (retry = 0; retry < BACKUP_LOADER; retry++) {

#ifdef CONFIG_CHECK_HDR
		find_load=0;
#endif

		int i=0,j=0;
#ifdef CONFIG_CHECK_HDR		
		unsigned char magic=0;
		unsigned int magic_loader=0;
#endif		
		unsigned char *ptr_data = (volatile unsigned char *)DRAM_DLOADER_ADDR;
		unsigned char *ptr_oob = (volatile unsigned char *)DRAM_DOOB_ADDR;
		for(i=0;i<((LOADER_IMAGE_SIZE+block_size-1)/block_size);i++){ //caculate how many block.

			NEXT_GOODBLOCK:
#ifdef CONFIG_RTK_REMAP_BBT
				if(start_page*page_size >= BOOT_SIZE - (2*BACKUP_BBT*block_size)){
#ifdef CONFIG_CHECK_HDR				
					puts("can't find magic\r\n");
#endif
					break;
			}
#else
			if(start_page*page_size >= BOOT_SIZE){
#ifdef CONFIG_CHECK_HDR				
					puts("can't find magic\r\n");
#endif
					break;
			}
#endif
			do{
				start_page+=ppb;
			}while(rtk_block_isbad(start_page));

			for(j=0;j<ppb;j++){ //each block have "ppb" pages.
#ifdef CONFIG_CHECK_HDR
				magic_loader = 0;
#endif
				if(rtk_read_ecc_page(start_page+j , ptr_data + (block_size*i)+(j*page_size), ptr_oob, page_size)){
					puts("page: ");puthex(start_page+j);puts(" -> read fail!\n\r");
					break;
				}
#ifdef CONFIG_CHECK_HDR				
				magic = *(ptr_data + (block_size*i)+(j*page_size));
				magic_loader |= magic << 24; 
				magic = *(ptr_data + (block_size*i)+(j*page_size) + 1);
				magic_loader |= magic << 16; 
				magic = *(ptr_data + (block_size*i)+(j*page_size) + 2); 		 
				magic_loader |= magic << 8; 			 
				magic = *(ptr_data + (block_size*i)+(j*page_size) + 3); 		 
				magic_loader |= magic;			 

				if(find_load && magic_loader == MAGIC_LOADER){

					/*During search time, we find another loader header, just load from here!*/
					puts("RunBackup: ");puthex(start_page);puts("\r\n");
					start_page -= ppb;
					goto SKIP_LOAD;
				}
				
				if(!find_load){
					if(magic_loader == MAGIC_LOADER){
						/* Find magic word and start to read! */
						puts("start_pageB: ");puthex(start_page);puts("\r\n");					 
						puts("find magic\r\n");puthex(magic_loader);puts("\r\n");						 
						find_load=1;
					}else{
						puts("start_pageB: ");puthex(start_page);puts("\r\n");					 
						puts("no magic\r\n");puthex(magic_loader);puts("\r\n");
						goto NEXT_GOODBLOCK;
					}
				}
#endif				
			}
			puts("block:");puthex(i); puts("\r\n"); 	
		}

		cache_flush();
		SKIP_LOAD:

		puts("Step2 Retry:"); puthex(retry); puts("\r\n");
#ifdef CONFIG_CHECK_HDR	
		//ptr_data+0x40, skip 64 bytes loader header!!!!
		ret = uncompressLZMA(uncompressedStart, &uncompressedLength, (ptr_data+LOAD_HEADER_LEN), LOADER_IMAGE_SIZE);
#else
		ret = uncompressLZMA(uncompressedStart, &uncompressedLength, ptr_data, LOADER_IMAGE_SIZE);
#endif
		//puts("Start:");puthex(uncompressedStart);puts(" Ret:");puthex(ret);puts("\r\n");
		//ret = uncompressLZMA(uncompressedStart, &uncompressedLength, 0x80100000, IMAGE_SIZE);
		//puts("2nart:");puthex(uncompressedStart);puts(" Ret:");puthex(ret);puts("\r\n");
		
		if(!ret)
		{
			void (*appStart)(void);
			//puts("jump to decompressed program!\n\r");

			/* jump to decompressed program */
			appStart = (void*)uncompressedStart;
			cache_flush();
			appStart();
		} else {
			extern void lzma_reset(void);
			lzma_reset();
		}
	}
	puts("Fail\r\n");
	while (1);


	return;
}

#else /* CONFIG_NAND_FLASH */

void RunLoader(void)
{
	int ret=1;
	unsigned int comprLen=0;
	int uncompressedLength;

	puts("\n\r\n\rStart to decompress!\n\r");

	/* infate RAM file */
	comprLen = (unsigned int)&_binArrayEnd - (unsigned int)_binArrayStart;
	ret = uncompressLZMA(uncompressedStart, &uncompressedLength, _binArrayStart, comprLen);
	if(!ret)
	{
		//puts("jump to decompressed program!\n\r");

		/* jump to decompressed program */
		void (*appStart)(void);
		appStart = (void*)uncompressedStart;
		cache_flush();
		appStart();
	}
	else
	{
		//puts("Decompress Loader Error!\n\r");
	
		/* error and hang */
		void (*bootstart) (void);
		bootstart = (void *) CPU_REBOOT_ADDR;
		bootstart();
	}

	return;
}
#endif /* CONFIG_NAND_FLASH */

void C_Entry( void )
{
#ifndef CONFIG_NO_FLASH
	const int calibrationDbgSize = 256;
	unsigned char calibrationDBG[calibrationDbgSize];
	
	memcpy(calibrationDBG, 0xA0600000, calibrationDbgSize);	//copy ddr calibration information from sram to dram
	unmap_sram();
	memcpy(0xA0600000, calibrationDBG, calibrationDbgSize);	//copy ddr calibration information to 0xA0600000 on dram
#endif

#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
	sys_clock_init();
    	initUart( 115200 );
#endif

#ifdef CONFIG_BOOT_MIPS
	/* 5281 has finished loading loader form flash to dram and starts running on dram */
	REG32(0xb8004100) = 1;
#endif

#ifdef CONFIG_NAND_FLASH
	RunNandLoader();
#else /* CONFIG_NAND_FLASH */
	RunLoader();	
#endif /* CONFIG_NAND_FLASH */

	return;
}

