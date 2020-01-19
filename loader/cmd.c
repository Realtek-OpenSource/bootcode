/******************************************************************
 *
 *	Cmd.c : Bootloader command parser routine
 *
 * History:
 * 10/23/2006  SH, Lee	Added flashsize, memsize, miisel commands
 *
 *****************************************************************/

#include "uart.h"
#include "zlib.h"
#include "board.h"
#include "mib.h"
#include "osk.h"
#include "help.h"
#ifdef SUPPORT_MULT_UPGRADE
#include "../tftpnaive/net.h"
#endif

#ifdef CONFIG_RTL8685_MEMCTL_CHK
#include "../boot/memctl/bspchip_8685.h"
#include "../boot/memctl/memctl_8685.h"
#endif

//ccwei 111116
#include "rtk_nand.h"
#include "rtk_bbt.h"
//end 
#ifdef CONFIG_SPI_NAND_FLASH
#include "spi_nand.h"
#endif
#if defined(CONFIG_SPI_NAND_FLASH) || defined(CONFIG_NAND_FLASH)
#include "rtk_nand_base.h"
#endif

#ifdef CONFIG_L2C_TEST
#include "../boot/memctl/mips_cache_ops.h"	
#endif

//tctsai 121107
//Number of elements in an array
#define countof(a) (sizeof(a) / sizeof((a)[0]))
static void print_help();


#ifndef ERROR
#define ERROR 	-1
#endif

#ifndef OK
#define OK		0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#ifdef	CONFIG_GDMA_SCAN
#define random_data_pool_src 0xa0100000
#define	random_data_max_pool_size 7168 // 7168
//unsigned char random_data_pool[random_data_max_pool_size];
static unsigned char * random_data_move_point=NULL;
#endif

#ifdef	CONFIG_MAC0_LBK
unsigned	char	cali_method=0;
unsigned char ttdata[1488] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0x73,0x68,0x00,0x00,0x01,0xc8,0x00,0x0d,0xc5,0xe1,0x00,0x0d,0xc5,0xd1,0x00,0x0d,
	0xc5,0xdd,0x00,0x0d,0xac,0x51,0x00,0x0d,0xb9,0x1d,0x00,0x02,0x00,0x00,0x00,0x01,
	0x00,0x10,0x40,0x03,0x01,0x48,0x8f,0x1d,0x42,0x00,0x00,0x00,0x00,0x09,0x6d,0x08,
	0x7f,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x1a,0x00,0x0d,0xc5,0xcd,0x00,0x3f,0x91,
	0x45,0x84,0x68,0x34,0x8a,0x09,0x0a,0x40,0x62,0xae,0x9e,0x29,0x20,0xb2,0xfa,0x5e,
	0xa8,0xca,0xda,0x5a,0xb1,0x06,0x88,0xa8,0x7c,0x0f,0x17,0xa8,0x02,0xcb,0xfd,0x80,
	0x64,0x7b,0x64,0x1a,0x99,0xbf,0xc4,0x06,0xdf,0x68,0xbd,0xfa,0x8e,0xb7,0xe6,0xad,
	0xbd,0x73,0xa9,0xe4,0xed,0x33,0x16,0x15,0x30,0xcc,0xe0,0x06,0x36,0x39,0xf4,0x81,
	0x6f,0x6c,0x70,0x04,0x23,0x43,0x37,0x48,0xd7,0xa3,0x0a,0xdc,0xa2,0x31,0x7a,0x9e,
	0x8e,0xaa,0x83,0x88,0x8c,0xe7,0x58,0x15,0x36,0x38,0x27,0x8e,0xfa,0x3a,0xcc,0xfb,
	0xa8,0x2c,0xa5,0xdf,0xfc,0xaf,0x9f,0x52,0x22,0x33,0x28,0x16,0x74,0x24,0x4a,0xe6,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0xe2,0x77,0x07,0xa0,0x02,0x11,0xc9,0x4c,0x61,0xed,0x3e,0xaa,0x2d,0xb9,0x15,0x80,
	0x5b,0xf4,0x16,0x31,0x9c,0xa0,0x83,0xbb,0x7a,0xa0,0xc9,0xf8,0xc3,0x19,0xa0,0x49,
	0xff,0x00,0xff,0x00,0x5a,0x5a,0xa5,0xa5,0x5a,0x5a,0xa5,0xa5,0xff,0x00,0x01,0x7e,
	0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
	0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,
	0x00,0x01,0xff,0xfe,0x00,0x02,0xff,0xfd,0x00,0x04,0xff,0xfb,0x00,0x08,0xff,0xf7,
	0x00,0x10,0xff,0xef,0x00,0x20,0xff,0xdf,0x00,0x40,0xff,0xbf,0x00,0x80,0xff,0x7f,
	0x01,0x00,0xfe,0xff,0x02,0x00,0xfd,0xff,0x04,0x00,0xfb,0xff,0x08,0x00,0xf7,0xff,
	0x10,0x00,0xef,0xff,0x20,0x00,0xdf,0xff,0x40,0x00,0xbf,0xff,0x80,0x00,0x7f,0x00,
	0xdd,0xf5,0xb6,0xb1,0xe2,0x28,0x93,0x6a,0x3d,0x8c,0x30,0x3b,0x66,0x18,0x0a,0x79,
	0x02,0xa4,0xda,0x61,0xf5,0xc7,0xc9,0xa4,0x64,0xe5,0x88,0x1b,0x80,0xd8,0x0d,0x82,
	0x05,0xaa,0x80,0xe4,0xa8,0x43,0x27,0xfc,0xf4,0xed,0x52,0x43,0x2a,0x52,0x4b,0x24,
	0x29,0x3b,0x04,0xeb,0x01,0xc4,0xa1,0x8d,0x9c,0x3a,0x3e,0x0d,0x39,0x91,0x52,0x45,
	0xc8,0x80,0xfa,0x9a,0x4e,0xcc,0x0b,0x4f,0xd9,0xc6,0x71,0x56,0xc7,0xe6,0x09,0x99,
	0xb0,0x00,0xc5,0x48,0x13,0x68,0x65,0x8e,0x36,0x2b,0x8d,0xdd,0xa4,0x49,0x4c,0x7b,	
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0x73,0x68,0x00,0x00,0x01,0xc8,0x00,0x0d,0xc5,0xe1,0x00,0x0d,0xc5,0xd1,0x00,0x0d,
	0xc5,0xdd,0x00,0x0d,0xac,0x51,0x00,0x0d,0xb9,0x1d,0x00,0x02,0x00,0x00,0x00,0x01,
	0x00,0x10,0x40,0x03,0x01,0x48,0x8f,0x1d,0x42,0x00,0x00,0x00,0x00,0x09,0x6d,0x08,
	0x7f,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x1a,0x00,0x0d,0xc5,0xcd,0x00,0x3f,0x91,
	0x45,0x84,0x68,0x34,0x8a,0x09,0x0a,0x40,0x62,0xae,0x9e,0x29,0x20,0xb2,0xfa,0x5e,
	0xa8,0xca,0xda,0x5a,0xb1,0x06,0x88,0xa8,0x7c,0x0f,0x17,0xa8,0x02,0xcb,0xfd,0x80,
	0x64,0x7b,0x64,0x1a,0x99,0xbf,0xc4,0x06,0xdf,0x68,0xbd,0xfa,0x8e,0xb7,0xe6,0xad,
	0xbd,0x73,0xa9,0xe4,0xed,0x33,0x16,0x15,0x30,0xcc,0xe0,0x06,0x36,0x39,0xf4,0x81,
	0x6f,0x6c,0x70,0x04,0x23,0x43,0x37,0x48,0xd7,0xa3,0x0a,0xdc,0xa2,0x31,0x7a,0x9e,
	0x8e,0xaa,0x83,0x88,0x8c,0xe7,0x58,0x15,0x36,0x38,0x27,0x8e,0xfa,0x3a,0xcc,0xfb,
	0xa8,0x2c,0xa5,0xdf,0xfc,0xaf,0x9f,0x52,0x22,0x33,0x28,0x16,0x74,0x24,0x4a,0xe6,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0xe2,0x77,0x07,0xa0,0x02,0x11,0xc9,0x4c,0x61,0xed,0x3e,0xaa,0x2d,0xb9,0x15,0x80,
	0x5b,0xf4,0x16,0x31,0x9c,0xa0,0x83,0xbb,0x7a,0xa0,0xc9,0xf8,0xc3,0x19,0xa0,0x49,
	0xcb,0x19,0x97,0xa3,0x3b,0x8d,0x72,0x95,0x6a,0x7e,0x36,0xd6,0xae,0x7a,0x8a,0x7e,
	0xff,0xa4,0x1f,0x38,0xfd,0xb3,0xf3,0x05,0xe5,0x7d,0xaa,0x16,0xf9,0x7d,0xb6,0x58,
	0x45,0x6f,0x2e,0x6d,0x3c,0x9f,0xa1,0xdc,0x55,0x0d,0x97,0xfb,0x79,0x55,0x76,0xe3,
	0x8f,0x0d,0xa5,0x85,0x3b,0xeb,0xff,0x6c,0x29,0x1a,0x8b,0xb7,0x52,0xb6,0xe0,0x18,
	0x73,0x34,0x7c,0xdf,0xbc,0x62,0xf9,0x84,0xc4,0x96,0x3b,0x57,0x0e,0x62,0xb5,0xa4,
	0x73,0x8e,0x23,0x02,0xbe,0x06,0xed,0x7a,0x87,0x5e,0x33,0x3b,0x9a,0x60,0xdc,0x6d,
	0x70,0xbe,0x44,0x65,0xa2,0xd5,0x01,0xd8,0x4b,0x18,0xea,0xbb,0xc4,0x7a,0x44,0x95,
	0xdd,0xf5,0xb6,0xb1,0xe2,0x28,0x93,0x6a,0x3d,0x8c,0x30,0x3b,0x66,0x18,0x0a,0x79,
	0x02,0xa4,0xda,0x61,0xf5,0xc7,0xc9,0xa4,0x64,0xe5,0x88,0x1b,0x80,0xd8,0x0d,0x82,
	0x05,0xaa,0x80,0xe4,0xa8,0x43,0x27,0xfc,0xf4,0xed,0x52,0x43,0x2a,0x52,0x4b,0x24,
	0x29,0x3b,0x04,0xeb,0x01,0xc4,0xa1,0x8d,0x9c,0x3a,0x3e,0x0d,0x39,0x91,0x52,0x45,
	0xc8,0x80,0xfa,0x9a,0x4e,0xcc,0x0b,0x4f,0xd9,0xc6,0x71,0x56,0xc7,0xe6,0x09,0x99,
	0xb0,0x00,0xc5,0x48,0x13,0x68,0x65,0x8e,0x36,0x2b,0x8d,0xdd,0xa4,0x49,0x4c,0x7b,	
	0x8f,0x0d,0xa5,0x85,0x3b,0xeb,0xff,0x6c,0x29,0x1a,0x8b,0xb7,0x52,0xb6,0xe0,0x18,
	0x73,0x34,0x7c,0xdf,0xbc,0x62,0xf9,0x84,0xc4,0x96,0x3b,0x57,0x0e,0x62,0xb5,0xa4,
	0x73,0x8e,0x23,0x02,0xbe,0x06,0xed,0x7a,0x87,0x5e,0x33,0x3b,0x9a,0x60,0xdc,0x6d,
	0x70,0xbe,0x44,0x65,0xa2,0xd5,0x01,0xd8,0x4b,0x18,0xea,0xbb,0xc4,0x7a,0x44,0x95,
	0xdd,0xf5,0xb6,0xb1,0xe2,0x28,0x93,0x6a,0x3d,0x8c,0x30,0x3b,0x66,0x18,0x0a,0x79,
	0x02,0xa4,0xda,0x61,0xf5,0xc7,0xc9,0xa4,0x64,0xe5,0x88,0x1b,0x80,0xd8,0x0d,0x82,
	0x05,0xaa,0x80,0xe4,0xa8,0x43,0x27,0xfc,0xf4,0xed,0x52,0x43,0x2a,0x52,0x4b,0x24,
	0x29,0x3b,0x04,0xeb,0x01,0xc4,0xa1,0x8d,0x9c,0x3a,0x3e,0x0d,0x39,0x91,0x52,0x45,
	0xc8,0x80,0xfa,0x9a,0x4e,0xcc,0x0b,0x4f,0xd9,0xc6,0x71,0x56,0xc7,0xe6,0x09,0x99,
	0xb0,0x00,0xc5,0x48,0x13,0x68,0x65,0x8e,0x36,0x2b,0x8d,0xdd,0xa4,0x49,0x4c,0x7b,	
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,0xa5,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0x73,0x68,0x00,0x00,0x01,0xc8,0x00,0x0d,0xc5,0xe1,0x00,0x0d,0xc5,0xd1,0x00,0x0d,
	0xc5,0xdd,0x00,0x0d,0xac,0x51,0x00,0x0d,0xb9,0x1d,0x00,0x02,0x00,0x00,0x00,0x01,
	0x00,0x10,0x40,0x03,0x01,0x48,0x8f,0x1d,0x42,0x00,0x00,0x00,0x00,0x09,0x6d,0x08,
	0x7f,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x1a,0x00,0x0d,0xc5,0xcd,0x00,0x3f,0x91,
	0x45,0x84,0x68,0x34,0x8a,0x09,0x0a,0x40,0x62,0xae,0x9e,0x29,0x20,0xb2,0xfa,0x5e,
	0xa8,0xca,0xda,0x5a,0xb1,0x06,0x88,0xa8,0x7c,0x0f,0x17,0xa8,0x02,0xcb,0xfd,0x80,
	0x64,0x7b,0x64,0x1a,0x99,0xbf,0xc4,0x06,0xdf,0x68,0xbd,0xfa,0x8e,0xb7,0xe6,0xad,
	0xbd,0x73,0xa9,0xe4,0xed,0x33,0x16,0x15,0x30,0xcc,0xe0,0x06,0x36,0x39,0xf4,0x81,
	0x6f,0x6c,0x70,0x04,0x23,0x43,0x37,0x48,0xd7,0xa3,0x0a,0xdc,0xa2,0x31,0x7a,0x9e,
	0x8e,0xaa,0x83,0x88,0x8c,0xe7,0x58,0x15,0x36,0x38,0x27,0x8e,0xfa,0x3a,0xcc,0xfb,
	0xa8,0x2c,0xa5,0xdf,0xfc,0xaf,0x9f,0x52,0x22,0x33,0x28,0x16,0x74,0x24,0x4a,0xe6,
	0xda,0xac,0xa7,0x7f,0xf1,0x69,0x40,0xdc,0xfc,0x8c,0xdc,0x47,0x8a,0xd0,0x8d,0x01,
	0xe2,0x77,0x07,0xa0,0x02,0x11,0xc9,0x4c,0x61,0xed,0x3e,0xaa,0x2d,0xb9,0x15,0x80,
	0x5b,0xf4,0x16,0x31,0x9c,0xa0,0x83,0xbb,0x7a,0xa0,0xc9,0xf8,0xc3,0x19,0xa0,0x49,
	0xcb,0x19,0x97,0xa3,0x3b,0x8d,0x72,0x95,0x6a,0x7e,0x36,0xd6,0xae,0x7a,0x8a,0x7e,
	0xff,0xa4,0x1f,0x38,0xfd,0xb3,0xf3,0x05,0xe5,0x7d,0xaa,0x16,0xf9,0x7d,0xb6,0x58,
	0x45,0x6f,0x2e,0x6d,0x3c,0x9f,0xa1,0xdc,0x55,0x0d,0x97,0xfb,0x79,0x55,0x76,0xe3,
	0x8f,0x0d,0xa5,0x85,0x3b,0xeb,0xff,0x6c,0x29,0x1a,0x8b,0xb7,0x52,0xb6,0xe0,0x18	};
#endif


extern board_param_t bParam;
/*
char Loadhelp[]={ 
	 "help\n\r"
	 "info\n\r"
	 "reboot\n\r"
	"run [app addr] [entry addr]\n\r"
	"r [addr]\n\r"
	"w [addr] [val]\n\r"
	"d [addr] <len>\n\r"
	"resetcfg\n\r"
	"mac [\"clear\"/\"osk\"/mac address]\n\r"
	"bootline\n\r"
	"entry [address]\n\r"
	"load [address]\n\r"
	"xmodem [address]\n\r"
	"tftp [ip] [server ip] [file name]\n\r"
#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER	
	"web\n\r"
#endif
#ifdef SUPPORT_MULT_UPGRADE
	"multicast\n\r"
#endif
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	"root\n\r"
#endif

};
*/
#if 0
#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
#define LOADER_HELP \
	"help\n\r"\
	"info\n\r"\
	"reboot\n\r"\
	"run [app addr] [entry addr]\n\r"\
	"r [addr]\n\r"\
	"w [addr] [val]\n\r"\
	"d [addr] <len>\n\r"\
	"resetcfg\n\r"\
	"mac [\"clear\"/\"osk\"/mac address]\n\r"\
	"bootline\n\r"\
	"entry [address]\n\r"\
	"load [address]\n\r"\
	"app [address]\n\r"\
	"xmodem [address]\n\r"\
	"bootp\n\r"\
	"tftp [ip] [server ip] [file name]\n\r"\
	"web\n\r"
#ifdef SUPPORT_MULT_UPGRADE
	\
	"multicast\n\r"
#endif
#else
#define LOADER_HELP \
	"help\n\r"\
	"info\n\r"\
	"reboot\n\r"\
	"run [app addr] [entry addr]\n\r"\
	"r [addr]\n\r"\
	"w [addr] [val]\n\r"\
	"d [addr] <len>\n\r"\
	"resetcfg\n\r"\
	"mac [\"clear\"/\"osk\"/mac address]\n\r"\
	"bootline\n\r"\
	"entry [address]\n\r"\
	"load [address]\n\r"\
	"app [address]\n\r"\
	"xmodem [address]\n\r"\
	"bootp\n\r"\
	"tftp [ip] [server ip] [file name]\n\r"
#ifdef SUPPORT_MULT_UPGRADE
	\"multicast\n\r"
#endif
	
	
#endif
#endif

int s2mac(char *mac, char *mac_str)
{
char chk_str[17];
int i, j;

	strcpy(chk_str, "0123456789abcdef");
	if(strlen(mac_str)!=12)
		return ERROR;
	
	for(i=0; i<12; i++)
	{
		/* to lower case */
		if(mac_str[i]>='A' && mac_str[i]<='F')
			mac_str[i] -= 0x20;
		for(j=0;j<16;j++)
		{
			if(mac_str[i]==chk_str[j])
			{
				mac[i/2] = (mac[i/2] << 4) | j;
				break;
			}
		}
		if(j==16)
			return ERROR;	/* not a HEX string */
	}	
	return OK;
}

unsigned int sd_dump_default_addr = 0x80000000;

int dump_mem(unsigned int addr, int len)
{
unsigned char	*raw,ch;
int			row,col,rowsz; 

	raw = (unsigned char *)addr;
	if (len == 0) {
 		rowsz = 20;
	}
	else {
		rowsz = (len + 15)/16;
	}
#ifdef BANK_AUTO_SWITCH
	unsigned int bank;
	bank = getbank();
	switch(bank)
	{
		case 1:
			addr += 0x400000;
			break;
		case 2:
			addr += 0x800000;
			break;
		case 3:
			addr += 0xC00000;
			break;
	}
#endif

	for (row=0;row<rowsz;row++)
	{ 	
	 	// Address
 		printf("0x%08X: ",(addr + row * 16));
 		
	 	// Show HEX
 		for (col=0;col<8;col++) {
 			printf("%02X ",raw[col]);
 		}
		//printf("- ");
 		for (col=8;col<16;col++) {
 			printf("%02X ",raw[col]);
 		}

 		// Show ASCII
	 	for (col=0;col<16;col++) {
 			if ((raw[col] < 0x20) || (raw[col] > 0x7e)) {
 				ch = '.';
 			}
 			else {
 				if ((raw[col] == 0x25) || (raw[col] == 0x5c))
 					ch = '.';
 				else
 					ch = raw[col];
 			}
 			printf("%c",ch);
 		}

 		raw += 16;

 		printf("\n\r");
	
	}	// end of for
	return TRUE;	
}



#define  ASCII_CR    0x0d
#define  ASCII_LF    0x0a
#define  ASCII_BS    0x08
#define  ASCII_BELL  0x07
#define  ASCII_TAB   0x09
#define  ASCII_XON   0x11
#define  ASCII_XOFF  0x13
#define	ASCII_SPACE	0x20
#define	ASCII_ESC		0x1B
#define	ASCII_UPARROW	0x48

#define putnstr(str,n)	do {			\
		printf ("%.*s", (int)n, str);	\
	} while (0)

#define CTL_CH(c)				((c) - 'a' + 1)
#define CTL_BACKSPACE			('\b')
#define CREAD_HIST_CHAR		('!')

#define getcmd_putch(ch)		putchar(ch)
#define getcmd_getch()		getchar()
#define getcmd_cbeep()		getcmd_putch('\a')

#define HIST_MAX		20
#define HIST_SIZE		256

static int hist_max = 0;
static int hist_add_idx = 0;
static int hist_cur = -1;
unsigned hist_num = 0;

char* hist_list[HIST_MAX];
char hist_lines[HIST_MAX][HIST_SIZE + 1];	 /* Save room for NULL */

#define add_idx_minus_one() ((hist_add_idx == 0) ? hist_max : hist_add_idx-1)

void hist_init(void)
{
	int i;

	hist_max = 0;
	hist_add_idx = 0;
	hist_cur = -1;
	hist_num = 0;

	for (i = 0; i < HIST_MAX; i++) {
		hist_list[i] = hist_lines[i];
		hist_list[i][0] = '\0';
	}
}

