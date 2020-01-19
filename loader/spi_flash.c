/*
 * spi_flash.c
 * 
 * History:
 *   Tylo,  draft version 
 *   SH     2008/08/01    Merge tylo v.9 and Andrew 0.8a to 0.9a
 */
#include "spi_flash.h"

#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
unsigned int SPI_EN4B_ADDR = 0xB8000330;
#define EN4B_SHIFT	19
#elif defined(CONFIG_RTL8685)
unsigned int SPI_EN4B_ADDR = 0xB8000100;
unsigned int SFCR2 = 0xB8001204;
#define EN4B_SHIFT	6
#elif defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
unsigned int SPI_EN4B_ADDR = 0xB8000100;
unsigned int SFCR2 = 0xB8001204;
#define EN4B_SHIFT	6
#else
unsigned int SPI_EN4B_ADDR = 0xB8000100;
#define EN4B_SHIFT	6
#endif
#define SPI_EN4B(i) ((i) << EN4B_SHIFT)
static int spi_en_4b=0;

#define LENGTH(i)       SPI_LENGTH(i)
#define CS(i)           SPI_CS(i)
#define IO_WIDTH(i)		SPI_IO_WIDTH(i)
#define RD_ORDER(i)     SPI_RD_ORDER(i)
#define WR_ORDER(i)     SPI_WR_ORDER(i)
#ifdef CONFIG_ADV_SPIC
#define READY(i)       	(SPI_READY(i) |SPI_IDLE(i))
#define CSB0_STATUS(i)	SPI_CSB0_STATUS(i)
#define CSB1_STATUS(i)	SPI_CSB1_STATUS(i)
#define CS_DEACTIVE(i)	(READY(i) | CSB0_STATUS(i) | CSB1_STATUS(i))
#define CS_ACTIVE(i)	(CSB0_STATUS(i) | CSB1_STATUS(i))
#else
#define READY(i)        SPI_READY(i)
#endif
#define CLK_DIV(i)      SPI_CLK_DIV(i)
#define RD_MODE(i)      SPI_RD_MODE(i)
#define SFSIZE(i)       SPI_SFSIZE(i)
#define TCS(i)          SPI_TCS(i)
#define RD_OPT(i)       SPI_RD_OPT(i)


/* SPI Flash Controller */
unsigned int SFCR=0;
unsigned int SFCSR=0;
unsigned int SFDR=0;

/*4B addr cycle*/
#define EN4B	0xB7
#define EX4B	0xE9


static inline void spi_ready(void)
{
   while (1)
   {
      if ( (*(volatile unsigned int *) SFCSR) & READY(1))
         break;
   } 
}
/*
 * SPI Flash Info
 */
const struct spi_flash_db   spi_flash_known[] =
{
   {0xC2, 0x20,   0}, /* MXIC MX25L1605D */
   {0xef, 0x40,   0}, /* winbond W25Q128FVFIG */   	
   {0x01, 0x20,   0}, /* Spansion S25FL128SAGMFI00*/ 	
   {0x01, 0x02,   0}, /* Spansion S25FL256SAGMFI00*/
   {0x1C, 0x31,  0}, /* EON */
   {0x1C, 0x30,  0}, /* EON */   
   {0x8C, 0x20,  0}, /* F25L016A */
   {0xEF, 0x30,  0}, /* W25X16 */
   {0x1F, 0x46,  0}, /* AT26DF161 */
   {0xBF, 0x25,  0}, /* 25VF016B-50-4c-s2AF */
   {0xC8, 0x40, 0}, /*GigaDevice GD25Q16*/
   {0xEF, 0x40, 0}, /*SPANSION S25FL016K*/
};

unsigned int SPI_DEFAULT=3;
unsigned int SPI_CMD=0;

/* Address cycle */
#define SPI_3B_Default		3
#define SPI_4B_Default		4

/* Command sequence */
enum SPI_CMD_SET{
	SPI_CMD_RDSR=0,
	SPI_CMD_Read,
	SPI_CMD_WREN,
	SPI_CMD_PP,
	SPI_CMD_CE,
	SPI_CMD_RDID,
	SPI_CMD_BE,
	SPI_CMD_WRSR,
	SPI_CMD_SE,

