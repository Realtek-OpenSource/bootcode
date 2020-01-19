/* amd29LvMtd.c - AMD AM29LV devices */
 

/* includes */
#include <stdlib.h>
#include "board.h"

//ccwei 111107
#include "global.h"
#include "rtk_nand.h"
#include "rtk_nand_base.h"
#include "rtk_bbt.h"
//end

/* defines */
#define OK	0
#define ERROR -1
#define flOK	OK
#define LOCAL	static
#define FALSE	0
#define flTimedOut	-1
#define flSectorNotFound	-2
#define CACHE_PIPE_FLUSH	sysWbFlush
#define DEBUGIT(fmt, arg...)
//typedef int	BOOL;
typedef char INT8;
typedef short INT16;
typedef long INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef int	STATUS;
typedef int FLStatus;
typedef int	FLBoolean;
typedef unsigned long	CardAddress;
typedef struct tFlash FLFlash;		/* Forward definition */

struct tFlash {
	unsigned short	type;
	unsigned short	flags;
	unsigned long	erasableBlockSize;
	unsigned long	chipSize;
	unsigned int	noOfChips;
	unsigned int	interleaving;
	unsigned long	baseAddress;
	void * 		(*map)(FLFlash *, CardAddress, int);
	FLStatus 	(*erase)(FLFlash *, int, int);
	FLStatus 	(*write)(FLFlash *, CardAddress, const void *, int, int);

};	

//#define FLASH_BASE_ADDR					0xbfc00000
#define AMD29LV_MTD_SECTOR_SIZE         (0x10000)	/* 64k */
#define AMD29LV_MTD_ERASE_SIZE		0x1000	/* 4K */
#ifdef UNUSE
#define AMD29LV_800_CHIP_SIZE           (0x100000)	/* 1MB */
#define AMD29LV_800_LAST_SECTOR_NUM     (AMD29LV_800_CHIP_SIZE / AMD29LV_MTD_SECTOR_SIZE - 1)
#endif
#define AMD29LV_160_CHIP_SIZE           (0x200000)	/* 2MB */
#define AMD29LV_160_LAST_SECTOR_NUM     (AMD29LV_160_CHIP_SIZE / AMD29LV_MTD_SECTOR_SIZE - 1)
#define AMD29LV_320_CHIP_SIZE           (0x400000)	/* 4MB */
#define AMD29LV_320_LAST_SECTOR_NUM     (AMD29LV_320_CHIP_SIZE / AMD29LV_MTD_SECTOR_SIZE - 1)
#define AMD29LV_640_CHIP_SIZE           (0x800000)	/* 8MB */
#define AMD29LV_640_LAST_SECTOR_NUM     (AMD29LV_640_CHIP_SIZE / AMD29LV_MTD_SECTOR_SIZE - 1)
#define AMD29LV_128_CHIP_SIZE           (0x1000000)	/* 16MB */
#define AMD29LV_128_LAST_SECTOR_NUM     (AMD29LV_128_CHIP_SIZE / AMD29LV_MTD_SECTOR_SIZE - 1)

#define AMD29LV_MTD_CHIP_CNT            (1)
#define AMD29LV_MTD_INTERLEAVE          (1)

#ifdef CONFIG_SPANSION_16M_FLASH
#define S29GL128P_SECTOR_SIZE 		  (0x20000)	/* 128k */
#define S29GL128P_CHIP_SIZE           	(0x1000000)	/* 16MB */
#define FLASH_BANK_SIZE 		  			(0x400000)	/* 4MB */
#define S29GL128P_LAST_SECTOR_NUM     (S29GL128P_CHIP_SIZE / S29GL128P_SECTOR_SIZE - 1)
#endif
/*alex_huang add 1280 support 20090909*/
#define AMD29LV_1280_MTD_SECTOR_SIZE         (0x10000)	/* 64k */
#define AMD29LV_1280_MTD_BOOT_SECTOR_SIZE         (0x2000)	/* 8k */

// definition to support SST flash
#define FLASH_SST_SUPPORT		1
#define SST39VF_MTD_SECTOR_SIZE         (0x1000)	/* 64k */
#define SST39VF_400_CHIP_SIZE           (0x80000)	/* 512KB */
#define SST39VF_400_LAST_SECTOR_NUM     (SST39VF_400_CHIP_SIZE / SST39VF_MTD_SECTOR_SIZE - 1)
#define SST39VF_800_CHIP_SIZE           (0x100000)	/* 1MB */
#define SST39VF_800_LAST_SECTOR_NUM     (SST39VF_800_CHIP_SIZE / SST39VF_MTD_SECTOR_SIZE - 1)
#define SST39VF_1601_CHIP_SIZE           (0x200000)	/* 2MB */
#define SST39VF_1601_LAST_SECTOR_NUM     (SST39VF_1601_CHIP_SIZE / SST39VF_MTD_SECTOR_SIZE - 1)
#define SST39VF_MTD_CHIP_CNT            (1)
#define SST39VF_MTD_INTERLEAVE          (1)


/* Save the last erase block on each device discovered in the array 
 * for NVRAM */
#undef SAVE_NVRAM_REGION



#undef DEBUG
#ifdef  DEBUG
    LOCAL UINT32 debug = DEBUG_ALWAYS;
    #define DEBUG_PRINT(mask, string) \
                if ((debug & mask) || (mask == DEBUG_ALWAYS)) \
                printf string
#else
    #define DEBUG_PRINT(mask, string)
#endif

/* local routines */

LOCAL FLStatus amd29lvSectorRangeErase(FLFlash* pVol, int, int);

LOCAL FLStatus amd29lvProgram(FLFlash*, CardAddress, const void *, int,
                             FLBoolean);

#ifdef CONFIG_SPANSION_16M_FLASH
LOCAL FLStatus s29gl128SectorRangeErase(FLFlash* pVol, int start, int sectorCount);
LOCAL FLStatus s29gl128Program(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite);
#endif

#ifdef CONFIG_SPI_FLASH
LOCAL FLStatus spiSectorRangeErase(FLFlash* pVol, int start, int sectorCount);
LOCAL FLStatus spiProgram(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite);
LOCAL void * spiMap(FLFlash* pVol, CardAddress address, int length);
int spiflashIdGet(FLFlash* pVol, UINT16* manCode, UINT16* devCode);
#endif

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
LOCAL FLStatus NandSectorRangeErase(FLFlash* pVol, int start, int sectorCount);
LOCAL FLStatus NandProgram(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite);
LOCAL void * NandMap(FLFlash* pVol, CardAddress address, int length);
#endif

LOCAL void * amd29lvMap(FLFlash*, CardAddress, int);

LOCAL void flashReset(FLFlash*);
LOCAL void flashIdGet(FLFlash*, UINT16*, UINT16*);
LOCAL void flashUnlock(FLFlash*);
LOCAL STATUS flashCheck16Bits(FLFlash*, UINT32);
LOCAL inline STATUS flashProgram16Bits(FLFlash*, volatile UINT32*, UINT16);
LOCAL inline void flashRegWrite16Bits(FLFlash*, UINT32, UINT16);
LOCAL UINT16 flashRegRead16Bits(FLFlash*, UINT32);
#if 0
LOCAL UINT16 flashRegRead8Bits(FLFlash*, UINT32);
#endif
LOCAL STATUS flashSectorErase(FLFlash*, int);

UINT16 DeviceID_cycle2, DeviceID_cycle3;	//added for verify mx29LV640M T/B