static void cread_add_to_hist(char *line)
{
	strcpy(hist_list[hist_add_idx], line);

	if (++hist_add_idx >= HIST_MAX)
		hist_add_idx = 0;

	if (hist_add_idx > hist_max)
		hist_max = hist_add_idx;

	hist_num++;
}
static char* hist_prev(void)
{
	char *ret;
	int old_cur;

	if (hist_cur < 0)
		return NULL;

	old_cur = hist_cur;
	if (--hist_cur < 0)
		hist_cur = hist_max;

	if (hist_cur == hist_add_idx) {
		hist_cur = old_cur;
		ret = NULL;
	} else
		ret = hist_list[hist_cur];

	return (ret);
}

static char* hist_next(void)
{
	char *ret;

	if (hist_cur < 0)
		return NULL;

	if (hist_cur == hist_add_idx)
		return NULL;

	if (++hist_cur > hist_max)
		hist_cur = 0;

	if (hist_cur == hist_add_idx) {
		ret = "";
	} else
		ret = hist_list[hist_cur];

	return (ret);
}

static void cread_add_char(char ichar, int insert, unsigned long *num,
	       unsigned long *eol_num, char *buf, unsigned long len)
{
	unsigned long wlen;

	/* room ??? */
	if (insert || *num == *eol_num) {
		if (*eol_num > len - 1) {
			getcmd_cbeep();
			return;
		}
		(*eol_num)++;
	}

	if (insert) {
		wlen = *eol_num - *num;
		if (wlen > 1) {
			memmove(&buf[*num+1], &buf[*num], wlen-1);
		}

		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
		while (--wlen) {
			getcmd_putch(CTL_BACKSPACE);
		}
	} else {
		/* echo the character */
		wlen = 1;
		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
	}
}

static void cread_add_str(char *str, int strsize, int insert, unsigned long *num,
	      unsigned long *eol_num, char *buf, unsigned long len)
{
	while (strsize--) {
		cread_add_char(*str, insert, num, eol_num, buf, len);
		str++;
	}
}

int kbd_proc(char *buf, int len)
{
	char kbd_cc;
	int i=0;
	int buf_idx = 0;
	int esc_len = 0;
	int ctrl_idx = 0;
	int insert=1;
	unsigned long num = 0;
	unsigned long eol_num = 0;
	unsigned long wlen;
	char esc_save[8];

	while(1)
	{
		if (poll_cc())
		    kbd_cc=getchar();
		else {
			run_nonblocking_tasks();
			continue;
		}

		
		/*
		 * handle standard linux xterm esc sequences for arrow key, etc.
		 */
		if (esc_len != 0) {
			if (esc_len == 1) {
				if (kbd_cc == '[') {
					esc_save[esc_len] = kbd_cc;
					esc_len = 2;
				} else {
					cread_add_str(esc_save, esc_len, insert,
						      &num, &eol_num, buf, len);
					esc_len = 0;
				}
				continue;
			}

			switch (kbd_cc) {
				case 'A':		/* up arrow */
					kbd_cc = CTL_CH('p');
					esc_len = 0;
					break;	/* pass off to ^P handler */
				case 'B':		/* down arrow */
					kbd_cc = CTL_CH('n');
					esc_len = 0;
					break;	/* pass off to ^N handler */
				default:
					esc_save[esc_len++] = kbd_cc;
					cread_add_str(esc_save, esc_len, insert,
					      &num, &eol_num, buf, len);
					esc_len = 0;
					continue;
			}
			
		}
	
	    	switch(kbd_cc) {
			case ASCII_ESC:	
				if (esc_len == 0) {
					esc_save[esc_len] = kbd_cc;
					esc_len = 1;
				} else {
					printf("impossible condition\n");
					esc_len = 0;
				}

				buf[buf_idx]=kbd_cc;
				buf_idx++;				
				break;		
			case ASCII_LF:
			case ASCII_CR:
				buf[buf_idx]='\0';
				printf("%c", ASCII_LF);
				return(1);
			case ASCII_TAB:
				buf[buf_idx]=ASCII_SPACE;
			    	if(buf_idx<len)
				{
					buf_idx++;
					printf("%c", ASCII_SPACE);
				}
				else 
					printf("%c", ASCII_BELL);
				break;
			case ASCII_BS:
				if(buf_idx>0) {
					buf_idx--;
					printf("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
				}
				break;	
			
			case CTL_CH('p'):
			case CTL_CH('n'):
				{
					char * hline;
					ctrl_idx = 0;

					if (kbd_cc == CTL_CH('p'))
						hline = hist_prev();
					else
						hline = hist_next();

					if (!hline) {
						getcmd_cbeep();
						continue;
					}
					/* nuke the current line */
					/* first, go home */
					while (num) {				
						getcmd_putch(CTL_BACKSPACE);	
						num--;				
					}	

					/* erase to end of line */
					if (num < eol_num) {				
						for(i=0; i<(eol_num - ctrl_idx);i++){
							printf("%c", ASCII_SPACE);
						} 
						do {					\
							getcmd_putch(CTL_BACKSPACE);	
						} while (--eol_num > num);		\
					}

					/* copy new line into place and display */
					strcpy(buf, hline);
					eol_num = strlen(buf);
					if (num < eol_num) {			
						wlen = eol_num - num;					
						for(i=0; i<wlen; i++)
							printf("%c",buf[num+i]);
						
						num = eol_num;			
					}	
					buf_idx = eol_num;
					continue;
				}
			default:
				buf[buf_idx]=kbd_cc;
			   	if(buf_idx<len)
			    	{	
					buf_idx++;
					printf("%c", kbd_cc);
				}
				else 
					printf("%c", ASCII_BELL);
	    	}		
	}	
}

int cmd_shell(void)
{
	char buf[256];

	print_banner();

#ifdef CONFIG_RTK_REMAP_BBT
	rtk_scan_v2r_bbt();
	rtk_nand_scan_bbt();
#endif
#ifdef CONFIG_RTK_NORMAL_BBT
	nand_scan_normal_bbt();
#endif

	hist_init();

	while(1)
	{
		if(kbd_proc(buf, sizeof(buf))){

			buf[sizeof(buf)] = '\0';	/* lose the newline */		
			if (buf[0] && buf[0] != CREAD_HIST_CHAR)
				cread_add_to_hist(buf);
			hist_cur = hist_add_idx;

			cmd_proc(buf);
		}
	}
}

unsigned long str2UL(char *str) 
{
	unsigned long	ul;
	unsigned char	ch;
	int				i;
	
	ul = 0;
	if ((str[0] == '0' && str[1] == 'X') ||
		(str[0] == '0' && str[1] == 'x'))
	{
		// Hex Mode
		for (i=2;i<strlen(str);i++)	{			
			ch = IsHexNumber(str[i]);
			if (ch == 0xff)	{ /* illegal char. */
				break;
			}			
			ul <<= 4;
			ul |= ch;
		}
	}
	else {
		// DEC Mode
		for (i=0;i<strlen(str);i++)	{
		
			ch = str[i];				
			if (ch < '0' || ch > '9')	{ /* illegal char. */
				break;
			}			
			ul *= 10;
			ul += (ch - '0');
		}			
	}
	
	return ul;
}

#ifdef CONFIG_RTL8685SB
#ifdef CONFIG_RLX
char cmd_prompt[] = "<RLX5281>";
#else
char cmd_prompt[] = "<MIPS1004K>";
#endif
#else
char cmd_prompt[] = "<RTL867X>";
#endif

#define MAX_ARGS	20
#define MAX_ARG_LEN	20
//#define REG32(reg)	(*(volatile unsigned int *)(reg))
//#define REG16(reg)	(*(volatile unsigned short *)(reg))
//#define REG8(reg)	(*(volatile unsigned char *)(reg))
//#define WRITE_MEM32(reg,val)	(*(volatile unsigned int *)(reg))=val
//#define READ_MEM32(reg)			*(volatile unsigned int *)(reg)

#ifdef CONFIG_GDMA_SCAN
//#define WRITE_MEM16(reg,val)	(*(volatile unsigned short *)(reg))=val
//#define READ_MEM16(reg)			*(volatile unsigned short *)(reg)

  #define GDMA_BASE    0xB800A000
  #define GDMACNR      (GDMA_BASE + 0x00)
  #define GDMAIMR      (GDMA_BASE + 0x04)
  #define GDMAISR      (GDMA_BASE + 0x08)
  #define GDMAICVL     (GDMA_BASE + 0x0C)
  #define GDMAICVR     (GDMA_BASE + 0x10)
  #define GDMASBP(i)   (GDMA_BASE + 0x20 + ((i) << 3))
  #define GDMASBL(i)   (GDMA_BASE + 0x24 + ((i) << 3))
  #define GDMADBP(i)   (GDMA_BASE + 0x60 + ((i) << 3))
  #define GDMADBL(i)   (GDMA_BASE + 0x64 + ((i) << 3))
  /* GDMA - Control Register */
   #define GDMA_ENABLE     (1<<31)         /* Enable GDMA */
   #define GDMA_POLL       (1<<30)         /* Kick off GDMA */
   #define GDMA_FUNCMASK   (0xf<<24)       /* GDMA Function Mask */
   #define GDMA_MEMCPY     (0x0<<24)       /* Memory Copy */
   #define GDMA_CHKOFF     (0x1<<24)       /* Checksum Offload */
   #define GDMA_STCAM      (0x2<<24)       /* Sequential T-CAM */
   #define GDMA_MEMSET     (0x3<<24)       /* Memory Set */
   #define GDMA_B64ENC     (0x4<<24)       /* Base 64 Encode */
   #define GDMA_B64DEC     (0x5<<24)       /* Base 64 Decode */
   #define GDMA_QPENC      (0x6<<24)       /* Quoted Printable Encode */
   #define GDMA_QPDEC      (0x7<<24)       /* Quoted Printable Decode */
   #define GDMA_MIC        (0x8<<24)       /* Wireless MIC */
   #define GDMA_MEMXOR     (0x9<<24)       /* Memory XOR */
   #define GDMA_MEMCMP     (0xa<<24)       /* Memory Compare */
   #define GDMA_BYTESWAP   (0xb<<24)       /* Byte Swap */
   #define GDMA_PATTERN    (0xc<<24)       /* Pattern Match */
   #define GDMA_SWAPTYPE0  (0<<22)         /* Original:{0,1,2,3} => {1,0,3,2} */
   #define GDMA_SWAPTYPE1  (1<<22)         /* Original:{0,1,2,3} => {3,2,1,0} */
   #define GDMA_ENTSIZMASK (3<<20)         /* T-CAM Entry Size Mask */
   #define GDMA_ENTSIZ32   (0<<20)         /* T-CAM Entry Size 32 bits */
   #define GDMA_ENTSIZ64   (1<<20)         /* T-CAM Entry Size 64 bits */
   #define GDMA_ENTSIZ128  (2<<20)         /* T-CAM Entry Size 128 bits */
   #define GDMA_ENTSIZ256  (3<<20)         /* T-CAM Entry Size 256 bits */
   #define GDMA_LDB 	   (1<<31)		   /* Last data block */
#endif

#ifdef	CONFIG_MAC0_LBK
extern	int Lan_Initialed;
unsigned	UDP_check3_failure=0;

int run_gzip_file_test(unsigned int src, unsigned long entry)
{
	int ret;
	unsigned int comprLen;
	unsigned int uncompressedLength;
	unsigned int oldbank, currbank;
	unsigned int offset;
	unsigned int rData;

	/* infate RAM file */
	comprLen = 1024*1024*2;
	//11/06/04' hrchen, in C_Entry(), printf() will destory the value of uncompressedLength
	//reset uncompressedLength again
	uncompressedLength = 0xFFFFFFFF;
	//ret = uncompress(entry, &uncompressedLength, src, comprLen);

	printf("Decompress file... ");

	rData = *((unsigned int*)src);
	currbank = 0;

	
	if ( rData==0x28cd3d45) {  //10/19/05' hrchen, cramfs case
	    //10/17/05' hrchen, +*((unsigned int)(src+4)) to skip file system
	    src+=*((unsigned int*)(src+4));  //skip cramfs image
    } else if ( rData==0x73717368) {  //11/14/05' hrchen, squashfs case
		if (currbank == 0)
			src+=((*((unsigned int*)(src+8))+0x0FFF)&(~0x0FFF));  //skip squashfs image
		else if (currbank == 1)
			src+=((*((unsigned int*)(src+8-0x400000))+0x0FFF)&(~0x0FFF));
		else if (currbank == 2)
			src+=((*((unsigned int*)(src+8-0x800000))+0x0FFF)&(~0x0FFF));
		else if (currbank == 3)
			src+=((*((unsigned int*)(src+8-0xC00000))+0x0FFF)&(~0x0FFF));
    }
	ret = uncompressLZMA(entry, &uncompressedLength, src, comprLen);		

	if (ret) { 
		printf("uncompress fail....\n\r");
		}	

	if(!ret )
	{
	printf("ok!\n\r");
#if	0
	void (*appStart)(void);

		/* jump to decompressed program */
		appStart = (void*)entry;
		//tylo, for 8672
		__asm__ volatile(
			"mtc0 $0,$20\n\t"
			"nop\n\t"
			"li $8,0x00000200\n\t"
			"mtc0 $8,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"mtc0 $0,$20\n\t"
			"nop"
			: /* no output */
			: /* no input */
				);
		appStart();
#endif	
	return 1;
	}	
	return ERROR; 
}

unsigned MAC0_LBK_TEST(void){
	unsigned char *comp_pBuf;
	unsigned count_no,compare_count,compare_fail_flag=0,loopback_cnt=50,pkt_tx_i,pkt_lenght=1400;
	unsigned REC_CONF,PHY_REG0;
	unsigned IP_module,IP_module_en=0xfd0017ff;
	unsigned char mac_addr[6]={0x00,0x01,0x02,0x03,0x04,0x05};
	Lan_Initialize(mac_addr);
	REC_CONF=(*(volatile unsigned int *)(0xb8018044));		//backup Receive Configuration.
	(*(volatile unsigned int *)(0xb801805c))=0x04000000;
	PHY_REG0=(*(volatile unsigned int *)(0xb801805c));		//backup PHY_REG0		
	(*(volatile unsigned int *)(0xb8018044))|=0x7f; 		//Receive Configuration register, all packet accept.
	(*(volatile unsigned int *)(0xb801805c))=0x84002100;	//Disable Auto. Neg.
	(*(volatile unsigned int *)(0xb8018040))=0x0f00;		//MAC loopback mode
	IP_module=(*(volatile unsigned int *)(0xb800330c));		//read IP module status
	
	ttdata[0x10]=(pkt_lenght-14)/256;
	ttdata[0x11]=(pkt_lenght-14)&0xFF;
	for(count_no=0;count_no<5;count_no++){
		Lan_Transmit(ttdata,pkt_lenght);
		comp_pBuf=Lan_Receive();
		if(cali_method==0x14){		
			for(pkt_tx_i=0;pkt_tx_i<loopback_cnt;pkt_tx_i++){
				Lan_Transmit(comp_pBuf+0x16,pkt_lenght);
				(*(volatile unsigned int *)(0xb800330c))=IP_module_en;		//addition
				comp_pBuf=Lan_Receive();
				(*(volatile unsigned int *)(0xb800330c))=IP_module;			//addition
				}
		}else{
			for(pkt_tx_i=0;pkt_tx_i<loopback_cnt;pkt_tx_i++){
				Lan_Transmit(comp_pBuf+0x16,pkt_lenght);
				comp_pBuf=Lan_Receive();
				}		
			}
		comp_pBuf+=0x16;
		for(compare_count=0;compare_count<pkt_lenght; compare_count++){
			if(comp_pBuf[compare_count]!=ttdata[compare_count]){
				compare_fail_flag=1;
				return 0;
				//break;
				}
			}
		}
	//printf("compare_fail_flag=%d \n\r",compare_fail_flag);
	return 1;
			(*(volatile unsigned int *)(0xb800330c))=IP_module;		//restore, IP module status.	
	(*(volatile unsigned int *)(0xb8018044))=REC_CONF;				//restore
	(*(volatile unsigned int *)(0xb801805c))=PHY_REG0;				//restore, enable Auto. Neg.
	Lan_Initialed=0;
}

unsigned TFTP_TEST(void){
	char *mac;
	unsigned image_len,successful_flag;
	char filename[]="vm.img";
	char dip[]="192.168.1.11";
	char sip[]="192.168.1.10";
	unsigned int ip = inet_addr(dip);
	unsigned int server = inet_addr(sip);
	char *load_buf;
	char *app_buf;
	
	
	get_param(&bParam);
	load_buf = (char*)bParam.load;
	//load_buf = (char*)DRAM_TEMP_LOAD_ADDR;
	app_buf = (char*)bParam.app;
	mac = bParam.mac[0];
	Lan_Stop();
	Lan_Initialize(mac);
	*(volatile unsigned int*)0xb8018010=0;
	*(volatile unsigned int*)0xb8018014=0;	
	if ( bootpReceive (mac, &image_len, ip, server, filename, load_buf) != 0 ){
		printf("TFTP ERROR\n\r");
		printf("0UDP_check3_failure=0x%x\n\r",UDP_check3_failure);		
		successful_flag=0;
		}
	else{
		if(UDP_check3_failure==0){
			printf("TFTP PASS\n\r");
			successful_flag=1;		
			}
		else{
			printf("TFTP ERROR\n\r");
			printf("1UDP_check3_failure=0x%x\n\r",UDP_check3_failure);
			successful_flag=0;			
			}	
		}
	UDP_check3_failure=0;   					//clean UDP_check3_failure
	printf("phy: TxOKCNT=%d, RxOKCNT=%d, TxErr=%d, RxErr=%d  \n\r ",*(volatile unsigned int*)0xb8018010&0xffff,*(volatile unsigned int*)0xb8018010>>16,*(volatile unsigned int*)0xb8018014&0xffff,*(volatile unsigned int*)0xb8018014>>16);
	return	successful_flag;
}

void print_DDR_cali_result(void){
	unsigned calc_pass_collect=0xffffffff,temp,t_loop;
	printf("===== DQS0 =====>");
	if(cali_method&0x1)
		printf(" TFTP = 0x%x, ",*(volatile unsigned int*)0xa0600048);		
	if(cali_method&0x2)
		printf(" decompress = 0x%x, ",*(volatile unsigned int*)0xa0600040);
	if(cali_method&0x4)
		printf(" MAC LBK = 0x%x, ",*(volatile unsigned int*)0xa0600050);
	printf("=====\n\r");
	printf("===== DQS1 =====>");
	if(cali_method&0x1)
		printf(" TFTP = 0x%x, ",*(volatile unsigned int*)0xa060004C);
	if(cali_method&0x2)
		printf(" decompress = 0x%x, ",*(volatile unsigned int*)0xa0600044);	
	if(cali_method&0x4)
		printf(" MAC LBK = 0x%x, ",*(volatile unsigned int*)0xa0600054);
	printf("=====\n\r");	
	
	
		//printf("========= DQS0 === decompress=0x%x, tftp=0x%x, MAC0=0x%x =========\n\r",*(volatile unsigned int*)0xa05f0040,*(volatile unsigned int*)0xa05f0044,*(volatile unsigned int*)0xa05f0050);
		//printf("========= DQS1 === decompress=0x%x, tftp=0x%x, MAC0=0x%x =========\n\r",*(volatile unsigned int*)0xa05f0048,*(volatile unsigned int*)0xa05f004c,*(volatile unsigned int*)0xa05f0054);
		if(cali_method&0x1)
			calc_pass_collect &= *(volatile unsigned int*)0xa0600048;
		if(cali_method&0x2)
			calc_pass_collect &= *(volatile unsigned int*)0xa0600040;
		if(cali_method&0x4)
			calc_pass_collect &= *(volatile unsigned int*)0xa0600050;			


	
	//calc_pass_collect = *(volatile unsigned int*)0xa0607F40 & (*(volatile unsigned int*)0xa0607F48) & (*(volatile unsigned int*)0xa0607F50);
	printf("=========>> DQS0 pass collection = 0x%x =========\n\r>>>>>",calc_pass_collect);
	for(t_loop=0;t_loop<32;t_loop++){
		if((calc_pass_collect >> t_loop)&1==1){
			printf("0x%x, ",t_loop);
			}
		}
	printf("\b\b<<<<<\n\r");
		calc_pass_collect=0xffffffff;
		if(cali_method&0x1)
			calc_pass_collect &= *(volatile unsigned int*)0xa060004C;
		if(cali_method&0x2)
			calc_pass_collect &= *(volatile unsigned int*)0xa0600044;
		if(cali_method&0x4)
			calc_pass_collect &= *(volatile unsigned int*)0xa0600054;		
	//calc_pass_collect = *(volatile unsigned int*)0xa0607F44 & (*(volatile unsigned int*)0xa0607F4C)& (*(volatile unsigned int*)0xa0607F54);
	printf("=========>> DQS1 pass collection = 0x%x =========\n\r>>>>>",calc_pass_collect);
	for(t_loop=0;t_loop<32;t_loop++){
		if((calc_pass_collect >> t_loop)&1==1){
			printf("0x%x, ",t_loop);
			}
		}
	printf("\b\b<<<<<\n\r\n\r");	
}



void	SDRAM_timing_check(unsigned down_flag)
{
	int			scan_position;
	unsigned	src,L0=0,R0=0,org_0220,temp,cali_method_1;
	unsigned	*result_TFTP_L0_R0;
	unsigned	*result_DECOMP_L0_R0;
	unsigned	*result_MAC_LBK_L0_R0;
	unsigned	tftp_fail_flag=1,decomp_fail_flag=1,mac0_lbk_fail_flag0=1;
	unsigned long entry;
	
	/////////////////////////
	cali_method_1=cali_method & 0xF;
	if(cali_method_1&0x1)
		tftp_fail_flag=0;
	if(cali_method_1&0x2)
		decomp_fail_flag=0;
	if(cali_method_1&0x4)
		mac0_lbk_fail_flag0=0;
	/////////////////////////
	get_param(&bParam);
	src=bParam.app;
	entry=bParam.entry;
	////////////////////////
	result_TFTP_L0_R0			= 0xA0600040;
	result_DECOMP_L0_R0			= 0xA0600044;
	result_MAC_LBK_L0_R0		= 0xA0600048;	
	*result_TFTP_L0_R0			= 0;
	*result_DECOMP_L0_R0		= 0;
	*result_MAC_LBK_L0_R0		= 0;
	/////////////////////////
	org_0220	=	*(volatile unsigned int *)(0xb8000220);
	scan_position	= (org_0220 & 0x01F00) >>8;
	/////////////////////////
	while(1){
		temp =*(volatile unsigned int *)(0xb8000220) & ~0x1F00;
		*(volatile unsigned int *)(0xb8000220)= temp | (scan_position << 8);
		printf("DRAM rx delay value= 0x%x, 0xb8000220=0x%x \n\r",scan_position,*(volatile unsigned int *)(0xb8000220));
				if(cali_method_1&0x1){
					if(tftp_fail_flag==0)
						if(TFTP_TEST()==1){
							(*result_TFTP_L0_R0) |= (1<<scan_position);
							}else{
								tftp_fail_flag++;
								}
					}
				if(cali_method_1&0x2){
					if(decomp_fail_flag==0)
						if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
							printf("decompress fail......\n\r");
							decomp_fail_flag++;	
						}else
							(*result_DECOMP_L0_R0) |= (1<<scan_position);
					}
				if(cali_method_1&0x4){		
					if(MAC0_LBK_TEST()==1){
							printf("MAC0_LBK_TEST pass.......\n\r");
							(*result_MAC_LBK_L0_R0) |= (1<<(scan_position));
						}else{
							printf("MAC0_LBK_TEST fail.......\n\r");											
							mac0_lbk_fail_flag0++;
							}
					}
				if(cali_method_1&0x8){
					if(GDMA_SDRAM_SCAN(0x100000)==1){
							printf("GDMA_TEST pass.......\n\r");
							(*result_MAC_LBK_L0_R0) |= (1<<(scan_position));
						}else{
							printf("GDMA_TEST fail.......\n\r");											
							mac0_lbk_fail_flag0++;
							}
					}				
				if(cali_method_1&0x1)
					printf(" TFTP = 0x%x \n\r",*(volatile unsigned int*)0xa0600040);		
				if(cali_method_1&0x2)
					printf(" decompress = 0x%x \n\r",*(volatile unsigned int*)0xa0600044);
				if(cali_method_1&0x4)
					printf(" MAC LBK = 0x%x \n\r",*(volatile unsigned int*)0xa0600048);
				////////////////
				if( (mac0_lbk_fail_flag0>=1) &(decomp_fail_flag>=1) & (tftp_fail_flag>=1))
					break;
				////////////////
				if(down_flag){
					scan_position--;
					if(scan_position < 0)
						break;
				}else{
					scan_position++;
					if(scan_position > 0x1F){

							break;			
						}
					}
				////////////////					
		}
	*(volatile unsigned int *)(0xb8000220)=	org_0220;
	if(cali_method_1&0x1)
		printf(" TFTP = 0x%x \n\r",*(volatile unsigned int*)0xa0600040);		
	if(cali_method_1&0x2)
		printf(" decompress = 0x%x \n\r",*(volatile unsigned int*)0xa0600044);
	if(cali_method_1&0x4)
		printf(" MAC LBK = 0x%x \n\r",*(volatile unsigned int*)0xa0600048);

}

