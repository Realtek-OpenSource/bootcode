#ifndef _NOR_FLASH_H_
#define _NOR_FLASH_H_

#define NFCR         0xB8001100

/*
 * Function Prototypes
 */
void nor_read(unsigned int chip, unsigned int address, unsigned short *data_out);
void nor_write(unsigned int chip, unsigned int address, unsigned short data_in);
void nor_probe(void);
void nor_burn_image(unsigned int chip, unsigned short *image_addr, unsigned int image_size);

void nor_erase_chip(int chip);
void nor_write_data(unsigned int chip, unsigned int address, unsigned int data_in);

/*
 * Structure Declaration
 */
struct nor_flash_type
{
   unsigned char command_set;        // Intel or AMD
   unsigned char maker_id;
   unsigned char device_size;        // 2 ^ N (bytes)
   unsigned char block_cnt;
   unsigned short sector_cnt[32];
   unsigned short sector_size[32];       // N * 256 (bytes)
   unsigned char block_erase_time;       // 2 ^ N (ms) : max
   unsigned char block_erase_time_avg;   // 2 ^ N (ms) : typical
   unsigned char program_time;           // 2 ^ N (us) : max
   unsigned char program_time_avg;       // 2 ^ N (us) : typical
   unsigned char boot_sector;            // 2: Bottom  3: Top
};

#endif /* _NOR_FLASH_H_ */