#ifdef BANK_AUTO_SWITCH
int flash_is_16MB(FLFlash* pVol)
{

	if ((((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221)) || 
		((pVol->type&0xff) == 0x7A) 	)
		return 1;
	else
		return 0;
}
#endif
/******************************************************************************
*
* amd29lvMTDIdentify - MTD identify routine (see TrueFFS Programmer's Guide)
*
* RETURNS: FLStatus
*
*/

FLStatus amd29lvMTDIdentify(FLFlash* pVol)
{
    UINT16 manCode;
    UINT16 devCode;

/*#ifdef CONFIG_SPI_FLASH
	if ((spiflashIdGet(pVol, &manCode, &devCode) == 0) )
#endif
#ifdef CONFIG_NAND_FLASH
	if ((nandflashGet(pVol, &manCode, &devCode) == 0))
#endif*/

#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
	if(IS_BOOT_NAND){
		//printf("[%s, line %d]\n\r",__FUNCTION__,__LINE__);
		if((nandflashGet(pVol, &manCode, &devCode) == 0))
			;
	}else
#endif	
	{

//	}else{
//end ccwei		
		//printf("[%s, line %d]\n\r",__FUNCTION__,__LINE__);
//ccwei 111107
#ifdef CONFIG_SPI_FLASH
		if ((spiflashIdGet(pVol, &manCode, &devCode) == 0) )
			flashIdGet(pVol, &manCode, &devCode);
#else
        printf("Error: you must define CONFIG_SPI_FLASH in menuconfig!\n\r");
#endif
//end
	}
#endif
    //flashIdGet(pVol, &manCode, &devCode);

	DEBUG_PRINT(DEBUG_ID,
                    ("amd29lvMTDIdentify Manufacturer: 0x%02x\n\r",
                    manCode));
	DEBUG_PRINT(DEBUG_ID,
                    ("amd29lvMTDIdentify Device: 0x%02x\n\r",
                    devCode));

#ifdef UNUSE
    if (devCode == 0x22DA || 	/* amd29LV800BT */
    	devCode == 0x225B)		/* amd29LV800BB */
	{
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = AMD29LV_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = AMD29LV_800_CHIP_SIZE - AMD29LV_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = AMD29LV_800_CHIP_SIZE;
		#endif
		pVol->noOfChips = AMD29LV_MTD_CHIP_CNT;
		pVol->interleaving = AMD29LV_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;
	}
	else
#endif	
#ifdef CONFIG_SPI_FLASH
    if ((devCode == 0x20)|| (devCode ==0x02) || (0x31 == devCode) || (0x30 == devCode)
	|| (0x46 == devCode) || (0x25 == devCode) || (0x40 == devCode))		/* SPI Flash */
    {
    	 ; // already set in spiflashIdGet
    }
    else
#endif
//ccwei
//#ifdef CONFIG_NAND_FLASH
//    if(devCode == 0xDA)
//	  if((devCode == 0xDA) || (devCode == 0xF1) || (devCode == 0x75))
	if((devCode == 0xDA) || (devCode == 0xF1) || (devCode == 0x75) || (devCode == 0xD3)
		|| (devCode == 0xF0)|| (devCode == 0xAB21)|| (devCode == 0xAA21) || (devCode == 0xD2))
    	{
		; //already set in nandflashGet()
	}
    else
//ccwei		
//#endif
    if (devCode == 0x22C4 || 	/* amd29LV160BT */
    	devCode == 0x2249)		/* amd29LV160BB */
	{
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = AMD29LV_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = AMD29LV_160_CHIP_SIZE - AMD29LV_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = AMD29LV_160_CHIP_SIZE;
		#endif
		pVol->noOfChips = AMD29LV_MTD_CHIP_CNT;
		pVol->interleaving = AMD29LV_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;
	}
	else
    if (devCode == 0x22A7 || 	/* amd29LV320BT */
    	devCode == 0x22A8 ||		/* amd29LV320BB */
    	devCode == 0x22F6 || 	/* amd29LV320D/BT */
    	devCode == 0x22F9)	/* amd29LV320D/BB */
	{
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = AMD29LV_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = AMD29LV_320_CHIP_SIZE - AMD29LV_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = AMD29LV_320_CHIP_SIZE;
		#endif
		pVol->noOfChips = AMD29LV_MTD_CHIP_CNT;
		pVol->interleaving = AMD29LV_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;
	}
    else
    if (devCode == 0x227E  		/* amd29LV640BT */
    	||devCode == 0x22CB			/* mx29lv640 bb */
    	||devCode == 0x227A)		/*mx29lv1280*/
	{
#ifdef CONFIG_SPANSION_16M_FLASH
		if ((DeviceID_cycle2 ==0x2221) && (DeviceID_cycle3 == 0x2201))
		{
			pVol->type = (manCode<<8)| (devCode&0xff);
			pVol->erasableBlockSize = S29GL128P_SECTOR_SIZE; // 128K
			pVol->chipSize = S29GL128P_CHIP_SIZE;			  //16MB
			pVol->noOfChips = 1 ;// 
			pVol->interleaving = 1;//
			pVol->write = s29gl128Program;
			pVol->erase = s29gl128SectorRangeErase;
			pVol->map = amd29lvMap;
		}
		else 
		{
#endif
/*alex_huang support amd 29lv 128 chip 20090909 */
			if(devCode == 0x227A)
			{
				pVol->type = (manCode<<8)| (devCode&0xff);
				pVol->erasableBlockSize = AMD29LV_1280_MTD_SECTOR_SIZE;
				pVol->chipSize = AMD29LV_128_CHIP_SIZE;
				pVol->noOfChips = AMD29LV_MTD_CHIP_CNT;
				pVol->interleaving = AMD29LV_MTD_INTERLEAVE;
				pVol->write = amd29lvProgram;
				pVol->erase = amd29lvSectorRangeErase;
				pVol->map = amd29lvMap;
			}
/*alex_huang end*/			
			else{
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = AMD29LV_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = AMD29LV_640_CHIP_SIZE - AMD29LV_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = AMD29LV_640_CHIP_SIZE;
		#endif
		pVol->noOfChips = AMD29LV_MTD_CHIP_CNT;
		pVol->interleaving = AMD29LV_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;
			}
		
#ifdef CONFIG_SPANSION_16M_FLASH
		}
#endif 
	}
    else
    	
#if FLASH_SST_SUPPORT
	if (0x2780 == devCode) { /* SST39VF400 */
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = SST39VF_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = SST39VF_400_CHIP_SIZE - SST39VF_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = SST39VF_400_CHIP_SIZE;
		#endif
		pVol->noOfChips = SST39VF_MTD_CHIP_CNT;
		pVol->interleaving = SST39VF_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;		
	} else
	if (0x2781 == devCode) { /* SST39VF800 */
		pVol->type = (manCode<<8)| (devCode&0xff);
		pVol->erasableBlockSize = SST39VF_MTD_SECTOR_SIZE;
		#ifdef SAVE_NVRAM_REGION
		pVol->chipSize = SST39VF_800_CHIP_SIZE - SST39VF_MTD_SECTOR_SIZE;
		#else
		pVol->chipSize = SST39VF_800_CHIP_SIZE;
		#endif
		pVol->noOfChips = SST39VF_MTD_CHIP_CNT;
		pVol->interleaving = SST39VF_MTD_INTERLEAVE;
		pVol->write = amd29lvProgram;
		pVol->erase = amd29lvSectorRangeErase;
		pVol->map = amd29lvMap;
	} else
	if (0x234B == devCode) {
#ifdef CONFIG_SPANSION_16M_FLASH
		if ((DeviceID_cycle2 ==0x2221) && (DeviceID_cycle3 == 0x2201))
		{
			pVol->type = (manCode<<8)| (devCode&0xff);
			pVol->erasableBlockSize = S29GL128P_SECTOR_SIZE; // 128K
			pVol->chipSize = S29GL128P_CHIP_SIZE;			  //16MB
			pVol->noOfChips = 1 ;// 
			pVol->interleaving = 1;//
			pVol->write = s29gl128Program;
			pVol->erase = s29gl128SectorRangeErase;
			pVol->map = amd29lvMap;
		}
		else 
		{
#else 	
			pVol->type = (manCode<<8)| (devCode&0xff);
			pVol->erasableBlockSize = SST39VF_MTD_SECTOR_SIZE;
			#ifdef SAVE_NVRAM_REGION
			pVol->chipSize = SST39VF_400_CHIP_SIZE - SST39VF_MTD_SECTOR_SIZE;
			#else
			pVol->chipSize = SST39VF_400_CHIP_SIZE;
			#endif
			pVol->noOfChips = SST39VF_MTD_CHIP_CNT;
			pVol->interleaving = SST39VF_MTD_INTERLEAVE;
			pVol->write = amd29lvProgram;
			pVol->erase = amd29lvSectorRangeErase;
			pVol->map = amd29lvMap;
#endif
#ifdef CONFIG_SPANSION_16M_FLASH
			}
#endif 
	} else
#endif
	{
        DEBUG_PRINT(DEBUG_ALWAYS,
                    ("amd29lvMTDIdentify Device unknown: 0x%02x\n\r",
                    devCode));
        return(ERROR);
	}    

    DEBUG_PRINT(DEBUG_ID, ("amd29lvMTDIdentify succeeds!\n\r"));
    return(flOK);
}


/******************************************************************************
* flashCheck16Bits - Wait for a flash operation to complete and determine the result
*
* Result:              0 = operation completed successfully
*                      1 = timeout during operation
*                      2 = error detected
*/

static STATUS flashCheck16Bits(FLFlash* pVol, UINT32 addr)
{
    volatile UINT8      initialData;
    volatile UINT8      nextData;
	int start = get_sys_time();    

    /* Read the 'initial' value of the status register */
    initialData = flashRegRead16Bits(pVol, addr);

    while(!timeout(start, 30000))
    {
        nextData = flashRegRead16Bits(pVol, addr);

        /* See if the toggle bit toggled. 
         * If it has not then the operation has completed successfully
         */
        if ((initialData & (1<<6)) == (nextData & (1<<6)))
        {
            /* D6 has stopped toggling Operation complete */
            return 0;
        }

        /* If still toggling then check D5 to see if an error was detected */
        if (initialData & (1<<5))
        {
            volatile UINT8 value1 = flashRegRead16Bits(pVol, addr);
            volatile UINT8 value2 = flashRegRead16Bits(pVol, addr);

            if ((value1 & (1<<6)) == (value2 & (1<<6)))
            {
                /* DQ6 is not toggling, so ignore the error bit */
                return 0;
            }
            else
            {
                /* DQ6 is still toggling so return the error */
				if(value2 & (1<<2))
					return 0;
            }
        }

        /* Update the initial data for the next pass */
        initialData = nextData;
        //delay_msec(1);
    }

    /* Operation timed out before completion */
    return 1;
}


/******************************************************************************
*
* amd29lvProgram - MTD write routine (see TrueFFS Programmer's Guide)
*
* RETURNS: FLStatus
*
*/

LOCAL FLStatus amd29lvProgram(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite)
{
    volatile UINT32* pFlash;
    UINT8* pBuffer;
    UINT16 data;
    STATUS rc = OK;
    int i;
#ifdef BANK_AUTO_SWITCH
    int oldbank;
#endif
    DEBUG_PRINT(DEBUG_PROGRAM,
                ("Program: 0x%08x, 0x%08x, %d\n\r", (unsigned int) address,
                 length, overwrite));

    /* Check alignment */
	pBuffer = (UINT8*)buffer;

#if 0
	DEBUG_PRINT(DEBUG_PROGRAM,("\n\r"));
	for(i=0;i<length;i++)
		DEBUG_PRINT(DEBUG_PROGRAM, ("%02X", pBuffer[i]));
	DEBUG_PRINT(DEBUG_PROGRAM, ("\n\r"));
#endif

    if ((address & 0x01) != 0)
    {
    	address--;
    	pFlash = (volatile UINT32*) pVol->map(pVol, address, 0);
    	data = (*pFlash&0xff00) | (*pBuffer&0xff);
    	flashProgram16Bits(pVol, pFlash, data);
    	pBuffer++;
    	address+=2;
    	length--;
	}
	
    /* Program 'length' bytes (2 bytes each iterations) */
    for (i = 0; i < length; i+=2)
	{
		data = *(UINT16*)&pBuffer[i];
        /* Don't bother programming if buffer data == format value */
        if (data == 0xffff)
            continue;

		pFlash = (volatile UINT32*) pVol->map(pVol, address+i, 0);
        /* Program 16 bits */
        rc = flashProgram16Bits(pVol, pFlash, data);
        if (rc != OK)
            return ERROR;
        if (0==(i&0x0FFFF)) {
	            printf(".");
			    DEBUG_PRINT(DEBUG_PROGRAM, (" \r\n[%x]",i));
			
				
        };
	}

	//The last byte was already written.
#if 0 	
	if ((length & 0x01) != 0)
	{
		int currbank;
		data = pBuffer[length-1];
		data = data << 8;
    	pFlash = (volatile UINT32*) pVol->map(pVol, address+length-1, 0);
#if defined(BANK_AUTO_SWITCH) && defined(CONFIG_SPANSION_16M_FLASH)
		if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
		{
			unsigned int offset;
			offset= (unsigned int)pFlash - FLASH_BASE_ADDR;
			oldbank=getbank();
			if(offset < 0x400000)
				s29gl128bank(0);
			else if(offset < 0x800000)
				s29gl128bank(1);
			else if(offset < 0xC00000)
				s29gl128bank(2);
			else if(offset < 0x1000000)
				s29gl128bank(3);
		}
		currbank = getbank();
		if (currbank == 0)
    		data = data | (*(volatile UINT16*)pFlash  & 0xff);
		else if (currbank == 1)
			data = data | (*((volatile UINT16*) ((unsigned int)pFlash-0x400000)) & 0xff);
#else
    		data = data | (*(volatile UINT16*)pFlash  & 0xff);
#endif
#if defined(BANK_AUTO_SWITCH) && defined(CONFIG_SPANSION_16M_FLASH)
	if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	{
			s29gl128bank(oldbank);
		
	}
#endif
			printf("\r\nLast!!! pflash :%x data:%x", pFlash, data);		
        rc = flashProgram16Bits(pVol, pFlash, data);

		
        if (rc != OK)
            return(flTimedOut);
	}
#endif	
    printf("\n\r");

    return(flOK);

}

/******************************************************************************
*
* amd29lvSectorRangeErase - MTD erase routine (see TrueFFS Programmer's Guide)
*
* RETURNS: FLStatus
*
*/

LOCAL FLStatus amd29lvSectorRangeErase(FLFlash* pVol, int sectorNum, int sectorCount)
{
    int i;
    STATUS rc;

    /* Check for valid range */

#ifdef UNUSE
    if ((pVol->type&0xff) == 0xDA ||			/* amd29LV800BT */
    	(pVol->type&0xff) == 0x5B)				/* amd29LV800BB */
	{
		if (sectorNum + sectorCount >  AMD29LV_800_LAST_SECTOR_NUM + 1)
	    {
	    DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r",
	                sectorNum, sectorCount));
			return (flSectorNotFound);
	    }
	}
