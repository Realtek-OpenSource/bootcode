#ifndef _SPI_NAND_FLASH_H_
#define _SPI_NAND_FLASH_H_




extern unsigned int ppb; /*page per block*/



int spi_nand_read_page_with_ecc(unsigned int flash_page, unsigned char *data_buf, unsigned char *oob_buf, unsigned int data_size);
void spi_nand_write_page_with_ecc(unsigned int flash_page, unsigned char *data_buf, unsigned char *oob_buf, unsigned int data_size);
void rtk_spi_nand_probe(void);


#endif //_SPI_NAND_FLASH_H_