	SPI_CMD_NONE
};

unsigned int command_set[][SPI_CMD_NONE+1] = {
	{0x05/*RDSR*/	
	, 0x03/*Read*/	
	, 0x06/*WREN*/	
	, 0x02/*PP*/		
	, 0xC7/*CE*/
	, 0x9F/*RDID*/	
	, 0xD8/*BE*/		
	, 0x01/*WRSR*/
	, 0x20/*SE*/

	},

	/* For MX25L25635F*/  
	{0x05/*RDSR*/	
	, 0x13/*Read4B*/	
	, 0x06/*WREN*/	
	, 0x12/*PP4B*/	
	, 0xC7/*CE*/
	, 0x9F/*RDID*/	
	, 0xDC/*BE4B*/		
	, 0x01/*WRSR*/
	, 0x21/*SE4B*/

	},
};


/*
 * SPI Flash Info
 */
struct spi_flash_type   spi_flash_info;

static volatile void flush_prediction(volatile unsigned int x) {
	if (x>1)
		flush_prediction(x-1);
}

void do_spi_en4b(int control_4B)
{	
	if(control_4B==1){
		spi_ready();
		/* EN4B (enable 4B addr mode) Command */
		*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (EN4B << 24);
		spi_ready();
		*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	}
	
	spi_en_4b = 1;
	 return;	
}

void do_spi_ex4b(void)
{
	spi_ready();
	/* EN4B (enable 4B addr mode) Command */
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = (EX4B << 24);
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	spi_en_4b = 0;

	return;
}

/*
*	Handle the SPI Flash 4Bbyte-address cycle mode	
*
*	In RTL8685 platform, bit 6 of register 0xb8000100 
*	0 --> SPI Flash 4Byte-address-cycle mode
*	1 --> SPI Flash 3Byte-address-cycle mode
*
*	In RTL8676/RTL8676S platform , bit 19 of register 0xb8000330 
*	0 --> SPI Flash 3Byte-address-cycle mode
*	1 --> SPI Flash 4Byte-address-cycle mode
*/
void spi_handle_4B(void)
{
	if(spi_en_4b==1)
		return;
	
#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
#if 1
	/* 
	*	Consider MX25L25635F Only
	*	==> Flash default in 3Byte-address-mode
	*	==> Set memory controller to 4Byte-address-mode
	*/
	SPI_CMD=1;
	SPI_DEFAULT=SPI_3B_Default;
	*(volatile unsigned int *)SFCR2 = 0x0ce08200;
	do_spi_en4b(0);		
	*(volatile unsigned int *)SPI_EN4B_ADDR &= ~(SPI_EN4B(1));
	
#else
	if( ((*(volatile unsigned int *)SPI_EN4B_ADDR) & SPI_EN4B(1)) == SPI_EN4B(1)){

		/* 
		*	Consider MX25L25635F
		*	==> Flash default in 3Byte-address-mode
		*	==> Set Flash and memory controller to 4Byte-address-mode
		*/
		SPI_CMD=1;
		SPI_DEFAULT=SPI_3B_Default;
		*(volatile unsigned int *)SFCR2 = 0x0ce08200;
		do_spi_en4b(0);		
		*(volatile unsigned int *)SPI_EN4B_ADDR &= ~(SPI_EN4B(1));
	}else{
		/* 
		*	Consider MX25L25735F, normal case
		*	==> Flash default in 4Byte-address-mode
		*	==> SoC strap pin is 4Byte-address-mode
		*/
		SPI_CMD=0;
		SPI_DEFAULT=SPI_4B_Default;
		do_spi_en4b(0);
	}
#endif
#elif defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
	if( ((*(volatile unsigned int *)SPI_EN4B_ADDR) & SPI_EN4B(1)) != SPI_EN4B(1)){
		SPI_CMD=0;
		SPI_DEFAULT=SPI_3B_Default;
		do_spi_en4b(1);
		*(volatile unsigned int *)SPI_EN4B_ADDR &= ~(SPI_EN4B(1));
	}else{
		SPI_CMD=0;
		SPI_DEFAULT=SPI_4B_Default;
		do_spi_en4b(0);
	}
#endif	

	return;
}