#endif	

    if ((pVol->type&0xff) == 0xC4 ||			/* amd29LV160BT */
    	(pVol->type&0xff) == 0x49)				/* amd29LV160BB */
	{
		if (sectorNum + sectorCount >  AMD29LV_160_LAST_SECTOR_NUM + 1)
	    {
	    DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r",
	                sectorNum, sectorCount));
			return (flSectorNotFound);
	    }
	}

    if ((pVol->type&0xff) == 0xA7 ||			/* amd29LV320BT */
    	(pVol->type&0xff) == 0xA8 ||			/* amd29LV320BB */
    	(pVol->type&0xff) == 0xF6 ||			/* amd29LV320D/BT */
    	(pVol->type&0xff) == 0xF9 )			/* amd29LV320D/BB */
	{
		if (sectorNum + sectorCount >  AMD29LV_320_LAST_SECTOR_NUM + 1)
	    {
	    DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r",
	                sectorNum, sectorCount));
			return (flSectorNotFound);
	    }
	}
//support mx29lv640 mt/b 
    if( (pVol->type&0xff) == 0x7E			/* mx29lv640 mt/b */
    || (pVol->type&0xff) == 0xCB)			/* mx29lv640 bb */	
	{
	
#ifdef 	CONFIG_SPANSION_16M_FLASH
		if (DeviceID_cycle2 == 0x2221) /* s29gl128p*/
		{			
			if (sectorNum + sectorCount >  S29GL128P_LAST_SECTOR_NUM + 1)
			{
				DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r",
					sectorNum, sectorCount));
				return (flSectorNotFound);
			}
		}
		else 
#endif 
		if (sectorNum + sectorCount >  AMD29LV_640_LAST_SECTOR_NUM + 1)
	    {
	    DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r",
	                sectorNum, sectorCount));
			return (flSectorNotFound);
	    }
	}
//support mx29lv640 mt/b
#if FLASH_SST_SUPPORT
    if ((pVol->type&0xff) == 0x80)			/* sst39vf400 */
    {
    	 if (sectorNum + sectorCount >  SST39VF_400_LAST_SECTOR_NUM + 1)
	 {
	     DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r", sectorNum, sectorCount));
	     return (flSectorNotFound);
	  } 	 
    }

    if ((pVol->type&0xff) == 0x81)			/* sst39vf800 */
    {
    	 if (sectorNum + sectorCount >  SST39VF_800_LAST_SECTOR_NUM + 1)
	 {
	     DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r", sectorNum, sectorCount));
	     return (flSectorNotFound);
	  } 	 
    }

     if ((pVol->type&0xff) == 0x4b)			/* sst39vf1601 */
    {
    	 if (sectorNum + sectorCount >  SST39VF_1601_LAST_SECTOR_NUM + 1)
	 {
	     DEBUG_PRINT(DEBUG_ALWAYS, ("Invalid sector range: %d - %d\n\r", sectorNum, sectorCount));
	     return (flSectorNotFound);
	  } 	 
    }

#endif

	/* Last sector is really 8 seperately erasable sectors */
    for (i = 0; i < sectorCount; i++)
	{
        /* Erase lower half */
        rc = flashSectorErase(pVol, sectorNum + i);
        if (rc != OK)
            return(flTimedOut);
        printf(".");
	}
    return(flOK);
}


/******************************************************************************
*
* amd29lvMap - MTD map routine (see TrueFFS Programmer's Guide)
*
* RETURNS: FLStatus
*
*/
LOCAL void * amd29lvMap(FLFlash* pVol, CardAddress address, int length)
{
    UINT32 flashBaseAddr = (pVol->baseAddress << 12);
    void * pFlash = (void *) (flashBaseAddr + address);
    DEBUG_PRINT(DEBUG_MAP, ("Mapping 0x%08x bytes at 0x%08x to %p\n\r", length,
                (unsigned int) address, pFlash));

    return(pFlash);
}

#ifdef CONFIG_SPANSION_16M_FLASH
/******************************************************************************
*
* s29gl128SectorRangeErase - MTD erase routine 
*						 - This function do not erase the block 0
*
* RETURNS: FLStatus
*
*/

LOCAL FLStatus s29gl128SectorRangeErase(FLFlash* pVol, int start, int sectorCount)
{
	if (start == 0){	
		start ++;
		sectorCount --;
	}
	if (sectorCount == 0)
		return(flOK);
	else 
		return amd29lvSectorRangeErase(pVol, start, sectorCount);
}

/******************************************************************************
*
*  - MTD 128K flash program routine 
*
* RETURNS: FLStatus
*
*/
LOCAL FLStatus s29gl128Program(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite)
{
	char *sector_buf;
	char *ptr;
	int ret, len;
#ifdef BANK_AUTO_SWITCH
	int oldbank=getbank();
#endif
	ptr = buffer;
	if (address < S29GL128P_SECTOR_SIZE)
	{
		sector_buf = malloc(S29GL128P_SECTOR_SIZE);
		if (!sector_buf)
			return ERROR;
			
		/* Read 1st sector*/
		#ifdef BANK_AUTO_SWITCH
			s29gl128bank(0);
		#endif
		memcpy(sector_buf, pVol->map(pVol, 0, 0), S29GL128P_SECTOR_SIZE);
		
		len = S29GL128P_SECTOR_SIZE - address;
		if (len > length)
			len = length;
		if (address == 0) // bootloader, reset as 0xff
			memset(sector_buf, 0xff, AMD29LV_MTD_SECTOR_SIZE);
		
		memcpy(sector_buf+address, buffer, len);
		/* Erase the 1st sector*/
		amd29lvSectorRangeErase(pVol, 0 , 1);
		/* write the whole sector*/
		amd29lvProgram(pVol, 0, sector_buf ,S29GL128P_SECTOR_SIZE, overwrite);
		length = length -len ;	
		ptr = (char*) buffer + len;
		address = S29GL128P_SECTOR_SIZE;
		free(sector_buf);
	}
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
	if (length > 0)
		return amd29lvProgram(pVol, address, ptr, length, overwrite);

	return(flOK);
}
int fbank  = 0;

int getbank()
{
	return fbank;
}
/******************************************************************************
*
*  - MTD 128K flash bank selection routine 
*
*/
void s29gl128bank(int x)
{
/*alex_huang support 16M direct address flash 20090909*/
#ifndef SUPPORT_DIRECT_16M_ADDR
	unsigned int	val;

	fbank = x;
	//configure GPA5 GPA6 as GPIO output
	val = *(volatile int *)0xb8003508;
	*(volatile int *)0xb8003508 =  val|0x00000060; 

	val = *(volatile int *)0xb800350c; 
	switch (x){
		case 1:
			*(volatile int *)0xb800350c |= (0x1 <<5);
			*(volatile int *)0xb800350c &= ~(0x1 <<6);
			break;
		case 2:
			*(volatile int *)0xb800350c &= ~(0x1 <<5);
			*(volatile int *)0xb800350c |=  (0x1 <<6);
			break;
		case 3:
			*(volatile int *)0xb800350c |=  (0x1 <<5);
			*(volatile int *)0xb800350c |=  (0x1 <<6);
			break;
		case 0:
		default:
			*(volatile int *)0xb800350c &= ~(0x1 <<5);
			*(volatile int *)0xb800350c &= ~(0x1 <<6);
	}
#endif	
/*alex_huang end*/
}
#if 0
/******************************************************************************
*  Utility function to destroy image head when decompress fails
*
*
*/
void destory_image_at(int src)
{
	FLFlash nvVol;
	int tmp = 0;

	nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
	nvVol.map = amd29lvMap;
	if (amd29lvMTDIdentify(&nvVol) == flOK)
	{	
		printf(" Destroy image at [%x]!!", src);
		nvVol.write(&nvVol, src-FLASH_BASE_ADDR, &tmp, 4, 0);
	}
}
#endif
#endif

