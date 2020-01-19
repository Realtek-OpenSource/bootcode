#include "nor_flash.h"

#define FLASH_BASE   0xBD000000

#define DIVISOR   25  // 25MHz

/* Timer */
#define TC_BASE      0xB8003100
   #define TC0DATA      (TC_BASE + 0x00)
   #define TC1DATA      (TC_BASE + 0x04)
   #define TC0CNT       (TC_BASE + 0x08)
   #define TC1CNT       (TC_BASE + 0x0C)
   #define TCCNR        (TC_BASE + 0x10)
   #define TCIR         (TC_BASE + 0x14)
   #define CDBR         (TC_BASE + 0x18)
   #define WDTCNR       (TC_BASE + 0x1C)

unsigned int test_timeout(unsigned int timer)
{
   if (timer == 1)
   {
      if (*(volatile unsigned int *)TCIR & 0x10000000)
         return 1;
      else
         return 0;
   }
   else
   {
      if (*(volatile unsigned int *)TCIR & 0x20000000)
         return 1;
      else
         return 0;
   }
}

void set_usec(unsigned int timer, unsigned int num)
{

	return;
   if (num >= 0x1000000)
   {
      printf("Input Out of Range\n");
      return;
   }

   if (timer == 1)
   {
      if ((*(volatile unsigned int *) TCIR) & 0x10000000)
         *(volatile unsigned int *) TCIR = (0x1 << 28);

      *(volatile unsigned int *) TCCNR = 0x0;
      *(volatile unsigned int *) CDBR = (DIVISOR << 16);
      *(volatile unsigned int *) TC1DATA = (num << 8);
      *(volatile unsigned int *) TCCNR = (0x2 << 28);
   }
   else
   {
      if ((*(volatile unsigned int *) TCIR) & 0x20000000)
         *(volatile unsigned int *) TCIR = (0x2 << 28);

      *(volatile unsigned int *) TCCNR = 0x0;
      *(volatile unsigned int *) CDBR = (DIVISOR << 16);
      *(volatile unsigned int *) TC0DATA = (num << 8);
      *(volatile unsigned int *) TCCNR = (0x8 << 28);
   }
}



/*
 * NOR Flash Info
 */
struct nor_flash_type   nor_flash_info[2];


/*
 * NOR Flash APIs
 */
void nor_read(unsigned int chip, unsigned int address, unsigned short *data_out)
{
   int start_address = 0;

   if (chip == 1)
      start_address = (1 << nor_flash_info[0].device_size);

   *data_out = *(volatile unsigned short *)((FLASH_BASE | start_address | address) & 0xFFFFFFFE);
}

void nor_write(unsigned int chip, unsigned int address, unsigned short data_in)
{
   int start_address = 0;

   if (chip == 1)
      start_address = (1 << nor_flash_info[0].device_size);

   *(volatile unsigned short *)((FLASH_BASE | start_address | address) & 0xFFFFFFFE) = data_in;
}


void nor_write_data(unsigned int chip, unsigned int address, unsigned int data_in)
{
   unsigned int i;
   unsigned int write_byte = 0;
   unsigned int write_hword = 0;

   unsigned short write_data, read_data;

   union U {
      unsigned char b[4];
      unsigned short s[2];
      unsigned int i;
   } u;

   u = (union U) data_in;
   write_byte = address;

   for (i = 0; i < 2; i++)
   {
      // Reset
      nor_write(chip, 0x000, 0xF0);   /* For AMD */
      nor_write(chip, 0x000, 0xFF);   /* For Intel */

      write_data = u.s[i];

      // Program
      if (nor_flash_info[chip].command_set == 1)
      {
         nor_write(chip, 0x000, 0x40);
      }
      else
      {
         nor_write(chip, 0xAAA, 0xAA);
         nor_write(chip, 0x555, 0x55);
         nor_write(chip, 0xAAA, 0xA0);
      }

      nor_write(chip, write_byte, write_data);

      if (nor_flash_info[chip].command_set == 1)
      {
         set_usec(0, (1 << nor_flash_info[chip].program_time));

         nor_read(chip, write_byte, &read_data);

         while (!(read_data & 0x80))
         {
            if (test_timeout(0))
            {
               printf("\nProgram Timeout");
               break;
            }

            nor_read(chip, write_byte, &read_data);
         }
      }
      else
      {
         set_usec(0, (1 << nor_flash_info[chip].program_time));

         nor_read(chip, write_byte, &read_data);

         while ((write_data ^ read_data) & 0x80)
         {
            if (test_timeout(0))
            {
               printf("\nProgram Timeout");
               break;
            }

            nor_read(chip, write_byte, &read_data);
         }
      }

      write_byte += 2;
   }

   // Reset
   nor_write(chip, 0x000, 0xF0);   /* For AMD */
   nor_write(chip, 0x000, 0xFF);   /* For Intel */
}


