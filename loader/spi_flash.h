#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

/*
 * Macro Definition
 */
#define SPI_CS(i)           ((i) << 30)   /* 0: CS0 & CS1   1: CS0   2: CS1   3: NONE */
#define SPI_LENGTH(i)       ((i) << 28)   /* 0 ~ 3 */
#define SPI_READY(i)        ((i) << 27)   /* 0: Busy  1: Ready */
#define SPI_CSB0_STATUS(i)	((i) << 11)   /* 0: active  1: not active */
#define SPI_CSB1_STATUS(i)	((i) << 10)   /* 0: active  1: not active */
#define SPI_IDLE(i)			((i) << 4)   /* 0: not idle state 1: in idle state  */

#define SPI_CLK_DIV(i)      ((i) << 29)   /* 0: DIV_2  1: DIV_4  2: DIV_6 ... 7: DIV_16 */
#define SPI_IO_WIDTH(i)		((i) << 25)	  /* 00: serial I/O ,01:Dual I/O ,10:quad I/O ,11:reserved	*/
#define SPI_RD_ORDER(i)     ((i) << 28)   /* 0: Little-Endian  1: Big-Endian */
#define SPI_WR_ORDER(i)     ((i) << 27)   /* 0: Little-Endian  1: Big-Endian */
#define SPI_RD_MODE(i)      ((i) << 26)   /* 0: Fast-Mode  1: Normal Mode */
#define SPI_SFSIZE(i)       ((i) << 23)   /* 0 ~ 7. 128KB * (i+1) */
#define SPI_TCS(i)          ((i) << 19)   /* 0 ~ 15 */
#define SPI_RD_OPT(i)       ((i) << 18)   /* 0: No-Optimization  1: Optimized for Sequential Access */


/*
 * Structure Declaration
 */
struct spi_flash_type
{
   unsigned char maker_id;
   unsigned char type_id;
   unsigned char capacity_id;
   unsigned char device_size;        // 2 ^ N (bytes)
//   unsigned char sector_cnt;
   unsigned int sector_cnt;	//enlarge sector_cnt, 16MB spi flash = 256 sectors
};

struct spi_flash_db
{
   unsigned char maker_id;
   unsigned char type_id;
   signed char size_shift;
};


/*
 * Function Prototypes
 */
volatile void spi_pio_init(void);

void spi_read(unsigned int address, unsigned int *data_out);
void spi_write(unsigned int address, unsigned char data_in);

void spi_erase_chip(void);

void spi_probe(void);
void spi_burn_image(unsigned char *image_addr, unsigned int image_size);
//void spi_ready(void);

#endif /* _SPI_FLASH_H_ */