/////////////////////////////////////////////
//int DDR_timing_check(unsigned int src, unsigned long entry)
#if 1
void DDR_timing_diff_dqs_check2(unsigned prioirty_dqs)
{
	unsigned src,temp=0,DLL0_temp=0,DQS_temp,DQS_temp1,loop_cnt,DQS_test_flag=0,DQS_test_status=0,cali_method_1;
	unsigned GDMA_L0,GDMA_R0,GDMA_L1,GDMA_R1,temp_i,tftp_fail_flag=0,decomp_fail_flag=0,tftp_fail_flag1=0,decomp_fail_flag1=0;
	unsigned mac0_lbk_fail_flag0=0,mac0_lbk_fail_flag1=0;
	unsigned *result_decompress_dqs0;
	unsigned *result_tftp_dqs0;
	unsigned *result_decompress_dqs1;
	unsigned *result_tftp_dqs1;	
	unsigned *result_mac0_lbk_dqs0;
	unsigned *result_mac0_lbk_dqs1;
	unsigned DLL0_org=0,DCR_org,DQS_org,tx_delay_org;
	unsigned long entry;
	get_param(&bParam);
	src=bParam.app;
	entry=bParam.entry;
	//calibration method decide 
	cali_method_1=cali_method & 0xF;
	//clean result
	result_decompress_dqs0 	= 0xa0600040;
	result_decompress_dqs1 	= 0xa0600044;	
	result_tftp_dqs0 		= 0xa0600048;	
	result_tftp_dqs1 		= 0xa060004c;
	result_mac0_lbk_dqs0 	= 0xa0600050;
	result_mac0_lbk_dqs1 	= 0xa0600054;
	
	*result_decompress_dqs0	=0;
	*result_tftp_dqs0		=0;	
	*result_decompress_dqs1	=0;
	*result_tftp_dqs1		=0;	
	*result_mac0_lbk_dqs0	=0;
	*result_mac0_lbk_dqs1	=0;
	//clean done
	//loading GDMA calibration value
	temp = (*(volatile unsigned int *)(0xb8003200)) & 0x0F;		//only for RLE0315 & RL6166 use.
	//temp=0;		//only for 133MHz
	if(temp==0 | temp==3){		// get 166MHz(0)  and 200MHz(3) calibration value.
		GDMA_L0=*(volatile unsigned int *)(0xa0600020);
		GDMA_R0=*(volatile unsigned int *)(0xa0600024);	
		GDMA_L1=*(volatile unsigned int *)(0xa0600028);
		GDMA_R1=*(volatile unsigned int *)(0xa060002c);
		}
	else{				// get 133MHz calibration value.
		GDMA_L0=*(volatile unsigned int *)(0xa0600030);
		GDMA_R0=*(volatile unsigned int *)(0xa0600034);	
		GDMA_L1=*(volatile unsigned int *)(0xa0600038);
		GDMA_R1=*(volatile unsigned int *)(0xa060003c);		
		}
	//loading GDMA calibration value, end line
	//check DDR or SDRAM onboard.
	temp = *(volatile unsigned int *)(0xb8001000);
	if(temp&0x80000000){
		DQS_org = *(volatile unsigned int *)(0xb8001050);
		DQS_test_flag=prioirty_dqs;
		TFTP_TEST();
			while(1){
				if(DQS_test_flag==0){
					temp_i = (((GDMA_R0+1)-GDMA_L0)/2)+2;		// temp_i
					printf("temp_i=%d\n\r",temp_i);
					tftp_fail_flag=tftp_fail_flag1=decomp_fail_flag=decomp_fail_flag1=0;	//clean flag.					
					for(loop_cnt=0;loop_cnt<temp_i;loop_cnt++){	//fixed DQS1 value, this value from GDMA scan and got median range.
						/////////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x3e000000 )>>25)+loop_cnt;	//get DQS0 origi value and plus offset 
							if(DQS_temp<=GDMA_R0){
								if(tftp_fail_flag==0 || decomp_fail_flag==0){
									DQS_temp1 = (DQS_org & (~0x3e000000 )) | (DQS_temp<<25); // clean DQS0 value and set test value
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS0 parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(cali_method_1&0x1){
										if(tftp_fail_flag==0)
											if(TFTP_TEST()==1){
												(*result_tftp_dqs0) |= (1<<DQS_temp);
												}else{
													tftp_fail_flag++;
													}
										}
									if(cali_method_1&0x2){
										if(decomp_fail_flag==0)
											if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
												printf("decompress fail......\n\r");
												decomp_fail_flag++;	
											}else{
												printf("decompress ok......\n\r");
												(*result_decompress_dqs0) |= (1<<DQS_temp);
												}
										}
									////////////////////////////////////////////////////////
									if(cali_method_1&0x4){		
										if(MAC0_LBK_TEST()==1){
												printf("MAC0_LBK_TEST pass.......\n\r");										
												(*result_mac0_lbk_dqs0) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag0++;
												}
										}
									////////////////////////////////////////////////////////
									//*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
									//printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));									
									}
								tftp_fail_flag=decomp_fail_flag=mac0_lbk_fail_flag0=0;	// for mark high/low temperture test.
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();
						//////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x3e000000 )>>25)-loop_cnt;	//DQS0 original set offset 
							if(DQS_temp>=GDMA_L0){
								if(tftp_fail_flag1==0 || decomp_fail_flag1==0){								
									DQS_temp1 = (DQS_org & (~0x3e000000 )) | (DQS_temp<<25); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(cali_method_1&0x1){
										if(tftp_fail_flag1==0)
											if(TFTP_TEST()==1)
												(*result_tftp_dqs0) |= (1<<DQS_temp);
											else
												tftp_fail_flag1++;	
										}
									////////////////////////////////////////////////////////										
									if(cali_method_1&0x2){									
										if(decomp_fail_flag1==0)
											if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
												printf("decompress fail......\n\r");
												decomp_fail_flag1++;	
											}else{
												printf("decompress ok......\n\r");
												(*result_decompress_dqs0) |= (1<<DQS_temp);
												}
										}
									////////////////////////////////////////////////////////
									if(cali_method_1&0x4){											
										if(MAC0_LBK_TEST()==1){
											printf("MAC0_LBK_TEST pass.......\n\r");										
											(*result_mac0_lbk_dqs0) |= (1<<DQS_temp);
											}else{
												mac0_lbk_fail_flag0++;
												printf("MAC0_LBK_TEST fail.......\n\r");												
												}
										}
									}
								tftp_fail_flag1=decomp_fail_flag1=mac0_lbk_fail_flag1=0;	// for mark high/low temperture test.								
								//*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								//printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
						*(volatile unsigned int *)(0xb8001050)=DQS_org;	
						print_DDR_cali_result();			
						}
						DQS_test_status|=1;
						DQS_test_flag=1;
					}
					else{
					*(volatile unsigned int *)(0xb8001050)=DQS_org;					
					temp_i = (((GDMA_R1+1)-GDMA_L1)/2)+2;		// temp_i
					printf("temp_i=%d\n\r",temp_i);					
					tftp_fail_flag=tftp_fail_flag1=decomp_fail_flag=decomp_fail_flag1=0;	//clean flag.					
					for(loop_cnt=0;loop_cnt<temp_i;loop_cnt++){	//fixed DQS1 value, this value from GDMA scan and got median range.					
						/////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x01F00000 )>>20)+loop_cnt;	//DQS1 original set offset 
							if(DQS_temp<=GDMA_R1){	
								if(tftp_fail_flag==0 || decomp_fail_flag==0){
									DQS_temp1 = (DQS_org & (~0x01F00000 )) | (DQS_temp<<20); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(cali_method_1&0x1){									
										if(tftp_fail_flag==0)
											if(TFTP_TEST()==1)
												(*result_tftp_dqs1) |= (1<<DQS_temp);
											else
												tftp_fail_flag++;
										}
									///////////////////////////////////////////////////////
									if(cali_method_1&0x2){									
										if(decomp_fail_flag==0)
											if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
												printf("decompress fail......\n\r");
												decomp_fail_flag++;	
											}else{
												printf("decompress ok......\n\r");
												(*result_decompress_dqs1) |= (1<<DQS_temp);
												}
										}
									////////////////////////////////////////////////////////
									if(cali_method_1&0x4){										
										if(MAC0_LBK_TEST()==1){
												printf("MAC0_LBK_TEST pass.......\n\r");
												(*result_mac0_lbk_dqs1) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag1++;
												}
										}
									}
								tftp_fail_flag=decomp_fail_flag=0;	// for mark high/low temperture test.								
								//*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								//printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();							
						//////////////////////////////////////////////////////////////////////////////	
							DQS_temp = ((DQS_org & 0x01F00000 )>>20)-loop_cnt;	//DQS1 original set offset 
							if(DQS_temp>=GDMA_L1){
								if(tftp_fail_flag1==0 || decomp_fail_flag1==0){	
									DQS_temp1 = (DQS_org & (~0x01F00000 )) | (DQS_temp<<20); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(cali_method_1&0x1){									
										if(tftp_fail_flag1==0)
											if(TFTP_TEST()==1)
												(*result_tftp_dqs1) |= (1<<DQS_temp);								
											else
												tftp_fail_flag1++;
										}
									////////////////////////////////////////////////////////										
									if(cali_method_1&0x2){									
										if(decomp_fail_flag1==0)
											if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
												printf("decompress fail......\n\r");
												decomp_fail_flag1++;	
											}else{
												printf("decompress ok......\n\r");
												(*result_decompress_dqs1) |= (1<<DQS_temp);
												}
										}
									////////////////////////////////////////////////////////
									if(cali_method_1&0x4){	
										if(MAC0_LBK_TEST()==1){
												printf("MAC0_LBK_TEST pass.......\n\r");										
												(*result_mac0_lbk_dqs1) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag1++;
												}
										}
									}
								tftp_fail_flag1=decomp_fail_flag1=0;	// for mark high/low temperture test.								
								//*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								//printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();
						}		
					DQS_test_flag=0;
					DQS_test_status|=2;
					}
					//printf("DQS_test_status=%d,DQS_test_flag=%d \n\r",DQS_test_status,DQS_test_flag );				
				if(DQS_test_status==3){
				 	*(volatile unsigned int *)(0xb8001050)=DQS_org;
					printf("\n\r\n\r\n\r");					
					break;
					}				
				}
		}
}

#else
void DDR_timing_diff_dqs_check2(unsigned prioirty_dqs)
{
	unsigned src,temp=0,DLL0_temp=0,DQS_temp,DQS_temp1,loop_cnt,DQS_test_flag=0,DQS_test_status=0;
	unsigned GDMA_L0,GDMA_R0,GDMA_L1,GDMA_R1,temp_i,tftp_fail_flag=0,decomp_fail_flag=0,tftp_fail_flag1=0,decomp_fail_flag1=0;
	unsigned mac0_lbk_fail_flag0=0,mac0_lbk_fail_flag1=0;
	unsigned *result_decompress_dqs0;
	unsigned *result_tftp_dqs0;
	unsigned *result_decompress_dqs1;
	unsigned *result_tftp_dqs1;	
	unsigned *result_mac0_lbk_dqs0;
	unsigned *result_mac0_lbk_dqs1;
	unsigned DLL0_org=0,DCR_org,DQS_org,tx_delay_org;
	unsigned long entry;
	get_param(&bParam);
	src=bParam.app;
	entry=bParam.entry;
	//clean result
	result_decompress_dqs0 = 0xa0607f40;
	result_tftp_dqs0 = 0xa0607f44;
	*result_decompress_dqs0=0;
	*result_tftp_dqs0=0;
	result_decompress_dqs1 = 0xa0607f48;
	result_tftp_dqs1 = 0xa0607f4c;
	*result_decompress_dqs1=0;
	*result_tftp_dqs1=0;
	result_mac0_lbk_dqs0 = 0xa0607f50;
	result_mac0_lbk_dqs1 = 0xa0607f54;	
	*result_mac0_lbk_dqs0=0;
	*result_mac0_lbk_dqs1=0;
	//clean done
	//loading GDMA calibration value
	temp = (*(volatile unsigned int *)(0xb8003200)) & 0x0F;		//only for RLE0315 & RL6166 use.
	if(temp==0){		// get 133MHz calibration value.
		GDMA_L0=*(volatile unsigned int *)(0xa0607f30);
		GDMA_R0=*(volatile unsigned int *)(0xa0607f34);	
		GDMA_L1=*(volatile unsigned int *)(0xa0607f38);
		GDMA_R1=*(volatile unsigned int *)(0xa0607f3c);		
		}
	else{				// get others MHz calibration value.
		GDMA_L0=*(volatile unsigned int *)(0xa0607f20);
		GDMA_R0=*(volatile unsigned int *)(0xa0607f24);	
		GDMA_L1=*(volatile unsigned int *)(0xa0607f28);
		GDMA_R1=*(volatile unsigned int *)(0xa0607f2c);	
		}
	//loading GDMA calibration value, end line
	//check DDR or SDRAM onboard.
	temp = *(volatile unsigned int *)(0xb8001000);
	if(temp&0x80000000){
		DQS_org = *(volatile unsigned int *)(0xb8001050);
		DQS_test_flag=prioirty_dqs;
		TFTP_TEST();
			while(1){
				if(DQS_test_flag==0){
					temp_i = ((GDMA_R0+1)-GDMA_L0)/2;		// temp_i
					tftp_fail_flag=tftp_fail_flag1=decomp_fail_flag=decomp_fail_flag1=0;	//clean flag.					
					for(loop_cnt=0;loop_cnt<temp_i;loop_cnt++){	//fixed DQS1 value, this value from GDMA scan and got median range.
						/////////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x3e000000 )>>25)+loop_cnt;	//get DQS0 origi value and plus offset 
							if(DQS_temp<=GDMA_R0){
								if(tftp_fail_flag==0 || decomp_fail_flag==0){
									DQS_temp1 = (DQS_org & (~0x3e000000 )) | (DQS_temp<<25); // clean DQS0 value and set test value
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS0 parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									#if 1
									if(tftp_fail_flag==0)
										if(TFTP_TEST()==1){
											(*result_tftp_dqs0) |= (1<<DQS_temp);
											}else{
												tftp_fail_flag++;
												}
									#endif
									////////////////////////////////////////////////////////
									if(MAC0_LBK_TEST()==1){
											printf("MAC0_LBK_TEST pass.......\n\r");										
											(*result_mac0_lbk_dqs0) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag0++;
												}
									////////////////////////////////////////////////////////									
									if(decomp_fail_flag==0)
										if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
											printf("decompress fail......\n\r");
											decomp_fail_flag++;	
										}else{
											printf("decompress ok......\n\r");
											(*result_decompress_dqs0) |= (1<<DQS_temp);
											}
									*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
									printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));									
									}
								tftp_fail_flag=decomp_fail_flag=mac0_lbk_fail_flag0=0;	// for mark high/low temperture test.
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();
						//////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x3e000000 )>>25)-loop_cnt;	//DQS0 original set offset 
							if(DQS_temp>=GDMA_L0){
								if(tftp_fail_flag1==0 || decomp_fail_flag1==0){								
									DQS_temp1 = (DQS_org & (~0x3e000000 )) | (DQS_temp<<25); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									#if 1
									if(tftp_fail_flag1==0)
										if(TFTP_TEST()==1)
											(*result_tftp_dqs0) |= (1<<DQS_temp);
										else
											tftp_fail_flag1++;	
									#endif	
									////////////////////////////////////////////////////////
									if(MAC0_LBK_TEST()==1){
											printf("MAC0_LBK_TEST pass.......\n\r");										
											(*result_mac0_lbk_dqs0) |= (1<<DQS_temp);
											}else{
												mac0_lbk_fail_flag0++;
												printf("MAC0_LBK_TEST fail.......\n\r");												
												}
									////////////////////////////////////////////////////////										
									if(decomp_fail_flag1==0)
										if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
											printf("decompress fail......\n\r");
											decomp_fail_flag1++;	
										}else{
											printf("decompress ok......\n\r");
											(*result_decompress_dqs0) |= (1<<DQS_temp);
											}
									}
								tftp_fail_flag1=decomp_fail_flag1=mac0_lbk_fail_flag1=0;	// for mark high/low temperture test.								
								*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
						*(volatile unsigned int *)(0xb8001050)=DQS_org;	
						print_DDR_cali_result();			
						}
						DQS_test_status|=1;
						DQS_test_flag=1;
					}
					else{
					*(volatile unsigned int *)(0xb8001050)=DQS_org;
					temp_i = ((GDMA_R1+1)-GDMA_L1)/2;		// temp_i
					tftp_fail_flag=tftp_fail_flag1=decomp_fail_flag=decomp_fail_flag1=0;	//clean flag.					
					for(loop_cnt=0;loop_cnt<temp_i;loop_cnt++){	//fixed DQS1 value, this value from GDMA scan and got median range.					
						/////////////////////////////////////////////////////////////////////////////
							DQS_temp = ((DQS_org & 0x01F00000 )>>20)+loop_cnt;	//DQS1 original set offset 
							if(DQS_temp<=GDMA_R1){	
								if(tftp_fail_flag==0 || decomp_fail_flag==0){
									DQS_temp1 = (DQS_org & (~0x01F00000 )) | (DQS_temp<<20); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(tftp_fail_flag==0)
										if(TFTP_TEST()==1)
											(*result_tftp_dqs1) |= (1<<DQS_temp);
										else
											tftp_fail_flag++;
									////////////////////////////////////////////////////////
									if(MAC0_LBK_TEST()==1){
											printf("MAC0_LBK_TEST pass.......\n\r");
											(*result_mac0_lbk_dqs1) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag1++;
												}
									///////////////////////////////////////////////////////
									if(decomp_fail_flag==0)
										if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
											printf("decompress fail......\n\r");
											decomp_fail_flag++;	
										}else{
											printf("decompress ok......\n\r");
											(*result_decompress_dqs1) |= (1<<DQS_temp);
											}
									}
								tftp_fail_flag=decomp_fail_flag=0;	// for mark high/low temperture test.								
								*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();							
						//////////////////////////////////////////////////////////////////////////////	
							DQS_temp = ((DQS_org & 0x01F00000 )>>20)-loop_cnt;	//DQS1 original set offset 
							if(DQS_temp>=GDMA_L1){
								if(tftp_fail_flag1==0 || decomp_fail_flag1==0){	
									DQS_temp1 = (DQS_org & (~0x01F00000 )) | (DQS_temp<<20); 
									*(volatile unsigned int *)(0xb8001050)=DQS_temp1;	//write DQS parameter
									printf("DQS0=0x%x ,DQS1=0x%x\n\r",(*(volatile unsigned int *)(0xb8001050)&0x3e000000)>>25,(*(volatile unsigned int *)(0xb8001050)&0x01F00000)>>20); //read DQS parameter
									if(tftp_fail_flag1==0)
										if(TFTP_TEST()==1)
											(*result_tftp_dqs1) |= (1<<DQS_temp);								
										else
											tftp_fail_flag1++;
									////////////////////////////////////////////////////////
									if(MAC0_LBK_TEST()==1){
											printf("MAC0_LBK_TEST pass.......\n\r");										
											(*result_mac0_lbk_dqs1) |= (1<<DQS_temp);
											}else{
												printf("MAC0_LBK_TEST fail.......\n\r");											
												mac0_lbk_fail_flag1++;
												}
									////////////////////////////////////////////////////////										
									if(decomp_fail_flag1==0)
										if(run_gzip_file_test(src,entry)==ERROR){			//check decompress 
											printf("decompress fail......\n\r");
											decomp_fail_flag1++;	
										}else{
											printf("decompress ok......\n\r");
											(*result_decompress_dqs1) |= (1<<DQS_temp);
											}
									}
								tftp_fail_flag1=decomp_fail_flag1=0;	// for mark high/low temperture test.								
								*(volatile unsigned int *)(0xa0100000)=0x5a5aa5a5;
								printf("write=0x5a5aa5a5, read=0x%x \n\r",*(volatile unsigned int *)(0xa0100000));
								}
							*(volatile unsigned int *)(0xb8001050)=DQS_org;	
							print_DDR_cali_result();
						}		
					DQS_test_flag=0;
					DQS_test_status|=2;
					}
					//printf("DQS_test_status=%d,DQS_test_flag=%d \n\r",DQS_test_status,DQS_test_flag );				
				if(DQS_test_status==3){
				 	*(volatile unsigned int *)(0xb8001050)=DQS_org;
					printf("\n\r\n\r\n\r");					
					break;
					}				
				}
		}
}
#endif
#endif	//#ifdef	CONFIG_MAC0_LBK