/*
 * This function shall be called when switching from MMIO to PIO mode
 */
#ifdef CONFIG_ADV_SPIC
volatile void spi_pio_init(void)
{
	spi_ready();
	flush_prediction(10);

	/* CS deactive */
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
	while (1)
	{
		if ( (*(volatile unsigned int *) SFCSR) & CS_DEACTIVE(1))
			break;
	} 

	/* CS active */
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(0) | READY(1);
	while (((*(volatile unsigned int *) SFCSR) & CS_ACTIVE(0))!=0)
	{
		;
	} 
	
	/* CS deactive */
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
	while (1)
	{
		if ( (*(volatile unsigned int *) SFCSR) & CS_DEACTIVE(1))
			break;
	} 

	return;

}
#else
volatile void spi_pio_init(void)
{
	spi_ready();
	flush_prediction(10); // RDC's suggestion for prediction on 1fc00000 conflict with SPI PIO, andrew
	//toggle_a6();
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);

	spi_ready(); delay_usec(3);
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);

	spi_ready(); delay_usec(3);
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

	spi_ready();

	return;
}
#endif

void spi_read(unsigned int address, unsigned int *data_out)
{
   /* De-Select Chip */
   *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

   /* RDSR Command */
   spi_ready();
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);

   *(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

   while (1)
   {
      unsigned int status;

      status = *(volatile unsigned int *) SFDR;

      /* RDSR Command */
      if ( (status & 0x01000000) == 0x00000000)
      {
         break;
      }
   }
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

   /* READ Command */
   spi_ready();
   *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);

   *(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_Read] << 24) | (address & 0xFFFFFF);

   /* Read Data Out */
   *data_out = *(volatile unsigned int *) SFDR;

   *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
}


void spi_write(unsigned int address, unsigned char data_in)
{
	spi_pio_init();
	
	/* WREN Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

	/* BP Command */
	spi_ready();

	if(spi_en_4b){
		*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_PP] << 24);
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (address & 0xFFFFFFFF);
	}else{
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_PP] << 24) | \
										(address & 0xFFFFFF);
	}

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = (data_in<<24) | 0xFFFFFF;

	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

	/* RDSR Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

	while (1)
	{
	  unsigned int status;

	  status = *(volatile unsigned int *) SFDR;

	  /* RDSR Command */
	  if ( (status & 0x01000000) == 0x00000000)
	     break;
	  }
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
}


