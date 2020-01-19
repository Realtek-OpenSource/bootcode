/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>                
#include <stdlib.h>
#include <stdarg.h>
#include "spi_flash.h"
#include "gpio.h"
#define _cache_flush	cache_flush
/* SPI flash setting */

extern struct spi_flash_type spi_flash_info;

static unsigned long flash_size, block_size;

/* Definitions for memory test error handing manner */
#define MT_SUCCESS    (0)
#define MT_FAIL       (-1)

#ifndef NULL
#define NULL ((void *)0)
#endif

#define LED_NUMBER		24
#define TEST_FAIL		0
#define TEST_SUCCESS	1
#define TEST_LED_ON		0
#define TEST_LED_OFF	1

/***********************************************
  * macro for virtual address to physical address
  ***********************************************/
#define UADDR(addr)             ((u32_t)(addr)|0x20000000)     // uncache address
#define CADDR(addr)             ((u32_t)(addr)&~0x20000000)    // cache address
#define PADDR(addr)             ((u32_t)(addr)&~0xE0000000)    // physical address

typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef unsigned long ulong;

//The default seting of memory test error handling manner is non-blocking 
//Using "Environment parameter" or "command flag setting" can change this
//"Environment parameter": setenv mt_freeze_block debug
//"command flag setting": mdram_test/mflash_test -b/-mt_block
static u32_t g_err_handle_block_mode=0; 
static char test_result = TEST_SUCCESS;
static char flash_led_status = TEST_LED_ON;


#define FLASHBASE       0xBD000000
#define HANDLE_FAIL \
({ \
	printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
	if(!g_err_handle_block_mode){\
		return MT_FAIL;\
	}else{ \
		while(1);\
	} \
})


#define CHECK_TIMEOUT \
({ \
	if (time_out>0){ \
		if (timeout(cur_ms, time_out)){	\
			printf("Flash Test TimeOut\n\n");\
			break;	\
		}	\
	} \
})

#define LED_SWITCH \
({ \
	if (flash_led_status == TEST_LED_ON){ \
		flash_led_status = TEST_LED_OFF; \
		gpioClear(LED_NUMBER); \
	} else { \
		flash_led_status = TEST_LED_ON; \
		gpioSet(LED_NUMBER); \
	} \
})

#define FLASH_TOP_ADDRESS(flash_size)	    (FLASHBASE+flash_size-1)
#define TEST_SIZE_PER_PATTREN	(0x10000)  	//64KB
static u32_t src_data_addr;
#define	SRC_DATA_ADDR		(src_data_addr)

extern void cache_flush(void);
/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

const u32_t flash_patterns[] = {
					0x00000000,
					0xff00ff00,
					0x00ff00ff,
					0x0000ffff,
					0xffff0000,
					0xffffffff,
					0x5a5aa5a5,
					0xa5a5a5a5,
					0x55555555, 
					0xaaaaaaaa, 
					0x01234567, 
					0x76543210, 
					0x89abcdef,
					0xfedcba98,
			};

/*
 * Function Declaration
 */
int flash_sect_erase (ulong addr_first, ulong addr_last)
{
	unsigned int i;
	unsigned int sector_first, sector_last;
	unsigned int size = addr_last - addr_first;
	
	sector_first = (addr_first - FLASHBASE) / block_size;
	sector_last = (addr_last - FLASHBASE) / block_size;

	for (i=sector_first; i<=sector_last; i++){
		spi_erase_block(i);
	}
	
	return 0;
}

extern void spi_program_by_byte(unsigned long flash_address,
								unsigned char *pData, unsigned int len);
/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * Returns:
 * ERR_OK          0 - OK
 * ERR_TIMOUT      1 - write timeout
 * ERR_NOT_ERASED  2 - Flash not erased
 * ERR_PROTECTED   4 - target range includes protected sectors
 * ERR_INVAL       8 - target address not in Flash memory
 * ERR_ALIGN       16 - target address not aligned on boundary
 *			(only some targets require alignment)
 */
int
flash_write (char *src, ulong addr, ulong len)
{
	// change address 
	addr -= FLASHBASE;
	spi_program_by_byte(addr, src, len);
	return 0;
}
 