void nor_probe(void)
{
   int chip;

   for (chip = 0; chip < 2; chip++)
   {
      unsigned short i;
      unsigned short temp1, temp2, temp3;

      // Reset
      nor_write(chip, 0x000, 0xF0);   /* For AMD */
      nor_write(chip, 0x000, 0xFF);   /* For Intel */

      // Read Manufacturer's ID
      nor_write(chip, 0xAAA, 0xAA);
      nor_write(chip, 0x555, 0x55);
      nor_write(chip, 0xAAA, 0x90);

      nor_read(chip, 0x000, &temp1);
      nor_flash_info[chip].maker_id = temp1;

      // Reset
      nor_write(chip, 0x000, 0xF0);   /* For AMD */
      nor_write(chip, 0x000, 0xFF);   /* For Intel */

      // Query
      nor_write(chip, 0x0AA, 0x98);

      // Read Command Set
      nor_read(chip, 0x026, &temp1);
      nor_flash_info[chip].command_set = temp1;

      // Read Device Size
      nor_read(chip, 0x04E, &temp1);
      nor_flash_info[chip].device_size = temp1;

      // Read Boot Sector Information      // Device Specific??!!
      nor_read(chip, 0x09E, &temp1);
      nor_flash_info[chip].boot_sector = temp1;

      // Read Block Count
      nor_read(chip, 0x058, &temp1);
      nor_flash_info[chip].block_cnt = temp1;

      // Iterate Each Block
      for (i = 0; i < temp1; i++)
      {
         // Read Sector Count
         nor_read(chip, 0x05A + i*8, &temp2);
         nor_read(chip, 0x05C + i*8, &temp3);

         if (nor_flash_info[chip].boot_sector == 3) // Top
            nor_flash_info[chip].sector_cnt[temp1-i-1] = (temp3 << 8) + temp2;
         else // 2 and others are treated as bottom
            nor_flash_info[chip].sector_cnt[i] = (temp3 << 8) + temp2;

         // Read Sector Size
         nor_read(chip, 0x05E + i*8, &temp2);
         nor_read(chip, 0x060 + i*8, &temp3);

         if (nor_flash_info[chip].boot_sector == 3) // Top
            nor_flash_info[chip].sector_size[temp1-i-1] = (temp3 << 8) + temp2;
         else // 2 and others are treated as bottom
            nor_flash_info[chip].sector_size[i] = (temp3 << 8) + temp2;
      }

      // Read Max. Block Erase Time
      nor_read(chip, 0x042, &temp1);
      nor_read(chip, 0x04A, &temp2);
      nor_flash_info[chip].block_erase_time = temp1 + temp2;
      nor_flash_info[chip].block_erase_time_avg = temp1;

      // Read Max. Porgram Time
      nor_read(chip, 0x03E, &temp1);
      nor_read(chip, 0x046, &temp2);
      nor_flash_info[chip].program_time = temp1 + temp2;
      nor_flash_info[chip].program_time_avg = temp1;
   }
}