#ifdef	CONFIG_GDMA_SCAN
int RANDOM_GEN(unsigned long addr,unsigned int len){
		unsigned long start=0;	
		unsigned char	*random_addr_1,*random_addr_2;
		unsigned int random_len,i=0,j,random_num1=0,random_num2=0,random_num3=0,random_data_cnt=0;
		random_len = len-len%4;
		random_addr_1 = (unsigned char *)addr;
		random_addr_2 = random_addr_1+(random_len/2);
		
		for(random_data_cnt=0;random_data_cnt<random_len/2;){
			random_num1 = get_tc1cnt();
			start = get_sys_time();
			while(!timeout(start, (random_num1%32)))   //delay random us
			random_num2 ^= (random_num1 | (get_tc1cnt()<<8));
			if(random_num1<10)	// avoid random_num1=0
				random_num1 = ttdata[ random_num2%(random_num1+10)+50];
			else
				random_num1 = ttdata[ random_num2%(random_num1)+50];
			random_num2 ^= random_num1 ;
			//printf("random_num2 0x%8x\n",random_num2);
			//===============================================
			//printf("random_data_pool %p length %d\n", random_data_pool, random_data_max_pool_size);
			//===============================================
			random_num3 = (~(random_num2 << 16));
			random_num3 &=0xffff0000;
			random_num3 |= random_num2;
			//printf("random_num3 0x%8x\n",random_num3);
			WRITE_MEM32(random_addr_1+random_data_cnt,random_num3);		//set random value to memory
			WRITE_MEM32(random_addr_2+random_data_cnt,~random_num3);
			random_num3 ^= (~random_num3) >> 4 ;
			WRITE_MEM32(random_addr_1+random_data_cnt+4,random_num3);	//set random value to memory
			WRITE_MEM32(random_addr_2+random_data_cnt+4,~random_num3);
			random_data_cnt+=8;

			}
#if 0		//dump random data pool		
		printf("00   01   02   03   04   05   06   07   08   09   0a   0b   0c   0d   0e   0f");
			  for (i=0; i<random_data_max_pool_size; i++){
					if(i%16==0) 
					printf("\n");			
					printf("0x%02x ", *(random_addr_1+i));
					}
			//===============================================
#endif			
		return TRUE;
}

int GDMA_SDRAM_SCAN(unsigned long sdram_size) {
		unsigned int i,j,cmp_i,sdram_test_result_fail_cnt=0,sdram_test_result_pass_cnt=0,offset_2b;
		unsigned int total_blocks=0,fail_block[100],current_data_length=0,src_data=0,cmp_data=0;
		unsigned long total_sdram_size=0;

		unsigned char	*random_addr_src;
		total_sdram_size = sdram_size ;
		current_data_length = (get_tc1cnt_dword()%random_data_max_pool_size);			//get the random value and assign current data length	
		current_data_length -= current_data_length%8;
		total_blocks = total_sdram_size / current_data_length;
		random_addr_src = (unsigned char *)(random_data_pool_src);
		printf("Scan address from %p to %p ,check block size %d  and blocks %d\n",random_data_pool_src,random_data_pool_src+(total_blocks+1)*current_data_length,current_data_length,total_blocks);
		if (RANDOM_GEN(random_data_pool_src,current_data_length)){		 
			//printf("random_data_src %p length %d\n", random_addr_src, current_data_length);			
#if 0				
			  	for (i=0; i<current_data_length; i++){
					if(i%16==0) 
						printf("\n");			
						printf("0x%02x ", *(random_data_pool_src+i));
					}
#endif				
			}

#if 1		
		for(i=0;i<total_blocks;i++){
			random_data_move_point = random_addr_src+current_data_length*i;
			//printf("random_data_move_point %p\r\n",random_data_move_point);
			//============RL6028 enable GDMA module ================		
			WRITE_MEM32(0xb800330c, READ_MEM32(0xb800330c)|0x400 );		//enable GDMA module (n10)from module enable control register(0xb800330c) .
			//printf("0xb800330c=0x%x \n",READ_MEM32(0xb800300c));
			//WRITE_MEM32(0xb8003000, READ_MEM32(0xb8003000)|0x200000 );	// GDMA interrupt enable (n21)from GIMR (0xb8003000) .
			//WRITE_MEM32(0xb8003004, READ_MEM32(0xb8003004)|0x200000 );	// GDMA interrupt pending flag (n21)from GISR (0xb8003004) .
			//WRITE_MEM32(0xb8003010, READ_MEM32(0xb8003010)|0x200000 );	// GDMA interrupt route select (n23-n20)from IRR (0xb8003010) .
			//============GDMA memory copy  =====================	
			//printf("break point 1\n");
			WRITE_MEM32(GDMACNR, 0);
			WRITE_MEM32(GDMACNR, GDMA_ENABLE | GDMA_MEMCPY);				
			WRITE_MEM32(GDMAIMR, 0);
			WRITE_MEM32(GDMAISR, READ_MEM32(GDMAISR));						//write 1 to clear 
			//WRITE_MEM32(GDMAICVL, 0);
			//WRITE_MEM32(GDMAICVR, 0);
			//WRITE_MEM32(GDMASBP(0), 0xA0204000);
			//printf("break point 2\n");
			WRITE_MEM32(GDMASBP(0), random_addr_src);						//source data point
			WRITE_MEM32(GDMASBL(0), current_data_length | GDMA_LDB );		// source data length
			//printf("break point 3\n");
			WRITE_MEM32(GDMADBP(0), random_data_move_point);				//destination data point
			WRITE_MEM32(GDMADBL(0), current_data_length | GDMA_LDB );		// destination data length
			//printf("break point 4\n");
			WRITE_MEM32(GDMACNR, GDMA_ENABLE | GDMA_POLL | GDMA_MEMCPY);	//startting  GDMA process
			//printf("break point 5\n");
			while (!(READ_MEM32(GDMAISR) & 0x80000000)) ;					//polling GDMA copy done flag
			//delay_msec(10);
			//===============================================		
			//printf("random_data_destination %p length %d\n", random_data_move_point, current_data_length);
			//printf("break point 6\n\r");
			}
		//WRITE_MEM32(random_data_move_point+16,0x12345678);						//write some data to any destination address ,verification gdma compare function .
		//============GDMA memory comparing ==================	
			sdram_test_result_fail_cnt=sdram_test_result_pass_cnt=0;		//reset result

			//
			//
		for(i=0;i<total_blocks;i++){
			random_data_move_point = random_addr_src+current_data_length*i;
			//printf("random_data_move_point %p\r\n",random_data_move_point);
			for(cmp_i=0;cmp_i<current_data_length;){
				//printf("src = 0x%x  ,dsc = 0x%x ,cmp_i=%d\n",random_addr_src+cmp_i,random_data_move_point+cmp_i,cmp_i);
				//if(REG32(random_addr_src+cmp_i)==REG32(random_data_move_point+cmp_i))
				src_data=READ_MEM32(random_addr_src+cmp_i);
				cmp_data=READ_MEM32(random_data_move_point+cmp_i);
				if(src_data==cmp_data)
					sdram_test_result_pass_cnt++;
				else{
					printf("src_data = 0x%x ,err_data = 0x%x \n",src_data,cmp_data);
					return FALSE;
					}
				cmp_i+=4;
				}
			}
			//printf("sdram_test_result_pass_cnt=%d\n",sdram_test_result_pass_cnt);
			return TRUE;
		//===============================================
#if 0	//dump random data after move 
				printf("00   01   02   03   04   05   06   07   08   09   0a   0b   0c   0d   0e   0f");
	            for (i=0; i<current_data_length; i++){
					if(i%16==0)	
						printf("\n");			
						printf("0x%02x ", *(random_data_move_point+i));
	            	}		
#endif


			//printf("Total blocks %d ,sdram_test_result_pass_cnt %d ,sdram_test_result_fail_cnt %d \r\n",total_blocks,sdram_test_result_pass_cnt,sdram_test_result_fail_cnt);
#endif			
		}

#endif


#if 0 // shrink bootloader size, remove unnecessary code
//---------------------------------------------------------------------
  #define GDMA_BASE    0xB800A000
      #define GDMACNR      (GDMA_BASE + 0x00)
      #define GDMAIMR      (GDMA_BASE + 0x04)
      #define GDMAISR      (GDMA_BASE + 0x08)
      #define GDMAICVL     (GDMA_BASE + 0x0C)
      #define GDMAICVR     (GDMA_BASE + 0x10)
      #define GDMASBP(i)   (GDMA_BASE + 0x20 + ((i) << 3))
      #define GDMASBL(i)   (GDMA_BASE + 0x24 + ((i) << 3))
      #define GDMADBP(i)   (GDMA_BASE + 0x60 + ((i) << 3))
      #define GDMADBL(i)   (GDMA_BASE + 0x64 + ((i) << 3))
  /* GDMA - Control Register */
   #define GDMA_ENABLE     (1<<31)         /* Enable GDMA */
   #define GDMA_POLL       (1<<30)         /* Kick off GDMA */
   #define GDMA_FUNCMASK   (0xf<<24)       /* GDMA Function Mask */
   #define GDMA_MEMCPY     (0x0<<24)       /* Memory Copy */
   #define GDMA_CHKOFF     (0x1<<24)       /* Checksum Offload */
   #define GDMA_STCAM      (0x2<<24)       /* Sequential T-CAM */
   #define GDMA_MEMSET     (0x3<<24)       /* Memory Set */
   #define GDMA_B64ENC     (0x4<<24)       /* Base 64 Encode */
   #define GDMA_B64DEC     (0x5<<24)       /* Base 64 Decode */
   #define GDMA_QPENC      (0x6<<24)       /* Quoted Printable Encode */
   #define GDMA_QPDEC      (0x7<<24)       /* Quoted Printable Decode */
   #define GDMA_MIC        (0x8<<24)       /* Wireless MIC */
   #define GDMA_MEMXOR     (0x9<<24)       /* Memory XOR */
   #define GDMA_MEMCMP     (0xa<<24)       /* Memory Compare */
   #define GDMA_BYTESWAP   (0xb<<24)       /* Byte Swap */
   #define GDMA_PATTERN    (0xc<<24)       /* Pattern Match */
   #define GDMA_SWAPTYPE0  (0<<22)         /* Original:{0,1,2,3} => {1,0,3,2} */
   #define GDMA_SWAPTYPE1  (1<<22)         /* Original:{0,1,2,3} => {3,2,1,0} */
   #define GDMA_ENTSIZMASK (3<<20)         /* T-CAM Entry Size Mask */
   #define GDMA_ENTSIZ32   (0<<20)         /* T-CAM Entry Size 32 bits */
   #define GDMA_ENTSIZ64   (1<<20)         /* T-CAM Entry Size 64 bits */
   #define GDMA_ENTSIZ128  (2<<20)         /* T-CAM Entry Size 128 bits */
   #define GDMA_ENTSIZ256  (3<<20)         /* T-CAM Entry Size 256 bits */

#if 0
#define GDMA_ENABLE	1<<31
#define GDMA_MEMCPY	0<<24
#define GDMA_LDB		1<<31
#define GDMA_POLL		1<<30
#endif

/* DMEM-DMA */
#define DDMA_BASE       0xB8006000

#define DDMA_DMEM_ADDR  (DDMA_BASE + 0x00)
#define DDMA_DRAM_ADDR  (DDMA_BASE + 0x04)
#define DDMA_CTRL       (DDMA_BASE + 0x08)

/* IMEM-DMA */
#define IDMA_BASE       0xB8006800
//test 0x100 size
#define IMEM_SIZE		0x1000
#define DMEM_SIZE		0x1000
#define IDMA_DMEM_ADDR  (IDMA_BASE + 0x00)
#define IDMA_DRAM_ADDR  (IDMA_BASE + 0x04)
#define IDMA_CTRL       (IDMA_BASE + 0x08)

#define MIN(x,y) (x < y ? x : y)
void cop3_use(void)
{
	/* COP3 Usable */
	__asm__ __volatile__ (
		"mfc0 $7, $12\n\t"
		"or   $7, 0x80000000\n\t"
		"mtc0 $7, $12\n\t"
		"nop\n\t"
		"nop\n\t"
		"li   $7, %0\n\t"
		"li   $6, %1\n\t"
		"mtc3 $7, $0\n\t"
		"nop\n\t"
		"mtc3 $6, $1\n\t"
		"nop\n\t"
		:
		: "i" (0x00C20000), "i" (0x00C20000 + IMEM_SIZE - 1)
	);
}

void dmem_on(void)
{
	/* DMEM On */
	__asm__ __volatile__ (
		"li   $7, 0x00000010\n\t"
		"mtc0 $7, $20\n\t"
	);
}

void dmem_off(void)
{
	/* DMEM Off */
	__asm__ __volatile__ (
		"li   $7, 0x00000020\n\t"
		"mtc0 $7, $20\n\t"
		: : : "$7"
	);
}

void imem_on(void)
{
	/* IMEM On */
	__asm__ __volatile__ (
		"li   $7, 0x00000010\n\t"
		"mtc0 $7, $20\n\t"
	);
}

void imem_off(void)
{
	/* IMEM Off */
	__asm__ __volatile__ (
		"li   $7, 0x00000020\n\t"
		"mtc0 $7, $20\n\t"
		: : : "$7"
	);
}

void dmemdmatest(void)
{
	unsigned int i;
	unsigned int cnt;
	unsigned int copied_words = 1;

	cop3_use();

	while (1)
	{
		if (copied_words > MIN((DMEM_SIZE >> 2), 8191))
			break;

		/* Fill SDRAM */
		i = 0xA0620000;

		while (1)
		{
			if (i >= (0xA0620000 + (copied_words << 2)))
				break;

			*(volatile unsigned int *) (i) = i;
			i += 4;
		}

		/* Clear DMEM Mapped DRAM */
		i = 0xA0C20000;

		while (1)
		{
			if (i >= (0xA0C20000 + (copied_words << 2)))
				break;

			*(volatile unsigned int *) (i) = 0x0;
			i += 4;
		}

		imem_on();

		#if 1
		/* Reset IMEM DMA */
		*(volatile unsigned int *) DDMA_CTRL = 0x00008000;
		*(volatile unsigned int *) DDMA_CTRL = 0x00000000;
		#endif

		/* Invoke SDRAM to IMEM copy */
		*(volatile unsigned int *) DDMA_DMEM_ADDR = 0;
		*(volatile unsigned int *) DDMA_DRAM_ADDR = 0x00620000;
		*(volatile unsigned int *) DDMA_CTRL = (1 << 14) | (1 << 13) | copied_words;

		/* Wait for DMA Completion */
		while ((*(volatile unsigned int *) DDMA_CTRL) & 0x4000) ;

		#if 1
		/* Reset IMEM DMA */
		*(volatile unsigned int *) DDMA_CTRL = 0x00008000;
		*(volatile unsigned int *) DDMA_CTRL = 0x00000000;
		#endif

		/* Invoke IMEM to SDRAM copy */
		*(volatile unsigned int *) DDMA_DMEM_ADDR = 0;
		*(volatile unsigned int *) DDMA_DRAM_ADDR = 0x00520000;
		*(volatile unsigned int *) DDMA_CTRL = (1 << 14) | (0 << 13) | copied_words;

		/* Wait for DMA Completion */
		while ((*(volatile unsigned int *) DDMA_CTRL) & 0x4000) ;

		/* Memory Compare */
		i = 0xA0620000;

		while (1)
		{
			unsigned int j = (*(volatile unsigned int *) (i));
			unsigned int k = (*(volatile unsigned int *) (i - 0x00100000));

			if (i >= (0xA0620000 + (copied_words << 2)))
				break;

			if (j != k)
			{
				printf("\n==> Copied Word: %d", copied_words);
				printf("\n==> SDRAM to DMEM DMA Failed!");

				printf("\n====>               SDRAM@ 0x%08X == 0x%08X", i, j);
				printf("\n====> DMEM's SDRAM Shadow@ 0x%08X == 0x%08X", i - 0x00100000, k);
			}

			i += 4;
		}

		imem_off();

		copied_words += 1;
	}

	printf("DMEM DMA Finished\n\r");
}
void imemdmatest(void)
{
	unsigned int i;
	unsigned int cnt;
	unsigned int copied_words = 1;

	cop3_use();

	while (1)
	{
		if (copied_words > MIN((IMEM_SIZE >> 2), 8191))
			break;

		/* Fill SDRAM */
		i = 0xA0620000;

		while (1)
		{
			if (i >= (0xA0620000 + (copied_words << 2)))
				break;

			*(volatile unsigned int *) (i) = i;
			i += 4;
		}

		/* Clear IMEM Mapped DRAM */
		i = 0xA0C20000;

		while (1)
		{
			if (i >= (0xA0C20000 + (copied_words << 2)))
				break;

			*(volatile unsigned int *) (i) = 0x0;
			i += 4;
		}

		imem_on();

		#if 1
		/* Reset IMEM DMA */
		*(volatile unsigned int *) IDMA_CTRL = 0x00008000;
		*(volatile unsigned int *) IDMA_CTRL = 0x00000000;
		#endif

		/* Invoke SDRAM to IMEM copy */
		*(volatile unsigned int *) IDMA_DMEM_ADDR = 0;
		*(volatile unsigned int *) IDMA_DRAM_ADDR = 0x00620000;
		*(volatile unsigned int *) IDMA_CTRL = (1 << 14) | (1 << 13) | copied_words;

		/* Wait for DMA Completion */
		while ((*(volatile unsigned int *) IDMA_CTRL) & 0x4000) ;

		#if 1
		/* Reset IMEM DMA */
		*(volatile unsigned int *) IDMA_CTRL = 0x00008000;
		*(volatile unsigned int *) IDMA_CTRL = 0x00000000;
		#endif

		/* Invoke IMEM to SDRAM copy */
		*(volatile unsigned int *) IDMA_DMEM_ADDR = 0;
		*(volatile unsigned int *) IDMA_DRAM_ADDR = 0x00520000;
		*(volatile unsigned int *) IDMA_CTRL = (1 << 14) | (0 << 13) | copied_words;

		/* Wait for DMA Completion */
		while ((*(volatile unsigned int *) IDMA_CTRL) & 0x4000) ;

		/* Memory Compare */
		i = 0xA0620000;

		while (1)
		{
			unsigned int j = (*(volatile unsigned int *) (i));
			unsigned int k = (*(volatile unsigned int *) (i - 0x00100000));

			if (i >= (0xA0620000 + (copied_words << 2)))
				break;

			if (j != k)
			{
				printf("\n==> Copied Word: %d", copied_words);
				printf("\n==> SDRAM to IMEM DMA Failed!");

				printf("\n====>               SDRAM@ 0x%08X == 0x%08X", i, j);
				printf("\n====> IMEM's SDRAM Shadow@ 0x%08X == 0x%08X", i - 0x00100000, k);
			}

			i += 4;
		}

		imem_off();

		copied_words += 1;
	}

	printf("IMEM DMA Finished\n\r");
}

//////////for performance evaluation
#define SRC_ADDR  0x80204000
#define DST_ADDR  0x80A04000

//#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
//#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

volatile unsigned long long CNT0, CNT1, CNT2, CNT3;