#ifdef CONFIG_SPI_FLASH
#include "spi_flash.h"
extern  struct spi_flash_type   spi_flash_info;
int spiflashIdGet(FLFlash* pVol, UINT16* manCode, UINT16* devCode)
{
	extern unsigned int spiflash;
	memset(&spi_flash_info,0, sizeof(struct spi_flash_type));
	spi_probe();

	*devCode  = 0;
	if (spi_flash_info.device_size != 0)
	{
		spiflash = 1;
		*manCode = spi_flash_info.maker_id;
		*devCode = spi_flash_info.type_id;
		pVol->type = (*manCode<<8)| (*devCode&0xff);
		//for spansion flash, its sector size is 64K-byte, so I will split the sector into 4K-byte sub-sector.
		pVol->erasableBlockSize = AMD29LV_MTD_SECTOR_SIZE; // default set to 4KB
		pVol->chipSize = (1 <<spi_flash_info.device_size);
		pVol->noOfChips = 1;
		pVol->interleaving = 1;
		pVol->write = spiProgram;
		pVol->erase = spiSectorRangeErase;
		pVol->map = spiMap;
		return 1;
	}
	
	spiflash = 0;
	return 0;
}
LOCAL FLStatus spiSectorRangeErase(FLFlash* pVol, int start, int sectorCount)
{
	int idx;
	
	if (start == 0){
		start ++;
		sectorCount --;
	}
	if (sectorCount == 0)
		return(flOK);

	if (((pVol->type>>8)&0xff) == 0x1F)
	{//atmel
		spi_global_unprotect(0x0C);
	}
/*ramen code for 16M spi flash*/	
	if (((pVol->type>>8)&0xff) == 0xC2)
	{//mxic
		spi_global_unprotect(0x3C);
	}
	else {
		spi_global_unprotect(0x1C);
	}

	for (idx = start; idx<(start+sectorCount); idx++)
	{
		printf(".");
		spi_erase_block(idx);
	}

	return(flOK);
}

LOCAL FLStatus spi4KSectorRangeErase(FLFlash* pVol, int start, int sectorCount)
{
	char *sector_buf;
	int idx;

	if (((pVol->type>>8)&0xff) == 0x1F)
	{//atmel
		spi_global_unprotect(0x0C);
	}
	else if (((pVol->type>>8)&0xff) == 0xC2)
	{//mxic
		spi_global_unprotect(0x3C);
	}
	else {
		spi_global_unprotect(0x1C);
	}

	//for spansion flash, its sector size is 64K-byte, so I will split the first sector into 4K-byte sub-sector.
	if (pVol->type == 0x0102) {
		if (start < 16)//first block(64K)
		{
			int sectorCntTmp;
			sector_buf = malloc(0x10000);
			if (!sector_buf) {
				printf("malloc fail\n");
				return ERROR;
			}
			
			/* Read 1st block*/
			memcpy(sector_buf, pVol->map(pVol, 0, 0),  0x10000);

			sectorCntTmp = ((16-start)>sectorCount) ? sectorCount : (16-start);
			memset(sector_buf+(start<<12), 0xff, sectorCntTmp<<12);

			spi_erase_block(0);
			spi_program_by_page(0, sector_buf, 0x10000);

			sectorCount -= sectorCntTmp;
			start = 16;
		}
	}
	if (start == 0){
		start ++;
		sectorCount --;
	}
	if (sectorCount == 0)
		return(flOK);
	
	if (pVol->type == 0x0102) {
		//merge 4K sector into 64K-byte size.
		sectorCount = ((start+sectorCount)>>4) + (((start+sectorCount) & 0xf) ? 1: 0) - (start>>4);
		start = (start>>4);
		for (idx=start; idx<(start+sectorCount); idx++)
		{
			printf(".");
			spi_erase_block(idx);
		}
	}
	else {
		for (idx = start; idx<(start+sectorCount); idx++)
		{
			printf(".");
			spi_erase_sector(idx);
		}
	}
	return(flOK);
}


LOCAL FLStatus spiProgram(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite)
{
	char *sector_buf;
	char *ptr;
	int len;
	
	//printf("\r\nspiProgram : address %x, buffer:%x, length :%d ",address, buffer,length);
	
	ptr = buffer;
	if (((pVol->type>>8)&0xff) == 0x1F)
	{//atmel
		spi_global_unprotect(0x0C);
	}
	else {
		spi_global_unprotect(0x1C);
	}
	
	if (address < 0x10000) //means in 1st block
	{
		sector_buf = malloc(0x10000);
		if (!sector_buf) {
			printf("malloc fail\n");
			return ERROR;
		}
		
		/* Read 1st sector*/
		memcpy(sector_buf, pVol->map(pVol, 0, 0),  0x10000);
		
		len = 0x10000 - address;
		if (len > length)
			len = length;
		if (address == 0) // bootloader, reset as 0xff
			memset(sector_buf, 0xff, 0x10000);
		
		memcpy(sector_buf+address, buffer, len);
		/* Erase the 1st sector*/
		spi_erase_block(0);
		/* write the whole sector*/
		switch((pVol->type>>8)&0xff)
		{
			case 0xBF://sst25VF016B-50-4c-s2AF
			case 0x8C://esmt
				spi_program_by_byte(0,sector_buf, 0x10000);
				break;
			default:
				spi_program_by_page(0,sector_buf, 0x10000);
				break;
		}
		length = length -len ;
		ptr = (char*) buffer + len;
		address = 0x10000;
		free(sector_buf);
	}
	if (length > 0) {
		//spi_program_image(0, address , ptr ,length);
		switch((pVol->type>>8)&0xff)
		{
			case 0xBF://sst25VF016B-50-4c-s2AF
			case 0x8C://esmt
				spi_program_by_byte(address , ptr ,length);
				break;
			default:
				spi_program_by_page(address , ptr ,length);
				break;
		}
	}
	return(flOK);
}

LOCAL void * spiMap(FLFlash* pVol, CardAddress address, int length)
{

   void * pFlash = (void *) (FLASH_BASE_ADDR + address ) ;
    DEBUG_PRINT(DEBUG_MAP, ("Mapping 0x%08x bytes at 0x%08x to %p\n\r", length,
                (unsigned int) address, pFlash));
    return(pFlash);
}
#endif //end CONFIG_SPI_FLASH

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
#include "rtk_nand.h"
extern  struct device_type  nand_info;
//extern int ppb, page_size;
int nandflashGet(FLFlash* pVol, UINT16* manCode, UINT16* devCode)
{
    DEBUG_PRINT(DEBUG_ALWAYS,("nandflashGet in\n\r"));	


	*devCode  = 0;
	if (nand_dev_info.size != 0)
	{
#ifdef CONFIG_SPI_NAND_FLASH
		*manCode = nand_dev_info.MAN_ID;//(nand_dev_info.id >> 8) & 0xff;
		*devCode = nand_dev_info.DEV_ID;//(nand_dev_info.id) & 0xff;
//		pVol->type = (*manCode<<8)| (*devCode&0xff);

#else
		*manCode = (nand_dev_info.id >> 24) & 0xff;
		*devCode =  (nand_dev_info.id >> 16) & 0xff;
		pVol->type = (*manCode<<8)| (*devCode&0xff);

#endif	/*CONFIG_SPI_NAND_FLASH*/	
		//for spansion flash, its sector size is 64K-byte, so I will split the sector into 4K-byte sub-sector.
		pVol->erasableBlockSize = nand_dev_info.BlockSize; // default set to 4KB
		pVol->chipSize = nand_dev_info.chipsize;
		pVol->noOfChips = nand_dev_info.num_chips;
		pVol->interleaving = 1;
		pVol->write = NandProgram;
		pVol->erase = NandSectorRangeErase;
		pVol->map = NandMap;
		return 1;
	}
    DEBUG_PRINT(DEBUG_ALWAYS,("nandflashGet out\n\r"));	
	return 0;
}

LOCAL FLStatus NandSectorRangeErase(FLFlash* pVol, int start, int sectorCount)
{
	int idx,i;
	
	if (start == 0){
		start ++;
		sectorCount --;
	}
	if (sectorCount == 0)
		return(flOK);

	for (idx = start; idx<(start+sectorCount); idx++)
	{
		//if(ERROR==rtk_erase_block(idx*ppb)){
			//ccwei: 120228
		if(ERROR==rtk_nand_base_info._erase_block(idx*ppb)){
#ifdef CONFIG_SPI_NAND_FLASH
			NfSpareBuf[0] = 0x00;
#else
            if(!NAND_ADDR_CYCLE)
				NfSpareBuf[OOB_BBI_OFF] = 0x00;    // set tag = 0xff in the 1st page of each block
			else
			    NfSpareBuf[5] = 0x00;	   // set tag = 0xff in the 1st page of each block					
#endif /*CONFIG_SPI_NAND_FLASH*/			    
            if(isLastPage){
			    for(i=1;i<3;i++) //each block have "ppb" pages.
			    	rtk_nand_base_info._nand_write_page_ecc((idx*ppb)+(ppb-i), NfDataBuf, NfSpareBuf, page_size);
					#if 0
			        if(rtk_write_ecc_page ( (idx*ppb)+(ppb-i), data_buf, sector_buf, page_size)){
				        printf("%s:Write nand flash page error! %d\n",__FUNCTION__, idx*ppb+(ppb-i));
		 	            //return ERROR;					
			        }
					#endif
			}else{
			    for(i=0;i<2;i++) //each block have "ppb" pages.
			    	rtk_nand_base_info._nand_write_page_ecc((idx*ppb)+i, NfDataBuf, NfSpareBuf, page_size);
					#if 0
			        if(rtk_write_ecc_page ( (idx*ppb)+i, data_buf, sector_buf, page_size)){
				        printf("%s:Write nand flash page error! %d\n",__FUNCTION__, idx*ppb+i);
		 	            //return ERROR;					
			        }
					#endif
			}

			printf("%s:erase nand flash block error! %d\n",__FUNCTION__, idx);
            //end				
			return ERROR;
		}
		
		//printf("%");
	}

	return(flOK);
}