void nor_burn_image(unsigned int chip, unsigned short *image_addr, unsigned int image_size)
{
   unsigned int write_byte = 0;
   unsigned int write_hword = 0;
   unsigned int i, j, k;
   unsigned short write_data, read_data;

   // Reset
   nor_write(chip, 0x000, 0xF0);   /* For AMD */
   nor_write(chip, 0x000, 0xFF);   /* For Intel */

   /* Iterate Each Block */
   for (i = 0; i < nor_flash_info[chip].block_cnt; i++)
   {
      /* Iterate Each Sector */
      for (j = 0; j < nor_flash_info[chip].sector_cnt[i] + 1; j++)
      {
         // Reset
         nor_write(chip, 0x000, 0xF0);   /* For AMD */
         nor_write(chip, 0x000, 0xFF);   /* For Intel */

         // Sector Erase
         if (nor_flash_info[chip].command_set == 1)
         {
            nor_write(chip, write_byte, 0x20);
            nor_write(chip, write_byte, 0xD0);
         }
         else
         {
            nor_write(chip, 0xAAA, 0xAA);
            nor_write(chip, 0x555, 0x55);
            nor_write(chip, 0xAAA, 0x80);
            nor_write(chip, 0xAAA, 0xAA);
            nor_write(chip, 0x555, 0x55);
            nor_write(chip, write_byte, 0x30);
         }

         {
            set_usec(0, (1 << nor_flash_info[chip].block_erase_time) * 1000);

            nor_read(chip, write_byte, &read_data);

            while ((read_data & 0x80) == 0x0)
            {
               if (test_timeout(0))
               {
                  printf("Erase Timeout\n");
                  break;
               }

               nor_read(chip, write_byte, &read_data);
            }
         }

         printf("Erased Sector Address 0x%08X\n", write_byte);

	//tylo
	nor_write(chip, 0xAAA, 0xAA);
       nor_write(chip, 0x555, 0x55);
       nor_write(chip, 0xAAA, 0x20);

         for (k = 0; k < (nor_flash_info[chip].sector_size[i] * 256); k += 2)
         {
            if (image_size == 0)
               break;

            // Reset
            //nor_write(chip, 0x000, 0xF0);   /* For AMD */
            //nor_write(chip, 0x000, 0xFF);   /* For Intel */

            // Program
            if (nor_flash_info[chip].command_set == 1)
            {
               nor_write(chip, 0x000, 0x40);
            }
            else
            {
               //nor_write(chip, 0xAAA, 0xAA);
               //nor_write(chip, 0x555, 0x55);
               nor_write(chip, 0x000, 0xA0);
            }

            write_data = *(volatile unsigned short *)((volatile unsigned char *)image_addr + write_byte);

            nor_write(chip, write_byte, write_data);
            //printf("write %x  %x\n\r",write_byte,write_data);

            if (nor_flash_info[chip].command_set == 1)
            {
               set_usec(0, (1 << nor_flash_info[chip].program_time));

               nor_read(chip, write_byte, &read_data);

               while (!(read_data & 0x80))
               {
                  if (test_timeout(0))
                  {
                     printf("Program Timeout\n");
                     break;
                  }

                  nor_read(chip, write_byte, &read_data);
               }
            }
            else
            {
               set_usec(0, (1 << nor_flash_info[chip].program_time));

               nor_read(chip, write_byte, &read_data);

               while ((write_data ^ read_data) & 0x80)
               {
                  if (test_timeout(0))
                  {
                     printf("Program Timeout\n");
                     break;
                  }

                  nor_read(chip, write_byte, &read_data);
               }
            }
#if 0
            // Reset
            nor_write(chip, 0x000, 0xF0);   /* For AMD */
            nor_write(chip, 0x000, 0xFF);   /* For Intel */

            /* Verify Written Byte/Word */
            nor_read(chip, write_byte, &read_data);

            if (read_data != write_data)
            {
               printf("Verify Error. Address: 0x%08X, Write: 0x%08X, Read: 0x%08X\n", write_byte, write_data, read_data);
            }
#endif
            write_byte += 2;
            image_size -= 2;
         }

	nor_write(chip, 0x000, 0x90);
	nor_write(chip, 0x000, 0x00);
	nor_write(chip, 0x000, 0xF0);   /* For AMD */
       nor_write(chip, 0x000, 0xFF);   /* For Intel */
       nor_write(chip, 0x000, 0x90);
	nor_write(chip, 0x000, 0x00);
	
         if (image_size == 0)
            break;
      }

      if (image_size == 0)
         break;
   }
}


void nor_erase_chip(int chip)
{
   unsigned int write_byte = 0;
   unsigned short i, j, k;
   unsigned short write_data, read_data;

   // Reset
   nor_write(chip, 0x000, 0xF0);   /* For AMD */
   nor_write(chip, 0x000, 0xFF);   /* For Intel */

   /* Iterate Each Block */
   for (i = 0; i < nor_flash_info[chip].block_cnt; i++)
   {
      /* Iterate Each Sector */
      for (j = 0; j < nor_flash_info[chip].sector_cnt[i] + 1; j++)
      {
         // Reset
         nor_write(chip, 0x000, 0xF0);   /* For AMD */
         nor_write(chip, 0x000, 0xFF);   /* For Intel */

         // Sector Erase
         if (nor_flash_info[chip].command_set == 1)
         {
            nor_write(chip, write_byte, 0x20);
            nor_write(chip, write_byte, 0xD0);
         }
         else
         {
            nor_write(chip, 0xAAA, 0xAA);
            nor_write(chip, 0x555, 0x55);
            nor_write(chip, 0xAAA, 0x80);
            nor_write(chip, 0xAAA, 0xAA);
            nor_write(chip, 0x555, 0x55);
            nor_write(chip, write_byte, 0x30);
         }

         {
            set_usec(0, (1 << nor_flash_info[chip].block_erase_time) * 1000);

            nor_read(chip, write_byte, &read_data);

            while ((read_data & 0x80) == 0x0)
            {
               if (test_timeout(0))
               {
                  printf("Erase Timeout\n");
                  break;
               }

               nor_read(chip, write_byte, &read_data);
            }
         }

         write_byte += nor_flash_info[chip].sector_size[i] * 256;
      }
   }
}