void spi_erase_chip(void)
{
   /* De-select Chip */
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

   /* RDSR Command */
   spi_ready();
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
   *(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

   while (1)
   {
      /* RDSR Command */
      if ( ((*(volatile unsigned int *) SFDR) & 0x01000000) == 0x00000000)
      {
         break;
      }
   }

   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

   /* WREN Command */
   spi_ready();
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
   *(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

   /* BE Command */
   spi_ready();
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
   *(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_BE] << 24);
   *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
}

void spi_probe(void)
{
	unsigned int i, temp;

/*
	if (probed == 1)//no more probe need!
		return;
	probed = 1;
*/	
	memset(&spi_flash_info, 0, sizeof(struct spi_flash_type));

#if defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	/* Increase the SPI clock to 25MHz */
	*(volatile unsigned int *) SFCR  =(*(volatile unsigned int *) SFCR & (~(0x7 << 29))) |SPI_CLK_DIV(3) ;
#else
	*(volatile unsigned int *) SFCR =*(volatile unsigned int *) SFCR |SPI_CLK_DIV(1);
#endif /* CONFIG_RTL8685S or CONFIG_RTL8685SB*/

	spi_pio_init();      

	/* Here set the default setting */
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	//printf("\n");
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);

	/* One More Toggle (May not Necessary) */
	// *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	// *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);

	/* RDID Command */
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDID] << 24;
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
	temp = *(volatile unsigned int *) SFDR;

	spi_flash_info.maker_id = (temp >> 24) & 0xFF;
	spi_flash_info.type_id = (temp >> 16) & 0xFF;
	spi_flash_info.capacity_id = (temp >> 8) & 0xFF;

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
	//printf("delay before spi_ready\n\r");
	spi_ready();

	/* Iterate Each Maker ID/Type ID Pair */
	for (i = 0; i < sizeof(spi_flash_known) / sizeof(struct spi_flash_db); i++)
	{
		if ( (spi_flash_info.maker_id == spi_flash_known[i].maker_id) &&
			(spi_flash_info.type_id == spi_flash_known[i].type_id) )
		{
			spi_flash_info.device_size = (unsigned char)((signed char)spi_flash_info.capacity_id + spi_flash_known[i].size_shift);

			//MXIC 32MB Flash
			if((spi_flash_info.maker_id == 0xc2) || (spi_flash_info.maker_id == 0x01)){
				if (spi_flash_info.device_size >= 0x19) {
					//printf("Enable 4B mode: Vendor:%x Prod:%x Capa:%x\n", spi_flash_info.maker_id, spi_flash_info.type_id, spi_flash_info.device_size);
					spi_handle_4B();	
				}				
			}

			//Spansion 32MB Flash
			if((spi_flash_info.maker_id == 0x01) || (spi_flash_info.maker_id == 0x02)){
				if (spi_flash_info.device_size >= 0x19) {
					//printf("Enable 4B mode: Vendor:%x Prod:%x Capa:%x\n", spi_flash_info.maker_id, spi_flash_info.type_id, spi_flash_info.device_size);
					spi_handle_4B();	
				}				
			}

			//GigaDevice 32MB Flash
			if((spi_flash_info.maker_id == 0xc8) || (spi_flash_info.maker_id == 0x40)){
				if (spi_flash_info.device_size >= 0x20) {
                			//patch for GigaDevice 64MB, which has different size offset from other flashes
					spi_flash_info.device_size -= 0x6;
				}
				
				if (spi_flash_info.device_size >= 0x19) {
					//printf("Enable 4B mode: Vendor:%x Prod:%x Capa:%x\n", spi_flash_info.maker_id, spi_flash_info.type_id, spi_flash_info.device_size);
					spi_handle_4B();	
				}
				
			}
			
			break;
		}
	}
	//for atmel and sst flash, its device_size should be re-calculated.
	if (spi_flash_info.maker_id == 0x1F) {
		spi_flash_info.device_size = (spi_flash_info.type_id & 0x1F) + 15;
	}
	else if (spi_flash_info.maker_id == 0xBF) {
		if (spi_flash_info.device_size == 0x41) //identifies the device as SST25VF032B
			spi_flash_info.device_size = 21;
		else
			spi_flash_info.device_size = 0;
	}
	
	spi_flash_info.sector_cnt = ((1<<spi_flash_info.device_size)>>16);
}

void spi_erase_sector(int sector)
{
	spi_pio_init();

	/* WREN Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);

	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

	/* SE Command */
	spi_ready();
	if(spi_en_4b){
		*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_SE] << 24);
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (sector << 12);
	}else{
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_SE] << 24) | (sector << 12);
	}
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

	/* RDSR Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

	while (1)
	{
		/* RDSR Command */
		if ( ((*(volatile unsigned int *) SFDR) & 0x01000000) == 0x00000000)
		{
			break;
		}
	}

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
}

void spi_erase_block(int sector)
{
	spi_pio_init();

	/* WREN Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);

	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

	/* SE Command */
	spi_ready();
	if(spi_en_4b){
		*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
		/*0xD8 is block erase command, 2^16 = 64KB*/
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_BE] << 24);// | (block << 16);	
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);	
		*(volatile unsigned int *) SFDR = (sector << 16);
	}else{
		*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
		*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_BE] << 24) | (sector << 16);
	}
	
	*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

	/* RDSR Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

	while (1)
	{
		/* RDSR Command */
		if ( ((*(volatile unsigned int *) SFDR) & 0x01000000) == 0x00000000)
		break;
	}

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
}

#ifdef CONFIG_SPI_FLASH
void spi_global_unprotect(unsigned char flag)
{
	spi_pio_init();

	/* De-select Chip */
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

	/* RDSR Command */
	spi_ready();
	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
	*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

	while (1)
	{
		unsigned int status;
		status = *(volatile unsigned int *) SFDR;

		/* RDSR Command */
		if ( (status & 0x01000000) == 0x00000000)
		{
			//ql: if block protected, then write status register 0
			if (status & (flag<<24))
			{
				*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

				/*WREN Command*/
				spi_ready();
				*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
				*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;
				*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

				spi_ready();

				//WRSR command
				*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
				*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_WRSR] << 24);
				*(volatile unsigned int *) SFDR = 0;
			}
			break;
		}
	}

	*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);
}