LOCAL FLStatus NandProgram(FLFlash *pVol, CardAddress address, 
		const void *  buffer, int length, FLBoolean overwrite)
{
	//char *sector_buf;
	char *ptr;
	int ret, len, i, rc;

    unsigned int start_page=0;
	//unsigned char *tmp_buf;
	
	//printf("[%s ] address 0x%08X, buffer:0x%08x, length :%d \n\r",__func__,address, buffer,length);
	
	ptr = buffer;
		

		/* write the whole sector*/
		//sector_buf = (unsigned char *)malloc(oob_size);
		memset(NfSpareBuf,0xff,oob_size);
		//tmp_buf = (unsigned char *)malloc(page_size);
		start_page = (address/(pVol->erasableBlockSize))*ppb;

        if( start_page ==0){
			//if(rtk_erase_block(0)){
			if(rtk_nand_base_info._erase_block(0)){
                printf("Erase block 0 error\n");
#ifdef CONFIG_SPI_NAND_FLASH
				NfSpareBuf[0] = 0x00;	   // marked as bad block
#else
				if(!NAND_ADDR_CYCLE)
					NfSpareBuf[OOB_BBI_OFF] = 0x00;	   // marked as bad block					
				else
					NfSpareBuf[5] = 0x00;	   // marked as bad block
#endif					
				if(isLastPage){
					for(i=1;i<3;i++) 
						rtk_nand_base_info._nand_write_page_ecc(start_page+(ppb-i), ptr+((ppb-i)*page_size), NfSpareBuf, page_size);
					#if 0
					if(rtk_write_ecc_page (start_page+(ppb-i), ptr+((ppb-i)*page_size), sector_buf, page_size)){
						printf("%s:Write nand flash page error! %d\n",__FUNCTION__, start_page+(ppb-i));
						//return ERROR;					
					}
					#endif
				}else{
				    for(i=0;i<2;i++) 
						rtk_nand_base_info._nand_write_page_ecc(start_page+i, ptr+(i*page_size), NfSpareBuf, page_size);
					#if 0
				    if(rtk_write_ecc_page (start_page+i, ptr+(i*page_size), sector_buf, page_size)){
						printf("%s:Write nand flash page error! %d\n",__FUNCTION__, start_page+i);
						//return ERROR;					
					}
					#endif
				}					
				//end				
				return ERROR;
			}
//printf("[%s, %d] address = 0x%08X, buffer = 0x%08X\n\r",__FUNCTION__,__LINE__,address, (unsigned int)buffer);
			for(i=0;i<ppb;i++) //each block have "ppb" pages.
				rtk_nand_base_info._nand_write_page_ecc(start_page+i, ptr+(i*page_size), NfSpareBuf, page_size);
#if 0
			    if(rtk_write_ecc_page ( start_page+i, ptr+(i*page_size), NfSpareBuf, page_size)){
                    int k=0;
			 	    printf("%s:Write nand flash page error! %d\n",__FUNCTION__, start_page+i);
					if(!NAND_ADDR_CYCLE)
						NfSpareBuf[OOB_BBI_OFF] = 0x00;  // marked as bad block
					else
						NfSpareBuf[5] = 0x00;  // marked as bad block
					if(isLastPage){
					    for(k=1;k<3;k++) 
					        if(rtk_write_ecc_page (start_page+ppb-k, ptr+((ppb-k)*page_size), NfSpareBuf, page_size)){
						        printf("Write nand flash page error! %d\n", start_page+ppb-k);
					        }

					}else{	
					    for(k=0;k<2;k++) 
					        if(rtk_write_ecc_page (start_page+k, ptr+(k*page_size), NfSpareBuf, page_size)){
						        printf("Write nand flash page error! %d\n", start_page+k);
					        }
					}
			 	    return ERROR;
			    }
#endif				
			//printf("\n\r");
			
		}else{
			//printf("[%s, %d] address = 0x%08X, buffer = 0x%08X\n\r",__FUNCTION__,__LINE__,address, (unsigned int)buffer);
                //ccwei 120113
			    //if(rtk_erase_block(start_page)){
			    if(rtk_nand_base_info._erase_block(start_page)){
					int k=0;
					printf("%s: Erase block %d error\n",__FUNCTION__, (address/(pVol->erasableBlockSize)));
                    //ccwei: 120228
#ifdef CONFIG_SPI_NAND_FLASH
					NfSpareBuf[0] = 0x00;    // set tag 
#else
					if(!NAND_ADDR_CYCLE)
						NfSpareBuf[OOB_BBI_OFF] = 0x00;	   // set tag 
					else
						NfSpareBuf[5] = 0x00;	   // set tag 
#endif	/*CONFIG_SPI_NAND_FLASH*/					
				    if(isLastPage){
					    for(k=1;k<3;k++) 
							rtk_nand_base_info._nand_write_page_ecc(start_page+ppb-k, ptr+((ppb-k)*page_size), NfSpareBuf, page_size);
							#if 0
					        if(rtk_write_ecc_page (start_page+ppb-k, ptr+((ppb-k)*page_size), sector_buf, page_size)){
						        printf("Write nand flash page error! %d\n", start_page+ppb-k);
					        }
							#endif

					}else{
					    for(k=0;k<2;k++) 
							rtk_nand_base_info._nand_write_page_ecc(start_page+k, ptr+(k*page_size), NfSpareBuf, page_size);
							#if 0
						    if(rtk_write_ecc_page (start_page+k, ptr+(k*page_size), sector_buf, page_size)){
							    printf("%s:Write nand flash page error! %d\n",__FUNCTION__, start_page+k);
						    }
							#endif
					}

					return ERROR;
				}

				for(i=0;i<ppb;i++) //each block have "ppb" pages.
					rtk_nand_base_info._nand_write_page_ecc(start_page+i, ptr+(i*page_size), NfSpareBuf, page_size);
#if 0
				    if(rtk_write_ecc_page ( start_page+i, ptr+(i*page_size), NfSpareBuf, page_size)){
                        int k=0;
						printf("Write nand flash page error! %d\n", start_page);
					    if(!NAND_ADDR_CYCLE)
						    NfSpareBuf[OOB_BBI_OFF] = 0x00;	   // set tag 
					    else
						    NfSpareBuf[5] = 0x00;  // marked as bad block
						if(isLastPage){
							for(k=1;k<3;k++) 
								if(rtk_write_ecc_page (start_page+ppb-k, ptr+((ppb-k)*page_size), NfSpareBuf, page_size)){
									printf("Write nand flash page error! %d\n", start_page+ppb-k);
								}
						}else{
					        for(k=0;k<2;k++) 
						        if(rtk_write_ecc_page (start_page+k, ptr+(k*page_size), NfSpareBuf, page_size)){
							        printf("%s:Write nand flash page error! %d\n",__FUNCTION__, start_page+k);
						        }						
						}

			 	        return ERROR;					
				    }
#endif					
			//printf("\n\r");
		}

		//free(sector_buf);
		//free(tmp_buf);
		


	return(flOK);
}

LOCAL void * NandMap(FLFlash* pVol, CardAddress address, int length)
{

   void * pFlash = (void *) (FLASH_BASE_ADDR + address ) ;
    DEBUG_PRINT(DEBUG_MAP, ("Mapping 0x%08x bytes at 0x%08x to %p\n\r", length,
                (unsigned int) address, pFlash));
    return(pFlash);
}


#endif
/******************************************************************************
*
* flashProgram16Bits - Program 16 bits at 2 byte aligned address.
*
* RETURNS: OK or ERROR
*
*/

LOCAL inline STATUS flashProgram16Bits(FLFlash* pVol, volatile UINT32* pData, UINT16 data)
{
    int retry = 10000;
    unsigned int time0;
#if defined(BANK_AUTO_SWITCH) | defined(IMAGE_DOUBLE_BACKUP_SUPPORT)
	int currbank;
#endif
#ifdef BANK_AUTO_SWITCH
	unsigned int offset;
    int oldbank=getbank();
#endif
    DEBUG_PRINT(DEBUG_PROG16, ("Programming 0x%04x to %p\n\r",
                data, (void *)pData));

    for (retry = 0; retry < 3; retry ++) {
    flashReset(pVol);
    flashUnlock(pVol);
    flashRegWrite16Bits(pVol, 0x5555*2, 0xa0a0);
#ifdef BANK_AUTO_SWITCH
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))	
	{
		offset= (unsigned int)pData - FLASH_BASE_ADDR;
		if(offset <0x400000)
			s29gl128bank(0);
		else if(offset < 0x800000)
			s29gl128bank(1);
		else if(offset < 0xC00000)
			s29gl128bank(2);
		else if( offset < 0x1000000)
			s29gl128bank(3);
	}
	currbank = getbank();
	if (currbank == 0)
		*((volatile UINT16*) pData) = data;
	else if (currbank == 1)
		*((volatile UINT16*) ((unsigned int)pData - 0x400000)) = data;
	else if (currbank == 2)
		*((volatile UINT16*) ((unsigned int)pData - 0x800000)) = data;
	else if (currbank == 3)
		*((volatile UINT16*) ((unsigned int)pData - 0xC00000)) = data;
#else
	*((volatile UINT16*) pData) = data;