inline void CP3_COUNTER_INIT( void )
{
__asm__ volatile\
("  ;\
	mfc0	$8, $12			;\
	nop						;\
	la		$9, 0x80000000	;\
	or		$8, $9			;\
	mtc0	$8, $12			;\
	nop						;\
	nop						;\
");
}


inline void CP3_COUNTER_CLEAR( void )
{
__asm__ volatile\
("  ;\
	mtc3		$0, $8		;\
	nop						;\
	nop						;\
	mtc3		$0, $9		;\
	nop						;\
	nop						;\
	mtc3		$0, $10		;\
	nop						;\
	nop						;\
	mtc3		$0, $11		;\
	nop						;\
	nop						;\
	mtc3		$0, $12		;\
	nop						;\
	nop						;\
	mtc3		$0, $13		;\
	nop						;\
	nop						;\
	mtc3		$0, $14		;\
	nop						;\
	nop						;\
	mtc3		$0, $15		;\
	nop						;\
	nop						;\
");
}


inline void CP3_COUNTER_START1( void )
{
__asm__ \
("  ;\
	li		$8, 0b10011000100100001000100010000	;\
	ctc3 	$8, $0			;\
	nop						;\
");
}


inline void CP3_COUNTER_STOP( void )
{
__asm__ volatile\
("	;\
	ctc3 	$0, $0			;\
	nop						;\
");
}


inline void CP3_COUNTER_GET( void )
{
__asm__ volatile\
("	;\
	mfc3	$9, $9			;\
	nop						;\
	la		$8, CNT0			;\
	sw		$9, 0($8)			;\
	mfc3	$9, $8			;\
	nop						;\
	nop						;\
	sw		$9, 4($8)			;\
	mfc3	$9, $11			;\
	nop						;\
	la		$8, CNT1			;\
	sw		$9, 0($8)			;\
	mfc3	$9, $10			;\
	nop						;\
	nop						;\
	sw		$9, 4($8)			;\
	mfc3	$9, $13			;\
	nop						;\
	la		$8, CNT2			;\
	sw		$9, 0($8)			;\
	mfc3	$9, $12			;\
	nop						;\
	nop						;\
	sw		$9, 4($8)			;\
	mfc3	$9, $15			;\
	nop						;\
	la		$8, CNT3			;\
	sw		$9, 0($8)			;\
	mfc3	$9, $14			;\
	nop						;\
	nop						;\
	sw		$9, 4($8)			;\
");
}


inline void DCACHE_WRITEBACK_INVALIDATE( void )
{
	/* Invalidate I-Cache */
	__asm__ volatile(
		"mtc0 $0,$20\n\t"
		"nop\n\t"
		"li $8,512\n\t"
		"mtc0 $8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"mtc0 $0,$20\n\t"
		"nop"
		: /* no output */
		: /* no input */
		);
}


int test_memcpy()
{
	unsigned int total_bytes = 0x100000; // 1MB
	unsigned int i, j;

	printf("\n");

	// Set Initial Value
	for (i = SRC_ADDR, j = DST_ADDR; i < (SRC_ADDR + total_bytes); i += 4, j += 4)
	{
		WRITE_MEM32(i,  i);
		WRITE_MEM32(j,  j);
	}

	DCACHE_WRITEBACK_INVALIDATE();
	CP3_COUNTER_INIT();
	CP3_COUNTER_CLEAR();
	CP3_COUNTER_START1();

	for (i = SRC_ADDR, j = DST_ADDR; i < (SRC_ADDR + total_bytes); i += 32, j += 32)
	{
		WRITE_MEM32(j + 0,  READ_MEM32(i + 0));
		WRITE_MEM32(j + 4,  READ_MEM32(i + 4));
		WRITE_MEM32(j + 8,  READ_MEM32(i + 8));
		WRITE_MEM32(j + 12, READ_MEM32(i + 12));
		WRITE_MEM32(j + 16, READ_MEM32(i + 16));
		WRITE_MEM32(j + 20, READ_MEM32(i + 20));
		WRITE_MEM32(j + 24, READ_MEM32(i + 24));
		WRITE_MEM32(j + 28, READ_MEM32(i + 28));
	}

	DCACHE_WRITEBACK_INVALIDATE();
	CP3_COUNTER_STOP();
	CP3_COUNTER_GET();

	printf( " %10s, %10s, %12s, %12s:\n(%4u %10u, %4u %10u, %4u %10u, %4u %10u)\n",
				"Cycles", "Insts", "Missed Insts", "Missed Cycles", 
		 *((unsigned long *)&CNT0), *((unsigned long *)&CNT0 + 1),
		 *((unsigned long *)&CNT1), *((unsigned long *)&CNT1 + 1),
		 *((unsigned long *)&CNT2), *((unsigned long *)&CNT2 + 1),
		 *((unsigned long *)&CNT3), *((unsigned long *)&CNT3 + 1)
	 );

	// Check Memcpy Value
	for (i = SRC_ADDR, j = DST_ADDR; i < (SRC_ADDR + total_bytes); i += 4, j += 4)
	{
		if (READ_MEM32(j) != i)
		{
			printf("Error at 0x%08X, Expected to be 0x%08X\n", j, i);
		}
	}
}


int test_memset()
{
	unsigned int total_bytes = 0x800000; // 8MB
	unsigned int i, j;

	printf("\n");
	printf("\n==> CPU Memset\n");

	// Set Initial Value
	for (j = DST_ADDR; j < (DST_ADDR + total_bytes); j += 4)
	{
		WRITE_MEM32(j,  j);
	}

	DCACHE_WRITEBACK_INVALIDATE();
	CP3_COUNTER_INIT();
	CP3_COUNTER_CLEAR();
	CP3_COUNTER_START1();

	for (j = DST_ADDR; j <= (DST_ADDR + total_bytes); j += 32)
	{
		WRITE_MEM32(j+0, 0x1);
		WRITE_MEM32(j+4, 0x1);
		WRITE_MEM32(j+8, 0x1);
		WRITE_MEM32(j+12, 0x1);
		WRITE_MEM32(j+16, 0x1);
		WRITE_MEM32(j+20, 0x1);
		WRITE_MEM32(j+24, 0x1);
		WRITE_MEM32(j+28, 0x1);
	}

	DCACHE_WRITEBACK_INVALIDATE();
	CP3_COUNTER_STOP();
	CP3_COUNTER_GET();

	printf( " %10s, %10s, %12s, %12s:\n(%4u %10u, %4u %10u, %4u %10u, %4u %10u)\n",
				"Cycles", "Insts", "Missed Insts", "Missed Cycles", 
		 *((unsigned long *)&CNT0), *((unsigned long *)&CNT0 + 1),
		 *((unsigned long *)&CNT1), *((unsigned long *)&CNT1 + 1),
		 *((unsigned long *)&CNT2), *((unsigned long *)&CNT2 + 1),
		 *((unsigned long *)&CNT3), *((unsigned long *)&CNT3 + 1)
	 );

	// Check Memset Value
	for (j = DST_ADDR; j < (DST_ADDR + total_bytes); j += 4)
	{
		if (READ_MEM32(j) != 0x1)
		{
			printf("Error at 0x%08X, Expected to be 0x%08X\n", j, 0x1);
		}
	}
}

void dumpflash(){
	int i;
	for(i=0;i<900000;i++){
		if(i%0x10==0 && i!=0)
			printf("\n\r");
		printf("%2x ",*(volatile unsigned char*)(FLASH_BASE_ADDR+0x10000+i));
	}
}
int spi_state=0;
#endif //#if 0  shrink bootloader size, remove unnecessary code


struct sdram_config{
	char row_cnt[5];
	char col_cnt[5];
	char bank_cnt[8];
};
struct sdram_config current_dcr;
int parse_dcr(unsigned int dcr_value){
	unsigned int val,result=1;
	//printf("dcr:%x\n\r",dcr_value);
	//find row count
	val = ((dcr_value&0x06000000)>>25);
	//printf("row:%d\n\r",val);
	switch(val){
		case 0:
			strcpy(current_dcr.row_cnt,"2K");
			break;
		case 1:
			strcpy(current_dcr.row_cnt,"4K");
			break;
		case 2:
			strcpy(current_dcr.row_cnt,"8K");
			break;
		case 3:
			strcpy(current_dcr.row_cnt,"16K");
			break;
		default:
			result=0;
			//printf("error DCR setting!\n\r");
			break;
	}

	//find col count
	val = ((dcr_value&0x01c00000)>>22);
	//printf("col:%d\n\r",val);
	switch(val){
		case 0:
			strcpy(current_dcr.col_cnt,"256");
			break;
		case 1:
			strcpy(current_dcr.col_cnt,"512");
			break;
		case 2:
			strcpy(current_dcr.col_cnt,"1K");
			break;
		case 3:
			strcpy(current_dcr.col_cnt,"2K");
			break;
		case 4:
			strcpy(current_dcr.col_cnt,"4K");
			break;
		default:
			result=0;
			//printf("error DCR setting!\n\r");
			break;
	}

	//find bank count
	val = ((dcr_value&0x00080000)>>19);
	//printf("bank:%d\n\r",val);
	switch(val){
		case 0:
			strcpy(current_dcr.bank_cnt,"2Banks");
			break;
		case 1:
			strcpy(current_dcr.bank_cnt,"4Banks");
			break;
		default:
			break;
	}
	return result;
}


#ifndef CONFIG_NO_FLASH
char flash_size[8];
int parse_flash_size(unsigned int nfcr){
	int result=1,size=((nfcr & 0x00070000)>>16);
	switch(size){
		case 0:
			strcpy(flash_size,"256K");
			break;
		case 1:
			strcpy(flash_size,"512K");
			break;
		case 2:
			strcpy(flash_size,"1M");
			break;
		case 3:
			strcpy(flash_size,"2M");
			break;
		case 4:
			strcpy(flash_size,"4M");
			break;
		case 5:
			strcpy(flash_size,"8M");
			break;
		case 6:
			strcpy(flash_size,"16M");
			break;
		default:
			result=0;
			//printf("error DCR setting!\n\r");
			break;
	}
	return result;
}
/////////
#endif /*CONFIG_NO_FLASH*/

char xstring[1024];
#define MAX_PARTITION 3
typedef struct partition_info_t
{
	int			valid;	/*1: valid, 0: invalid*/
	int			type;	/*boot/image/config*/
#define PARTI_TYPE_BOOT 0x1
#define PARTI_TYPE_IMG	0x2
#define PARTI_TYPE_CONF	0x4
	unsigned int		addrstart;
	unsigned int		addrend;
	unsigned char		cover_flag;
}partition_info;

extern partition_info partiton_info_array[MAX_PARTITION];
extern void decode(char *buf, int buflen);
typedef struct flash_config_header{
	unsigned int length;
	unsigned int checksum;
}FLASH_CFG_HDR;


//command handlers
static int handler_bootline(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	char bbuf[BOARD_BOOTLINE_SZ];

	printf("Please enter new bootline: ");
	kbd_proc(bbuf, BOARD_BOOTLINE_SZ);
	get_param(&bParam);
	strcpy(bParam.bootline, bbuf);
	set_param(&bParam);

	return 0;
}

static int handler_d(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned long daddr;
	int dlen;
#ifdef BANK_AUTO_SWITCH
	unsigned int oldbank, currbank;
	unsigned int offset;
#endif
	unsigned long raddr;
	int tmpLen, offlen=0;

	if(strlen(argv[1])) // address field present
		daddr = str2UL(argv[1]);
	else
		daddr = sd_dump_default_addr;
			
	if(strlen(argv[2])) // len field present
	{
		dlen = str2UL(argv[2]);
		//if(dlen>256)
		//	dlen = 256;
	}
	else
		dlen = 128;
	
	raddr = daddr;
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= raddr - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);

	currbank = getbank();

	if (currbank == 1)
		raddr = (unsigned int)raddr - 0x400000;
	else if (currbank == 2)
		raddr = (unsigned int)raddr - 0x800000;
	else if (currbank == 3)
		raddr = (unsigned int)raddr - 0xC00000;
#endif
	//dump_mem(raddr, dlen);
			
	do {
		tmpLen = 0xC0000000 - (unsigned int)raddr;
		if (tmpLen > (dlen-offlen)) tmpLen = (dlen-offlen);			
		dump_mem(raddr, tmpLen);				
		offlen += tmpLen;	
		if (offlen >= dlen) break;				
		raddr += tmpLen;
		
#ifdef BANK_AUTO_SWITCH
		currbank++;
		s29gl128bank(currbank);
		raddr = FLASH_BASE_ADDR;
#endif
	} while(1);
	
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
	sd_dump_default_addr = daddr + dlen;

	return 0;
}

static int handler_entry(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	get_param(&bParam);
	bParam.entry = str2UL(argv[1]);
	set_param(&bParam);

	return 0;
}

#ifndef CONFIG_NO_FLASH
static int handler_flashsize(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int result=1;
	int nfcr=0xfff88000;
#if 0
	int intval;
	if(argv[1]){
		intval = str2UL(argv[1]);
		//Only support 1M/2M/4M/8M flash
		if(intval==1||intval==2||intval==4||intval==8) {
			get_param(&bParam);
			bParam.flash_size = intval;
			set_param(&bParam);
		} else {
			printf("Wrong flash size!\n\r");
		}
	}
#endif
	if(!strcmp(argv[1],"256")){
			;
	}
	else if(!strcmp(argv[1],"512")){
		nfcr |= 0x00010000;
	}
	else if(!strcmp(argv[1],"1")){
		nfcr |= 0x00020000;
	}
	else if(!strcmp(argv[1],"2")){
		nfcr |= 0x00030000;
	}
	else if(!strcmp(argv[1],"4")){
		nfcr |= 0x00040000;
	}
	else if(!strcmp(argv[1],"8")){
		nfcr |= 0x00050000;
	}
	else if(!strcmp(argv[1],"16")){
		nfcr |= 0x00060000;
	}
	else{
		result=0;
		printf("wrong flash size setting!\n\r");
	}

	if(result==1){
		get_param(&bParam);
		bParam.flash_size = nfcr;
		set_param(&bParam);
	}
	return 0;
}
#endif /*CONFIG_NO_FLASH*/

static int handler_help(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	/*
	sprintf(xstring,Loadhelp);
	printf("%s", xstring);
	printf("flashsize [256(k)/128(k)/1(M)/2(M)/4(M)/8(M)/16(M)]\n\rmemsize ROW[2k/4k/8k/16k] COL[256/512/1k/2k/4k] BANK[2/4]\n\r");	
	*/
	print_help();
	return 0;	
}

static int handler_info(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int i;
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	unsigned int partFlag;
#endif
	get_param(&bParam);
	print_banner();
	if(!strcmp(bParam.id, BOARD_PARAM_ID))
	{
		printf("BootLine: %s\n\r", bParam.bootline);
		for(i=0;i<N_MAC;i++)
		{
			unsigned char *mac = bParam.mac[i];
			if(mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5])
				printf("MAC Address [%d]: %02X:%02X:%02X:%02X:%02X:%02X\n\r", i, \
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			else
				break;
		}
		printf("Entry Point: 0x%08X\n\r", (unsigned int)bParam.entry);
		printf("Load Address: 0x%08X\n\r", (unsigned int)bParam.load);
		printf("Application Address: 0x%08X\n\r", (unsigned int)bParam.app);
		printf("HS Offset: 0x%08X\n\r", HW_SETTING_OFFSET);
		printf("CS Offset: 0x%08X\n\r", CURRENT_SETTING_OFFSET);
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
		partFlag = *(unsigned int *)IMAGE_LOCATION_FLAG_ADDR;
		if (!partFlag)
			printf("Root: %s at 0x%x\n\r", "First", FIRST_IMAGE_ADDR);
		else
			printf("Root: %s at 0x%x\n\r", "Second", SECOND_IMAGE_ADDR);
#endif
#ifndef CONFIG_NO_FLASH
#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
		if(parse_flash_size(bParam.flash_size))
			printf("Flash Size: %s\n\r",flash_size);
		else
			printf("Wrong Flash Setting!\n\r");
#endif /* CONFIG_RTL8676 || CONFIG_RTL8676S */
#endif /*CONFIG_NO_FLASH*/
#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S) 
		if(parse_dcr(bParam.mem_size))
			printf("Memory Configuration: ROW:%s COL:%s Bank:%s\n\r", current_dcr.row_cnt,current_dcr.col_cnt,current_dcr.bank_cnt);
		else
			printf("Wrong Memory Configuration\n\r");
		printf("MII Selection: %x (0: Int. PHY	1: Ext. PHY)\n\r", (unsigned int)bParam.MII_select);
#endif /* CONFIG_RTL8676 || CONFIG_RTL8676S */
#ifdef CONFIG_NFBI_SLAVE
{
		unsigned char *p, *pfile;
		bootconf_t bc;
		p=(unsigned char*)&bParam.ip;
		printf("Local IP: %d.%d.%d.%d\n\r", p[0],p[1],p[2],p[3] );
	
		if(get_bootconf(&bc)==0)
		{
			p=&bc.serverip;
			pfile=bc.filename;
		}else{
			p=0;
			pfile=0;
		}
		printf("Server IP: %d.%d.%d.%d\n\r", p[0],p[1],p[2],p[3] );
		printf("File Name: %s\n\r", (pfile)?pfile:"" );
}			
#endif /*CONFIG_NFBI_SLAVE*/
		printf("\n\r");
	}

	return 0;
}

static int handler_load(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	get_param(&bParam);
	bParam.load = str2UL(argv[1]);
	set_param(&bParam);
	return 0;
}

