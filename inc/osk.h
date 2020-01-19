/* OSK related definition.
 *
 */

#ifndef IMAGEHDR_H
#define IMAGEHDR_H 


typedef struct {
	unsigned int	key;		/* magic key */

#define BOOT_IMAGE             0xB0010001
#define BOOT_IMAGE_8672        0xB0010002
#define CONFIG_IMAGE           0xCF010002

//ccwei
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
#define Booter                 0x426F6F74 //'B' 'o' 'o' 't'
#define Loader                 0x4C6F6164 //'L' 'o' 'a' 'd'
#endif
//end

/*ql:20080721 START: different IC with different IMG KEY*/
#ifndef MULTI_IC_SUPPORT
#define APPLICATION_IMAGE      0xA0000003
#else
#define APPLICATION_IMG_8671       0xA0000003
//#define APPLICATION_IMG_8671P      0xA0000002
#define APPLICATION_IMG_8672       0xA0000004
#define APPLICATION_IMG_8671B      0xA0000008
#define APPLICATION_IMG_8671B_CD   0xA0000010
/*ql:20080729 START: if sachem register read fail, then don't check image key*/
#define APPLICATION_IMG_ALL        0xA0000000
/*ql:20080729 END*/
#endif
/*ql:20080721 END*/
#define BOOTPTABLE             0xB0AB0004


	unsigned int	address;	/* image loading DRAM address */
	unsigned int	length;		/* image length */
	unsigned int	entry;		/* starting point of program */
	unsigned short	chksum;		/* chksum of */
	
	unsigned char	type;
#define KEEPHEADER    0x01   /* set save header to flash */
#define FLASHIMAGE    0x02   /* flash image */
#define COMPRESSHEADER    0x04       /* compress header */
#define MULTIHEADER       0x08       /* multiple image header */
#define IMAGEMATCH        0x10       /* match image name before upgrade */
	
	
	unsigned char	   date[25];  /* sting format include 24 + null */
	unsigned char	   version[16];
        unsigned int  *flashp;  /* pointer to flash address */

} IMGHDR;
#define HEADERSIZE		64

#endif