#endif	
    CACHE_PIPE_FLUSH();
    
    time0 = get_sys_time();
    while ((get_sys_time()-time0) < FLASH_TIMEOUT) {
        //result = flashCheck16Bits(pVol, 0);

#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT		
        if(((currbank == 0) && (*((volatile UINT16*) pData)==data)) ||
			((currbank == 1) && (*((volatile UINT16*) ((unsigned int)pData-0x400000))==data)) ||
			((currbank == 2) && (*((volatile UINT16*) ((unsigned int)pData-0x800000))==data)) ||
			((currbank == 3) && (*((volatile UINT16*) ((unsigned int)pData-0xC00000))==data)))
#else
        if(*(volatile UINT16*) pData==data) 				
#endif			
        {
    	   flashReset(pVol);
#ifdef BANK_AUTO_SWITCH
		//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
		if(flash_is_16MB(pVol))
			s29gl128bank(oldbank);
#endif
	   return(OK);
	}
    }

    }
#if 0
	int result, i;
	while(retry)
	{
		flashReset(pVol);
	    flashUnlock(pVol);
	    flashRegWrite16Bits(pVol, 0x5555*2, 0xa0a0);

		/* write data */
		*pData = data;
		CACHE_PIPE_FLUSH();
		
		result = flashCheck16Bits(pVol, 0);
	
		if(result==0)
		{
			for (i=0;i<0x600;i++) asm("	NOP");
		}

		if(*pData==data) 
		{
			flashReset(pVol);
			return(OK);
		}
		retry--;
	}	
#endif
	DEBUG_PRINT(DEBUG_ALWAYS, ("Program 0x%x Timeout %x %x\n\r", (unsigned int)pData, (unsigned int)*pData, data));
	flashReset(pVol);
#ifdef BANK_AUTO_SWITCH
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
		s29gl128bank(oldbank);
#endif
	return(ERROR);

}

/******************************************************************************
*
* flashSectorErase - Erase sector.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS flashSectorErase(FLFlash* pVol, int sectorNum)
{
    UINT32 offset;
    UINT32 size;
    volatile UINT16* pFlash;
    UINT32 sectorAddr;
    UINT32 sectorCnt;
    UINT32 sectorOffset[] = {0, 0x4000, 0x6000, 0x8000};
    UINT32 i;
#ifdef BANK_AUTO_SWITCH
	int oldbank=getbank();
#endif
	//e8b:  type:0x7e DeviceID_cycle2:0x2221
#ifdef UNUSE
    if ((pVol->type&0xff) == 0xDA)			/* amd29LV800BT */
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
		if(sectorNum == AMD29LV_800_LAST_SECTOR_NUM)
			sectorCnt = 4;
		else
			sectorCnt = 1;

	}
	else
#endif	
    if ((pVol->type&0xff) == 0xC4)			/* amd29LV160BT */
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
		if(sectorNum == AMD29LV_160_LAST_SECTOR_NUM)
			sectorCnt = 4;
		else
			sectorCnt = 1;

	}
	else
    if ((pVol->type&0xff) == 0xA7||			/* amd29LV320BT */
    	(pVol->type&0xff) == 0xF6)			/* amd29LV320D/BT */
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
		if(sectorNum == AMD29LV_320_LAST_SECTOR_NUM)
			sectorCnt = 8;
		else
			sectorCnt = 1;

	}
    else 
#ifdef UNUSE    
    if ((pVol->type&0xff) == 0x5B || 	/* amd29LV800BB */
    	(pVol->type&0xff) == 0x49) 		/* amd29LV160BB */
#else    	
    if ((pVol->type&0xff) == 0x49) 		/* amd29LV160BB */
#endif    	
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
		if(sectorNum == 0)
			sectorCnt = 4;
		else
			sectorCnt = 1;
	}
    else 
    if ((pVol->type&0xff) == 0xA8|| 		/* amd29LV320BB */
    	(pVol->type&0xff) == 0xF9)			/* amd29LV320D/BB */
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
		if(sectorNum == 0)
			sectorCnt = 8;
		else
			sectorCnt = 1;
	}

#ifdef CONFIG_SPANSION_16M_FLASH
   else if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	{
		offset = sectorNum * pVol->erasableBlockSize;
		size = pVol->erasableBlockSize;
		sectorAddr = offset;
		sectorCnt = 1;
#ifdef BANK_AUTO_SWITCH
		if(offset <0x400000)
			s29gl128bank(0);
		else if(offset < 0x800000)
			s29gl128bank(1);
		else if(offset < 0xC00000)
			s29gl128bank(2);
		else if( offset < 0x1000000)
			s29gl128bank(3);
#endif
	}
	
#endif	
//support mx29lv640m t/b  / mx29lv640bb
    else 
    if ((pVol->type&0xff) == 0x7E) 		/* mx29lv640m t/b */
	{
        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
        size = AMD29LV_MTD_SECTOR_SIZE;
        sectorAddr = offset;
            if(DeviceID_cycle3 == 0x2200)	//mx29lv640m bottom
            {
		if(sectorNum == 0)
			sectorCnt = 8;
		else
			sectorCnt = 1;
            	
            } else 
            if (DeviceID_cycle3 == 0x2201)	//mx29lv640m top
            {
		if(sectorNum == AMD29LV_640_LAST_SECTOR_NUM)
			sectorCnt = 8;
		else
			sectorCnt = 1;
	    }
	}
	else if ((pVol->type&0xff) == 0x7A) 		/* mx29lv1280d b */
	{
		
	  /*alex_huang support mx29lv128 flash 2009 09 09*/ 
		if(sectorNum == 0){
			sectorCnt = 8;
			offset = sectorNum * AMD29LV_1280_MTD_BOOT_SECTOR_SIZE;
       			size = AMD29LV_1280_MTD_BOOT_SECTOR_SIZE;
			
		}	
		else
			{
			offset = sectorNum * AMD29LV_1280_MTD_SECTOR_SIZE;
       		size = AMD29LV_1280_MTD_SECTOR_SIZE;
			sectorCnt = 1;
		}
		/*alex_huang end*/
		 sectorAddr = offset;
#ifdef BANK_AUTO_SWITCH
		if(offset <0x400000)
			s29gl128bank(0);
		else if(offset < 0x800000)
			s29gl128bank(1);
		else if(offset < 0xC00000)
			s29gl128bank(2);
		else if( offset < 0x1000000)
			s29gl128bank(3);
#endif
	}
    else
    	if((pVol->type&0xff) == 0xCB)
	{
	        offset = sectorNum * AMD29LV_MTD_SECTOR_SIZE;
       	 size = AMD29LV_MTD_SECTOR_SIZE;
	        sectorAddr = offset;
		if(sectorNum == 0)
			sectorCnt = 8;
		else
			sectorCnt = 1;
		
	}
#if FLASH_SST_SUPPORT
	else 
    	if ((pVol->type&0xff) == 0x80 || /* sst39LV400 */
    	     (pVol->type&0xff) == 0x81 || /* sst39LV800 */
    	     (pVol->type&0xff) == 0x4b /* sst39LV1601 */
    		) 		
	{
		offset = sectorNum * SST39VF_MTD_SECTOR_SIZE;
        	size = SST39VF_MTD_SECTOR_SIZE;
        	sectorAddr = offset;
        	sectorCnt = 1;	
	}
#endif
	
    DEBUG_PRINT(DEBUG_ERASE, ("Erasing sector %d, 0x%02x\n\r",
                sectorNum, (unsigned int)sectorAddr));

    /* Erase the sector */
	for(i=0; i<sectorCnt; i++)
	{
		UINT16 Datum;
	        if ((pVol->type&0xff) == 0xA7 || 	/* amd29LV320BT */
    		    (pVol->type&0xff) == 0xA8 ||	/* amd29LV320BB */
		    (pVol->type&0xff) == 0xF6 || 	/* amd29LV320D/BT */
    	            (pVol->type&0xff) == 0xF9		/* amd29LV320D/BB */
    	//	    ||(pVol->type&0xff) == 0x7A              /*mx29LV128D B*/
    		    )			
	    	{
    		    pFlash = (volatile UINT16*) pVol->map(pVol, offset+i*0x2000, size);
			    sectorAddr = offset+i*0x2000;
	    	} 
		else if ( (pVol->type&0xff) == 0x7A )
		{
		 /*alex_huang support mx29lv128 flash 2009 09 09*/ 
			pFlash = (volatile UINT16*) pVol->map(pVol, offset+i*size, size);
			    sectorAddr = offset+i*size;
		}
#ifdef CONFIG_SPANSION_16M_FLASH
		   else if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
			{
				pFlash = (volatile UINT16*) pVol->map(pVol, offset+i*0x20000, size);
			    sectorAddr = offset+i*0x20000;
			}
#endif	
			else if ((pVol->type&0xff) == 0x7E 	/* mx29lv640cB */
       	 	||(pVol->type&0xff) == 0xCB)	/* mx29lv640 bb */
	    	{
        		pFlash = (volatile UINT16*) pVol->map(pVol, offset+i*0x2000, size);
			    sectorAddr = offset+i*0x2000;
    		} else 
	//support mx29lv640cb    	
#if FLASH_SST_SUPPORT
        if ((pVol->type&0xff) == 0x80 || 	/* sst39vf400 */
    	    (pVol->type&0xff) == 0x81 ||	/* sst39vf800 */
    	    (pVol->type&0xff) == 0x4b)  	/* sst39vf1601 */
  	{
  	    	    pFlash = (volatile UINT16*) pVol->map(pVol, offset+i*0x1000, size);
		    sectorAddr = offset+i*0x1000;

  	} else
#endif
	    	{
    		    pFlash = (volatile UINT16*) pVol->map(pVol, offset+sectorOffset[i], size);
		    sectorAddr = offset+sectorOffset[i];
			
		};
		//printf("\na) %d\n", get_sys_time());
		flashReset(pVol);
		/* erase sequence */
	    flashUnlock(pVol);
		flashRegWrite16Bits(pVol, 0x5555*2, 0x8080);
	    flashUnlock(pVol);
    		/* Hit the locations for the sector */
		flashRegWrite16Bits(pVol, sectorAddr, 0x3030);
		// printf("\nb) %d\n", get_sys_time());
		/* check DQ7 to ensure the device has accepted the command sequence */
#if 1
		do {
			//CACHE_PIPE_FLUSH();
			Datum = flashRegRead16Bits(pVol, sectorAddr);
			//CACHE_PIPE_FLUSH();			
		} while (0==(Datum & (1<<7)));

#endif
#if 0
		UINT32 errorcounter=0;
		while((flashRegRead16Bits(pVol, sectorAddr)&(1<<7))==0){
			if( errorcounter++ > 0x100000) {
				printf("sector erase failed!\n");
				return (ERROR);
			}
		}
		/* check DQ3, hit complete, to check erase operation has begun */
		errorcounter =0;		
		while((flashRegRead16Bits(pVol, sectorAddr)&(1<<3))==0){
			if( errorcounter++ > 0x100000)
				printf("sector erase failed2!\n");				
				return (ERROR);
		}		
		//printf("\nd) %d\n", get_sys_time());	
#endif
#if 0
		BOOL erased = FALSE;
		erased = flashCheck16Bits(pVol, sectorAddr);
		if(erased == 1)
		{
        		DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase timeout, %p\n\r", pFlash));
	        	flashReset(pVol);
       	 	return(ERROR);
	        }
       	 else if(erased == 2)
		{
	        	DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase error, %p\n\r", pFlash));
        		flashReset(pVol);
       	 	return(ERROR);
	        }
		 //printf("\nc) %d\n", get_sys_time());
#endif
	}
	
	flashReset(pVol);