static int handler_mac(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int i;
	get_param(&bParam);
	if (!strcmp(argv[1], "show"))
	{
		char *mac;
		mac = bParam.mac[0];
		printf("1:%2x-%2x-%2x-%2x-%2x-%2x\n\r", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		mac = bParam.mac[1];
		printf("2:%2x-%2x-%2x-%2x-%2x-%2x\n\r", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	else if(!strcmp(argv[1], "clear"))
	{
		memset(bParam.mac, 0, N_MAC*8);	

		if(set_param(&bParam)==ERROR)
			printf("set error!\n\r");
	}
#ifndef CONFIG_NO_FLASH
	else if(!strcmp(argv[1], "osk"))
	{
		unsigned char *mac;
		/* find null record */
		for(i=0;i<N_MAC;i++)
		{
			mac = bParam.mac[i];
			if(!(mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5]))
				break;
		}
		if(i<N_MAC)
		{
			if(s2mac(mac, argv[2])==OK)
			{										
				if (ERROR == set_osk_hwaddr(mac))
					printf("set error!\n\r");

			}
			else
				printf("ERROR!\n\r");
		}
		else
			printf("ERROR!\n\r");
			
	}
#endif /*CONFIG_NO_FLASH*/
	else
	{
		char *mac;
		/* find null record */
		for(i=0;i<N_MAC;i++)
		{
			mac = bParam.mac[i];
			if(!(mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5]))
				break;
		}
		if(i<N_MAC)
		{
			if(s2mac(mac, argv[1])==OK)
			{
				if(set_param(&bParam)==ERROR)
					printf("set error!\n\r");
			}
			else
				printf("ERROR!\n\r");
		}
		else
			printf("ERROR!\n\r");
	}
	
	return 0;
}

static int handler_memsize(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
#ifdef SDRAM_AUTO_DETECT
	printf("memsize is automaticly detected!\n\r");
#else
	int intval2;
	int dcr=0x50000000;
	int result=1;
#if 0
	if(argv[1]) {
		intval2 = str2UL(argv[1]);
		//Only support 4M/8M/16M Memory
		if(intval2==4||intval2==8||intval2==16)
		{
			get_param(&bParam);
			bParam.mem_size = intval2;
			set_param(&bParam);
		} 
		else 
		{
			printf("Wrong memory size!\n\r");
		}
	}
#endif
	//memsize ROW[2k/4k/8k/16k] COL[256/512/1k/2k/4k] BANK[2/4]
	if(!strcmp(argv[1],"2k")){
		;
	}
	else if(!strcmp(argv[1],"4k")){
		dcr |= 0x02000000;
	}
	else if(!strcmp(argv[1],"8k")){
		dcr |= 0x04000000;
	}
	else if(!strcmp(argv[1],"16k")){
		dcr |= 0x06000000;
	}
	else{
		result=0;
		printf("wrong ROW setting!\n\r");
	}
	
	if(!strcmp(argv[2],"256")){
			;
	}
	else if(!strcmp(argv[2],"512")){
		dcr |= 0x00400000;
	}
	else if(!strcmp(argv[2],"1k")){
		dcr |= 0x00800000;
	}
	else if(!strcmp(argv[2],"2k")){
		dcr |= 0x00600000;
	}
	else if(!strcmp(argv[2],"4k")){
		dcr |= 0x01000000;
	}
	else{
		result=0;
		printf("wrong COL setting!\n\r");
	}
	
	if(!strcmp(argv[3],"2")){
		;
	}
	else if(!strcmp(argv[3],"4")){
		dcr |= 0x00080000;
	}
	else{
		result=0;
		printf("wrong BANK setting!\n\r");
	}

	if(result==1){
		get_param(&bParam);
		bParam.mem_size = dcr;
		set_param(&bParam);
		printf("set DCR 0x%x\n\r",dcr);
	}
#endif		

	return 0;
}

static int handler_r(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
#ifdef BANK_AUTO_SWITCH
	unsigned int oldbank, currbank;
	unsigned int offset;
#endif
	unsigned long addr;

	addr = str2UL(argv[1]);
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= addr - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	currbank = getbank();
	if (currbank == 1)
		addr = (unsigned int)addr - 0x400000;
	else if (currbank == 2)
		addr = (unsigned int)addr - 0x800000;
	else if (currbank == 3)
		addr = (unsigned int)addr - 0xC00000;
#endif
	printf("%08x\n\r", *((unsigned int *)addr));

#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif	

	return 0;
}

static int handler_reboot(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
#ifdef CONFIG_SPANSION_16M_FLASH
	s29gl128bank(0);
#endif
	//ccwei
	//#ifdef CONFIG_NAND_FLASH
#ifdef CONFIG_RTL8685_PERI
	WRITE_MEM32(Timer_WDTCTRLR,Timer_WDT_E);
#else /*CONFIG_RTL8685_PERI*/
	WRITE_MEM32(Timer_CDBR,0x10000000);
	WRITE_MEM32(Timer_WDTCNR,0x0);
#endif /*CONFIG_RTL8685_PERI*/
	/*
	#else
	void (*appStart)(void);
	appStart = (void*)CPU_REBOOT_ADDR;
	appStart();
	#endif
	*/
	return 0;
}

#ifndef CONFIG_NO_FLASH
static int handler_resetcfg(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
#ifndef IMAGE_DOUBLE_BACKUP_SUPPORT
	/*
	char cfgbuffer[DEFAULT_SETTING_MAX_LEN+HW_SETTING_MAX_LEN+CURRENT_SETTING_MAX_LEN];

	//reset DS/CS and keep HS setting
	memset(cfgbuffer, 0xff, sizeof(cfgbuffer));
	memcpy(cfgbuffer+DEFAULT_SETTING_MAX_LEN, FLASH_BASE_ADDR+HW_SETTING_OFFSET, HW_SETTING_MAX_LEN);
	*/
	printf("reset configuration...\n\r");
	/*
	if(amd29lvEraseNV(DEFAULT_SETTING_OFFSET, DEFAULT_SETTING_MAX_LEN) != OK) {
		printf("Default setting erase error!\n\r");
	}
	*/
	if(amd29lvEraseNV(CURRENT_SETTING_OFFSET, CURRENT_SETTING_MAX_LEN) != OK) {
		printf("Current setting erase error!\n\r");
	}
	//write_file(FLASH_BASE_ADDR+DEFAULT_SETTING_OFFSET, cfgbuffer, sizeof(cfgbuffer));
#else
	/* ql:20080718 START: current setting backup is supported in e8b, so current setting backup should be reset too,
	  * and HW seetting don't need to keep.
	  */
	printf("reset configuration...\n\r");
	/*
	if(amd29lvEraseNV(HW_SETTING_OFFSET, HW_SETTING_MAX_LEN) != OK) {
		printf("HW setting erase error!\n\r");
	}
	*/
	if(amd29lvEraseNV(CURRENT_SETTING_OFFSET, CURRENT_SETTING_MAX_LEN) != OK) {
		printf("Current setting erase error!\n\r");
	}
#ifdef E8B_SUPPORT
	if(amd29lvEraseNV(CS_BAKUP_OFFSET, CURRENT_SETTING_MAX_LEN) != OK) {
		printf("Current setting backup erase error!\n\r");
	}
#endif
	/*ql:20080718 END*/
#endif

	return 0;
}
#endif /*CONFIG_NO_FLASH*/


static int handler_run(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	//tctsai
	//since we check the number of parameters before entering the handler,
	//we don't need this if statment
	/*
	if(argc==3)
	{
		run_gzip_file(str2UL(argv[1]), str2UL(argv[2]));
	}
	*/
	
	run_gzip_file(str2UL(argv[1]), str2UL(argv[2]));

	return 0;
}	

#if !defined(CONFIG_NO_FLASH) && !defined(CONFIG_NO_NET)
static int handler_tftp(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	char *mac;
	unsigned int image_len;
	unsigned int ip = inet_addr(argv[1]);
	unsigned int server = inet_addr(argv[2]);
	char *load_buf;
	char *app_buf;
	
	get_param(&bParam);
	load_buf = (char*)bParam.load;
	//load_buf = (char*)DRAM_TEMP_LOAD_ADDR;
	app_buf = (char*)bParam.app;
	mac = bParam.mac[0];
	Lan_Initialize(mac);
	if ( bootpReceive (mac, &image_len, ip, server, argv[3], load_buf) != 0 )
		printf("TFTP ERROR\n\r");
	else
	{
	
		printf("Writing file...\n\r");
		/*ql:20080723 START: don't upgrade raw binary*/
		//if (writeImage(load_buf)) {
		//	printf("No header, assuming raw binary\n\r");
		//	write_file(app_buf, load_buf, image_len);
		//}
		writeImage(load_buf);
		/*ql:20080723 END*/
			
	}

	return 0;
}
#endif

static int handler_w(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
#ifdef BANK_AUTO_SWITCH
	unsigned int oldbank, currbank;
	unsigned int offset;
#endif
	unsigned long addr;

	addr = str2UL(argv[1]);
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= addr - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	currbank = getbank();
	if (currbank == 1)
		addr = (unsigned int)addr - 0x400000;
	else if (currbank == 2)
		addr = (unsigned int)addr - 0x800000;
	else if (currbank == 3)
		addr = (unsigned int)addr - 0xC00000;
#endif
	*((unsigned int *)addr) = str2UL(argv[2]);
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif

	return 0;
}

#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
static int handler_web(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	get_param(&bParam);
	rs_httpd();

	return 0;
}
#endif

static int handler_xmodem(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	xmodem_download(str2UL(argv[1]));
	return 0;
}

#if 0 // shrink bootloader size, remove unnecessary code
static int handler_flashcontent(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int i;
	for(i=0;i<0x7000;i++)
		if(*(volatile unsigned char *)(0x80408000+i) != *(volatile unsigned char *)(FLASH_BASE_ADDR+0x8000+i))
			{printf("flash content error at 0x%x\n\r",(FLASH_BASE_ADDR+0x8000+i));break;}

	return 0;
}

static int handler_readtest(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int addr,addrdata;
	*(volatile unsigned int*)0xb8003110=0xf0000000;
	printf("counter:%x\n",*(volatile unsigned int*)0xb8003108);
	for(addr=0;addr<0x80000;addr+=4){
		addrdata+=*(volatile unsigned int*)(FLASH_BASE_ADDR+addr);
	}
	printf("counter:%x\n",*(volatile unsigned int*)0xb8003108);


	return 0;
}

static int handler_flashcheck(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	dumpflash();
	
	return 0;
}

//tylo, IMEM_DMA test
static int handler_imemdma(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	imemdmatest();
	
	return 0;
}

static int handler_dmemdma(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	dmemdmatest();

	return 0;
}

//tylo, performance evaluation
static int handler_memcpy(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	test_memcpy();

	return 0;
}

static int handler_sram(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int result=1,count;
			
	/* Unmapped Memory Segment */
	*(volatile unsigned int *) 0xB8001304 = 0x8;
	*(volatile unsigned int *) 0xB8001300 = 0x00000001;
			
	/* SRAM Segment */
	*(volatile unsigned int *) 0xB8004008 = 0x0;
 	*(volatile unsigned int *) 0xB8004004 = 0x8;
	*(volatile unsigned int *) 0xB8004000 = 0x00000001;
#if 1
	for(count=0;count<0x8000;){
		REG32(0x80000000+count)=0x12345678;
		if(REG32(0x80000000+count)!=0x12345678){
			printf("error in 0x%x!\n",0x80000000+count);
			result=0;
		}
		count+=4;
	}
	for(count=0;count<0x8000;){
		REG32(0x80000000+count)=0x9abcdef0;
		if(REG32(0x80000000+count)!=0x9abcdef0){
			printf("error in 0x%x!\n",0x80000000+count);
			result=0;
		}
		count+=4;
	}
#if 0 //sram simple test
	//test memory 0xB0000000
	*(volatile unsigned int *)0xB0000000 = 0x12345678;
	if(REG32(0xB0000000)!=0x12345678){
		printf("error in 0x%x",0xB0000000);
		result =0;
	}
#endif
#endif
	if(result)
		printf("SRAM test PASS\n\r");
	else
		printf("SRAM test FAIL\n\r");

	return 0;
}

static int handler_r16(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned long addr;
	addr = str2UL(argv[1]);
	printf("%04x\n\r", *((unsigned short *)addr));

	return 0;
}

static int handler_r8(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned long addr;
	addr = str2UL(argv[1]);
	printf("%04x\n\r", *((unsigned char *)addr));

	return 0;
}

static int handler_w16(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned long addr;
	addr = str2UL(argv[1]);
	*((unsigned short *)addr) = (unsigned short)str2UL(argv[2]);

	return 0;
}

static int handler_w8(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned long addr;
	addr = str2UL(argv[1]);
	*((unsigned char *)addr) = (unsigned char)str2UL(argv[2]);

	return 0;
}

static int handler_tftpx(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	char *mac;
	unsigned int image_len;
	unsigned int ip = inet_addr(argv[1]);
	unsigned int server = inet_addr(argv[2]);
	char *load_buf;
	//char *app_buf;

	get_param(&bParam);
	//load_buf = (char*)(bParam.load+0x300000);// 0x80300000, for vmbin.img
	//app_buf = (char*)bParam.app; 	
	mac = bParam.mac[0];
	load_buf = str2UL(argv[4]);
	Lan_Initialize(mac);
    if ( bootpReceive (mac, &image_len, ip, server, argv[3], load_buf) != 0 )
    	printf("TFTP ERROR\n\r");
	else
	{
		printf("xfer %xh bytes to %xh\n\r", image_len, load_buf);			
		//run_gzip_file(load_buf+0x40,0x80000000);
		//bootpReceive (mac, &image_len, ip, server, argv[3], load_buf) ;
	}

	return 0;
}

static int handler_runx(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	//tctsai
	//since we check the number of parameters before entering the handler,
	//we don't need this if statment
	/*
	if (argc == 2) {
		void (*func)(void);
		func = str2UL(argv[1]);
		func();
	}
	*/

	void (*func)(void);
	func = str2UL(argv[1]);
	func();
		
	return 0;
}

#endif

#ifdef CONFIG_SPANSION_16M_FLASH
static int handler_bank(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int nk;
	nk = str2UL(argv[1]);
	printf("Choose bank:%d \r\n",nk);
	s29gl128bank(nk);

	return 0;
}
#endif


#ifndef CONFIG_NO_FLASH
//tylo, for 8672 fpga memory controller test
static int handler_sdram(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int count,result=1;
	for(count=0;count<0x700000;){
		WRITE_MEM32(0xa0000000+count,0x0);
		count+=4;
	}
	for(count=0;count<0x700000;){
		if(REG32(0xa0000000+count)!=0){
			printf("reset fail!\n\r");
			result=0;
		}
		count+=4;
	}
	printf("8-bit test\n\r");
	for(count=0;count<0x700000;){
		REG8(0xa0000000+count)=(0x5a);
		count+=1;
	}
	for(count=0;count<0x700000;){
		if(REG8(0xa0000000+count)!=(0x5a)){
			printf("error in 0x%x!\n",0xa0000000+count);
			result=0;
		}
		count+=1;
	}
	
	for(count=0;count<0x700000;){
		WRITE_MEM32(0xa0000000+count,0x0);
		count+=4;
	}
	for(count=0;count<0x700000;){
		if(REG32(0xa0000000+count)!=0){
			printf("reset fail!\n\r");
			result=0;
		}
		count+=4;
	}
	printf("16-bit test\n\r");
	for(count=0;count<0x700000;){
		REG16(0xa0000000+count)=(0xa5a5);
		count+=2;
	}
	for(count=0;count<0x700000;){
		if(REG16(0xa0000000+count)!=(0xa5a5)){
			printf("error in 0x%x	  %x=>%x!\n",0xa0000000+count,count%0xffff,REG8(0xa0000000+count));
			result=0;
		}
		count+=2;
	}
	for(count=0;count<0x700000;){
		WRITE_MEM32(0xa0000000+count,0x0);
		count+=4;
	}
	for(count=0;count<0x700000;){
		if(REG32(0xa0000000+count)!=0){
			printf("reset fail!\n\r");
			result=0;
		}
		count+=4;
	}
	printf("32-bit test\n\r");
	for(count=0;count<0x700000;){
		REG32(0xa0000000+count)=(0x5a5a5a5a);
		count+=4;
	}
	for(count=0;count<0x700000;){
		if(REG32(0xa0000000+count)!=(0x5a5a5a5a)){
			printf("error in 0x%x!\n",0xa0000000+count);
			result=0;
		}
		count+=4;
	}
	if(result)
		printf("SDRAM test PASS\n\r");
	else
		printf("SDRAM test FAIL\n\r");

	return 0;
}
#endif /*CONFIG_NO_FLASH*/

#ifndef CONFIG_NO_FLASH
static int handler_ferase(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int len;

	if (strlen(argv[2]))
		len = str2UL(argv[2]);
	else	
		len = 1;
	printf("erase %x len %x\n", (unsigned int)str2UL(argv[1]), (unsigned int)len);
	if(amd29lvEraseSector(str2UL(argv[1]), len) != OK) {
		printf("Config space 0 erase error!\n\r");
	}

	return 0;
}

//for debug
static int handler_fwrite(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int fvalue;

	fvalue = str2UL(argv[2]);
	printf("write 0x%x to 0x%x\n", fvalue, str2UL(argv[1]));
	amd29lvWrite((const void *)str2UL(argv[1]), (const void *)fvalue, str2UL(argv[3]));

	return 0;
}
#endif /*CONFIG_NO_FLASH*/

#ifndef CONFIG_NO_NET
static int handler_bootp(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int image_len;
	char *mac;
	char filename[32];
	char *load_buf;
	char *app_buf;

	get_param(&bParam);
	load_buf = (char*)bParam.load;
	app_buf = (char*)bParam.app;
	mac = bParam.mac[0];
	Lan_Initialize(mac);
    if ( bootpReceive (mac, &image_len, 0, 0, filename, load_buf) != 0 )
    	printf("bootp ERROR\n\r");
	else
	{
		printf("writing file...\n\r");
		write_file(app_buf, load_buf, image_len);
	}

	return 0;
}
#endif /*CONFIG_NO_NET*/

#ifdef SUPPORT_MULT_UPGRADE
static int handler_multicast(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int crc32_tmp; 	

	if(!multicast_start())
	{
		printf("time out ......\n\r");
		total_crc32 =0;
		total_len =0;
	}
	else {
		get_param(&bParam);
	
		printf("\r\n multicast upgrade tool receive image OK ...\n\r");
		crc32_tmp = gz_crc32(0,bParam.load,total_len);
			
		if(total_crc32!=crc32_tmp)
		{
			printf("crc32 error total_crc32=%x crc32_tmp=%x\n\r",total_crc32, crc32_tmp);
		}
		else 
		{
			total_crc32 =0;
			total_len =0;
			printf("Writing file...\n\r");
			int i=0;
			for( i=0; i<MAX_PARTITION; i++)
			{
				if(! partiton_info_array[i].valid)
					continue;
				if(partiton_info_array[i].type == PARTI_TYPE_IMG)
				{
					printf("Going to write kernel to flash\n\r");
					writeImage(bParam.load+partiton_info_array[i].addrstart);
					amd29lvEraseSector(0x1F0000, 20); // erase osk router's current setting partition.
					printf("erase flash ok\n\r");
					printf("upgrade kernel successfully!\n\r");
				}
				else if(partiton_info_array[i].type == PARTI_TYPE_BOOT)
				{
					printf("Going to write bootloader to flash\n\r");
					char *load_buf = bParam.load + partiton_info_array[i].addrstart;
					char *dest_buf = (char *)FLASH_BASE_ADDR;
					unsigned long len = partiton_info_array[i].addrend - partiton_info_array[i].addrstart;
					if(writeImage(load_buf)) 
					{
						printf("No header, assuming raw binary\n\r");
						if(write_file(dest_buf, load_buf, len)!=0)
							printf("write bootloader failed\n\r");
					}
					else
						printf("upgrade bootloader successfully!\n\r");
				}
				else if(partiton_info_array[i].type == PARTI_TYPE_CONF) 
				{
					char *load_buf = bParam.load + partiton_info_array[i].addrstart;
					char *dest_buf = (char *)(FLASH_BASE_ADDR+0x1f0000);
					unsigned long len = partiton_info_array[i].addrend - partiton_info_array[i].addrstart;
					FLASH_CFG_HDR *cfg_hdr = (FLASH_CFG_HDR *)load_buf;
					if(cfg_hdr->checksum != getCfgChksum((load_buf+sizeof(FLASH_CFG_HDR)), cfg_hdr->length, 0))
					{
						printf("config checksum error\n\r");
						continue;
					}
					printf("Going to write config to flash\n\r");
#ifdef CONFIG_CONFIG_FILE_ENCRYPT
					decode(load_buf, len);
#endif
					if(write_file(dest_buf, load_buf, len)!=0)
						printf("write current-config failed\n\r");
					else
						printf("upgrade conf successfully!\n\r");
				}
			}
			//writeImage(bParam.load);
		}
	}

	return 0;
}
#endif

#ifndef CONFIG_NO_NET
//Add for booting from RAM
static int handler_tftpm(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	char *mac;
	unsigned int image_len;
	unsigned int ip = inet_addr(argv[1]);
	unsigned int server = inet_addr(argv[2]);
	char *load_buf;
	char *app_buf;

	get_param(&bParam);
#ifdef NEW_FLASH_LAYOUT
	load_buf = DRAM_TEMP_LOAD_ADDR;
#else
#ifdef CONFIG_NO_FLASH
	load_buf = (char*)(bParam.load+0x400000);// 0x80400000, for linux+initramfs
#else /*CONFIG_NO_FLASH*/
	load_buf = (char*)(bParam.load+0x300000);// 0x80300000, for vmbin.img
#endif /*CONFIG_NO_FLASH*/
#endif
	app_buf = (char*)bParam.app;	
	mac = bParam.mac[0];
	Lan_Initialize(mac);
	if ( bootpReceive (mac, &image_len, ip, server, argv[3], load_buf) != 0 )
		printf("TFTP ERROR\n\r");
	else
	{
#ifdef CONFIG_NO_FLASH
		printf("Boot from kernel file...\n\r");
		check_and_run_file(load_buf, bParam.entry);
#else /*CONFIG_NO_FLASH*/
		printf("Boot from file...\n\r");
		run_gzip_file(load_buf+0x40,0x80000000);
#endif /*CONFIG_NO_FLASH*/
		//bootpReceive (mac, &image_len, ip, server, argv[3], load_buf) ;
	}

	return 0;
}
#endif

static int handler_app(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	get_param(&bParam);
	bParam.app = str2UL(argv[1]);
	set_param(&bParam);

	return 0;
}

#ifdef CONFIG_PCIE_HOST
static int handler_hrst(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	printf("PCIE HOST Reset\n\r");
	PCIE_reset_procedure(0,0,1, 0xb8b10000);

	return 0;
}

static int handler_pcie_r(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	HostPCIe_ReadPhyMdioWrite(str2UL(argv[1]), str2UL(argv[2]));

	return 0;
}

static int handler_pcie_w(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	HostPCIe_SetPhyMdioWrite(str2UL(argv[1]),str2UL(argv[2]),str2UL(argv[3]));

	return 0;
}

static int handler_pcie_rst(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	PCIe_Reset();

	return 0;
}

static int handler_Pcie_D_LBK(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	Pcie_D_LBK();

	return 0;
}

static int handler_Pcie_LBK(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	Pcie_LBK();

	return 0;
}
#endif

static int handler_miisel(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	if(argv[1]) {
		if(str2UL(argv[1])==0 || str2UL(argv[1])== 1) {
			get_param(&bParam);
			bParam.MII_select= str2UL(argv[1]);
			set_param(&bParam);
		} else {
			printf("Wrong MII selection setting!\n\r");
		}
	}

	return 0;
}

#ifdef CONFIG_RTL8685_MEMCTL_CHK
void show_DRAM_phy_parameters(void)
{
	unsigned int i;
	volatile unsigned int *ptr;

	printf("\tSYSREG_DDRCKODL_REG(0x%08x):0x%08x;\n", \
			SYSREG_DDRCKODL_REG, *((unsigned int *)SYSREG_DDRCKODL_REG) );
	ptr = (unsigned int *)DACCR;
	printf("\tDACCR(0x%08x):\n", (unsigned int)ptr);
	for(i = 0; i < 10; i++){
		printf("\t\t(0x%08x):0x%08x", (unsigned int)ptr, *ptr );
		ptr++;
		printf(", 0x%08x ", *ptr );
		ptr++;
		printf(", 0x%08x ", *ptr );
		ptr++;
		printf(", 0x%08x\n", *ptr);
		ptr++;
	}
	return;
}

/*
 * Check DRAM Configuration setting. 
 */
int chk_dram_cfg(unsigned int  dram_freq_mhz)
{
	int     	rcode = 0;
	unsigned int sys_clk=0, dram_type=0; 
	const unsigned int cmu_ctlr_reg = REG32(SYSREG_CMUCTLR_REG);

	/* Show current CPU/DSP/LX clock*/
	printf("System clock:\n\r");
	sys_clk = board_CPU_freq_mhz(cmu_ctlr_reg);
	printf("\tCPU clock : %uMHz\n",sys_clk);
#ifdef CONFIG_RTL8685
	sys_clk = board_DSP_freq_mhz(cmu_ctlr_reg);
	printf("\tDSP clock : %uMHz\n",sys_clk);
#endif /* CONFIG_RTL8685 */
	sys_clk = board_LX_freq_mhz(cmu_ctlr_reg, 0,0);
	printf("\tLX0 clock : %uMHz\n",sys_clk);
	sys_clk = board_LX_freq_mhz(cmu_ctlr_reg, 1,1);
	printf("\tLX1 clock : %uMHz\n",sys_clk);
	sys_clk = board_LX_freq_mhz(cmu_ctlr_reg, 1,2);
	printf("\tLX2 clock : %uMHz\n",sys_clk);
	sys_clk = board_LX_freq_mhz(cmu_ctlr_reg, 0,3);
	printf("\tLXP clock : %uMHz\n",sys_clk);
#if defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	sys_clk = board_SPIF_freq_mhz(0);
	printf("\tSPIi clock: %uMHz\n",sys_clk);
	sys_clk = board_SPIF_freq_mhz(1);
	printf("\tSPIo clock: %uMHz\n",sys_clk);
#endif

	/* Get current DRAM freq. */
	if(dram_freq_mhz==0){
		dram_freq_mhz = board_DRAM_freq_mhz(); 
		printf("\tDRAM clock: %dMHz\n", dram_freq_mhz);
	}else{
		printf("\tDRAM clock: %dMHz\n", dram_freq_mhz);
	}

	/* Show frequency register information */
	printf("Freq. Register settings:\n\r");
	printf("\tSYSCLK_CONTROL	(0x%08x):0x%08x\n", SYSREG_SYSCLK_CONTROL_REG, REG32(SYSREG_SYSCLK_CONTROL_REG));
	printf("\tMCKG_PHS_SEL	(0x%08x):0x%08x\n", SYSREG_MCKG_PHS_SEL_REG, REG32(SYSREG_MCKG_PHS_SEL_REG));
	printf("\tMCKG_FREQ_DIV	(0x%08x):0x%08x\n", SYSREG_MCKG_FREQ_DIV_REG,REG32(SYSREG_MCKG_FREQ_DIV_REG));
	printf("\tLX_PLL_SEL	(0x%08x):0x%08x\n",SYSREG_LX_PLL_SEL_REG,REG32(SYSREG_LX_PLL_SEL_REG));
	printf("\tCMUCTLR		(0x%08x):0x%08x\n", SYSREG_CMUCTLR_REG,REG32(SYSREG_CMUCTLR_REG));
#if defined(CONFIG_RTL8685S) || defined(CONFIG_RTL8685SB)
	printf("\tCMUOC0CR	(0x%08x):0x%08x\n", SYSREG_CMUOC0CR_REG,REG32(SYSREG_CMUOC0CR_REG));
	printf("\tCMULXBR		(0x%08x):0x%08x\n", SYSREG_CMULXBR_REG,REG32(SYSREG_CMULXBR_REG));
#endif
	printf("\tDRAM_CLK_EN	(0x%08x):0x%08x\n", SYSREG_DRAM_CLK_EN_REG,REG32(SYSREG_DRAM_CLK_EN_REG));

	/* Show register informantion. */
	printf("DRAM Register settings:\n\r");
	printf("\tMCR (0x%08x):0x%08x\n", MCR, REG32(MCR));
	printf("\tDCR (0x%08x):0x%08x\n", DCR, REG32(DCR));
	printf("\tDTR0(0x%08x):0x%08x\n", DTR0,REG32(DTR0));
	printf("\tDTR1(0x%08x):0x%08x\n", DTR1,REG32(DTR1));
	printf("\tDTR2(0x%08x):0x%08x\n", DTR2,REG32(DTR2));
	printf("\tDDCR(0x%08x):0x%08x\n", DDCR, REG32(DDCR));
	printf("\tDCDR(0x%08x):0x%08x\n", DCDR, REG32(DCDR));

	show_DRAM_phy_parameters();

	/* DRAM memory type in MCR (DDR1/2/3). */
	printf("DRAM Type: ");

	dram_type = memctlc_DDR_Type();
	if(dram_type==IS_DDR3_SDRAM)
		printf("DDR3 SDRAM \n\r");
	else if(dram_type==IS_DDR2_SDRAM)
		printf("DDR2 SDRAM \n\r"); 
	else if(dram_type==IS_DDR_SDRAM)
		printf("DDR SDRAM \n\r");
	else
		printf("ERROR UNKNOWN! \n\r");

	/* 
	 * Check DCR
	 */
	memctlc_check_DCR();

	
	/* 
	 * Check DTR
	 */
	memctlc_check_DTR(dram_freq_mhz);


	/*
	 * Chech DDCR and DCDR
	 */
	if(dram_type==1){
#ifdef CONFIG_DDR1_USAGE
		/* if DDR SDRAM : Check DDR SDRAM DQS delay in DDCR. */
		memctlc_check_DQS_range();
		/* if DDR SDRAM : Check 90 phase delay in DCDR. */
		memctlc_check_90phase_range();
#endif
	}else if(dram_type==2){
#ifdef CONFIG_DDR2_USAGE
		memctlc_check_ZQ();
		/* Check TX/RX value ?*/
#endif
	}else if(dram_type==3){
#ifdef CONFIG_DDR3_USAGE
		memctlc_check_ZQ();
		/* Check TX/RX value ?*/
#endif
	}

	return rcode;
}

#if !defined(CONFIG_BOOT_MIPS) && !defined(CONFIG_MIPS)
static int handle_cali_8685(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	printf("DDR calibration for RTL8685\n\r");

	DDR_Calibration();

	return 0;
}
#endif

static int handle_diag_8685(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int dram_freq_mhz;

	if (argv[1]) {
		dram_freq_mhz = str2UL(argv[1]);
	}else{
		dram_freq_mhz = 0;	
	}

	return chk_dram_cfg(dram_freq_mhz);
}
#endif

#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ

void CpuFreqMsg(void)
{
	printf("\nCPU Clock range (400MHz~600MHz)\n\r"\
		"Please enter the clock rate:");
	return;
}

int SetCpuParam(unsigned int clockrate)  
{
	int cpu_div=0;

	if((clockrate<400) ||(clockrate>600)){
		printf("CPU doesn't support %3u clock rate\n", clockrate);
		return ERROR;
	}
	else{
		cpu_div = (clockrate/25) - 2;
		printf("CPU divsor : %u\n",cpu_div);
		bParam.sysclk_ctrl &= ~(SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK);
		bParam.sysclk_ctrl |= (cpu_div << SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S);
	}

	return OK;
}

void DramFreqMsg(void)
{
	if(memctlc_DDR_Type()==IS_DDR2_SDRAM){
		printf("\nDRAM Clock options\n\r"\
				"\t(0) 200MHz\n\r"\
				"\t(1) 300MHz\n\r"\
				"\t(2) 350MHz\n\r"\
				"\t(3) 400MHz\n\r"\
				"\t(4) 500MHz\n\r"\
				"\n\r\tPlease select:");
	}else if(memctlc_DDR_Type()==IS_DDR3_SDRAM){
		printf("\nDRAM Clock options\n\r"\
				"\t(0) 300MHz\n\r"\
				"\t(1) 350MHz\n\r"\
				"\t(2) 400MHz\n\r"\
				"\n\r\tPlease select:");
	}

	return;
}

int SetDramParam(unsigned int options)  
{
	int dram_div=0;
	unsigned int dram_clock=0;

	if(memctlc_DDR_Type()==IS_DDR2_SDRAM){

		switch(options){
			case 0:
				dram_clock = 200;
				break;
			case 1:
				dram_clock = 300;
				break;
			case 2:
				dram_clock = 350;
				break;
			case 3:
				dram_clock = 400;
				break;
			case 4:
				dram_clock = 500;
				break;
			default:
				dram_clock = 300;
		}
	}else if(memctlc_DDR_Type()==IS_DDR3_SDRAM){

		switch(options){
			case 0:
				dram_clock = 300;
				break;
			case 1:
				dram_clock = 350;
				break;
			case 2:
				dram_clock = 400;
				break;
			default:
				dram_clock = 300;
		}
	}

	dram_div = (dram_clock*2)/25 -2;
	printf("DRAM divsor : %u\n",dram_div);
	bParam.sysclk_ctrl &= ~(SYSREG_SYSCLK_CONTROL_SDPLL_MASK);
	bParam.sysclk_ctrl |= (dram_div << SYSREG_SYSCLK_CONTROL_SDPLL_FD_S);

	return OK;
}

void LxFreqMsg(void)
{
	printf("\nLX Clock range (100MHz~200MHz)\n\r"\
		"Please enter the clock rate:");
	return;
}

int SetLxParam(unsigned int clockrate)  
{
	int lx_div=0;

	if((clockrate<100) ||(clockrate>200)){
		printf("LX doesn't support %3u clock rate\n", clockrate);
		return ERROR;
	}
	else{
		lx_div = (1000/clockrate) - 2;
		printf("LX divsor : %u\n",lx_div);
		bParam.lx_pll_sel &= ~(SYSREG_LX_PLL_CTRL_LXPLL_FD_MASK);
		bParam.lx_pll_sel |= (lx_div << SYSREG_LX_PLL_CTRL_LXPLL_FD_S);
	}

	return OK;
}

void CMUModeMsg(void)
{
	printf("\n(0)CMU disable (1)CMU fixed (2)CMU dynamic\n\r"\
		"Please enter the selection:");
	return;
}

int SetCMUModeParam(unsigned int select)  
{
	if((select<0) ||(select>3)){
		printf("CMU doesn't support this mode\n");
		return ERROR;
	}
	else{
		bParam.cmu_ctrl &= ~(SYSREG_CMUCTLR_CMU_MD_MASK);
		bParam.cmu_ctrl |= (select << SYSREG_CMUCTLR_CMU_MD_FD_S);
	}

	return OK;
}

void CMUDivMsg(void)
{
	printf("\nDivsor 2^n, n=0~7\n\r"\
		"Please enter the value of n:");
	return;
}

int SetCMUDivParam(unsigned int select)  
{
	if((select<0) ||(select>8)){
		printf("CMU doesn't support this divsor\n");
		return ERROR;
	}
	else{
		bParam.cmu_ctrl &= ~(SYSREG_CMUCTLR_CMU_CPU0_FREQ_DIV_MASK);
		bParam.cmu_ctrl |= (select << SYSREG_CMUCTLR_CMU_CPU0_FREQ_DIV_FD_S);
	}

	return OK;
}


enum freq_option {
	SET_CPU_CLK=0,
	SET_DRAM_CLK,
	SET_LX_CLK,
	SET_CMU_MODE,
	SET_CMU_DIV,
	SET_NONE
};

struct handle_SoC_clock {
	enum freq_option sel;
	void (*ShowMsgFunc) (void);
	int (*SetParamFunc) (unsigned int clockrate);
};

struct handle_SoC_clock SoC_Clock_list[] = {
	{SET_CPU_CLK, 		CpuFreqMsg, 	SetCpuParam},
	{SET_DRAM_CLK, 	DramFreqMsg, 	SetDramParam},
	{SET_LX_CLK, 		LxFreqMsg, 		SetLxParam},
	{SET_CMU_MODE, 	CMUModeMsg, 	SetCMUModeParam},
	{SET_CMU_DIV, 		CMUDivMsg, 		SetCMUDivParam},
	{-1, NULL, NULL}
};

void SysFreqMsg(void)
{
	printf("\nSystem options:\n\r"\
		"\t(%d) Set CPU clock\n\r"\
		"\t(%d) Set DRAM clock\n\r"\
		"\t(%d) Set LX clock\n\r"\
		"\t(%d) Set CMU mode\n\r"\
		"\t(%d) Set OCP0 CMU divsor\n\r"\
		"\t(%d) Save and exit\n\r"\
		"\n\r\tPlease select:", SET_CPU_CLK, SET_DRAM_CLK, SET_LX_CLK, SET_CMU_MODE, SET_CMU_DIV,SET_NONE) ;
	return;
}

static int handle_freq_8685s(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int result=0;
	char bbuf[10];
	struct handle_SoC_clock *HandleClock;
	unsigned int HandleSet = SET_NONE;

	get_param(&bParam);

	while(1)
	{
		SysFreqMsg();
		kbd_proc(bbuf, 10);
		HandleSet = str2UL(bbuf);
		
		if (HandleSet == SET_NONE) {
			printf("Save and exit\n\r");
			set_param(&bParam);
			break;
		}else{

			for (HandleClock = &SoC_Clock_list[0]; HandleClock->ShowMsgFunc!= NULL; HandleClock++){
				if(HandleClock->sel == HandleSet)
				{
					while(1)
					{
						/* Show message */
						HandleClock->ShowMsgFunc();

						/* Get the parameters */
						kbd_proc(bbuf, 10);
						if(HandleClock->SetParamFunc(str2UL(bbuf))==OK)
							break;
					}
				}
			}			
		}
	}

	return result;
	
}
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */


#if defined(CONFIG_MEM_TEST)
#ifdef CONFIG_RTL8676S
static int handle_flash_test(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n====== flash test =====\n");
	flash_test(test_time, round);
	return 0;
}

static int handle_dram_test(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n====== dram test =====\n");
	dram_test(test_time, round);
	return 0;
}
#else
static int handle_dram_test(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n\r====== dram test =====\n\r");
	dram_test(test_time, round, (1 << 1) | (1 << 0));
	return 0;
}

static int handle_dram_test_dp(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n\r====== dram test data prefetch=====\n\r");
	dram_test(test_time, round, (1 << 2));
	return 0;
}

static int handle_dram_test_ip(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n\r====== dram test inst. prefetch=====\n\r");
	dram_test(test_time, round, (1 << 3));
	return 0;
}

static int handle_dram_test_bp(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n\r====== dram test data and inst. prefetch=====\n\r");
	dram_test(test_time, round, (1 << 4));
	return 0;
}

#ifdef CONFIG_RLX
static int handle_dram_test_all(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int test_time, round;

	if (argv[1]){
		test_time = str2UL(argv[1]);
	} else{
		test_time = 0;	
	}

	if (argv[2]){
		round = str2UL(argv[2]);
	} else{
		round = 0;	
	}

	printf("\n\r====== dram test all=====\n\r");
	dram_test(test_time, round, (1 << 0) | (1 << 1) |(1 << 2) |(1 << 3) |(1 << 4));
	return 0;
}
#endif //CONFIG_RLX
#endif //CONFIG_RTL8676S
#endif //CONFIG_MEM_TEST

#ifdef CONFIG_L2C_TEST
enum l2c_mode{
        WT = 0x0,
        UC = 0x2,
        WB = 0x3,
        CWBE = 0x4,
        CWB = 0x5,
        UCA = 0x7,
	DISABLE = 0xd,
};

#define GCR_BASE                0xbfbf8008
#define CCA_V_OFFSET    	5
#define CCA_V_MASK              (0x7 << CCA_V_OFFSET)
#define CCA_ENABLE              (1 << 4)
#define CCA_MASK                (0xf0)
extern void flush_l2cache(void);
static void disable_l2c_CCA(void) {
        /* flush L2 cache */
        flush_l2cache();

        /* sync */
        //_mips_sync();

        /* disable l2c CCA */
        REG32(GCR_BASE) &= ~CCA_MASK;
        printf("L2CT: L2 CCA is disabled, GCR_BASE: 0x%08x\n\r", REG32(GCR_BASE));
}

static void change_l2c_CCA(int mode) {
        unsigned int tmp = 0;

        /* get and set CCA mode*/
        tmp = REG32(GCR_BASE);
        tmp &= ~CCA_MASK;
        tmp |= (mode << CCA_V_OFFSET) | CCA_ENABLE;

        printf("L2CT: L2 CCA Mode will be changed from 0x%x to 0x%x, GCR_BASE: 0x%08x -> 0x%08x\n\r",
                        (REG32(GCR_BASE) & CCA_V_MASK) >> CCA_V_OFFSET, mode, REG32(GCR_BASE), tmp);

        /* flush L2 cache */
        flush_l2cache();

        /* sync */
        //_mips_sync();

        /* apply change to CCA mode */
        REG32(GCR_BASE) = tmp;

        printf("L2CT: L2 CCA Mode change is applied, GCR_BASE: 0x%08x\n\r", REG32(GCR_BASE));

}

static int handler_l2c_mode(char argv[MAX_ARGS][MAX_ARG_LEN+1]) {
	unsigned int mode;
	unsigned int tmp = 0;
	unsigned int config2 = read_c0_config2();

	if (argv[1]) {
		mode = str2UL(argv[1]);
	} else {
		return 1;
	}

	if (config2 & (1 << 12))
	{
		printf("WARN: your configuration has bypassed L2 cache!\n\r");
		return 0;
	}
	
	switch (mode) {
		case WT:
		case UC:
		case WB:
		case CWBE:
		case CWB:
		case UCA:
			change_l2c_CCA(mode);
			break;

		case DISABLE:
			disable_l2c_CCA();
			break;

		default:
			printf("invalid mode\n\r");
			return 1;
	}
	return 0;
} 
#endif

#ifdef CONFIG_BTG_TEST
extern int btg_sub_sys(int argc, char argv[MAX_ARGS][MAX_ARG_LEN+1]);
extern int current_cmd_argc;

static int handler_btg_test(char argv[MAX_ARGS][MAX_ARG_LEN+1]) {
	int ret = 0;
	int i;

#if 0
	printf("[BTG] argc = %d\n\r", current_cmd_argc);
	printf("[BTG] argv = ");
	for (i = 0; i < current_cmd_argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n\r");
#endif
	ret = btg_sub_sys(current_cmd_argc, argv);
	
	//printf("[BTG] done\n\r");
	return ret;
}
#endif

#ifdef CONFIG_MAC0_LBK
static int handler_cali(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	//if(argc<3)
	/*
	{
		printf("Usage:cali [0/1] [1/2/3/4/5/6/7]\n\r");
		printf("first parameter:0:DQS0 first test, 1:DQS1 first test, SDRAM for 0:up_count 1:down_count\n\r");
		printf("second parameter:[n0]=1 for TFTP, [n1]=1 for decompress, [n2]=1 for MAC LBK  \n\r");			
		printf("Example:\n\r");
		printf("  cali 0 4\n\r");
	}
	else
	{*/
		cali_method=str2UL(argv[2]);
		printf("cali_method=0x%x \n\r",cali_method);
		if(*(volatile unsigned int *)(0xb8001000)&0x80000000)
		{
			DDR_timing_diff_dqs_check2(str2UL(argv[1]));
			printf("............. DDR calibration finish .............\n\r");
		}
		else
		{
			SDRAM_timing_check(str2UL(argv[1]));
			printf("............. SDRAM rx parameter scan finish .............\n\r");
		}
	//}
	
	return 0;
}
#endif

#ifdef	CONFIG_GDMA_SCAN
static int handler_gdma_scan(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned int sdram_size,sdram_scan,scan_times,sdram_i,sdram_scan_temp=0,fail_cnt=0;
	unsigned rx_dly_Left_flag=0,rx_dly_right_flag=0,rx_dly_Left=0,rx_dly_Right=0;
	unsigned tx_scan=0,init_start_test=0;
	unsigned char rx_para_record[16][16];
	/*
	if(argc<2){
		printf("Usage:gdma_scan [0/1/2....16] [memory size] [times]\n\r");
		//break;
		return;
	}
	*/		
	if(!strcmp(argv[1],""))
		init_start_test=0;
	else
		init_start_test=str2UL(argv[1]);		//Unit:1Mbytes			
			
	if(!strcmp(argv[2],""))
		sdram_size=0x100000;
	else
		sdram_size=str2UL(argv[2])*0x100000;		//Unit:1Mbytes
				
	if(!strcmp(argv[3],""))
		scan_times=1;
	else
		scan_times=str2UL(argv[3]);

	for(tx_scan=0;tx_scan<16;tx_scan++)
	{		
		//P_sd_clk_o_Dly[3:0], SDRAM Clock delay control for external SDRAM. This signal should be earlier 3ns before internal clk_m and clk_rx.
		sdram_scan_temp=REG32(0xb8003304)&0xFFFF0FFF;	//0xb8003304[n.15-n.12]
		REG32(0xb8003304)=sdram_scan_temp|(tx_scan<<12);					
		for(sdram_scan=init_start_test;sdram_scan<16;sdram_scan++)
		{
			sdram_scan_temp=REG32(0xb8003304)&0xFFFFFFF0;
			REG32(0xb8003304)=sdram_scan_temp|sdram_scan;					
			for(sdram_i=0;sdram_i<scan_times;sdram_i++)
			{
				if(GDMA_SDRAM_SCAN(sdram_size))
					printf("Pass ... ");
				else
				{
					printf("fail ... ");
					fail_cnt++;
				}
				printf("SCAN count= %d / %d ,fail_cnt=%d\n\r",sdram_i,scan_times,fail_cnt,REG32(0xb8003304));
			}
			printf("REG32(0xb8003304)=0x%08x , fail cnt=%d\n\r",REG32(0xb8003304),fail_cnt);

			if(fail_cnt==0){
				rx_para_record[tx_scan][sdram_scan]=0;
				if(rx_dly_Left_flag==0)
				{
					rx_dly_Left=sdram_i;
					rx_dly_Left_flag=1;
				}
			}
			else
			{
				rx_para_record[tx_scan][sdram_scan]=1;
				if(rx_dly_Left_flag==1)
				{
					rx_dly_Right=sdram_i-1;
					rx_dly_right_flag=1;
				}
			}
			fail_cnt=0;
		}
	}

	REG32(0xb8003304)=0x0d;
	printf("\n\r");
	printf("			 0 1 2 3 4 5 6 7 8 9 a b c d e f\n\r");
	for(tx_scan=0;tx_scan<16;tx_scan++)
	{
		printf("Scan result= ");
		for(sdram_scan=0;sdram_scan<16;sdram_scan++)
		{
			printf("%d ",rx_para_record[tx_scan][sdram_scan]);
		}
		printf("\n\r");
	}

	return 0;
}	
#endif

#ifdef CONFIG_SPI_NAND_FLASH
static int handler_spi_nand(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	int ret=-1;
	int page=0,i;
	char *snf_data_buf;
	char *snf_oob_buf;
	snf_oob_buf = (unsigned char*)0x80300000;
	snf_data_buf = (unsigned char*)0x80200000;
	if(!strcmp(argv[1],"read"))
	{
		/*DMA READ WITH ECC*/
		memset(snf_oob_buf,0xff,OOB_SIZE);
		memset(snf_data_buf,0xff,CHUNK_SIZE);		
	    printf("=============DMA-DATA===============================================\n\r");
		spi_nand_read_page_with_ecc(str2UL(argv[2]),snf_data_buf,snf_oob_buf,CHUNK_SIZE);
		cache_flush();
	    dump_mem((unsigned int)snf_data_buf,2048);
	    printf("=============DMA-OOB===============================================\n\r");
	    dump_mem((unsigned int)(snf_data_buf+2048),OOB_SIZE);
	}
    else if(!strcmp(argv[1],"erase"))
	{
		/*BLOCK ERASE*/
		int total_page_count = (chip_size >> page_shift);
		if(!strcmp(argv[2],"all")){
			for(i=0;i<total_page_count;i+=ppb)
				spi_nand_block_erase(i);
		}else{
			page = str2UL(argv[2]);
			if(spi_nand_block_erase(str2UL(argv[2]))==-1)
			{
				printf("%s: erase block %d fail!\n",__func__, page/ppb);
			}
		}
	}
    else if(!strcmp(argv[1],"pio"))
	{
		if(!strcmp(argv[2],"read"))
		{
			/*PIO READ*/
			memset(snf_oob_buf,0xff,OOB_SIZE);
			memset(snf_data_buf,0xff,CHUNK_SIZE);
	    	printf("=============PIO-DATA===============================================\n\r");
			spi_nand_pio_read(str2UL(argv[3]),snf_data_buf,CHUNK_SIZE,0);
	    	dump_mem((unsigned int)snf_data_buf,2048);
	    	printf("=============PIO-OOB===============================================\n\r");
	    	dump_mem((unsigned int)(snf_data_buf+2048),OOB_SIZE);					
		}else if(!strcmp(argv[2],"write"))
		{
			/*PIO WRITE*/
			spi_nand_block_erase(str2UL(argv[3]));
			memset(snf_oob_buf,0x00,OOB_SIZE);
			memset(snf_data_buf,0x00,CHUNK_SIZE);
			spi_nand_pio_write(str2UL(argv[3]),snf_data_buf,CHUNK_SIZE,0);
		}
	}
    else if(!strcmp(argv[1],"write"))
	{
		/*DMA WRITE WITH ECC*/
		spi_nand_block_erase(str2UL(argv[2]));
		memset(snf_oob_buf,0x00,OOB_SIZE);
		memset(snf_data_buf,0x00,CHUNK_SIZE);
		spi_nand_write_page_with_ecc(str2UL(argv[2]),snf_data_buf,snf_oob_buf,CHUNK_SIZE);		
	}else if(!strcmp(argv[1],"dump")){
		if(!strcmp(argv[2],"bbt")){
			dump_normal_BBT();
		}
	}	
	return 0;
}
#endif
#ifdef CONFIG_NAND_FLASH
static int handler_nand(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	char *load_buf;
	char *app_buf;
	unsigned char *oob_buf;
	int page, i, tmp;
	unsigned char id[6];

	oob_buf = (unsigned char *)malloc(oob_size);

	if(!strcmp(argv[1],"id"))
	{			
		//rtk_nand_read_id(id);
		rtk_nand_read_id();
	}
	else if(!strcmp(argv[1],"read"))
	{
		get_param(&bParam);
		load_buf = (char*)bParam.load;
		app_buf = (char*)bParam.app;
		//ccwei 111116			
		if(!NAND_ADDR_CYCLE)
		{
#ifdef CONFIG_NAND_PAGE_2K
			rtk_PIO_read(str2UL(argv[2]),0,528,load_buf);
		    rtk_PIO_read(str2UL(argv[2]),528,528,load_buf+528);
		    rtk_PIO_read(str2UL(argv[2]),1056,528,load_buf+1056);
			rtk_PIO_read(str2UL(argv[2]),1584,528,load_buf+1584);
			printf("=============PIO-DATA===============================================\n\r");
			dump_mem((unsigned int)load_buf,512);
			dump_mem((unsigned int)load_buf+528,512);				
			dump_mem((unsigned int)load_buf+1056,512);							
			dump_mem((unsigned int)load_buf+1584,512);										
			printf("=============PIO-OOB===============================================\n\r");
			dump_mem((unsigned int)load_buf+512,16);
			dump_mem((unsigned int)load_buf+1040,16);
			dump_mem((unsigned int)load_buf+1568,16);
			dump_mem((unsigned int)load_buf+2096,16);
				
		    printf("=============DMA-DATA===============================================\n\r");
		    rtk_read_ecc_page(str2UL(argv[2]),load_buf,oob_buf,2048);
		    dump_mem((unsigned int)load_buf,2048);
		    printf("=============DMA-OOB===============================================\n\r");
		    dump_mem((unsigned int)oob_buf,64);
#endif
#ifdef CONFIG_NAND_PAGE_512				
					rtk_PIO_read_512(str2UL(argv[2]),load_buf);
					printf("=============PIO-DATA===============================================\n\r");
					dump_mem((unsigned int)load_buf,512);
					printf("=============PIO-OOB===============================================\n\r");
					dump_mem((unsigned int)load_buf+512,16);
					printf("=============DMA-DATA===============================================\n\r");
					rtk_read_ecc_page(str2UL(argv[2]),load_buf,oob_buf,512);
					dump_mem((unsigned int)load_buf,512);
					printf("=============DMA-OOB===============================================\n\r");
					dump_mem((unsigned int)oob_buf,16);
#endif				
		}
		else
		{
			//rtk_read_ecc_page(str2UL(argv[2]),load_buf,oob_buf,512);
#ifdef CONFIG_NAND_PAGE_512				
			rtk_PIO_read_512(str2UL(argv[2]),load_buf);
			printf("=============PIO-DATA===============================================\n\r");
			dump_mem((unsigned int)load_buf,512);
			printf("=============PIO-OOB===============================================\n\r");
			dump_mem((unsigned int)load_buf+512,16);
#endif				
			printf("=============DMA-DATA===============================================\n\r");
		    rtk_read_ecc_page(str2UL(argv[2]),load_buf,oob_buf,512);
			dump_mem((unsigned int)load_buf,512);
			printf("=============DMA-OOB===============================================\n\r");
			dump_mem((unsigned int)oob_buf,16);

		}
	}
	else if(!strcmp(argv[1],"erase"))
	{
		//ccwei 111116			
		if(!strcmp(argv[2],"all"))
		{
			//skip boot + loader!!!
			tmp = (LOADER_IMAGE_SIZE)/block_size;
			if(tmp==0)
				tmp +=2; //64K/128K = 0;
			else
				tmp +=1; //64K/16K = 4;
			printf("hi tmp = %d\n\r",tmp);

        	for(i=(tmp*ppb);i<(chip_size/page_size);i+=ppb)
			{
				printf("page = %d block %d\n\r",i,i/ppb);
				if(rtk_erase_block(i) == -1)
				{
					int k=0;
					if(!NAND_ADDR_CYCLE)
                    	oob_buf[OOB_BBI_OFF] = 0x00;
                    else
                        oob_buf[5] = 0x00;
					if(isLastPage)
					{
				    	for(k=1;k<3;k++)
						{
                        	rtk_write_ecc_page(i+(ppb-k),load_buf,oob_buf,page_size);
				        }
					}
					else
					{
				        for(k=0;k<2;k++)
						{
                        	rtk_write_ecc_page(i+k,load_buf,oob_buf,page_size);
				        }
					}
					
					printf("erase block %d fail!\n", i/ppb);
				}
			}
		}
		else
		{
			page = str2UL(argv[2]);
			if(rtk_erase_block(page) == -1)
			{
				//ccwei: 120228
				printf("%s: erase block %d fail!\n",__FUNCTION__, page/ppb);
            	if(!NAND_ADDR_CYCLE)
					oob_buf[OOB_BBI_OFF] = 0x00;
            	else
            		oob_buf[5] = 0x00;

				if(isLastPage)
				{
					for(i=1;i<3;i++)
                		rtk_write_ecc_page(page+(ppb-i),load_buf,oob_buf,page_size);
				}
				else
				{
					for(i=0;i<2;i++)
                    	rtk_write_ecc_page(page+i,load_buf,oob_buf,page_size);			  
				}		  
			}
		}
	}
    else if(!strcmp(argv[1],"write"))
	{
		int kk;
        load_buf = (char*)bParam.load;
        for(kk = 0; kk < 16; kk++)
        	load_buf[kk] = kk;
        page = str2UL(argv[2]);
        if(page == 0)
		{
            printf("you can't write page 0!\n");
        }
		else
		{
            if(!NAND_ADDR_CYCLE)
				oob_buf[OOB_BBI_OFF] = 0x00;
            else
                oob_buf[5] = 0x00;
			if(isLastPage)
			{
				rtk_write_ecc_page(page+(ppb-1),load_buf,oob_buf,page_size);
			}
			else
			{
                rtk_write_ecc_page(page,load_buf,oob_buf,page_size);
			}
		}
	}		
	else if(!strcmp(argv[1],"probe"))
	{
		rtk_parallel_nand_probe();
	}
		
	return 0;
}
#endif

static int handler_tt(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	printf("time=%d\n\r", get_sys_time());

	return 0;
}

#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
static int handler_root(char argv[MAX_ARGS][MAX_ARG_LEN+1])
{
	unsigned partFlag;
	char bbuf[2];
	partFlag = *(unsigned int *)IMAGE_LOCATION_FLAG_ADDR;
	if (!partFlag)
		printf("root: first 0x%x\n", FIRST_IMAGE_ADDR);
	else
		printf("root: second 0x%x\n", SECOND_IMAGE_ADDR);

	printf("Change root to (1)first (2)second ? ");
	kbd_proc(bbuf, 1);
	if (bbuf[0] == '1') {
		partFlag = 0;
		printf("Change to first ");
		amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &partFlag, 4);
		printf("\n");
	}
	else if (bbuf[0] == '2') {
		partFlag = 0xffffffff;
		printf("Change to second ");
		amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &partFlag, 4);
		printf("\n");
	}
	else
		printf("No changed !\n\r");

	return 0;
}
#endif