void spi_program_by_byte(unsigned long flash_address ,unsigned char *pData, unsigned int len)
{
	unsigned int idx;
	unsigned int cnt=0;

	for (idx=0; idx<len; idx++) {
		spi_write(flash_address++, pData[idx]);
		if (cnt++ >= 65536) {
			printf(".");
			cnt=0;
		}
	}
}

/* currently this function is dedicate for 64KB flash size and 256B page size*/
void spi_program_by_page(unsigned long flash_address ,unsigned char *image_addr, unsigned int image_size)
{
	unsigned int temp;
	unsigned int i, j, k;
	unsigned char *cur_addr;
	unsigned int cur_size;
	unsigned int cnt;
	unsigned int sect_cnt;

	cur_addr = image_addr;
	cur_size = image_size;

	if (flash_address & ((1 << 16) -1))
	{
		printf("\r\nflash_address must be 64KB aligned!!");
		return;
	}

	sect_cnt = flash_address >> 16; //flash_address must be 64KB aligned
	
	/* Iterate Each Sector */
	for (i = sect_cnt; i < spi_flash_info.sector_cnt; i++)
	{
		printf(".");

		/* Iterate Each Page,  64 KB = 256(page size)*256(page number) */
		for (j = 0; j < 256; j++)
		{
			if (cur_size == 0)
				break;

			/* WREN Command */
			spi_ready();
			*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
			*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_WREN] << 24;
			*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

			/* PP Command */
			spi_ready();
			if(spi_en_4b){
				*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
				*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_PP] << 24);
				*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
				*(volatile unsigned int *) SFDR = (((i << 16) | (j << 8)) & 0xFFFFFFFF);
			}else{
				*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
				*(volatile unsigned int *) SFDR = (command_set[SPI_CMD][SPI_CMD_PP] << 24) |\
											(i << 16) | (j << 8);
			}

			for (k = 0; k < 64; k++)
			{
				temp = (*(cur_addr)) << 24 | (*(cur_addr + 1)) << 16 | (*(cur_addr + 2)) << 8 | (*(cur_addr + 3));

				spi_ready();
				if (cur_size >= 4)
				{
					*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1) | READY(1);
					cur_size -= 4;
				}
				else
				{
					*(volatile unsigned int *) SFCSR = LENGTH(cur_size-1) | CS(1) | READY(1);
					cur_size = 0;
				}

				*(volatile unsigned int *) SFDR = temp;
				cur_addr += 4;

				if (cur_size == 0)
					break;
			}

			*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

			/* RDSR Command */
			spi_ready();
			*(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1) | READY(1);
			*(volatile unsigned int *) SFDR = command_set[SPI_CMD][SPI_CMD_RDSR] << 24;

			cnt = 0;
			while (1)
			{
				unsigned int status = *(volatile unsigned int *) SFDR;

				/* RDSR Command */
				if ((status & 0x01000000) == 0x00000000)
					break;

				if (cnt > 200000)
				{
					printf("\nBusy Loop for RSDR: %d, Address at 0x%08X\n", status, (i<<12)+(j<<8));
				busy:
					goto busy;
				}
				cnt++;
			}
			*(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
		}

		if (cur_size == 0)
			break;
	} /* Iterate Each Sector */
}

#else
void spi_burn_image(unsigned int chip, unsigned char *image_addr, unsigned int image_size)
{
   unsigned int temp;
   unsigned int i, j, k;
   unsigned char *cur_addr;
   unsigned int cur_size;
   unsigned int cnt;

   cur_addr = image_addr;
   cur_size = image_size;

    spi_pio_init();
	spi_probe();

   /* Iterate Each Sector */
   for (i = 0; i < spi_flash_info[chip].sector_cnt; i++)
   {
      unsigned int spi_data;

      /* WREN Command */
      spi_ready();
      *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1+chip) | READY(1);

      *(volatile unsigned int *) SFDR = 0x06 << 24;
      *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

      /* SE Command */
      spi_ready();
      *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1+chip) | READY(1);
      *(volatile unsigned int *) SFDR = (0xD8 << 24) | (i * 65536);
      *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

      /* RDSR Command */
      spi_ready();
      *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1+chip) | READY(1);
      *(volatile unsigned int *) SFDR = 0x05 << 24;

      while (1)
      {
         /* RDSR Command */
         if ( ((*(volatile unsigned int *) SFDR) & 0x01000000) == 0x00000000)
         {
            break;
         }
      }

      *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