#if defined(CONFIG_SPANSION_16M_FLASH) && defined(BANK_AUTO_SWITCH)
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
		s29gl128bank(oldbank);
#endif
	return(OK);
}

/******************************************************************************
*
* flashRegWrite16Bits - Write 16 bits to 2 byte aligned address.
*
* RETURNS: N/A
*
*/
LOCAL  void flashRegWrite16Bits(FLFlash* pVol, UINT32 addr, UINT16 data)
{

    UINT32 flashBaseAddr = (pVol->baseAddress << 12);
#ifdef BANK_AUTO_SWITCH
	int oldbank=getbank();
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
	{
		if(addr < 0x400000)
			s29gl128bank(0);
		else if(addr < 0x800000)
			s29gl128bank(1);
		else if(addr < 0xC00000)
			s29gl128bank(2);
		else if( addr < 0x1000000)
			s29gl128bank(3);
	}
#endif
    /* Adjust addr for amd29LV160 */
    addr = flashBaseAddr + addr;
    DEBUG_PRINT(DEBUG_WRITE, ("Writing 0x%08x to 0x%08x\n\r", data, (unsigned int)addr));
    /* Write */
#ifdef BANK_AUTO_SWITCH
	int currBank;
	currBank = getbank();
	if (currBank == 0)
    	*((volatile UINT16*) addr) = data;
	else if (currBank == 1)
		*((volatile UINT16*) ((unsigned int)addr-0x400000)) = data;
	else if (currBank == 2)
		*((volatile UINT16*) ((unsigned int)addr-0x800000)) = data;
	else if (currBank == 3)
		*((volatile UINT16*) ((unsigned int)addr-0xC00000)) = data;
#else
	*((volatile UINT16*) addr) = data;
#endif
    CACHE_PIPE_FLUSH();

#ifdef BANK_AUTO_SWITCH
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
		s29gl128bank(oldbank);
#endif
}

/******************************************************************************
*
* flashRegRead16Bits - Read 16 bits from 2 byte aligned address.
*
* RETURNS: data at specified address
*
*/
LOCAL UINT16 flashRegRead16Bits(FLFlash* pVol, UINT32 addr)
{
    UINT16 data;
    UINT32 flashBaseAddr = (pVol->baseAddress << 12);
#ifdef BANK_AUTO_SWITCH
	int oldbank=getbank();
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
	{
		if(addr <0x400000)
			s29gl128bank(0);
		else if(addr < 0x800000)
			s29gl128bank(1);
		else if(addr < 0xC00000)
			s29gl128bank(2);
		else if( addr < 0x1000000)
			s29gl128bank(3);
	}
#endif
    addr = flashBaseAddr + addr;
#ifdef BANK_AUTO_SWITCH
	int currbank;
	currbank = getbank();
	if (currbank == 0)
    	data = *((volatile UINT16*) addr);
	else if (currbank == 1)
		data = *((volatile UINT16*) ((unsigned int)addr-0x400000));
	else if (currbank == 2)
		data = *((volatile UINT16*) ((unsigned int)addr-0x800000));
	else if (currbank == 3)
		data = *((volatile UINT16*) ((unsigned int)addr-0xC00000));
#else
	data = *((volatile UINT16*) addr);
#endif
    DEBUG_PRINT(DEBUG_READ, ("Read 0x%08x from 0x%08x\n\r", data, (unsigned int)addr));
    CACHE_PIPE_FLUSH();
#ifdef BANK_AUTO_SWITCH
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
		s29gl128bank(oldbank);
#endif	
    return(data);
}

/******************************************************************************
*
* flashRegRead8Bits - Read 8 bits from 2 byte aligned address.
*
* RETURNS: data at specified address
*
*/
#if 0
LOCAL UINT16 flashRegRead8Bits(FLFlash* pVol, UINT32 addr)
{
    UINT16 data;
    UINT32 flashBaseAddr = (pVol->baseAddress << 12);
#ifdef BANK_AUTO_SWITCH
	int oldbank=getbank();
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
	{
		if(addr <0x400000)
			s29gl128bank(0);
		else if(addr < 0x800000)
			s29gl128bank(1);
		else if(addr < 0xC00000)
			s29gl128bank(2);
		else if( addr < 0x1000000)
			s29gl128bank(3);
	}
#endif
    addr = flashBaseAddr + addr;
    data = *((volatile UINT16*) addr);
    data = data >> 8;
    DEBUG_PRINT(DEBUG_READ, ("Read 0x%08x from 0x%08x\n\r", data, (unsigned int)addr));
    CACHE_PIPE_FLUSH();
#ifdef BANK_AUTO_SWITCH
	//if (((pVol->type&0xff) == 0x7E)  &&  (DeviceID_cycle2 == 0x2221))		/* s29gl128p*/
	if(flash_is_16MB(pVol))
		s29gl128bank(oldbank);
#endif
    return(data);
}
#endif

/******************************************************************************
*
* flashIdGet - Get flash man. and device codes.
*
* RETURNS: N/A
*
*/

LOCAL void flashIdGet(FLFlash* pVol, UINT16* manCode, UINT16* devCode)
{
    flashUnlock(pVol);
    flashRegWrite16Bits(pVol, 0x5555*2, 0x0090);
    *manCode = flashRegRead16Bits(pVol, 0x00);
    *devCode = flashRegRead16Bits(pVol, 0x01*2);
    if(*devCode==0x227E) {
    	DeviceID_cycle2 = flashRegRead16Bits(pVol, 0x0e*2);
    	DeviceID_cycle3 = flashRegRead16Bits(pVol, 0x0f*2);// 0x2200 BB  0x2201 BT
    	/*
		DEBUG_PRINT(DEBUG_ALWAYS,
                    ("mx29lv640: 0x%02x 0x%02x\n\r",flashRegRead16Bits(pVol, 0x0e*2),
                    flashRegRead16Bits(pVol, 0x0f*2)
             		));    		
        */
    }
	
    	
    flashReset(pVol);

}

/******************************************************************************
*
* flashUnlock - Write unlock sequence to flash section.
*
* RETURNS: N/A
*
*/

LOCAL void flashUnlock(FLFlash* pVol)
{
    flashRegWrite16Bits(pVol, 0x5555*2, 0xaaaa);
    flashRegWrite16Bits(pVol, 0x2aaa*2, 0x5555);
}

/******************************************************************************
*
* flashReset - Write reset sequence to flash section.
*
* RETURNS: N/A
*
*/

LOCAL void flashReset(FLFlash* pVol)
{
    flashRegWrite16Bits(pVol, 0x5555*2, 0xaaaa);
    flashRegWrite16Bits(pVol, 0x2aaa*2, 0x5555);
    flashRegWrite16Bits(pVol, 0x5555*2, 0xf0f0);
}

/******************************************************************************
*
* amd29lvWrite - Erase and Write Area
*
* RETURNS: N/A
*
*/
extern board_param_t bParam;

STATUS amd29lvWrite(const void * dest, const void *  buffer, int length)
{
	FLFlash nvVol;
	int start, n_blk, status;

	//11/09/05' hrchen, disable NIC
#ifndef CONFIG_NO_NET
	Lan_Stop();		
#endif

	nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
	nvVol.map = amd29lvMap;

	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;
	//if (dest >= nvVol.chipSize)
	//	return ERROR;
#ifdef COMBO	
	if (((int)(dest-FLASH_BASE_ADDR)+ length) > nvVol.chipSize )
	{
		printf("Exceed Flash boundary, Flash size : 0x%x\r\n", (unsigned int)nvVol.chipSize);
		return ERROR;
	}
#endif
	//printf("dest addr %x\n",(int)dest);

	start = ((UINT32)dest-FLASH_BASE_ADDR)/nvVol.erasableBlockSize;
	n_blk = (length+nvVol.erasableBlockSize-1)/nvVol.erasableBlockSize;

	if(((int)dest==FLASH_BASE_ADDR)||(int )dest==FLASH_START_4M_SPACE){
		/*alex_huang support mx29lv128 flash 2009 09 09*/ 
		if ((nvVol.type&0xff) == 0x7A)
		{
			start = ((UINT32)dest-FLASH_BASE_ADDR)/AMD29LV_1280_MTD_BOOT_SECTOR_SIZE;
			n_blk = (((UINT32)dest & (AMD29LV_1280_MTD_BOOT_SECTOR_SIZE-1))+length)/AMD29LV_1280_MTD_BOOT_SECTOR_SIZE;		
		}
		/*alex_huang end*/
	}

#ifdef CONFIG_SPANSION_16M_FLASH
	if (nvVol.erasableBlockSize == S29GL128P_SECTOR_SIZE)
	{
		/*if ((dest+length-FLASH_BASE_ADDR) > FLASH_BANK_SIZE)
		  {
		  printf("\r\nExceed Flash Bank Size!!\r\n");
		  return ERROR;
		  }*/
		start = (((UINT32)dest-FLASH_BASE_ADDR) >> 17 );
		n_blk = (((UINT32)dest & (nvVol.erasableBlockSize-1))+length)/nvVol.erasableBlockSize;			
		if (((UINT32)dest+length) & (nvVol.erasableBlockSize-1))
			n_blk ++;
	}
#endif	
	DEBUGIT("\n1) %d\n", get_sys_time());
	if(nvVol.erase(&nvVol, start, n_blk)!=flOK)
		return ERROR;
	DEBUGIT("\n2) %d\n", get_sys_time());
	status = nvVol.write(&nvVol, (UINT32)dest-FLASH_BASE_ADDR, buffer, length, 0);
	DEBUGIT("\n3) %d\n", get_sys_time());
	//11/09/05' hrchen, enable NIC again
#ifndef CONFIG_NO_NET
	get_param(&bParam);
	Lan_Initialize(bParam.mac[0]);
#endif
	return status;
}