#if defined (CONFIG_DEBUG_CMD)
static int handle_memset(char argv[MAX_ARGS][MAX_ARG_LEN+1])  {
	unsigned int addr, size;
	int  val;
	addr = str2UL(argv[1]);
	val = str2UL(argv[2]);
	size = str2UL(argv[3]);
	
	memset((void *)addr, val, size);
	return 0;
}

static int handle_memcmp(char argv[MAX_ARGS][MAX_ARG_LEN+1])  {
	unsigned int addr1, addr2, size, n;
	unsigned char *p1, *p2;
	
	addr1 = str2UL(argv[1]);
	addr2 = str2UL(argv[2]);
	size =  str2UL(argv[3]);
	
	p1 = (unsigned char *)addr1;
	p2 = (unsigned char *)addr2;
	for (n = 0; n < size; n++) {
		if (p1[n]!=p2[n]) {
			printf("Mismatch!\r\n addr %x[%x]!=%x[%x]\r\n", &p1[n],p1[n],&p2[n],p2[n]);
			return 0;
		}
	}
	printf("Identical! \r\naddr %x=%x(%xh)\r\n", p1,p2,size);
	
	return 0;
}

static int enable_cp3(void) {
	
	__asm__ __volatile__ (
		"	.set	push						\n"
		"	.set	noreorder					\n"			
		" 	mfc0	$8,	$12						\n"
		"	or		$8,0x80000000				\n"
		" 	mtc0	$8, $12						\n"
		"	nop									\n"		
	);		
}