#if 1
      printf("Erase Sector: %d\n", i);
#endif

      /* Iterate Each Page */
      for (j = 0; j < 256; j++)
      {
         if (cur_size == 0)
            break;

         /* WREN Command */
         spi_ready();
         *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1+chip) | READY(1);
         *(volatile unsigned int *) SFDR = 0x06 << 24;
         *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

         /* PP Command */
         spi_ready();
         *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1+chip) | READY(1);
         *(volatile unsigned int *) SFDR = (0x02 << 24) | (i * 65536) | (j * 256);

         for (k = 0; k < 64; k++)
         {
            temp = (*(cur_addr)) << 24 | (*(cur_addr + 1)) << 16 | (*(cur_addr + 2)) << 8 | (*(cur_addr + 3));

            spi_ready();
            if (cur_size >= 4)
            {
               *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1+chip) | READY(1);
               cur_size -= 4;
            }
            else
            {
               *(volatile unsigned int *) SFCSR = LENGTH(cur_size-1) | CS(1+chip) | READY(1);
               cur_size = 0;
            }

            *(volatile unsigned int *) SFDR = temp;

            cur_addr += 4;

            if (cur_size == 0)
               break;
         }

         *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);

         /* RDSR Command */
         spi_ready();
         *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(1+chip) | READY(1);
         *(volatile unsigned int *) SFDR = 0x05 << 24;

         cnt = 0;
         while (1)
         {
            unsigned int status = *(volatile unsigned int *) SFDR;

            /* RDSR Command */
            if ((status & 0x01000000) == 0x00000000)
            {
                break;
            }

            if (cnt > 2000)
            {
               printf("\nBusy Loop for RSDR: %d, Address at 0x%08X\n", status, i*65536+j*256);
busy:
               goto busy;
            }
            cnt++;
         }

         *(volatile unsigned int *) SFCSR = LENGTH(0) | CS(3) | READY(1);

         /* Verify Burned Image */
         /* READ Command */
         spi_ready();
         *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(1+chip) | READY(1);
         *(volatile unsigned int *) SFDR = (0x03 << 24) | (i * 65536) | (j * 256);

         for (k = 0; k < 64; k++)
         {
            unsigned int data;

            temp = (*(cur_addr -256 + (k<<2) )) << 24 | (*(cur_addr -256 + (k<<2) + 1)) << 16 | (*(cur_addr - 256 + (k<<2) + 2)) << 8 | (*(cur_addr - 256 + (k<<2) + 3));

            data = *(volatile unsigned int *) SFDR;

            if ((data != temp))
            {
               printf("\nVerify Error at 0x%08X: Now 0x%08X, Expect 0x%08X",
                      i*65536+j*256+(k<<2), data, temp);

halt_here:
               goto halt_here;

            }
         }

         *(volatile unsigned int *) SFCSR = LENGTH(3) | CS(3) | READY(1);
      }

      if (cur_size == 0)
         break;
   } /* Iterate Each Sector */
}
#endif
void spi_test(){
	printf("probing!!");
	spi_probe();

	spi_erase_sector(1);
	spi_write(0x10004,0x11223344);
	spi_write(FLASH_START+0x10000,0x55667788);
	
	//printf("writing");
#if 0
	unsigned int testspi;
	unsigned int tmp;

	for (testspi = 0; testspi< 64*1024;testspi+=4)
	{
		spi_write(0,0+testspi,testspi);
		if((testspi & 0xffff) == 0)
			printf(".");
		spi_read(0,0+testspi,&tmp);
		if (testspi != tmp )
			printf("[%x]:%x",testspi,tmp);
		
	}
#endif	

}