/* Function Name: 
 * 	nor_normal_patterns
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
int flash_normal_patterns(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i, j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < (sizeof(flash_patterns)/sizeof(u32_t)); i++)
	{
		_cache_flush();

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = flash_patterns[i];
			src_start++;
		}

		_cache_flush();
		
		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash:pattern[%d](0x%x) setting pass\n", i, flash_patterns[i]);
		
		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		LED_SWITCH;
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check flash data sequentially. Uncached address */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j<test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data sequentially. Cached address */
		src_start = (u32_t *)(CADDR((u32_t)flash_start));

		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(CADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		LED_SWITCH;
		printf("Flash: pattern[%d](0x%x) pass\n", i, flash_patterns[i]);
		printf("pattern[%d](0x%x) completed\n", i, flash_patterns[i]);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_1(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t walk_pattern;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = (1 << i);
		
		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		LED_SWITCH;
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		LED_SWITCH;
		printf("Flash: pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_0(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t walk_pattern;
	u32_t flash_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = ~(1 << i);
		
		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash:pattern[%d](0x%x) setting passed\n", i, walk_pattern);
		
		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		LED_SWITCH;
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		LED_SWITCH;
		printf("Flash:pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ((u32_t)src_start << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)src_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)src_start , start_value, (((u32_t)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash: rotate %d setting passed\n", i);
		
		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		LED_SWITCH;
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		_dram_start = (u32_t *) SRC_DATA_ADDR;
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)_dram_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, (((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}

		LED_SWITCH;
		printf("Flash: rotate %d 0x%x passed\n", i, flash_start);
		printf("rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_com_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ~(((u32_t)src_start) << i);
			src_start++;
		}
		
		_cache_flush();
		/* check data */  
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)src_start << i)))
			{
				 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
				        (u32_t)src_start , start_value, ~(((u32_t)src_start) << i), 
						 __FUNCTION__, __LINE__);
 			     HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash: ~rotate %d setting passed\n", i);
		
		_cache_flush();
		
		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		LED_SWITCH;
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check data */  
		_dram_start = (u32_t *) SRC_DATA_ADDR;
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)_dram_start << i)))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, ~(((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}

		LED_SWITCH;
		printf("Flash: ~rotate %d 0x%x passed\n", i, flash_start);
		printf("~rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

//extern void spi_test();
int flash_test(int time_out, int round)
{
	int i, j;
	unsigned int addr, data, count, cur_ms, read_pattern, start_addr;
	unsigned char write_pattern;


	printf("\nTimeout: %d Second\n", time_out);
	printf("Test %d Round\n", round);

	printf("\n\n");
	spi_probe();

	//DRAM Temp Space
	src_data_addr = 0x80000000;
	flash_size = (1<<spi_flash_info.device_size);
	block_size = flash_size/spi_flash_info.sector_cnt;

	printf("SPI Flash Size: %d MB\n", flash_size/1048576);
	printf("SPI Flash Sector Cnt: %d\n", spi_flash_info.sector_cnt);

	start_addr = FLASHBASE + 2 * block_size;
	cur_ms = get_sys_time();
	count = 0;
	time_out *= 1000; // change to ms

	gpioConfig(LED_NUMBER, GPIO_FUNC_OUTPUT);
	while ((!timeout(cur_ms, time_out)) || (count<round)){
		/* partial range */
		if(MT_SUCCESS != flash_normal_patterns(start_addr, block_size, flash_size)){
			HANDLE_FAIL;
			break;
		}

		CHECK_TIMEOUT;

		if(MT_SUCCESS != flash_walking_of_1(start_addr, block_size, flash_size)){
			HANDLE_FAIL;
			break;
		}

		CHECK_TIMEOUT;

		if(MT_SUCCESS != flash_walking_of_0(start_addr, block_size, flash_size)){
			HANDLE_FAIL;
			break;
		}

		CHECK_TIMEOUT;

		if(MT_SUCCESS != flash_addr_rot(start_addr, block_size, flash_size)){
			HANDLE_FAIL;
			break;
		}

		CHECK_TIMEOUT;

		if(MT_SUCCESS != flash_com_addr_rot(start_addr, block_size, flash_size)){
			HANDLE_FAIL;
			break;
		}

		CHECK_TIMEOUT;

		printf("== %d runs mflash_test PASS==\n\n", ++count);		
	}//for(i=0;i<cmd_info.test_loops;i++){

	gpioSet(LED_NUMBER);
	printf("Flash: Test %d Round\n", count);

	if (test_result == TEST_SUCCESS)
		printf("===== PASS =====\n\n");
	else
		printf("===== FAIL =====\n\n");

	return 0;
}