static int handle_dmem(char argv[MAX_ARGS][MAX_ARG_LEN+1])  {
	unsigned int addr, top;
	addr = str2UL(argv[1]);
	#define DMEM_SIZE 0x2000
	#define DMEM_MASK (DMEM_SIZE-1) 	
	addr &= 0xfffe000;
	top  = addr + DMEM_SIZE - 1;
	
	enable_cp3();
	
	printf("Loading %x-%x to DMEM\r\n",addr, top);
	
	__asm__ __volatile__ (
		"	.set	push						\n"
		"	.set	noreorder					\n"			
		" 	mtc3	%0, $4						\n"
		"	nop									\n"
		" 	mtc3	%1, $5						\n"
		"	nop									\n"
		"	mtc0	$0, $20						\n"
		"	nop									\n"
		"	li		$8,0x00000400				\n"
		"	mtc0	$8, $20						\n"
		"	.set	pop							\n"
		:
		: "r" (addr), "r" (top)		
		: "$8"
	);
	return 0;
}
#endif 

//the command table
typedef struct cmd_table{
	char const command[MAX_ARG_LEN+1];
	int (*handler)(char argv[MAX_ARGS][MAX_ARG_LEN+1]);
	int min_args;
	char *help_text;
}CMD_TABLE;

CMD_TABLE cmdTable[] =
{
	//command			handler			min. # paras.		help text
	{"app",				handler_app,			1,		HELP_APP},
#ifdef CONFIG_SPANSION_16M_FLASH
	{"bank",			handler_bank,			0,		HELP_BANK},
#endif	
	{"bootline",			handler_bootline,		0,		HELP_BOOTLINE},
#ifndef CONFIG_NO_NET
	{"bootp",			handler_bootp,			0,		HELP_BOOTP},
#endif /*CONFIG_NO_NET*/
#ifdef CONFIG_MAC0_LBK
	{"cali",			handler_cali,			2,		HELP_CALI},
#endif
	{"d",				handler_d,			2,		HELP_D},
	{"entry",			handler_entry,			1,		HELP_ENTRY},
#ifndef CONFIG_NO_FLASH
	{"ferase",			handler_ferase,			2,		HELP_FERASE},
	{"fwrite",			handler_fwrite,			2,		HELP_FWRITE},
	{"flashsize",			handler_flashsize,		1,		HELP_FLASHSIZE},
#endif /*CONFIG_NO_FLASH*/
#ifdef	CONFIG_GDMA_SCAN
	{"gdma_scan",			handler_gdma_scan,		3,		HELP_GDMA_SCAN},
#endif
	{"help",			handler_help,			0,		HELP_HELP},
	{"info",			handler_info,			0,		HELP_INFO},
	{"load",			handler_load,			1,		HELP_LOAD},
	{"mac",				handler_mac,			1,		HELP_MAC},
	{"memsize",			handler_memsize,		3,		HELP_MEMSIZE},
	{"miisel",			handler_miisel,			1,		HELP_MIISEL},
#ifdef SUPPORT_MULT_UPGRADE
	{"multicast",			handler_multicast,		0,		HELP_MULTICAST},
#endif
#ifdef CONFIG_NAND_FLASH
	{"nand",			handler_nand,			2,		HELP_NAND},
#endif	
#ifdef CONFIG_SPI_NAND_FLASH
	{"nand",			handler_spi_nand,			2,		HELP_SPI_NAND},
#endif
	{"r",				handler_r,			1,		HELP_R},
	{"reboot",			handler_reboot,			0,		HELP_REBOOT},
#ifndef CONFIG_NO_FLASH
	{"resetcfg",			handler_resetcfg,		0,		HELP_RESETCFG},
#endif /*CONFIG_NO_FLASH*/
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	{"root",			handler_root,			0,		HELP_ROOT},
#endif
	{"run",				handler_run,			2,		HELP_RUN},
#ifndef CONFIG_NO_FLASH
	{"sdram",			handler_sdram,			0,		HELP_SDRAM},
#endif /*CONFIG_NO_FLASH*/
	{"tt",				handler_tt,			0,		HELP_TT},
#if !defined(CONFIG_NO_FLASH) && !defined(CONFIG_NO_NET)
	{"tftp",			handler_tftp,			3,		HELP_TFTP},
#endif
#ifndef CONFIG_NO_NET
	{"tftpm",			handler_tftpm,			3,		HELP_TFTPM},
#endif
	{"w",				handler_w,			2,		HELP_W},
#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
	{"web",				handler_web,			0,		HELP_WEB},
#endif	
	{"xmodem",			handler_xmodem,			1,		HELP_XMODEM},
#if 0 //shrink bootloader size, remove unnecessary code
	{"dmemdma",			handler_dmemdma	,		0,		HELP_DMEMDMA},
	{"flashcheck",			handler_flashckeck,		0,		HELP_FLASHCHECK},
	{"flashconetnt",		handler_flashcontent,		0,		HELP_FLASHCONTENT},
	{"imemdma",			handler_imemdma,		0,		HELP_IMEMDMA},
	{"memcpy",			handeler_memcpy,		0,		HELP_MEMCPY},
	{"r16",				handler_r16,			1,		HELP_R16},
	{"r8",				handler_r8,			1,		HELP_R8},
	{"readtest",			handler_readtest,		0,		HELP_READTEST},
	{"runx",			handler_runx,			1,		HELP_RUNX},
	{"sram",			handler_sram,			0,		HELP_SRAM},
	{"tftpx",			handler_tftpx,			4,		HELP_TFTPX},
	{"w16",				handler_w16,			2,		HELP_W16},
	{"w8",				handler_w8,			2,		HELP_W8},
#endif	//#if 0 shrink bootloader size, remove unnecessary code
#ifdef CONFIG_PCIE_HOST
	{"hrst",			handler_hrst,			0,		HELP_HRST},
	{"pcie_r",			handler_pcie_r,			2,		HELP_PCIE_R},
	{"pcie_w",			handler_pcie_w,			3,		HELP_PCIE_W},
	{"pcie_rst",			handler_pcie_rst,		0,		HELP_PCIE_RST},
	{"Pcie_D_LBK",			handler_Pcie_D_LBK,		0,		HELP_PCIE_D_LBK},
	{"Pcie_LBK",			handler_Pcie_LBK,		0,		HELP_PCIE_LBK},
#endif
#ifdef CONFIG_RTL8685_MEMCTL_CHK
#if !defined(CONFIG_BOOT_MIPS) && !defined(CONFIG_MIPS)
	{"cali_8685",		handle_cali_8685,			0,	HELP_CALI_8685},
#endif
	{"diag_8685",		handle_diag_8685,			1, 	HELP_DIAG_8685},
#endif
#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
	{"freq_8685s",		handle_freq_8685s,			0, 	HELP_FREQ_8685S},
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */
#if defined(CONFIG_MEM_TEST)
	{"dram_test",		handle_dram_test,			2, 	HELP_DRAM_TEST},
#ifdef CONFIG_RTL8676S
	{"flash_test",		handle_flash_test,			2, 	HELP_FLASH_TEST},
#else
	{"dram_test_dp",		handle_dram_test_dp,		2, 		HELP_DRAM_TEST_DP},
	{"dram_test_ip",		handle_dram_test_ip,		2, 		HELP_DRAM_TEST_IP},
	{"dram_test_bp",		handle_dram_test_bp,		2, 		HELP_DRAM_TEST_BP},
#ifndef CONFIG_MIPS
	{"dram_test_all",		handle_dram_test_all,		2, 		HELP_DRAM_TEST_ALL},
#endif //CONFIG_MIPS
#endif //CONFIG_RTL8676S
#endif //CONFIG_MEM_TEST
#ifdef CONFIG_L2C_TEST
	{"l2c",				handler_l2c_mode,		1,		HELP_L2C_MODE},
#endif
#ifdef CONFIG_BTG_TEST
	{"btg",				handler_btg_test,		0, 		HELP_BTG_TEST},
#endif
#if defined (CONFIG_DEBUG_CMD)
	{"memset",		handle_memset,			3, 	HELP_MEMSET},
	{"memcmp",		handle_memcmp,			3, 	HELP_MEMCMP},
	{"dmem",		handle_dmem,			1, 	HELP_DMEM},
#endif
};


void print_help()
{
	int i;
	for(i = 0; i < countof(cmdTable); i++)
	{
		printf("%s", cmdTable[i].help_text);
	}
}
#ifdef CONFIG_BTG_TEST
int current_cmd_argc = 0;
#endif
int cmd_proc(char *buf)
{
	unsigned int	i;
	char argv[MAX_ARGS][MAX_ARG_LEN+1];
	int	argc = 0;
	int arg_idx = 0;

	//parse the input command into argv[0], argv[1], ..., argv[MAX_ARGS]
	//argv[0] is the command
	//argv[1~MAX_ARGS] are the parameters

	for(i=0; buf[i]!='\0'; i++)
	{
		if(buf[i]==' '){
			argv[argc][arg_idx]='\0';
			argc++;
			arg_idx=0;
		}
		else {
			if(arg_idx<MAX_ARG_LEN)
			{	
				argv[argc][arg_idx]=buf[i];
				arg_idx++;
			}
		}
	}
	argv[argc][arg_idx]='\0';
	if(arg_idx)
		argc++;

	//find the command handler here
	for(i = 0; i < countof(cmdTable); i++)
	{
		if(!strcmp(argv[0], cmdTable[i].command))
		{
			if(argc > cmdTable[i].min_args)
			{	
#ifdef CONFIG_BTG_TEST
				current_cmd_argc = argc;
#endif
				if(cmdTable[i].handler(argv) != 0) printf("Command: %s failed\n\r", cmdTable[i].command);
			}
			else
			{
				printf("Number of the parameters is invalid!\n\r");
				printf("Usage: %s\n\r",cmdTable[i].help_text);
			}
			break;
		}
	}

	if(i >= countof(cmdTable))
	{
		if(buf[0] != '\0')
			printf("%s: command not found\n\rtype 'help' for the commands info.\n\r\n\r", argv[0]);
	}
	
	memset(argv, 0, sizeof(argv));
	printf("%s", cmd_prompt);
	return TRUE;
}