/******************************************************************************
*
* For saving size reason, new NVram area is at 0xbfc0e000 to 0xbfc0ffff
*
* RETURNS: N/A
*
*/


/******************************************************************************
*
* amd29lvEraseNV - Erase reserved area
*
* RETURNS: N/A
*
*/
STATUS amd29lvEraseNV(UINT32 sectorAddr, int length)
{
	FLFlash nvVol, *pVol;
	volatile UINT16* pFlash;
	BOOL erased = FALSE;
	int status=OK;

	//11/09/05' hrchen, disable NIC
#ifndef CONFIG_NO_NET
	Lan_Stop();		
#endif

	nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
	nvVol.map = amd29lvMap;
	pVol = &nvVol;
#ifdef CONFIG_SPANSION_16M_FLASH
	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;

	if ((nvVol.chipSize ==  S29GL128P_CHIP_SIZE) && (nvVol.erasableBlockSize == S29GL128P_SECTOR_SIZE))
	{

		int start, n_blk;
		start = sectorAddr/nvVol.erasableBlockSize;
		n_blk = (length+nvVol.erasableBlockSize-1)/nvVol.erasableBlockSize;

		DEBUGIT("\n1) %d\n", get_sys_time());
		if(nvVol.erase(&nvVol, start, n_blk)!=flOK)
			status =  ERROR;
		goto return_amd29lvEraseNV;
	}

#endif	
#ifdef CONFIG_SPI_FLASH
	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;
	printf("type:0x%x\n\r", nvVol.type);
	if ((nvVol.type == 0xC220 ) || (nvVol.type == 0x0102 ) || (nvVol.type == 0x1C31) || (nvVol.type == 0x8C20) ||
			(nvVol.type == 0xEF30) || (nvVol.type == 0x1F46) || (nvVol.type == 0xBF25))
	{	// If Uniform Flash occupy first 64KB, we block the erase function here.
		int start, n_blk;
		start = sectorAddr/nvVol.erasableBlockSize;
		n_blk = (length+nvVol.erasableBlockSize-1)/nvVol.erasableBlockSize;
		DEBUGIT("\n1) %d\n", get_sys_time());

		if(nvVol.erase(&nvVol, start, n_blk)!=flOK)
			status =  ERROR;
		goto return_amd29lvEraseNV;
	}
#endif
	/* Note, the following program segments are kept for boot type flash*/
	pFlash = (volatile UINT16*) pVol->map(pVol, sectorAddr, length);
	flashReset(pVol);
	/* erase sequence */
	flashUnlock(pVol);
	flashRegWrite16Bits(pVol, 0x5555*2, 0x8080);
	flashUnlock(pVol);
	/* Hit the locations for the sector */
	flashRegWrite16Bits(pVol, sectorAddr, 0x3030);
	/* check DQ7 to ensure the device has accepted the command sequence */
	while((flashRegRead16Bits(pVol, sectorAddr)&(1<<7))==0);
	/* check DQ3, hit complete, to check erase operation has begun */
	while((flashRegRead16Bits(pVol, sectorAddr)&(1<<3))==0);

	erased = flashCheck16Bits(pVol, sectorAddr);
	if(erased == 1) {
		DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase timeout, %p\n\r", (void *)pFlash));
		flashReset(pVol);
		status = ERROR;
		goto return_amd29lvEraseNV;
	}
	else if(erased == 2) {
		DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase error, %p\n\r", (void *)pFlash));
		flashReset(pVol);
		status = ERROR;
		goto return_amd29lvEraseNV;
	}
	flashReset(pVol);

return_amd29lvEraseNV:	
	//11/09/05' hrchen, enable NIC again
#ifndef CONFIG_NO_NET
	get_param(&bParam);
	Lan_Initialize(bParam.mac[0]);
#endif
	return status;
}

STATUS amd29lvEraseSector(UINT32 sectorAddr, int length)
{
	FLFlash nvVol, *pVol;
	volatile UINT16* pFlash;
	BOOL erased = FALSE;
	int status=OK;

	//11/09/05' hrchen, disable NIC
#ifndef CONFIG_NO_NET
	Lan_Stop();		
#endif

	nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
	nvVol.map = amd29lvMap;
	pVol = &nvVol;
#ifdef CONFIG_SPANSION_16M_FLASH
	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;

	if ((nvVol.chipSize ==  S29GL128P_CHIP_SIZE) && (nvVol.erasableBlockSize == S29GL128P_SECTOR_SIZE))
	{
		printf("find spansion flash\n\r");
		int start, n_blk;
		start = sectorAddr/nvVol.erasableBlockSize;
		n_blk = (length+nvVol.erasableBlockSize-1)/nvVol.erasableBlockSize;

		DEBUGIT("\n1) %d\n", get_sys_time());
		if(nvVol.erase(&nvVol, start, n_blk)!=flOK)
			status =  ERROR;
		goto return_amd29lvEraseNV;
	}

#endif	
#ifdef CONFIG_SPI_FLASH
	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;
	
	if ((nvVol.type == 0xC220 ) || (nvVol.type == 0x0102 ) || (nvVol.type == 0x1C31) || (nvVol.type == 0x8C20) ||
		(nvVol.type == 0xEF30) || (nvVol.type == 0x1F46) || (nvVol.type == 0xBF25))
	{	// If Uniform Flash occupy first 64KB, we block the erase function here.
		int start, n_blk;
		printf("find spi flash\n\r");
		start = sectorAddr/AMD29LV_MTD_ERASE_SIZE;
		n_blk = (length+AMD29LV_MTD_ERASE_SIZE-1)/AMD29LV_MTD_ERASE_SIZE;
		DEBUGIT("\n1) %d\n", get_sys_time());
		
		if(spi4KSectorRangeErase(&nvVol, start, n_blk)!=flOK)
			status =  ERROR;
		goto return_amd29lvEraseNV;
	}
#endif
	/* Note, the following program segments are kept for boot type flash*/
	pFlash = (volatile UINT16*) pVol->map(pVol, sectorAddr, length);
	flashReset(pVol);
	/* erase sequence */
	flashUnlock(pVol);
	flashRegWrite16Bits(pVol, 0x5555*2, 0x8080);
	flashUnlock(pVol);
	/* Hit the locations for the sector */
	flashRegWrite16Bits(pVol, sectorAddr, 0x3030);
	/* check DQ7 to ensure the device has accepted the command sequence */
	while((flashRegRead16Bits(pVol, sectorAddr)&(1<<7))==0);
	/* check DQ3, hit complete, to check erase operation has begun */
	while((flashRegRead16Bits(pVol, sectorAddr)&(1<<3))==0);

	erased = flashCheck16Bits(pVol, sectorAddr);
	if(erased == 1) {
		DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase timeout, %p\n\r", (void *)pFlash));
		flashReset(pVol);
		status = ERROR;
		goto return_amd29lvEraseNV;
	}
	else if(erased == 2) {
		DEBUG_PRINT(DEBUG_ALWAYS, ("Sector erase error, %p\n\r", (void *)pFlash));
		flashReset(pVol);
		status = ERROR;
		goto return_amd29lvEraseNV;
	}
	flashReset(pVol);

return_amd29lvEraseNV:	
	//11/09/05' hrchen, enable NIC again
#ifndef CONFIG_NO_NET
	get_param(&bParam);
	Lan_Initialize(bParam.mac[0]);
#endif
	return status;
}


/******************************************************************************
*
* amd29lvWriteNV - Write without erase
*
* RETURNS: N/A
*
*/
STATUS amd29lvWriteNV(const void * dest, const void *  buffer, int length)
{
	FLFlash nvVol;
	int start, n_blk, status;

	//11/09/05' hrchen, disable NIC
#ifndef CONFIG_NO_NET
	Lan_Stop();
#endif

	nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
	nvVol.map = amd29lvMap;
	if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;

	start = ((UINT32)dest-FLASH_BASE_ADDR)/nvVol.erasableBlockSize;
	n_blk = (length+nvVol.erasableBlockSize-1)/nvVol.erasableBlockSize;
	status = nvVol.write(&nvVol, (UINT32)dest-FLASH_BASE_ADDR, buffer, length, 0);

	//11/09/05' hrchen, enable NIC again
#ifndef CONFIG_NO_NET
	get_param(&bParam);
	Lan_Initialize(bParam.mac[0]);
#endif
	return status;
}


/******************************************************************************
*
* amd29lvReadNV - Erase reserved area
*
* RETURNS: N/A
*
*/
STATUS amd29lvReadNV(const void *  buffer, const void * src, int length)
{
FLFlash nvVol;


    nvVol.baseAddress = FLASH_BASE_ADDR >> 12;
    nvVol.map = amd29lvMap;
    if (amd29lvMTDIdentify(&nvVol) != flOK)
		return ERROR;
	#ifdef CONFIG_SPI_FLASH
	memcpy((void *)buffer, nvVol.map(&nvVol, (UINT32)src-FLASH_BASE_ADDR, 0), length);
	#endif
	return OK;
}
