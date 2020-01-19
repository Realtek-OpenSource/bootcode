/*************************************************************
*
*  Loader.c - main loader routine
*
*
* History:
*  10/23/2006	SH, Lee	added flashsize/memsize/miisel commands. (00.01.11=>12)
*  10/26/2006   SH, Lee Added big model support
*  10/20/2006   SH, Lee	Determine memory configration from flash (00.01.12=>00.01.12a)
*  03/12/2007	SH	Fix cannot upgrade fw from web if decompress failed
*  07/14/2008   SH  Upgrade version from 7c to 08
*  07/30/2008   Andrew, 08a add SPI controller patch (tylo)
*  08/01/2008   SH  Merge Tylo 0.9 and Andrew 0.8a to 0.9a, identify RL6028A/6028B and 6085 for MAC delay
*  04/28/2009   SH  Links up MAC5 for IAD demoboard v.1.0.3
*************************************************************/

/*
DESCRIPTION
Initialize user application code.
*/ 

#include "../tcp/cc.h"
#include "../tcp/err.h"
#include "../tcp/def.h"
#include "../tcp/http.h"
#include "zlib.h"
#include "board.h"
#ifndef CONFIG_NO_FLASH
#include "mib.h"
#endif /*CONFIG_NO_FLASH*/
#include "../tftpnaive/net.h"
#include "../tftpnaive/tftpnaive.h"
#if CONFIG_OSK_APP_SUPPORT	
#include "osk.h"	
#endif

#ifdef CONFIG_RTL8685_MEMCTL_CHK
#include "../boot/memctl/memctl_8685.h"
#endif

#if defined(CONFIG_SPI_NAND_FLASH) || defined(CONFIG_NAND_FLASH)
#include "rtk_nand_base.h"
#endif
#ifdef CONFIG_SPI_NAND_FLASH
#include "spi_nand.h"
#endif
#ifdef CONFIG_NAND_FLASH
#include "rtk_nand.h" // rtk_check_nand_space, DRAM_DLOADER_ADDR
#endif
#if defined(CONFIG_RTK_NORMAL_BBT) || defined(CONFIG_RTK_REMAP_BBT)
#include "rtk_bbt.h"
#endif
//#ifdef SUPPORT_MULT_UPGRADE
#include "gpio.h"
//#endif
#ifndef ERROR
#define ERROR 	-1
#endif

#ifndef OK
#define OK		0
#endif

//ccwei 111031
#if defined(CONFIG_SPI_NAND_FLASH) || defined(CONFIG_NAND_FLASH)
int tftp_img=0;
//bootloader will use these parameter to note kernel the rootfs image addr.
long SDKStart=0;
long SDKStart2=0;
long RunBackup=0;
#endif
//end 

#ifdef CONFIG_SPI_FLASH
extern unsigned int SPI_DEFAULT;
extern unsigned int SPI_CMD;
#endif

const char logo_msg[] = 
{
	"(c)Copyright Realtek, Inc. 2013" 
#ifdef E8B_SUPPORT
	"\n\rProject RTL8676/RTL8685 E8 LOADER (LZMA)\n\rVersion "
#elif IMAGE_DOUBLE_BACKUP_SUPPORT
	"\n\rProject RTL8676/RTL8685 Dual-Image LOADER (LZMA)\n\rVersion "
#else
	"\n\rProject RTL8676/RTL8685 LOADER (LZMA)\n\rVersion "
#endif
};

const char version_msg[] =
{
#if defined(COMBO)
        "2.2.3(" CONFIG_SOC_NAME ")"
#elif defined(E8B_SUPPORT)
        "00.01.07"
#else
        "00.00.01"
 #endif

#ifdef CONFIG_NO_FLASH
	" w/o flash "
#endif /*CONFIG_NO_FLASH*/
};


typedef void (*nb_func_t)(void);
static nb_func_t nb_tasks[4];

int register_nonblocking_task(nb_func_t x){
	int i; 
	for(i; i<sizeof(nb_tasks)/sizeof(nb_tasks[0]); i++) {
		if (nb_tasks[i])
			continue;
		nb_tasks[i] = x;
		return 0;
	}
	return -1;
}

void run_nonblocking_tasks(void) {
	int i; 
	for(i; i<sizeof(nb_tasks)/sizeof(nb_tasks[0]); i++) {	
		if (nb_tasks[i])
			nb_tasks[i]();
	}
}



/*ql:20080721 START: use icVer to record the version of IC*/
#ifdef MULTI_IC_SUPPORT
#define IC8671				1
//#define IC8671P				2
#define IC8672				3
#define IC8671B				4
#define IC8671B_costdown	5
#endif
/*ql:20080721 END*/

/* for loader debug function */
board_param_t bParam;
//board_param_t bdinfo;

#ifdef CONFIG_MIPS
#define cache_flush mips_cache_flush
#else
void cache_flush(void){
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
#endif

#ifdef CONFIG_SPI_FLASH
unsigned int spiflash=0;//indicate if it is spi flash
#endif /*CONFIG_SPI_FLASH*/
/*move to board.h*/
#if 0
#ifdef CONFIG_SPI_FLASH
#define _PARM_BACK_START_spi		0x0
//because spansion flash sector is 64KB, so I will backup 64KB for all spi flash
#define _PARM_BACK_SZ_spi		0x10000
#endif

#ifndef CONFIG_SPANSION_16M_FLASH
//reserve only 0x40 space for boot parameter in case the size of bootloader large than 64k-256,  #define _PARM_START		0xbfc0ff00
#define _PARM_START			(FLASH_BASE_ADDR+0x0ffc0)
#define _PARM_END			(FLASH_BASE_ADDR+0x10000)
#define _PARM_SZ			0x40
#define _PARM_BACK_START	0x8000
#define _PARM_BACK_SZ		0x8000
#else
/*ql 20090116 16M spansion flash is Uniform 64Kword/128KByte Sector Architecture, so set_param() should be different from previous*/
#define _PARM_START			(FLASH_BASE_ADDR+0x1ff00)
#define _PARM_END			(FLASH_BASE_ADDR+0x20000)
#define _PARM_SZ			0x100
#define _PARM_BACK_START	0x0
#define _PARM_BACK_SZ		0x20000
#endif
#endif

#ifdef CONFIG_NFBI_SLAVE
int get_bootconf(bootconf_t *pbc)
{

	if(pbc)
	{
		memcpy( pbc, (void*)BOOTCONF_START, BOOTCONF_SIZE );
		if(pbc->magic==BOOTCONF_MAGIC)
		{
			//printf( "get_bootconf(): ok\n\r" );
			return 0;
		}
	}
	//printf( "get_bootconf(): failed\n\r" );
	return -1;
}
#endif /*CONFIG_NFBI_SLAVE*/

int get_param(board_param_p buf)
{
	int ret;
	unsigned long addr;
	char id_buf[8];
	char *ptr;
#ifdef CONFIG_SPANSION_16M_FLASH
#ifdef BANK_AUTO_SWITCH
	int	tmp = getbank();
	s29gl128bank(0);
#endif
#endif
	for(addr=_PARM_START; addr<_PARM_END; addr+=BOARD_PARAM_SZ) {
#ifdef CONFIG_NO_FLASH
		memcpy(id_buf, addr, sizeof(id_buf) );
		ret=OK;
#else  /*CONFIG_NO_FLASH*/
 		ret = amd29lvReadNV(id_buf, addr, sizeof(id_buf));
#endif /*CONFIG_NO_FLASH*/
		if(ret!=OK)
			return ret;
		if(!strcmp(id_buf, BOARD_PARAM_ID)) {
#ifdef CONFIG_NO_FLASH
			memcpy(buf, addr, BOARD_PARAM_SZ );
			ret=OK;
#else  /*CONFIG_NO_FLASH*/
	 		ret = amd29lvReadNV(buf, addr, BOARD_PARAM_SZ);
#endif /*CONFIG_NO_FLASH*/
			if(ret!=OK)	return ret;
			goto get_mac_ip;
		}
	}
	
	
	if(addr>=_PARM_END) {
		memset(buf, 0, sizeof(board_param_t));
		strcpy(buf->id, BOARD_PARAM_ID);
		strcpy(buf->bootline, BOARD_PARAM_BOOT);
		memcpy(buf->mac[0], BOARD_PARAM_MAC, 6);
		buf->entry = BOARD_PARAM_ENTRY;
		buf->load = BOARD_PARAM_LOAD;
		if(IS_BOOT_NAND){
//ccwei 
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
//			buf->app = DRAM_DLOADER_ADDR;
            buf->app = DRAM_DIMAGE_ADDR;

#endif
		}else{
			buf->app = BOARD_PARAM_APP;
		}
		buf->ip = BOARD_PARAM_IP;
		buf->flash_size = BOARD_PARAM_FLASHSIZE;
#ifdef SDRAM_AUTO_DETECT
{
		#include "rtl8672_asicregs.h"
		buf->mem_size = *(unsigned int *)MTCR0;
}
#else
		buf->mem_size = BOARD_PARAM_MEMSIZE;
#endif
		buf->MII_select = BOARD_PARAM_MIISEL;

#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
		buf->sysclk_ctrl = CONFIG_SYS_PLL_CTRL;
		buf->mckg_freq_div = CONFIG_MCKG_FREQ_DIV;
		buf->lx_pll_sel = CONFIG_LX_PLL_SEL;
		buf->cmu_ctrl = CONFIG_SYS_CMU_CTRL;
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */

#ifndef CONFIG_NO_FLASH
		///ql 20090115 if bParam is null, then read MAC from runtime config
#ifdef CONFIG_SPI_FLASH 	
		ptr = (char *)FLASH_BASE_ADDR+HW_SETTING_OFFSET;
		if( memcmp(ptr, HS_CONF_SETTING_SIGNATURE_TAG, SIGNATURE_LEN) == 0 ) {
			HW_MIB_Tp pHWMIB;
			ptr+=sizeof(PARAM_HEADER_T);
			pHWMIB = (HW_MIB_Tp)ptr;
			memcpy(buf->mac[0], pHWMIB->elanMacAddr, MAC_ADDR_LEN);
		}
#endif	//endif CONFIG_SPI_FLASH	
		//printf("%s get mac:%02x-%02x-%02x-%02x-%02x-%02x\n\r", __FUNCTION__, buf->mac[0][0], 
		//	buf->mac[0][1], buf->mac[0][2], buf->mac[0][3], buf->mac[0][4], buf->mac[0][5]);
#endif //CONFIG_NO_FLASH
	}

get_mac_ip:
#ifdef CONFIG_NO_FLASH
	#ifdef CONFIG_NFBI_SLAVE
	{
		bootconf_t bc;
		if(get_bootconf(&bc)==0)
		{
			memcpy(buf->mac[0], bc.mac, 6);
			buf->ip=bc.ip;
		}
	}
	#endif /*CONFIG_NFBI_SLAVE*/
#else /*CONFIG_NO_FLASH*/
	//ql 20090115 START: comment below code, just read MAC from bparam, because bparam is consistent with MAC of runtime config

#if 0	
	/*
	 	To avoide the conflict between kernel and bootloader,
		here separate the IP address location definition from curent-setting!
	 */

	//read MAC of runtime config
	ptr = (char *)FLASH_BASE_ADDR+HW_SETTING_OFFSET;
	if( memcmp(ptr, HS_CONF_SETTING_SIGNATURE_TAG, SIGNATURE_LEN) == 0 ) {
		HW_MIB_Tp pHWMIB;
	    ptr+=sizeof(PARAM_HEADER_T);
	    pHWMIB = (HW_MIB_Tp)ptr;
		memcpy(buf->mac[0], pHWMIB->elanMacAddr, MAC_ADDR_LEN);
	}
#endif
	//ql 20090115 END
	//read IP of runtime config
#ifdef CONFIG_SPI_FLASH
	/*ptr = (char *)FLASH_BASE_ADDR+CURRENT_SETTING_OFFSET;
	if( memcmp(ptr, CS_CONF_SETTING_SIGNATURE_TAG, SIGNATURE_LEN) == 0 ) {
		MIB_Tp pMIB;
	    ptr+=sizeof(PARAM_HEADER_T);
	    pMIB = (MIB_Tp)ptr;
		memcpy(&buf->ip, pMIB->ipAddr, IP_ADDR_LEN);
	}*/
#endif //endif CONFIG_SPI_FLASH
#ifdef CONFIG_SPANSION_16M_FLASH
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(tmp);
#endif
#endif 	
#endif /*CONFIG_NO_FLASH*/
	return OK;
}

#ifdef CONFIG_NO_FLASH
int set_param(board_param_p buf)
{
	memcpy(_PARM_START, buf, sizeof(board_param_t));
	return OK;
}
#else /*CONFIG_NO_FLASH*/
#ifndef CONFIG_SPANSION_16M_FLASH
int set_param(board_param_p buf)
{
	unsigned long addr;
	//ql 20090115 when modify mac, then update to HW-setting
	char *pBackup;
/* 
 * Erase parameter section (the last section of bootloader flash layout) before set 
 * the new one. If defined #if 1 that means using the old fashion.
 */

	addr = _PARM_START;
#ifdef CONFIG_SPI_FLASH
	if (spiflash)
	{
		pBackup = (char *)DRAM_TEMP_LOAD_ADDR;
		
	        //Clean memory backup section 
		memset(pBackup, 0xff, _PARM_BACK_SZ_spi);
	        //Copy backup section from flash to memory(128K/sector)
		memcpy(pBackup, (const void *)(FLASH_BASE_ADDR+_PARM_BACK_START_spi), _PARM_BACK_SZ_spi);
	        //Set modified parameters
		memcpy(pBackup+_PARM_BACK_SZ_spi-_PARM_SZ, buf, sizeof(board_param_t));

	        //program the backup section and modified parameters
		amd29lvWrite(FLASH_BASE_ADDR+_PARM_BACK_START_spi, pBackup,_PARM_BACK_SZ_spi);
	}
	else {
#endif
		pBackup = malloc(_PARM_BACK_SZ);
		if(!pBackup)	return ERROR;

	        //Clean memory backup section 
		memset(pBackup, 0xff, _PARM_BACK_SZ);
	        //Copy backup section from flash to memory
		memcpy(pBackup, (const void *)(FLASH_BASE_ADDR+_PARM_BACK_START), _PARM_BACK_SZ-_PARM_SZ);
	        //Set modified parameters
		memcpy(pBackup+_PARM_BACK_START-_PARM_SZ, buf, sizeof(board_param_t));        
		
		if(amd29lvEraseNV(_PARM_BACK_START, _PARM_BACK_SZ) != OK) {
			printf("erase 0x%x fail\n\r", _PARM_BACK_START);
			free(pBackup); return ERROR;
		}

#ifndef IMAGE_DOUBLE_BACKUP_SUPPORT
		if(amd29lvEraseNV(_PARM_BACK_START+_PARM_BACK_SZ-_PARM_SZ, _PARM_SZ) != OK) {
			free(pBackup); return ERROR;
		}
#endif

	        //program the backup section and modified parameters
		amd29lvWriteNV(FLASH_BASE_ADDR+_PARM_BACK_START, pBackup,_PARM_BACK_SZ );
		free(pBackup);
#ifdef CONFIG_SPI_FLASH
	}
#endif
        return OK;
}
#else
//ql 20090116 only for 16M spansion flash
int set_param(board_param_p buf)
{

#ifdef BANK_AUTO_SWITCH
	int tmp =getbank();
#endif
	//ql 20090115 when modify mac, then update to HW-setting

#ifdef BANK_AUTO_SWITCH
	s29gl128bank(0);
#endif

	//char *pBackup = malloc(_PARM_BACK_SZ);
	//if(!pBackup) {
	//	printf("malloc 128K fail\n\r");
	//	return ERROR;
	//}
	char *pBackup = (char *)DRAM_TEMP_LOAD_ADDR;

        //Clean memory backup section 
	memset(pBackup, 0xff, _PARM_BACK_SZ);
        //Copy backup section from flash to memory(128K/sector)
	memcpy(pBackup, FLASH_BASE_ADDR+_PARM_BACK_START, _PARM_BACK_SZ);
        //Set modified parameters
	memcpy(pBackup+_PARM_BACK_SZ-_PARM_SZ, buf, sizeof(board_param_t));

        //program the backup section and modified parameters
	//amd29lvWriteNV(FLASH_BASE_ADDR, pBackup,SECTOR_SIZE);
	amd29lvWrite(FLASH_BASE_ADDR, pBackup,_PARM_BACK_SZ);
	//free(pBackup);

	//ql 20090115 when modify mac, then update to HW-setting
#if 0//HW_SETTING_OFFSET==0x20000
#define HW_SETTING_SIZE	0x20000
	char *ptr;

	ptr = (char *)FLASH_BASE_ADDR+HW_SETTING_OFFSET;
	if( memcmp(ptr, HS_CONF_SETTING_SIGNATURE_TAG, SIGNATURE_LEN) == 0 ) {
		int i, idx;
		for(i=0;i<N_MAC;i++)
		{
			char *mac;
			mac = buf->mac[i];
			if(mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5])
				break;
		}
		if (i < N_MAC) 
			idx=i;
		else
			idx=0;
		{
			HW_MIB_Tp pHWMIB;
			
			memset(pBackup, 0xff, _PARM_BACK_SZ);
			memcpy(pBackup, FLASH_BASE_ADDR+HW_SETTING_OFFSET, HW_SETTING_SIZE);
			
			pHWMIB = (HW_MIB_Tp)(pBackup+sizeof(PARAM_HEADER_T));			
			memcpy(pHWMIB->elanMacAddr, buf->mac[idx], MAC_ADDR_LEN);

			amd29lvWrite(FLASH_BASE_ADDR+HW_SETTING_OFFSET, pBackup,HW_SETTING_SIZE );
			//printf("%s set mac(%d):%02x-%02x-%02x-%02x-%02x-%02x\n\r", __FUNCTION__, idx, buf->mac[idx][0], 
			//	buf->mac[idx][1], buf->mac[idx][2], buf->mac[idx][3], buf->mac[idx][4], buf->mac[idx][5]);
		}
	}
#endif
	//free(pBackup);

#ifdef BANK_AUTO_SWITCH
	s29gl128bank(tmp);
#endif
        return OK;
}

#endif

/* This function writes hardware address to flash matching the OSK's internal configuration
* 
*/
#define OSK_CFG_SIZE 0x2000
#define OSK_CFG_ADDR 0xBFC04000
int set_osk_hwaddr(unsigned char *hwaddr)
{
	int ret = ERROR;
	char *pCfg;

	pCfg = malloc( OSK_CFG_SIZE );
	if(!pCfg)	
		goto ERROR1;

	memcpy(pCfg, (const void *)OSK_CFG_ADDR, OSK_CFG_SIZE); // backup current config.
	memset(&pCfg[ 0x1f80 ], 0xff, 0x10); 
	memcpy(&pCfg[ 0x1f80 ], hwaddr, 6); // append mac to the end.
	if (OK != amd29lvEraseNV(0x4000, OSK_CFG_SIZE)) 
		goto ERROR2;
	// SST flash has uniform 4k block.
	if (OK != amd29lvEraseNV(0x5000, 0x1000)) 
		goto ERROR2;
	
	ret = amd29lvWriteNV(OSK_CFG_ADDR, pCfg, OSK_CFG_SIZE);
ERROR2:
	free(pCfg);
ERROR1:
	return ret;
}
#endif /*CONFIG_NO_FLASH*/


#ifdef CONFIG_NFBI_SLAVE
#define NFBI_BASE				0xb8019000
#define NFBI_SYSCR				(NFBI_BASE+0x08)
#define NFBI_SYSSR				(NFBI_BASE+0x0c)
#define NFBI_ISR				(NFBI_BASE+0x14)
#define NFBI_SR_CheckSumDone	(1<<15)
#define NFBI_SR_CheckSumOK		(1<<14)
#define NFBI_SR_BootcodeReady	(1<<5)
#define NFBI_CR_KernelcodeReady	(1<<8)	//0x100
#define NFBI_IP_KernelcodeReady	(1<<8)	//0x100
//#define REG32(reg)   (*(volatile unsigned int *)((unsigned int)reg))
#define NFBI_IMAGE_KERNADDR		0x80400000
static void nfbi_set_bootcode_ready(void)
{
	REG32(NFBI_SYSSR)=REG32(NFBI_SYSSR)|NFBI_SR_BootcodeReady;
	printf( "nfbi_set_bootcode_ready(): NFBI_SYSSR=0x%08x (b5)\n", REG32(NFBI_SYSSR));
}
static int nfbi_get_firmware_ready(void)
{
	int ret=0;
	if( REG32(NFBI_ISR) & NFBI_IP_KernelcodeReady )
	{
		if( REG32(NFBI_SYSCR) & NFBI_CR_KernelcodeReady )
			ret=1;
		REG32(NFBI_ISR)=NFBI_IP_KernelcodeReady;
	}

	return ret;
}

void nfbi_set_checksumok(int i)
{
	unsigned int value;
	value=REG32(NFBI_SYSSR);
	value&=~(NFBI_SR_CheckSumDone|NFBI_SR_CheckSumOK);
	if(i) value|=(NFBI_SR_CheckSumDone|NFBI_SR_CheckSumOK); //ok	
	else value|=NFBI_SR_CheckSumDone; //fail
	REG32(NFBI_SYSSR)=value;
	printf( "nfbi_set_checksum(): NFBI_SYSSR=0x%08x (bit15,14)\n", value);
}

#ifndef CONFIG_NO_NET
static int get_tftpm_cmd(board_param_t *bp, char *t)
{
	unsigned long lip, sip;
	unsigned char *pfile, *pl, *ps;
	bootconf_t bc;
	
	if(!bp||!t) 
		return -1;

	lip=bp->ip;
	sip=0;
	pfile=0;
	if(get_bootconf(&bc)==0)
	{
		sip=bc.serverip;
		pfile=bc.filename;
	}
	if((lip==0)||(sip==0)||(pfile==0)||(strlen(pfile)==0))
		return -1;

	pl=&lip;
	ps=&sip;
	sprintf( t, "tftpm %d.%d.%d.%d %d.%d.%d.%d %s", 
		pl[0],pl[1],pl[2],pl[3],ps[0],ps[1],ps[2],ps[3], pfile );

	printf( "get_tftpm_cmd(): %s\n", t );
	return 0;
}
#endif /*CONFIG_NO_NET*/
#endif /*CONFIG_NFBI_SLAVE*/



extern char cmd_prompt[];

void print_banner(void)
{
    printf( logo_msg );
    printf( version_msg );
    printf(" (%s %s)", __DATE__, __TIME__);
#ifdef CONFIG_RTL8685SB
#ifdef CONFIG_RLX
	printf("\n\r\n\r<RLX5281>");
#else
	printf("\n\r\n\r<MIPS1004K>");
#endif
#else
    printf("\n\r\n\r<RTL867X>");
#endif
}

//remove for image size
#ifndef CONFIG_NO_FLASH
#ifdef CONFIG_FAILRUN_BOOTM
int decompress_image(unsigned char *dest, unsigned int destLen, unsigned char *source, unsigned int sourceLen)
{
int err;
z_stream d_stream; /* decompression stream */

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = source;
	d_stream.avail_in = 0;
	d_stream.next_out = dest;            /* discard the output */
	d_stream.avail_out = destLen;
			
	err = inflateInit(&d_stream);
	if (err != Z_OK)
		printf("inflateInit error!\n\r");

	while(d_stream.total_in < sourceLen)
	{
		if(d_stream.avail_in ==0)
			d_stream.avail_in = 1024*32;
		err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_STREAM_END || err < 0)
			break;
	}
	err = inflateEnd(&d_stream);
	return err;			
}
#endif
#endif /*CONFIG_NO_FLASH*/


#ifdef COPY_ON_DECOMPRESS
int copyImageFromFlashToDRAM(char *fs, unsigned int fs_len, char *ram_buf, unsigned int *ram_len)
{
	unsigned int oldbank, currbank;
	unsigned int offset;
	IMGHDR *pImgHdr;
	int binary_len;//binary image size.
	int vmlen;//whole image length
	int tmpLen;//copy len one time
	int offlen=0;//offset to ram_buf

	//set bank
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= (unsigned int)fs - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	
	currbank = getbank();
	if (currbank == 1) {
		fs = (char *)((unsigned int)fs - 0x400000);
	}
	else if (currbank == 2) {
		fs = (char *)((unsigned int)fs - 0x800000);
	}
	else if (currbank == 3) {
		fs = (char *)((unsigned int)fs - 0xC00000);
	}
#endif

	pImgHdr= fs+fs_len;
	//patch
#ifdef BANK_AUTO_SWITCH
	unsigned int oldbank1, currbank1;
	oldbank1 = getbank();
	offset = (unsigned int)pImgHdr - FLASH_BASE_ADDR;
	if(offset < 0x400000)
		s29gl128bank(currbank);
	else if(offset < 0x800000)
		s29gl128bank(currbank+1);
	else if(offset < 0xC00000)
		s29gl128bank(currbank+2);
	else if( offset < 0x1000000)
		s29gl128bank(currbank+3);
	
	currbank1 = getbank();
	if (currbank1 == (currbank+1)) {
		pImgHdr = (IMGHDR *)((unsigned int)pImgHdr - 0x400000);
	}
	else if (currbank1 == (currbank+2)) {
		pImgHdr = (IMGHDR *)((unsigned int)pImgHdr - 0x800000);
	}
	else if (currbank1 == (currbank+3)) {
		pImgHdr = (IMGHDR *)((unsigned int)pImgHdr - 0xC00000);
	}
#endif
	//ql 20081223 patch when binary_len is too large, not need to copy to SDRAM...
	if (pImgHdr->length > IMAGE_MAX_SIZE)
		return 0;
	binary_len = pImgHdr->length - fs_len;
	*ram_len = binary_len;
	vmlen = fs_len + sizeof(IMGHDR) + binary_len;
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank1);
#endif

	do {
		tmpLen = 0xC0000000 - (unsigned int)fs;
		if (tmpLen > (vmlen-offlen))
			tmpLen = (vmlen-offlen);
		//printf("copy from 0x%x to 0x%x\n", fs, ram_buf+offlen);
		memcpy(ram_buf + offlen, fs, tmpLen);
		offlen += tmpLen;

		if (offlen >= vmlen)
			break;

		fs += tmpLen;
#ifdef BANK_AUTO_SWITCH
		currbank++;
		s29gl128bank(currbank);
		fs = FLASH_BASE_ADDR;
#endif
	} while(1);
	
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
	return 1;
}
#endif

// for image with header in flash
#ifdef CONFIG_IMAGE_HEADER_CHECK
int isImageIntegrated(char * fs, unsigned int fs_len)
{
#ifndef NEW_FLASH_LAYOUT
	unsigned int oldbank, currbank;
	unsigned int offset;
	unsigned int rData;
#endif
	//jim we assume file system is double-word alignment...
	IMGHDR *pImgHdr;
	unsigned short int *ptr;
	unsigned int sum;
	int count;
	int binary_len;
	unsigned short cksum;
	unsigned char *binary_section;

	//set bank
#ifndef NEW_FLASH_LAYOUT
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= (unsigned int)fs - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	currbank = getbank();
	if (currbank == 1) {
		fs = (char *)((unsigned int)fs - 0x400000);
	}
	else if (currbank == 2) {
		fs = (char *)((unsigned int)fs - 0x800000);
	}
	else if (currbank == 3) {
		fs = (char *)((unsigned int)fs - 0xC00000);
	}
#endif
#endif
	pImgHdr= fs+fs_len;
	binary_len=pImgHdr->length-fs_len;
	cksum=pImgHdr->chksum;
	binary_section=(unsigned char*)(pImgHdr+1);
	ptr=(unsigned short*)fs;
	count=fs_len;
	sum=cksum;
	//sum the file system's checksum...
	while(count > 1) {
		sum += *ptr;	
		if ( sum>>31)
			sum = (sum&0xffff) + ((sum>>16)&0xffff);
		ptr++;
		count -= 2;
	}
	//sum the binary section checksum.
	ptr=(unsigned short*)binary_section;
	count=binary_len;
	//printf("fs_len=0x%08x binary_len=0x%08x\n",fs_len,binary_len);
	if(binary_len <=0 || binary_len > IMAGE_MAX_SIZE)
		return 0; 
	while(count > 1) {
		sum += *ptr;	
		if ( sum>>31)
			sum = (sum&0xffff) + ((sum>>16)&0xffff);
		ptr++;
		count -= 2;
	}
	if (count > 0) 
		sum += (*((unsigned char*)ptr) << 8) & 0xff00;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
#ifndef NEW_FLASH_LAYOUT
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
#endif
	if (sum == 0xffff) 
		sum = 0;
	if(sum==0)
		return 1;
	else
		return 0;
	
}
#endif

/*ql: 20080723 START: get Image Key according IC type*/
#ifdef MULTI_IC_SUPPORT
int getImgKey()
{
	unsigned short sachem_ver_reg;
	unsigned int reg_clk;
	unsigned int key;
	int icVer;
	
	sachem_ver_reg = *(volatile unsigned short *)0xb8600020;//IC8672
	if (!sachem_ver_reg) {//I think it must be 8671, read value to ensure
		sachem_ver_reg = *(volatile unsigned short *)0xb8000020;//IC8671
		switch(sachem_ver_reg) {
			case 0x1a08:
			case 0x081a:
			case 0x1a0a:
			case 0x0a1a:
			case 0x1a1a:
			case 0x1a2a:
			case 0x2a1a:
				icVer = IC8671;
				break;
			default:
				printf("unknown sachem version!\n\r");
				icVer = 0;
				break;
		}
	} else {
			
			reg_clk = *(volatile unsigned int *)0xB8003200;
			if ((reg_clk & 0x00001f00) == 0)
				icVer = IC8672;
			else if ((reg_clk & 0x00100000) == 0x00100000)
				icVer = IC8671B;
			else
				icVer = IC8671B_costdown;
	}
	
	switch(icVer)
	{
		case IC8671:
			key = APPLICATION_IMG_8671;
			break;
		//case IC8671P:
		//	key = APPLICATION_IMG_8671P;
		//	break;
		case IC8672:
			key = APPLICATION_IMG_8672;
			break;
		case IC8671B:
			key = APPLICATION_IMG_8671B;
			break;
		case IC8671B_costdown:
			key = APPLICATION_IMG_8671B_CD;
			break;
		default:
			/*ql:20080729 START: if sachem register read fail, then don't check image key*/
			//key = 0;
			key = APPLICATION_IMG_ALL;
			/*ql:20080729 END*/
			break;
	}

	return(key);
}
#endif
/*ql: 20080723 END*/

//these 2 are from linux-2.4.x/include/linux/cramfs_fs.h
#define CRAMFS_MAGIC		0x28cd3d45	/* some random number */
#define CRAMFS_SIGNATURE	"Compressed ROMFS"
//this is from linux-2.4.x/include/linux/cramfs_fs.h
#define SQUASHFS_MAGIC		0x73717368
#define SQUASHFS_V4_VER		0x0004


int run_gzip_file(unsigned int src, unsigned long entry)
{
	int ret=0;
	unsigned int comprLen;
	unsigned int uncompressedLength;
#ifdef BANK_AUTO_SWITCH
	unsigned int oldbank;
#endif
	unsigned int currbank;
#ifdef BOOT_FROM_NEWEST_IMAGE
	unsigned int offset;
#endif
	unsigned int rData;
#ifdef CONFIG_IMAGE_HEADER_CHECK
	unsigned int checksumOK=0;
#endif
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_IMAGE_HEADER_CHECK
	unsigned int fs_len_check=0;
	unsigned int src_base_check=0;
    #endif
#endif
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	unsigned int ori_src=src;
	unsigned long ori_entry=entry;
	unsigned int double_image_checked=0;
	unsigned int src_base;
	unsigned int fs_len;
	// Kaohj --- decompress from flash or memory ?
	int flash_file;
#ifdef BOOT_FROM_NEWEST_IMAGE
	//ql_xu: use partFlag to check the latest img.
	unsigned int partFlag=0;

#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= IMAGE_LOCATION_FLAG_ADDR - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	
	currbank = getbank();
	if (currbank == 0)
		offset = IMAGE_LOCATION_FLAG_ADDR;
	else if (currbank == 1)
		offset = IMAGE_LOCATION_FLAG_ADDR-0x400000;
	else if (currbank == 2)
		offset = IMAGE_LOCATION_FLAG_ADDR-0x800000;
	else if (currbank == 3)
		offset = IMAGE_LOCATION_FLAG_ADDR-0xC00000;
#else
	offset = IMAGE_LOCATION_FLAG_ADDR;
#endif
	partFlag = *(unsigned int *)offset;
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
	if (src & 0x30000000)
		flash_file = 1; // file in flash
	else
		flash_file = 0; // file in memory
	if (flash_file) {
		if (!partFlag)//first rootfs is the latest
			src = FIRST_IMAGE_ADDR;
		else
			src = SECOND_IMAGE_ADDR;
	}
#endif

Double_image_Label:	
#endif
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_IMAGE_HEADER_CHECK
printf("%s--%d!\n\r",__func__,__LINE__);			
        src_base_check=src;
	    printf("check src:%p\n", src);
    #endif
#endif

#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	//jim 2008/02/20  execute checksum to sure image integrity...
	src_base=src;
	#ifdef CONFIG_IMAGE_HEADER_CHECK
	checksumOK=0;
    	#endif
	ret=0;
#endif
	/* infate RAM file */
	comprLen = 1024*1024*2;
	//11/06/04' hrchen, in C_Entry(), printf() will destory the value of uncompressedLength
	//reset uncompressedLength again
	uncompressedLength = 0xFFFFFFFF;
	//ret = uncompress(entry, &uncompressedLength, src, comprLen);
#ifndef BOOT_FROM_NEWEST_IMAGE
	printf("Decompress file... ");
#else
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	if (FIRST_IMAGE_ADDR == src)
		printf("\nDecompress first image... ");
	else if (SECOND_IMAGE_ADDR == src)
		printf("\nDecompress second image... ");
	else
#endif
		printf("\nDecompress file at 0x%x...", src);
#endif
#ifdef BANK_AUTO_SWITCH
	oldbank = getbank();
	offset= src - FLASH_BASE_ADDR;
	if(offset <0x400000)
		s29gl128bank(0);
	else if(offset < 0x800000)
		s29gl128bank(1);
	else if(offset < 0xC00000)
		s29gl128bank(2);
	else if( offset < 0x1000000)
		s29gl128bank(3);
	
	currbank = getbank();
	if (currbank == 0)
		rData = *((unsigned int *)src);
	else if (currbank == 1)
		rData = *((unsigned int *)(src - 0x400000));
	else if (currbank == 2)
		rData = *((unsigned int *)(src - 0x800000));
	else if (currbank == 3)
		rData = *((unsigned int *)(src - 0xC00000));
#else
	rData = *((unsigned int*)src);
	currbank = 0;
#endif
	
	if ( CRAMFS_MAGIC==rData) {  //10/19/05' hrchen, cramfs case
		//10/17/05' hrchen, +*((unsigned int)(src+4)) to skip file system
		src+=*((unsigned int*)(src+4));  //skip cramfs image
	} else if ( SQUASHFS_MAGIC==rData) {  //11/14/05' hrchen, squashfs case
		if (currbank == 0)
			src+=((*((unsigned int*)(src+8))+0x0FFF)&(~0x0FFF));  //skip squashfs image
		else if (currbank == 1)
			src+=((*((unsigned int*)(src+8-0x400000))+0x0FFF)&(~0x0FFF));
		else if (currbank == 2)
			src+=((*((unsigned int*)(src+8-0x800000))+0x0FFF)&(~0x0FFF));
		else if (currbank == 3)
			src+=((*((unsigned int*)(src+8-0xC00000))+0x0FFF)&(~0x0FFF));
#ifdef CONFIG_SQUASHFS_V4
    }else if( (SQUASHFS_MAGIC==SWAP32(rData)) && (SQUASHFS_V4_VER==SWAP16(*((unsigned short*)(src+28)))) ) {
		printf( "squashfs v4... " );
		if (currbank == 0)
			src+=((SWAP32(*((unsigned int*)(src+40)))+0x0FFF)&(~0x0FFF));  //skip squashfs image
		else if (currbank == 1)
			src+=((SWAP32(*((unsigned int*)(src+40-0x400000)))+0x0FFF)&(~0x0FFF));
		else if (currbank == 2)
			src+=((SWAP32(*((unsigned int*)(src+40-0x800000)))+0x0FFF)&(~0x0FFF));
		else if (currbank == 3)
			src+=((SWAP32(*((unsigned int*)(src+40-0xC00000)))+0x0FFF)&(~0x0FFF));
#endif //CONFIG_SQUASHFS_V4
#ifdef CONFIG_NO_FLASH
	}else{ //no filesystem, try kernel directly
		printf( "kernel..." );
#endif /*CONFIG_NO_FLASH*/
	}
//ccwei
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_IMAGE_HEADER_CHECK
    fs_len_check=src-src_base_check;
printf("check squash src-A:%p src_base_check:%p fs_len:%d\n\r",src, src_base_check, fs_len_check);
    if(isImageIntegrated((char*)src_base_check, fs_len_check))
    {
	    src=src+sizeof(IMGHDR);
printf("check squash src-B:%p sizeof(IMGHDR):%d\n\r", src, sizeof(IMGHDR));
	    checksumOK=1;
    }
printf("checksumOK %d\n\r", checksumOK);
    #endif
#endif
//end
#ifdef BANK_AUTO_SWITCH
	s29gl128bank(oldbank);
#endif
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	//jim 2008/02/20  execute checksum to sure image integrity...
	fs_len=src-src_base;
	if(src_base == SECOND_IMAGE_ADDR  || src_base == FIRST_IMAGE_ADDR)
	{
#ifdef BOOT_FROM_NEWEST_IMAGE
//printf("src=0x%08x src_base=0x%08x fs_len=0x%08x comprLen=0x%08x\n",src,src_base,fs_len,comprLen);
#if	defined(COPY_ON_DECOMPRESS)&&!defined(SUPPORT_Z_NEW_LAYOUT)
		copyImageFromFlashToDRAM((char *)src_base, fs_len, (char *)DRAM_TEMP_LOAD_ADDR, &comprLen);
		#ifdef CONFIG_IMAGE_HEADER_CHECK
		if (isImageIntegrated((char *)DRAM_TEMP_LOAD_ADDR, fs_len))
		{
			if(double_image_checked)
			{
				//second image checksum ok....
				//set non-zero to 0xbfc04000/0xc0020000
				unsigned long flag=0;
#ifdef BANK_AUTO_SWITCH
				int tmp;

				tmp = getbank();
				s29gl128bank(0);
#endif
				if (!partFlag)
					flag = 0xFFFFFFFF;
				amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &flag, 4);

#ifdef BANK_AUTO_SWITCH
				s29gl128bank(tmp);
#endif
			}
			src=src+sizeof(IMGHDR);
			checksumOK=1;
		//	printf("checksum ok!\n");
		}
		#endif
#else
		#ifdef CONFIG_IMAGE_HEADER_CHECK
		if(isImageIntegrated((char*)src_base, fs_len))
		{
			//unsigned char hw_setting[HW_SETTING_SIZE];
			//check image's location flag in flash 0xC0020000...
			if(double_image_checked)
			{
				//second image checksum ok....
				//set non-zero to 0xbfc04000/0xc0020000
				unsigned long flag=0;
#ifdef BANK_AUTO_SWITCH
				int tmp;

				tmp = getbank();
				s29gl128bank(0);
#endif
				if (!partFlag)
					flag = 0xFFFFFFFF;
				amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &flag, 4);

#ifdef BANK_AUTO_SWITCH
				s29gl128bank(tmp);
#endif
			}
			src=src+sizeof(IMGHDR);
			checksumOK=1;
		}
		#endif
#endif
#else
		#ifdef CONFIG_IMAGE_HEADER_CHECK
		unsigned int data;
		if(isImageIntegrated((char*)src_base, fs_len))
		{
			unsigned char flag[4]={0};
			//unsigned char hw_setting[HW_SETTING_SIZE];
			//check image's location flag in flash 0xC0020000...
			if(!double_image_checked)
			{
				//first image checksum ok...
				//set zero to 0xC0020000.
#ifdef BANK_AUTO_SWITCH
				oldbank = getbank();
				offset= IMAGE_LOCATION_FLAG_ADDR - FLASH_BASE_ADDR;
				if(offset <0x400000)
					s29gl128bank(0);
				else if(offset < 0x800000)
					s29gl128bank(1);
				else if(offset < 0xC00000)
					s29gl128bank(2);
				else if( offset < 0x1000000)
					s29gl128bank(3);
				
				currbank = getbank();
				if (currbank == 0)
					offset = IMAGE_LOCATION_FLAG_ADDR;
				else if (currbank == 1)
					offset = IMAGE_LOCATION_FLAG_ADDR-0x400000;
				else if (currbank == 2)
					offset = IMAGE_LOCATION_FLAG_ADDR-0x800000;
				else if (currbank == 3)
					offset = IMAGE_LOCATION_FLAG_ADDR-0xC00000;
#else
				offset = IMAGE_LOCATION_FLAG_ADDR;
#endif
				data = *(unsigned int *)offset;
#ifdef BANK_AUTO_SWITCH
				s29gl128bank(oldbank);
#endif
				if(data !=0)
				{
					//memcpy(hw_setting, (char*)HW_SETTING_ADDR, HW_SETTING_SIZE);
					//amd29lvEraseNV(IMAGE_LOCATION_FLAG_ADDR-FLASH_BASE_ADDR, 4);
					//amd29lvWriteNV((unsigned char*)HW_SETTING_ADDR, hw_setting, HW_SETTING_SIZE);
					amd29lvWriteNV((unsigned char*)IMAGE_LOCATION_FLAG_ADDR, flag, 4);
				}
			}else
			{
				//second image checksum ok....
				//set non-zero to 0xbfc04000
				flag[3]=0xFF;
#ifdef BANK_AUTO_SWITCH
				oldbank = getbank();
				offset= IMAGE_LOCATION_FLAG_ADDR - FLASH_BASE_ADDR;
				if(offset <0x400000)
					s29gl128bank(0);
				else if(offset < 0x800000)
					s29gl128bank(1);
				else if(offset < 0xC00000)
					s29gl128bank(2);
				else if( offset < 0x1000000)
					s29gl128bank(3);
				
				currbank = getbank();
				if (currbank == 0)
					offset = IMAGE_LOCATION_FLAG_ADDR;
				else if (currbank == 1)
					offset = IMAGE_LOCATION_FLAG_ADDR-0x400000;
				else if (currbank == 2)
					offset = IMAGE_LOCATION_FLAG_ADDR-0x800000;
				else if (currbank == 3)
					offset = IMAGE_LOCATION_FLAG_ADDR-0xC00000;
#else
				offset = IMAGE_LOCATION_FLAG_ADDR;
#endif
				data = *(unsigned int *)offset;
#ifdef BANK_AUTO_SWITCH
				s29gl128bank(oldbank);
#endif

				if(data ==0)
				{
					//memcpy(hw_setting, (char*)HW_SETTING_ADDR, HW_SETTING_SIZE);
					amd29lvEraseNV(IMAGE_LOCATION_FLAG_ADDR-FLASH_BASE_ADDR, 4);
					//amd29lvWriteNV((unsigned char*)HW_SETTING_ADDR, hw_setting, HW_SETTING_SIZE);
					amd29lvWriteNV((unsigned char*)IMAGE_LOCATION_FLAG_ADDR, flag, 4);
				}
			}
			src=src+sizeof(IMGHDR);
			checksumOK=1;
		}
		#endif
#endif
	}
	#ifdef CONFIG_IMAGE_HEADER_CHECK
	else { // file in memory, tftpm
		src=src+sizeof(IMGHDR); // skip image header
		checksumOK=1;
	}
	if(checksumOK)
	#endif
#endif
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_IMAGE_HEADER_CHECK
	if(checksumOK)
	#endif
#endif
	{
#if	!defined(COPY_ON_DECOMPRESS)||defined(SUPPORT_Z_NEW_LAYOUT)
#ifdef BANK_AUTO_SWITCH
		oldbank = getbank();
		offset= src - FLASH_BASE_ADDR;
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
			src = src - 0x400000;
		if (currbank == 2)
			src = src - 0x800000;
		if (currbank == 3)
			src = src - 0xC00000;
#endif
		ret = uncompressLZMA(entry, &uncompressedLength, src, comprLen);		
#else
		unsigned char *binary_section;
		if (flash_file)
			binary_section = (unsigned char *)(DRAM_TEMP_LOAD_ADDR+fs_len+sizeof(IMGHDR));
		else
			binary_section = src;
		ret = uncompressLZMA(entry, &uncompressedLength, binary_section, comprLen);
	//	printf("ret=%d\n",ret);
#endif

#ifndef NEW_FLASH_LAYOUT
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
		if (currbank == 1)
			src = src + 0x400000;
		if (currbank == 2)
			src = src + 0x800000;
		if (currbank == 3)
			src = src + 0xC00000;
#endif
#ifdef BANK_AUTO_SWITCH
		s29gl128bank(oldbank);
#endif
#endif
	}

#if CONFIG_OSK_APP_SUPPORT	
	if (ret) { // try OSK if uClinux failed.
		IMGHDR *pImgHdr;
		/*ql:20080721 START: check image key according to IC version*/
#ifdef MULTI_IC_SUPPORT
		unsigned int key;
#endif
		printf("uncompress fail ");
		/*ql:20080721 END*/

#ifdef CONFIG_OSK_CT_DUAL_IMAGE
		unsigned int is_second_image=0;
#endif

try_load_again:		
#ifdef CONFIG_OSK_CT_DUAL_IMAGE
		if(is_second_image)
			src = 0xBFE00000;
#endif
		pImgHdr = (IMGHDR *)src;
		//printf("Trying OSK...\n\r");
#ifndef MULTI_IC_SUPPORT
		if (APPLICATION_IMAGE == pImgHdr->key) {		
			entry = pImgHdr->address;
			src += HEADERSIZE;
			printf("OSK image found ");
			ret = uncompressLZMA(entry, &uncompressedLength, src, comprLen);
			entry = pImgHdr->entry;
		}
#else
		/*ql:20080721 START: check image key according to IC version*/
		key = getImgKey();
		
		if ((key == (key & pImgHdr->key)) && (((pImgHdr->key>>28)&0xf) == ((key>>28)&0xf))) {
			entry = pImgHdr->address;
			src += HEADERSIZE;
			printf("OSK image found ");
			ret = uncompressLZMA(entry, &uncompressedLength, src, comprLen);
			entry = pImgHdr->entry;
		}
		/*ql:20080721 END*/
#endif

#ifdef CONFIG_OSK_CT_DUAL_IMAGE
	
	if(ret  && !is_second_image)
	{
		is_second_image = 1;
		printf("try second\r\n");
		goto try_load_again;
	}
#endif

	}	
#endif
 
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_IMAGE_HEADER_CHECK
	if(!ret && checksumOK)
    #else
	if(!ret )
    #endif //IMAGE_HEADER_CHECK
#else
    if(!ret )
#endif //CONFIG_NAND_FLASH
	{		
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
#ifdef BOOT_FROM_NEWEST_IMAGE
		if(flash_file && double_image_checked)
		{
			if (!partFlag)
				partFlag = 0xFFFFFFFF;
			else
				partFlag = 0;
			amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &partFlag, 4);
		}
#endif
#endif
		printf("ok!\n\r");
		#ifndef CONFIG_NO_NET
		#ifdef CONFIG_RTL865XC
		SwCore_DisableNIC();
		printf("NIC disabled\n\r");
		#endif
		#endif
//ccwei 111116	
//	void (*appStart)(void);
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
#ifdef CONFIG_RTK_REMAP_BBT
        void (*appStart)(long);
#else
		void (*appStart)(long, long, long);
#endif
#else
		void (*appStart)(long, long);
#endif
		/* jump to decompressed program */
		appStart = (void*)entry;
		
		cache_flush();
//ccwei 111116		
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
#ifdef CONFIG_RTK_REMAP_BBT
        appStart(RunBackup);
#else
        appStart(RunBackup, SDKStart, SDKStart2);
#endif
#else
		#ifdef CONFIG_NO_FLASH
		appStart(0,0);
		#else
		appStart(SPI_CMD, SPI_DEFAULT);
		#endif /*CONFIG_NO_FLASH*/
#endif
	}	
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	if(flash_file && !double_image_checked)
	{
		double_image_checked=1;
#ifdef BOOT_FROM_NEWEST_IMAGE
		if (partFlag)
			src = FIRST_IMAGE_ADDR;
		else
			src=SECOND_IMAGE_ADDR;
#else
		src=SECOND_IMAGE_ADDR;
#endif
		entry=ori_entry;
		ret=0;
		goto Double_image_Label;
	}
#endif
	printf("failed!\n\r");
	return ERROR; 
}






/* console utility */

int check_break(int val)
{
char ch;
int i;
	val = UpperChar(val);
	for(i=0; i<1000; i++);
	if(poll_cc())
	{
		ch = getcc();
		ch = UpperChar(ch);
		if(ch == val)
			return 1;
	}
	return 0;
}

#ifndef CONFIG_NO_FLASH

/*
 *	dest: flash address to be written
 *	src:  data buffer to write to
 *	src_len: write length
 *	Return: 0 on success; -1 on failed
 */
static int save_hs(unsigned int dest, unsigned int src, unsigned int src_len)
{
	unsigned int hs_start, offset;
	int ret=0;
	
	hs_start = FLASH_BASE_ADDR+HW_SETTING_OFFSET;
	if (dest<hs_start && (dest+src_len) > hs_start) {
			offset = hs_start-dest;
			//printf("hs: copy 0x%x to 0x%x len 0x%x\n", hs_start, (src+offset), HW_SETTING_MAX_LEN);
			memcpy((char *)(src+offset), (char *)hs_start, HW_SETTING_MAX_LEN);
	}
	else if (dest>=hs_start && dest<(hs_start+HW_SETTING_MAX_LEN)) {
			//printf("hs: copy 0x%x to 0x%x len 0x%x\n", dest, src, (hs_start+HW_SETTING_MAX_LEN) - dest);
			memcpy((void *)src, (const void *)dest, (hs_start+HW_SETTING_MAX_LEN) - dest);
	}
	else
		ret = -1;
	
	return ret;
}

/*
 *	dest: flash address to be written
 *	src:  data buffer to write to
 *	src_len: write length
 *	Return: 0 on success; -1 on failed
 */
static int save_cs(unsigned int dest, unsigned int src, unsigned int src_len)
{
	unsigned int cs_start, offset;
	int ret=0;
	
	cs_start = FLASH_BASE_ADDR+CURRENT_SETTING_OFFSET;
	if (dest<cs_start && (dest+src_len) > cs_start) {
			offset = cs_start-dest;
			//printf("cs: copy 0x%x to 0x%x len 0x%x\n", cs_start, (src+offset), CURRENT_SETTING_MAX_LEN);
			memcpy((char *)(src+offset), (char *)cs_start, CURRENT_SETTING_MAX_LEN);
	}
	else if (dest>=cs_start && dest<(cs_start+CURRENT_SETTING_MAX_LEN)) {
			//printf("hs: copy 0x%x to 0x%x len 0x%x\n", dest, src, (cs_start+CURRENT_SETTING_MAX_LEN) - dest);
			memcpy((void *)src, (const void *)dest, (cs_start+CURRENT_SETTING_MAX_LEN) - dest);
	}
	else
		ret = -1;
	
	return ret;
}

int write_file(char* dest, char* src, int src_len)
{
#if 0
  char *ptr_src, *ptr_dst, buffer[0x10000];
#endif

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
  int offset=0, copy_length;
  //ccwei 111031
  int block_count=0;
  unsigned int start_page=0;
  int i, j;
#endif
    
  int rc=-1;
	//10/25/05' hrchen, merge config space and bootloader
	//config is at 0xbfc04000~0xbfc07fff
#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_NO_FLASH)
	if (dest >= FLASH_START_4M_SPACE)
		dest = FLASH_BASE_ADDR+(dest-FLASH_START_4M_SPACE);
	if (dest<(FLASH_BASE_ADDR+0x10000)) {  //update bootloader related stuff
		
		#if 0
		if ( (((unsigned int)dest+src_len)>(FLASH_BASE_ADDR+0x8000))&&((unsigned int)dest<(FLASH_BASE_ADDR+0x4000)) ) {
			//assume dest always before 0xbfc04000, and dest+src_len is always behind 0xbfc08000
			offset = (unsigned int)(FLASH_BASE_ADDR+0x4000)-(unsigned int)dest;
			ptr_dst = (char *)((unsigned int)src+offset);
			ptr_src = (char *)(FLASH_BASE_ADDR+0x4000);
			copy_length = 0x4000;
			memcpy(ptr_dst, ptr_src, copy_length);
		} else if ( ((unsigned int)dest==(FLASH_BASE_ADDR+0x4000))&&(src_len==0x4000) ) {  //write config space
			ptr_src = src;
			src = (char*)buffer;
			memcpy(src, FLASH_BASE_ADDR, 0x10000);
			memcpy(src+0x4000, ptr_src, 0x4000);
			dest = FLASH_BASE_ADDR;
			src_len = 0x10000;
		};
		#endif
		// Kaohj -- keep hs and cs
		save_hs((unsigned int)dest, (unsigned int)src, src_len);
		save_cs((unsigned int)dest, (unsigned int)src, src_len);
	};
#endif //CONFIG_NAND_FLASH
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
	printf("[%s-%d]dest=%p src=%p src_len=%d FLASH_BASE_ADDR=0x%x\n",__func__,__LINE__,dest,src,src_len,FLASH_BASE_ADDR);
	if(tftp_img){
		upgrade_kernel(src,src_len);
		tftp_img = 0;
	}else{
		if(dest == FLASH_BASE_ADDR){
			printf("[%s-%d]\n",__func__,__LINE__);
			upgrade_bootloader(src, src_len);
		}else{
			printf("[%s-%d]\n",__func__,__LINE__);
			upgrade_kernel(src,src_len);
		}
	}
	rc = 0;
#else
	rc = amd29lvWrite(dest, src, src_len);
#endif //CONFIG_NAND_FLASH
#if 0
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
	unsigned int addr=0;
	unsigned char *tmp_oob;
    //printf("original src_len %d dest 0x%x\n\r",src_len, dest);
	block_count = (src_len+(block_size-1))/block_size;
    src_len = block_count*block_size;
	tmp_oob = NfSpareBuf;
	memset(tmp_oob,0xff,OOB_SIZE);	
    //printf("A:block_count = %d src_len %d src 0x%x\n\r", block_count, src_len, src);
    if(tftp_img){
#if 0
		unsigned int addr=0;
		unsigned char *tmp_oob;
		tmp_oob = (unsigned char*)malloc((sizeof(char)*ppb*oob_size));
		if(!tmp_oob){
			printf("can't alloc memory for tmp_oob buf!!!\n\r");
			return -1;
		}
		//printf("(sizeof(char)*ppb*oob_size) %d!!!\n\r", (sizeof(char)*ppb*oob_size));
		memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));
#endif		
#ifdef CONFIG_RTK_REMAP_BBT
		unsigned int start_block=0;
#endif

	    for(j=0;j<BACKUP_IMAGE;j++){
		    if(j==1){
			    start_page = ((IMG_BACKUP_ADDR)/page_size)-ppb;
#ifdef CONFIG_RTK_REMAP_BBT		
				start_block = (IMG_BACKUP_ADDR >> block_shift);
#endif
			    offset = 0;
		    }else{
			    start_page = ((IMG_START_ADDR)/page_size)-ppb;
#ifdef CONFIG_RTK_REMAP_BBT		
				start_block = (IMG_START_ADDR >> block_shift);
#endif
			    offset = 0;
		    }
#ifdef CONFIG_RTK_REMAP_BBT
			printf("start blockv:%d\n\r",start_block);
            for(i=start_block;i<block_count+start_block;i++){ //caculate how many block.
                //real_addr = (bbt_v2r[i].block_r << block_shift);
			    //real_page = bbt_v2r[i].block_r * ppb;
                addr = (i << block_shift);
			    //printf("blockv:%d addrv:%d pagev:%d\n\r",bbt_v2r[i].block_r, addr, page);
                //if(rtk_erase_block(real_page)){
                if(nand_erase_nand(addr, block_size)){
                    printf("%s: erase blockv:%d pagev:%d fail!\n",__FUNCTION__, i, i * ppb);
					break;
                }
				memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
                if(nand_write_ecc(addr, block_size, src+offset, tmp_oob)){
                    printf("%s: nand_write_ecc addrv :%x error\n",__FUNCTION__, addr);
                    break;
				}
			    offset += block_size;//shift buffer ptr one block each time.
            }
#else //CONFIG_RTK_NAND_BBT
//printf("CC:start_page %d is ok, dest 0x%x \n\r",start_page, dest);
//printf("B:block_count = %d src_len %d src 0x%x\n\r", block_count, src_len, src);		 
		    for(i=0;i<block_count;i++){ //caculate how many block.
 		        NEXT_BLOCK:
				if(!j && (start_page*page_size >= IMG_BACKUP_ADDR)){
					printf("Warning: block[%d] overwrite IMG_BACKUP_ADDR region!!\n",i);
					break;
				}
		        do{
			        start_page+=ppb;
				}while(nand_checkbad_block(start_page));
                //ccwei 120113
				//rc = amd29lvWrite(dest + start_page*page_size, src+offset, block_size); //write one block data into flash each time;
/*
				if(amd29lvWrite(dest + start_page*page_size, src+offset, block_size)){
                    printf("HW ECC error on this block %d, just skip it!\n", (start_page/ppb));
*/
				//addr = (start_page << page_shift);
				if(nand_erase_nand(start_page, block_size)){
					printf("%s: erase block:%d page:%d fail!\n",__FUNCTION__, (addr >> block_shift), start_page);
					goto NEXT_BLOCK;
				}
				/*reset oob buf in each block*/
				memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
				if(nand_write_ecc(start_page, block_size, src+offset, tmp_oob)){
					printf("%s: nand_write_ecc addr :%x error\n",__FUNCTION__, addr);
					goto NEXT_BLOCK;
				}
			    offset += block_size;//shift buffer ptr one block each time.
		    }
#endif //CONFIG_RTK_NAND_BBT			
	    }
		tftp_img = 0;
	 }else{
        /*In bootloader BOOT_SIZE, only support skip bad block method!!!!*/
        //write booter
        if(dest == FLASH_BASE_ADDR){
		    rc = amd29lvWrite(dest, src, block_size); //boot always in block 0;
		    if(rc)
                printf("HW ECC error on block 0!\n");				
			offset += block_size;//shift buffer ptr to block 1
			block_count--;
#ifdef CONFIG_RTL8685
	#ifdef CONFIG_NAND_PAGE_512
		NEXT_BLOCK_3:
		do{
			start_page+=ppb;
		}while(nand_checkbad_block(start_page));
		addr = (start_page << page_shift);
		if(nand_erase_nand(start_page, block_size)){
			printf("%s: erase block:%d page:%d fail!\n",__FUNCTION__, (addr >> block_shift), start_page);
			goto NEXT_BLOCK_3;
		}
		memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
		if(nand_write_ecc(addr, block_size, src+offset, tmp_oob)){
			printf("%s: nand_write_ecc addr :%x error\n",__FUNCTION__, addr);
			goto NEXT_BLOCK_3;
		}
		offset += block_size;//shift buffer ptr one block each time.
		block_count--;
	#endif
#endif
	    }
		//write loader
		//start_page = 0;
printf("start_page %d , offset %d block_count %d\n\r",start_page, offset, block_count);
	    for(j=0;j<BACKUP_LOADER;j++){
#ifdef CONFIG_RTL8685
	#ifdef CONFIG_NAND_PAGE_512			
			offset = 2*block_size;//(data in ram) move src ptr offset to loader
	#else
			offset = block_size;//(data in ram) move src ptr offset to loader
	#endif
#else
			    offset = block_size;
#endif
//printf("start_page %d , offset %d block_count %d dest 0x%x\n\r",start_page, offset, block_count, dest);
		    for(i=0;i<block_count;i++){ //caculate how many block.
				NEXT_BLOCK_2:
#ifdef CONFIG_RTK_REMAP_BBT
		        if(start_page*page_size >= BOOT_SIZE - 2*BACKUP_BBT*block_size){
				    printf("Warning: bootloader can't exceed more than (1MB- BBT_area) region!!\n");
                    rc = 1;
					goto EXIT;
					//break;
				}
				do{
					start_page+=ppb;
				}while(rtk_block_isbad(start_page*page_size));
				printf("start_page %d is ok\n\r",start_page);
				if(amd29lvWrite(dest + start_page*page_size, src+offset, block_size)){
					printf("HW ECC error on this block %d, just skip it!\n", (start_page/ppb));
					goto NEXT_BLOCK_2;
				}
				offset += block_size;//shift buffer ptr one block each time.
#else
                if(start_page*page_size >= BOOT_SIZE){
			        printf("Warning: bootloader can't exceed more than (1MB) region!!\n");
                    rc = 1;
					goto EXIT;
					//break;
				}
				do{
					start_page+=ppb;
				}while(nand_checkbad_block(start_page));
				//addr = (start_page << page_shift);
				if(nand_erase_nand(start_page, block_size)){
					printf("%s: erase block:%d page:%d fail!\n",__FUNCTION__, (addr >> block_shift), start_page);
					goto NEXT_BLOCK_2;
				}
				memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
				if(nand_write_ecc(start_page, block_size, src+offset, tmp_oob)){
					printf("%s: nand_write_ecc addr :%x error\n",__FUNCTION__, addr);
					goto NEXT_BLOCK_2;
				}
			    offset += block_size;//shift buffer ptr one block each time.
#endif
#if 0
				do{
				    start_page+=ppb;
			    }while(rtk_block_isbad(start_page*page_size));
printf("start_page %d is ok\n\r",start_page);
		        //rc = amd29lvWrite(dest + start_page*page_size, src+offset, block_size); //write one block data into flash each time;
		        if(amd29lvWrite(dest + start_page*page_size, src+offset, block_size)){
                    printf("HW ECC error on this block %d, just skip it!\n", (start_page/ppb));
					goto NEXT_BLOCK_2;
				}
			    offset += block_size;//shift buffer ptr one block each time.
//printf("block_count %d, src_len %d\n\r",block_count, src_len);
#endif
		    }
	    }
	 } //tftp_img
EXIT:
	if(tmp_oob)
		free(tmp_oob);	
//end 111116
#else
    rc = amd29lvWrite(dest, src, src_len);
#endif //CONFIG_NAND_FLASH
#endif
	return rc;
//end 111031	
}
#endif //CONFIG_NO_FLASH

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)

/*Only prloader + loader include ECC info, 
so block size is _block_size_ (2048 + 64)
*/

#define get_block_count(len, b_size) ((len+(b_size-1))/b_size)

int upgrade_bootloader(char* src, int src_len)
{
	int offset=0,offset_tmp=0, s_len=0;
	int block_count=0,i,j,ret=0;
	unsigned int start_page=0;
	unsigned char *tmp_oob;
	#ifdef CONFIG_SPI_NAND_FLASH
	tmp_oob = NfSpareBuf;
	memset(tmp_oob,0xff,OOB_SIZE);	
	#else
	printf("[%s-%d] ppb=%d,oob_size=%d\n\r",__func__,__LINE__,ppb,oob_size);
	tmp_oob = (unsigned char*)malloc((sizeof(char)*ppb*oob_size));
	if(!tmp_oob){
		printf("can't alloc memory for tmp_oob buf!!!\n\r");
		return -1;
	}
	printf("[%s-%d]\n\r",__func__,__LINE__);
	memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
	#endif
	printf("[%s-%d]_block_size_=%d\n\r",__func__,__LINE__,_block_size_);
	block_count = get_block_count(src_len,_block_size_);//(src_len+(block_size-1))/block_size;
	printf("[%s-%d]\n\r",__func__,__LINE__);
    s_len = block_count*_block_size_;
	printf("start_page %d , offset %d block_count %d s_len=%d\n\r",start_page, offset, block_count,s_len);
	rtk_nand_base_info._erase_block(start_page);
	#ifdef CONFIG_SPI_NAND_FLASH
	/*For SPI Nand: preloader have special mapping to Rom Code, they hope we don't 
	    use ecc encode in preloader part. So we use nand write page to write raw data with 
	    ecc data already encode by bchenc tool. in block zero.
	*/
    nand_write_page(start_page, _block_size_, src);
	#else
    nand_write_ecc(start_page, block_size, src, tmp_oob);
	#endif
	block_count--;
	offset += _block_size_;
	s_len -= _block_size_;
	printf("start_page %d , offset %d block_count %d s_len=%d\n",start_page, offset, block_count,s_len);
	ret = backup_loader(src+offset,tmp_oob,s_len);
	#ifndef CONFIG_SPI_NAND_FLASH
	if(tmp_oob)
		free(tmp_oob);
	#endif
	return ret;
}
/*
src: source data buffer point to loader start addr
src_len: source data len
copies: number copies of backup
*/
/*Only prloader + loader include ECC info, 
so block size is _block_size_ (2048 + 64)
*/

int backup_loader(char* src, char* oob, int src_len)
{
	int i,j, offset=0;
	int block_count = get_block_count(src_len,_block_size_);
	int start_page = 0, ret=0;
	printf("[%s-%d]src=%p, src_len=%d block_count=%d\n",__func__,__LINE__,src,src_len,block_count);
	for(i=0;i<BACKUP_LOADER;i++){
		offset = 0; 
		printf("%d offset=%d\n\r",__LINE__,offset);
		for(j=0;j<block_count;j++){ //caculate how many block loader have
			NEX_GOB: /*next good block*/
			if(start_page*page_size >= BOOT_SIZE){
			    printf("Warning: bootloader can't exceed more than (1MB) region!!\n");
                ret = -1;
			}
			do{
				start_page+=ppb;
			}while(nand_checkbad_block(start_page));
			printf("%d start_page=%d\n\r",__LINE__,start_page);
			if(nand_erase_nand(start_page, block_size)){
				printf("%s: erase block:%d page:%d fail!\n",__FUNCTION__, (start_page/ppb), start_page);
				goto NEX_GOB;
			}
		#ifndef CONFIG_SPI_NAND_FLASH
			memset(oob,0xff,(sizeof(char)*ppb*oob_size));	
		#endif
#ifdef CONFIG_SPI_NAND_FLASH
	/*For SPI Nand: preloader have special mapping to Rom Code, they hope we don't 
	    use ecc encode in preloader part. So we use nand write page to write raw data with 
	    ecc data already encode by bchenc tool. in block zero.
	*/		
			if(nand_write_page(start_page, _block_size_, src+offset)){
				printf("%s: nand_write_ecc start_page :%x error\n",__FUNCTION__, start_page);
				goto NEX_GOB;
			}
			offset += _block_size_;//shift buffer ptr one block each time.	
#else
			if(nand_write_ecc(start_page, block_size, src+offset, oob)){
				printf("%s: nand_write_ecc start_page :%x error\n",__FUNCTION__, start_page);
				goto NEX_GOB;
			}
			offset += block_size;//shift buffer ptr one block each time.	
#endif
		}
	}
	return ret;
}
int upgrade_kernel(char* src, int src_len)
{
	int ret=-1;
	unsigned char *tmp_oob;
#ifdef CONFIG_SPI_NAND_FLASH
	tmp_oob = NfSpareBuf;
	memset(tmp_oob,0xff,OOB_SIZE);	
#else
	tmp_oob = (unsigned char*)malloc((sizeof(char)*ppb*oob_size));
	if(!tmp_oob){
		printf("can't alloc memory for tmp_oob buf!!!\n\r");
		return -1;
	}
	memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));	
#endif
	ret = backup_image(src,tmp_oob,src_len);

#ifndef CONFIG_SPI_NAND_FLASH
	if(tmp_oob)
		free(tmp_oob);
#endif
	return ret;
}
#define find_page(addr)((addr >> page_shift))
int find_img_start_page(int i)
{
	int start_page=0;
	switch(i)
	{
		case 0:	return find_page(IMG_START_ADDR);
		case 1: return find_page(IMG_BACKUP_ADDR);
		default:
			printf("None define right now!!\n");
			break;
	}
	return start_page;
}
/*
src: source data buffer point to kernel image
src_len: source data len
copies: number copies of backup
*/
/*NOTE: vm.img must not include ECC data.*/
int backup_image(char* src, char* oob, int src_len)
{
	int i,j, offset=0;
	int block_count = get_block_count(src_len,block_size);
	int start_page = 0, ret=0;
	printf("[%s-%d]src=%p, src_len=%d block_count=%d\n",__func__,__LINE__,src,src_len,block_count);
	for(i=0;i<BACKUP_IMAGE;i++){
		offset = 0; 
		start_page = find_img_start_page(i);
		if(!start_page){
			printf("something wrong in backup img! in page %d\n",start_page);
			return -1;			
		}
		//printf("%d offset=%d start_page=%d\n\r",__LINE__,offset, start_page);
		start_page -= ppb;
		for(j=0;j<block_count;j++){ //caculate how many block loader have
			NEX_GOB2: /*next good block*/
			do{
				start_page+=ppb;
			}while(nand_checkbad_block(start_page));
			//printf("%d start_page=%d\n\r",__LINE__,start_page);
			if(nand_erase_nand(start_page, block_size)){
				printf("%s: erase block:%d page:%d fail!\n",__FUNCTION__, (start_page/ppb), start_page);
				goto NEX_GOB2;
			}
			#ifndef CONFIG_SPI_NAND_FLASH
				memset(oob,0xff,(sizeof(char)*ppb*oob_size));	
			#endif
			if(nand_write_ecc(start_page, block_size, src+offset, oob)){
				printf("%s: nand_write_ecc start_page :%x error\n",__FUNCTION__, start_page);
				goto NEX_GOB2;
			}
			offset += block_size;//shift buffer ptr one block each time.			
		}
	}
	return ret;
}
#endif

/*==== Andrew. Added to support OSK header ====>*/

#define ntohs
#define uint16 unsigned short
unsigned short 
ipchksum(unsigned short *ptr, int count, unsigned short resid)
{
	register unsigned int sum = resid;
       if ( count==0) 
       	return(sum);
        
	while(count > 1) {
		sum += ntohs(*ptr);	
		if ( sum>>31)
			sum = (sum&0xffff) + ((sum>>16)&0xffff);
		ptr++;
		count -= 2;
	}

	if (count > 0) 
		sum += (*((unsigned char*)ptr) << 8) & 0xff00;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	if (sum == 0xffff) 
		sum = 0;
	return (unsigned short)sum;
}


// return 0 if OK, else return -1;
static int check_image_header(unsigned char *src) 
{
	IMGHDR *pHeader;
	unsigned int csum, length;
	/*ql:20080721 START: check image key according to IC version*/
#ifdef MULTI_IC_SUPPORT
	unsigned int key;
#endif
	/*ql:20080721 END*/

	pHeader = (IMGHDR *)src;
#ifndef MULTI_IC_SUPPORT
	if (APPLICATION_IMAGE != pHeader->key) {
		printf("Unknown key. %x\n\r", (unsigned int)pHeader);	
		return -1;
	}
#else
	/*ql:20080721 START: check image key according to IC version*/
	key = getImgKey();

	if ((key != (key & pHeader->key)) || (((pHeader->key>>28)&0xf) != ((key>>28)&0xf))) {
		printf("Unknown key. %x\n\r", (unsigned int)pHeader);	
		return -1;
	}
	/*ql:20080721 END*/
#endif

	//if (!(pHeader->type & FLASHIMAGE)) {
	//	printf("DRAM loading not currently supported.\n");
	//	return -1;
	//}

	//printf("Image length=%d, flashp=%xh\n\r", pHeader->length, pHeader->flashp);

	length = pHeader->length;
	csum = pHeader->chksum;
	csum = ipchksum((uint16*) &pHeader[1], length, csum);
	if (csum != 0) {
		printf("checksum failed  %x  %x\n\r",(unsigned int)src,csum);
		return -1;		
	}
	
	return 0;
}


#ifdef CONFIG_NO_FLASH
void check_and_run_file(unsigned int src, unsigned long entry)
{
	if( check_image_header(src)==0 )
	{
		#ifdef CONFIG_NFBI_SLAVE
		nfbi_set_checksumok(1);
		#endif /*CONFIG_NFBI_SLAVE*/
		run_gzip_file(src+sizeof(IMGHDR), entry);
	}else{
		#ifdef CONFIG_NFBI_SLAVE
		nfbi_set_checksumok(0);
		#endif /*CONFIG_NFBI_SLAVE*/
	}

	return;
}
#endif /*CONFIG_NO_FLASH*/



#ifndef CONFIG_NO_FLASH
#ifdef NEW_FLASH_LAYOUT
int saveImageToFlash(char* wdst, char* wsrc, int length)
{
	unsigned int bank;
	unsigned char *pDst = wdst;
	unsigned int offset, tmpLen, offlen=0;

	offset = (unsigned int)pDst - FLASH_BASE_ADDR;
	if(offset <0x400000)
		bank = 0;
	else if(offset < 0x800000)
		bank = 1;
	else if(offset < 0xC00000)
		bank = 2;
	else if( offset < 0x1000000)
		bank = 3;
	
	do {
		tmpLen = FLASH_BASE_ADDR+(bank+1)*0x400000-(unsigned int)pDst;
		if (tmpLen > (length-offlen))
			tmpLen = (length-offlen);
		//printf("wdst=%xh, wsrc=%xh length=%xh\n\r", pDst, wsrc+offlen, tmpLen);
		write_file(pDst, wsrc+offlen, tmpLen);
		offlen += tmpLen;
		
		if (offlen >= length)
			break;
	
		pDst += tmpLen;
		bank++;
	} while(1);

	return 1;
}
#endif

// return 0 if OK, else return -1;
int writeImageSingle(unsigned char *src) {
	IMGHDR *pHeader;
	unsigned int csum, length;
	char *wdst, *wsrc;
	/*ql:20080721 START: check image key according to IC version*/
#ifdef MULTI_IC_SUPPORT
	unsigned int key;
#endif
	/*ql:20080721 END*/
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
#ifdef BOOT_FROM_NEWEST_IMAGE
	unsigned partFlag;
#endif
#endif

	pHeader = (IMGHDR *)src;
	length = pHeader->length;
	if (check_image_header(src)<0)
		return -1;

	// Kaohj -- write to older one
#if defined(IMAGE_DOUBLE_BACKUP_SUPPORT) && defined(BOOT_FROM_NEWEST_IMAGE)
	if (pHeader->flashp != FLASH_BASE_ADDR) {
		partFlag = *(unsigned int *)IMAGE_LOCATION_FLAG_ADDR;
		if (!partFlag) // first image is the latest
			wdst = SECOND_IMAGE_ADDR;
		else
			wdst = FIRST_IMAGE_ADDR;
	}
	else // rom image
		wdst = FLASH_BASE_ADDR;
#else
//ccwei 111116
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    tftp_img = 1;
	wdst = (unsigned char *)FLASH_BASE_ADDR;
#else
    wdst = (unsigned char *)pHeader->flashp;
#endif
//end 111116
#endif
	if (pHeader->type&KEEPHEADER) {
		wsrc = src;
		length += sizeof(IMGHDR);
	} else {
		wsrc = (char *)&pHeader[1];
	}
	
	printf("Writing at 0x%x\n", (unsigned int)wdst);
#ifndef NEW_FLASH_LAYOUT
	write_file(wdst, wsrc, length);
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	//write image file to backup space
	if(wdst == FIRST_IMAGE_ADDR)
	{
		//Flash space after 2MB, is backup image file.....
		write_file(SECOND_IMAGE_ADDR, wsrc, length);
	}
#endif
#else
	//printf("wdst=%x, wsrc=%xh\n\r",wdst,wsrc);
	saveImageToFlash(wdst, wsrc, length);
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
	// Kaohj -- toggle root
	if (partFlag)
		partFlag = 0;
	else
		partFlag = 0xffffffff;
	amd29lvWrite((void *)IMAGE_LOCATION_FLAG_ADDR, &partFlag, 4);
#endif
	
	#if 0
	if(wdst == FIRST_IMAGE_ADDR)
	{
		//Flash space after 2MB, is backup image file.....
		saveImageToFlash((char *)SECOND_IMAGE_ADDR, wsrc, length);
	}
	#endif
#endif

	return 0;
}


int writeImage(IMGHDR *pHeader) {
	unsigned int totallength, length;
	unsigned char *datap;
	IMGHDR *pTmpHdr;
	int ret;
	/*ql:20080721 START: check image key according to IC version*/
#ifdef MULTI_IC_SUPPORT
	unsigned int key;
#endif
	/*ql:20080721 END*/

	//printf("App key=%x    type=%02x\n", pHeader->key, pHeader->type);

#ifndef MULTI_IC_SUPPORT
	if (APPLICATION_IMAGE != pHeader->key) {
		return -1;
	}
#else
	/*ql:20080721 START: check image key according to IC version*/
	key = getImgKey();

	if ((key != (key & pHeader->key)) || (((pHeader->key>>28)&0xf) != ((key>>28)&0xf))) {
		printf("key %08X error!\n\r", pHeader->key);
		/*ql: 20080723 START: here wrong image will be considered to be boot file or raw binary, it is dangerous, 
		* how can we filter wrong image out???
		* now I think if the high 4 bit(28-31) of key is 0x0a, then it is image file, so don't write as raw binary... 
		*/
		if ((pHeader->key>>28)&0xf == 0xa)
			return 0;
		/*ql: 20080723 END*/
		return -1;
	}
	printf("key %08X OK\n\r", pHeader->key);
	/*ql:20080721 END*/
#endif

	if ( !(pHeader->type&MULTIHEADER))
	{
		return writeImageSingle(pHeader);
	}

	totallength = pHeader->length;
	pTmpHdr = &pHeader[1];	
	length = pTmpHdr->length + sizeof(IMGHDR);
	datap = (char *)pTmpHdr;

	printf("Length=%d\n", pHeader->length);
	while (totallength > length) {
		ret = writeImageSingle(pTmpHdr);
		if (ret) {		
			return ret;
		}
		datap = &datap[ pTmpHdr->length + sizeof(IMGHDR) ];
		pTmpHdr = (IMGHDR *)datap;
		length += (pTmpHdr->length + sizeof(IMGHDR));
		datap = (char *)pTmpHdr;
	}
	return writeImageSingle(pTmpHdr);
}

/*<==== Andrew. Added to support OSK header ====*/
#endif /*CONFIG_NO_FLASH*/

int xmodem_download(unsigned long dest)
{
unsigned long len;
char *load_buf = (char*)bParam.load;
char *dest_buf = (char*)dest;
	get_param(&bParam);
	load_buf = (char*)bParam.load;
	if(load_buf)
	{
		xModemStart();
		printf("xModem upload...\n\r");
#ifdef E8B_SUPPORT
	/*jim 20081020, support xmodem download binary e8b file, reqeusted by Peter */
	len = xModemRxBuffer(load_buf, 8*1024*1024);
#else
    	len = xModemRxBuffer(load_buf, 4*1024*1024);
#endif
		if(len!=0)
		{
			printf("\n\rwriting file...%x %x %d\n\r", (unsigned int)dest_buf, (unsigned int)load_buf, (int)len);
#ifdef CONFIG_NO_FLASH
			memcpy( dest_buf, load_buf, len );
#else /*CONFIG_NO_FLASH*/
			if (writeImage(load_buf)) {
				printf("No header, assuming raw binary\n\r");
                /*jim 20091208 after discussed with tylo, 8672 loader check is unnecessary now*/
				//tylo, for 8672 loader check
				/*if((*(volatile unsigned int*)(load_buf+0x10)!=0)&&(dest_buf==FLASH_BASE_ADDR)){
					printf("NOT 8672 loader!!!\n\r");
					return ERROR;
				}
                */
				return write_file(dest_buf, load_buf, len);
			}
			//return write_file(dest_buf, load_buf, len);
#endif /*CONFIG_NO_FLASH*/
		}
		else
			printf("\n\rdownload failed!!\n\r");
	}
 	else
		printf("\n\rdownload buffer failed!!\n\r");
	return ERROR;
}


#ifdef SUPPORT_MULT_UPGRADE
extern int multicast_frame_alive;
extern int multicast_frame_stop;

//#define REG32(reg)	(*(volatile unsigned int *)(reg))

int  multicast_start() {
	unsigned long start_one;
	unsigned long start_end;
	printf("support multicast tool\n\r");
	memcpy(netif.IEEEIA, &bParam.mac[1], 6);
	netif.ip = 0;
	Lan_Initialize(bParam.mac[1]);
	start_end = get_sys_time();
	start_one = get_sys_time();

	while(! timeout(start_end, 1000 * 3) ){
		if(timeout(start_one, 200)){
			start_one = get_sys_time();
		}
		processPacket();
		while( !timeout(start_end, 1000 * 60 * 5) && multicast_frame_finish != -1 && 
			! multicast_frame_stop && (multicast_frame_start||multicast_frame_alive)) {
			if(timeout(start_one, 200)){
				start_one = get_sys_time();
			}
			processPacket();
		}
	}
	multicast_frame_start = 0;
	if(multicast_frame_finish == -1){
		multicastListFree();
		multicast_frame_finish = 0;
		return 1;
	} 
	else
		return 0;
}
#endif


#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
extern int Lan_Transmit(void * buff, unsigned int length);

void	rs_httpd(void)
{
	struct tcp_pcb	*pcb;

	memcpy(netif.IEEEIA, &bParam.mac[0], 6);
//	printf("\nMAC: %02x-%02x-%02x-%02x-%02x-%02x.\n", 
//		netif.IEEEIA[0], netif.IEEEIA[1], netif.IEEEIA[2], netif.IEEEIA[3], netif.IEEEIA[4], netif.IEEEIA[5]);
	netif.send = Lan_Transmit;
	
    netif.ip=bParam.ip;
    
    Lan_Initialize(&bParam.mac[0]);
    
	/* create vlan */
/*	rtl_vlan_param_t vp;
	bzero((void *) &vp, sizeof(rtl_vlan_param_t));
	memcpy(&vp.gMac, &bdinfo.mac[0], 6);
	vp.egressUntag = 0x1f;
	vp.mtu = 1500;
	vp.memberPort = 0x1f;

	if ( swCore_vlanCreate(8, &vp) != 0 )
	{
		printf( "\nCreating vlan fails.\n" );
		while(1);
	}
*/

	/* request an IP address through BOOTP */
/*	printf("\n********'b' to get IP address through BOOTP.\n********'h' to set IP address manually.\n");
//	printf("\n********'h' to set IP address manually.\n");

//	printf("test0\n");
	char	ch;
	
	if ( getchar(&ch) )
	{
		if ( ch == 'h' ) {
			printf("Enter IP address XX.XX.XX.XX:");
			netif.ip = strToIp();
			printf("\n");

		} else if ( ch == 'b' ) {
			netRequestIP ();
		}
	}
*/	
	
//	printf("Start initialization.\n");
#ifdef STATS
	stats_init();
//	printf("stats_init over.\n");
#endif
	mem_init();
//	printf("mem_init over.\n");
	pbuf_init();
//	printf("pbuf_init over.\n");
	memp_init();
//	printf("memp_init over.\n");
	ip_init();
//	printf("ip_init over.\n");
	tcp_init();
//	printf("tcp_init over.\n");
	

//	while(1);

	pcb = tcp_new();
	
	if (tcp_bind(pcb, (u32_t*)&(netif.ip), HTTP_SRV_PORT)!=ERR_OK) {
//		printf("Bind fail. At line: %d, in file %s\n", __LINE__, __FILE__);
	}

//	printf("pcb bind port is %d\n", pcb->local_port);
	if ((pcb=tcp_listen(pcb))==NULL) {
//		printf("Bind fail. At line: %d, in file %s\n", __LINE__, __FILE__);
	}

//	printf("test2\n");

	pcb->accept = http_accept;
//	tcp_accept(pcb, http_accept);
//	tcp_recv(pcb, http_recv);
//	printf("test3\n");

//	printf( "Listening on port %d\n",HTTP_SRV_PORT);
	printf("The local IP is %d.%d.%d.%d\nListening......\n", (netif.ip>>24)&0xff, (netif.ip>>16)&0xff, (netif.ip>>8)&0xff, (netif.ip)&0xff);
//	printf("Listening......\n");
	while(1) {
		run_nonblocking_tasks();
		processPacket();
	}
}
#endif

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
#ifdef CONFIG_CONFIG_FILE_ENCRYPT
#define big2little(i)  ( ((i<<24)&0xFF000000) | ((i >> 24)&0x000000FF ) | \
                     ((i<<8)&0x00FF0000) | ((i >> 8)&0x0000FF00 ))

static unsigned long sn_matrix[8][8] = {
{0x818C8D80, 0x9B9D909C, 0xa5A9aDa0, 0xb8A9F5b8, 0xb5b8b8D0, 0xc0c5c4A2, 0xcDcCc5cA, 0xd4dBE9dC},
{0xd3dEACd8, 0xeBe79DAA, 0xe9FFFBFA, 0xe0eF9E94, 0xe1e18Da9, 0xfDfEF58A, 0x8587BE8C, 0x008E8AF8},
{0x008AE4C9, 0x8885C780, 0x84848784, 0x9AF6F0A8, 0x95989380, 0xa1BCF9a4, 0xaF96A9a3, 0xaD81EFE7},
{0xdDB9F6d2, 0xd6d18Bd8, 0xc6F0c8c4, 0xc9B6c188, 0xb5b4b1bE, 0xbAF9ABb8, 0xb3b0D5b0, 0x00a8C5a1},
{0xd2eCCEfF, 0xe5e8C990, 0xe3AEA5eA, 0x8B8AD5a8, 0xbFDAc8A2, 0xa2F9E4FC, 0xb4b5b7b5, 0xa1aDaBaF},
{0xe5D4E1e5, 0xe0AAeB93, 0xe3C6CDA0, 0x84a3DF94, 0xaAAEE3E0, 0xa58EDC8B, 0x99F490CF, 0x90a6b4B8},
{0xc4D697F6, 0xc4cCc4B1, 0xcEc2F287, 0xd6d2CA9C, 0xd18680dE, 0x00d1FDb4, 0xb0bED8b2, 0xb0c9c9c2},
{0x00f0ACfE, 0xeA8F8EBD, 0xd19185c0, 0xa2BDF1FC, 0xaC859Fd8, 0xfCe3c8a8, 0x8C9494B6, 0x98CD95A6}};


void decode(char *buf, int buflen) 
{
	int		pd = 0;
	int		pos;
	char*	p = buf;
	unsigned long key;

	if (p == NULL || buflen == 0)
	{
		return;
	}

	while (pd < buflen)
	{
		pos = pd % 256;
		key = big2little(*((unsigned long*)sn_matrix + (pos >> 2)));
		pos &= 0x3;
		*p ^= *((unsigned char *)(&key) + pos);
		p++;
		pd++;
	}
}

#endif
#if 1
typedef struct flash_config_header{
	unsigned int length;
	unsigned int checksum;
}FLASH_CFG_HDR;

unsigned int getCfgChksum(uint16 *ptr, int count, uint16 resid)
{
	register uint32 sum = resid;
	if ( count==0) return(sum);
	while(count > 1) {
		sum += *ptr;
		ptr++;
		count -= 2;
	}

	if (count > 0)
		sum += (*((unsigned char *)ptr) << 8) & 0xff00;
	return sum;
}

#endif

#ifdef CONFIG_RTL8685_MEMCTL_CHK
void show_current_DRAM_info(void)
{
	unsigned int i, window_start, window_len;
	unsigned int bit_loc;
	
	printf("Calibration result Write Window: \n\r");
	/* DQ0 ~ DQ15 */
	for(i=0;i<16;i++){

		bit_loc = i;
		window_start=0;
		window_len = 2*( ((REG32(DACDQR+i*4)>>24)& 0x1f) - window_start);
		printf("DQ(%02d): %02d ~ %02d, ", i, window_start, window_start+window_len);
		if(i%4 == 3)
			printf("\n");
	}

	
	printf("Calibration result Read Window Rising edge: \n\r");
	for(i=0;i<16;i++){
		window_start = (REG32(DACDQR+i*4)) & 0x1f;
		window_len = ((REG32(DACDQR+i*4)>>16) & 0x1f) -window_start + 1;
		printf("DQ(%02d): %02d ~ %02d, ", i, window_start, window_start+window_len);
		if(i%4 == 3)
			printf("\n");
	}
	printf("Calibration result Read Window falling edge: \n\r");
	for(i=16;i<32;i++){
		window_start = (REG32(DACDQR+i*4)) & 0x1f;
		window_len = ((REG32(DACDQR+i*4)>>16) & 0x1f) -window_start + 1;
		printf("DQ(%02d): %02d ~ %02d, ", i-16, window_start, window_start+window_len);
		if(i%4 == 3)
			printf("\n");
	}


}
#endif

#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S)
void reduce_power(void)
{
	unsigned int app_addr;

	if(IS_RTL8685){

		if(!IS_SDS_IF){
			/* SERDES disable */
			app_addr = 0xb8000068;
			REG32(app_addr) |= (1<<15);
		}
		
		/* PCIe 1 host disable */
		app_addr = 0xb8000600;
		REG32(app_addr) &= ~(1<<7);

#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_NO_FLASH)
		/* NAND Flash controller disable */
		REG32(app_addr) &= ~(1<<16);
#endif
		/* VoIP disable */
		REG32(app_addr) &= ~((1<<12)| (1<<13));
		
		/* IPSec disable */
		REG32(app_addr) &= ~(1<<11);

		/* GDMA disable */
		REG32(app_addr) &= ~((1<<10)| (1<<9));
#ifndef CONFIG_NFBI_SLAVE
		/* NFBI disable */
		REG32(app_addr) &= ~(1<<15);
#endif

	}
	else if(IS_RTL8685S){

		/* SPI NAND disable */
		app_addr = 0xb8000600;
		REG32(app_addr) &=~(1<<30);

#ifndef CONFIG_NFBI_SLAVE
		/* NFBI disable */
		REG32(app_addr) &= ~(1<<15);
#endif
		/* VoIP disable */
		REG32(app_addr) &= ~((1<<12)| (1<<13) |(1<<2));

		/* GDMA disable */
		REG32(app_addr) &= ~(1<<9);

	}
	
	return;
}
#endif

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
rtk_nand_info_t rtk_nand_info;
#endif
void check_loader_mode(int mode)
{
	unsigned long start;
	
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
	int i,j;
	unsigned int start_page=0;
	//unsigned char __attribute__ ((aligned(64))) oob_buf[64];
	unsigned char *data_buf = (unsigned char *)(DRAM_DIMAGE_ADDR);
	int jump_to_backup=0;
#endif

	if(mode)
		goto run_loader_shell;


	
#ifdef CONFIG_RTL8685_MEMCTL_CHK
	show_current_DRAM_info();
#endif
	
#ifdef CONFIG_RTL8685
	reduce_power();
#endif
	
	printf("\n\r\n\r");
	printf("Booting\n\r");
	calibrate_delay();
	printf("Press 'ESC' to enter BOOT console...\n\r");
#if defined(CONFIG_SPI_NAND_FLASH) || defined(CONFIG_NAND_FLASH)
	rtk_nand_base_probe();
#endif
	get_param(&bParam);

#ifndef CONFIG_NO_NET
	//for 8685 FPGA skip it!!!
	#ifndef CONFIG_FPGA_8685S
	Lan_Initialize(&bParam.mac[0]);
#endif /*CONFIG_NO_NET*/

#endif
#ifdef CONFIG_NO_FLASH
	printf("Boot from SRAM\n\r");
#else
	if(IS_BOOT_NAND){
		#ifdef CONFIG_SPI_NAND_FLASH
			printf("Boot from SPI NAND flash\n\r");
		#else
		printf("Boot from NAND flash\n\r");
//ccwei
#ifndef CONFIG_NAND_FLASH
		printf("ERROR: A-you didn't choose NAND flash in menuconfig\n\r");
#endif
		#endif /*CONFIG_SPI_NAND_FLASH*/
	}else{
		printf("Boot from NOR/SPI flash\n\r");
	}
#endif /*CONFIG_NO_FLASH*/

/*XXX*/
#ifdef SUPPORT_MULT_UPGRADE
	/* turn off DSL led: zyxel requirement*/
	//*(volatile unsigned int*)(POCR) &= (~((1<<NICLED0_GPD0)|(1<<NICLED1_GPD1)));
	//*(volatile unsigned int*)(POCR)|= (1<<USBLED1_GPD5);
	//*(volatile unsigned int*)(POCR) |= ((1<<NICLED2_GPD2)|(1<<NICLED3_GPD3)|(1<<USBLED0_GPD4)|(1<<USBLED1_GPD5));
	//*(volatile unsigned int*)(GPIO_PABCD_CNR)=0;
	//*(volatile unsigned int*)(GPIO_PABCD_PTYPE) = 0x0;	
	//REG32(POCR) = REG32(POCR) & (~(1<<NICLED0_GPD0));
	gpioConfig(LED_DSL, GPIO_FUNC_OUTPUT);
	gpioSet(LED_DSL);					
	delay_msec(1000);
#endif
#ifdef CONFIG_RE8306
	printf("Enable RTL8306..\n\r");
    Enable_8306();	 
	init_lanswitch();
#endif	//ENABLE_8306    

	
#ifdef WEB_ACCOUNT_SUPPORT
	updateAccount();
#endif


#ifdef CONFIG_NO_FLASH
{
	char tftpm_cmd[128];

#ifdef CONFIG_NFBI_SLAVE
	nfbi_set_bootcode_ready();
#endif /*CONFIG_NFBI_SLAVE*/
		
	while(1)
	{
		start = get_sys_time();
		while(!timeout(start, 1000))
			if(check_break(0x1B))
				goto run_loader_shell;

#ifdef CONFIG_NFBI_SLAVE
		if( nfbi_get_firmware_ready() )
		{
			printf( "firmware ready\n" );
			check_and_run_file( NFBI_IMAGE_KERNADDR, bParam.entry );
		}

#ifndef CONFIG_NO_NET
		if( get_tftpm_cmd(&bParam, tftpm_cmd)==0 )
			cmd_proc(tftpm_cmd);
#endif /*CONFIG_NO_NET*/
#endif /*CONFIG_NFBI_SLAVE*/
	}
}
#else  /*CONFIG_NO_FLASH*/

#ifdef CONFIG_PCIE_HOST
	printf("PCIE HOST Reset\n\r");
	PCIE_reset_procedure(0,0,1, 0xb8b10000);
#endif

#ifdef CONFIG_BOOT_CMDLINE
	goto run_loader_shell;
#else

	start = get_sys_time();
	//tylo, 8672 timer issue
	while(!timeout(start, 1000))
		if(check_break(0x1B))
			goto run_loader_shell;
#endif

/*#ifdef CONFIG_NAND_FLASH
		//printf("[%s, line %d] bParam.app = 0x%08X, data_buf = 0x%08X\n\r",__FUNCTION__,__LINE__,bParam.app, (unsigned int)data_buf);
		nand_read_ecc(from, 0x400000, data_buf, oob_buf);
		bParam.app = (unsigned int)data_buf;
#endif*/
//ccwei
//#ifdef CONFIG_NAND_FLASH
	if(IS_BOOT_NAND){		
//ccwei
#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
    #ifdef CONFIG_CHECK_HDR
        int find_vimg=0;
        unsigned char magic=0;
		unsigned int magic_vimg=0;
    #endif		
        unsigned int page_data=0;
        unsigned int search_region=0, start_position=0;
    #ifdef CONFIG_RTK_REMAP_BBT
		unsigned int start_block=0, real_addr=0, count=0, addr=0;
    #endif
        unsigned char *ptr_data;// = (volatile unsigned char *)0x80A00000;
        unsigned char *ptr_oob = (volatile unsigned char *)DRAM_DOOB_ADDR;
		search_region = APPL_SIZE;
    #ifdef CONFIG_RTK_REMAP_BBT		
		rtk_scan_v2r_bbt();
		rtk_nand_scan_bbt();
    #endif
	#ifdef CONFIG_RTK_NORMAL_BBT
		unsigned int addr=0;
		nand_scan_normal_bbt();
	#endif
RUN_BACKUP:
	    ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
printf("jump to backup %d ptr_data:%p\n\r",jump_to_backup,ptr_data);
		if(jump_to_backup){
		    start_page = ((IMG_BACKUP_ADDR)/page_size)-ppb;
			start_position = IMG_BACKUP_ADDR;
    #ifdef CONFIG_RTK_REMAP_BBT
			start_block = (IMG_BACKUP_ADDR >> block_shift);//virtual block index
    #endif			
            RunBackup = 1;
    #ifdef CONFIG_CHECK_HDR
			find_vimg=0;			
    #endif
		}
        else{
		    start_page = ((IMG_START_ADDR)/page_size)-ppb;			
			start_position = IMG_START_ADDR;
    #ifdef CONFIG_RTK_REMAP_BBT			
			start_block = (IMG_START_ADDR >> block_shift);//virtual block index
    #endif			
        }
    #ifdef CONFIG_RTK_REMAP_BBT
		printf("start blockv %d\n\r",start_block);
		count=0;
        for(i=start_block;i<(APPL_SIZE/block_size)+start_block;i++){ //caculate how many block.
			addr = (i << block_shift);//real block index, addr.
		    //printf("blockv:%d addrv:%d\n\r",i,addr);
				if(nand_read_ecc(addr, block_size, ptr_data+(count*block_size), ptr_oob)){
                    printf("%s: nand_read_ecc addrv :%x error\n",__FUNCTION__, addr);
                    break;
				}
/*				
#ifdef CONFIG_CHECK_HDR
			magic_vimg = 0;
#endif
   			for(j=0;j<ppb;j++){
			    if(nand_read_ecc(addr+j*page_size, page_size, ptr_data+(count*block_size)+(j*page_size), ptr_oob)){
				    printf("%s: nand_read_ecc addrv :%x error\n",__FUNCTION__, addr);
				    break;
			    }
#ifdef CONFIG_CHECK_HDR
			    if(!find_vimg){
			        magic = *(ptr_data + (count*block_size) + j*page_size);
			        magic_vimg |= magic << 24; 
 			        magic = *(ptr_data + (count*block_size) + j*page_size + 1);
			        magic_vimg |= magic << 16; 
 			        magic = *(ptr_data + (count*block_size) + j*page_size + 2);			 
			        magic_vimg |= magic << 8; 			 
 			        magic = *(ptr_data + (count*block_size) + j*page_size + 3);			 
			        magic_vimg |= magic;
			        if(magic_vimg == MAGIC_SQUASH){//find magic word and start to read!
				        printf("vm1: find magic addr: %d magic_vimg 0x%x\n", addr, magic_vimg);
			 	        find_vimg=1;
			        }else{
				        printf("vm1: no magic addr: %d magic_vimg 0x%x\n", addr, magic_vimg);
			        }				
			    }
#endif //CONFIG_CHECK_HDR
   			}
*/			
			count++;//block counter
		}
    #else //CONFIG_RTK_NAND_BBT
printf("%s-[%d]: SDK start 0x%x\n\r",__FUNCTION__,__LINE__,((start_page+ppb)*page_size));
		for(i=0;i<(APPL_SIZE/block_size);i++){ //caculate how many block.
    #ifdef CONFIG_CHECK_HDR
		    NEXT_OK_BLOCK:
		    if((!find_vimg) && (start_page*page_size >= start_position + search_region)){
				printf("can't find magic word!!! 0x%x\n", start_page*page_size);
				SDKStart = IMG_START_ADDR; //inform kernel we chose which image.										
				break;			
			}
    #endif //CONFIG_CHECK_HDR			
			do{
				start_page+=ppb;
			}while(nand_checkbad_block(start_page));

//			for(j=0;j<ppb;j++){ //each block have "ppb" pages.
//			if(rtk_read_ecc_page(start_page+j , 0x80A00000+ (block_size*i) + (j * page_size), 0x80200000, page_size)){
#ifdef CONFIG_CHECK_HDR
				magic_vimg = 0;
#endif
/*	
			    if(rtk_read_ecc_page(start_page+j , ptr_data+ (block_size*i) + (j * page_size), ptr_oob, page_size)){
                    printf("read ecc page :%d error\n", start_page+j);				
				    break;
			    }
*/
				//addr = ((start_page+j) << page_shift);
				if(nand_read_ecc(start_page, block_size, ptr_data+ (block_size*i), ptr_oob)){
					printf("nand_read_ecc page :%d error\n", start_page+j);				
				    break;
			    }
    #ifdef CONFIG_CHECK_HDR
			    magic = *(ptr_data + (block_size*i));
			    magic_vimg |= magic << 24; 
 			    magic = *(ptr_data + (block_size*i)+1);
			    magic_vimg |= magic << 16; 
 			    magic = *(ptr_data + (block_size*i)+2);			 
			    magic_vimg |= magic << 8; 			 
 			    magic = *(ptr_data + (block_size*i)+3);			 
			    magic_vimg |= magic;
			    if(!find_vimg){
			        if(magic_vimg == MAGIC_SQUASH){//find magic word and start to read!
				        printf("vm1: find magic start_pageB: %d magic_vimg 0x%x\n", start_page, magic_vimg);
			 	        find_vimg=1;
						if(jump_to_backup)
							SDKStart2 = (start_page*page_size); //inform kernel we chose which image.						
						else
                            SDKStart = (start_page*page_size); //inform kernel we chose which image. 						
			        }else{
				        printf("vm1: no magic start_pageB: %d magic_vimg 0x%x\n", start_page, magic_vimg);
				        goto NEXT_OK_BLOCK;
			        }
			    }//find_vimg
    #endif //end CONFIG_CHECK_HDR			    
			//}
		}
    #endif //CONFIG_RTK_NAND_BBT		
		bParam.app = (unsigned int)data_buf;
//ccwei: calculate the backup image starting address!!
		ptr_data = (volatile unsigned char *)DRAM_TEMP_IMAGE_ADDR;
        start_page = ((IMG_BACKUP_ADDR)/page_size)-ppb;
		start_position = IMG_BACKUP_ADDR;		
#ifdef CONFIG_CHECK_HDR	
		find_vimg=0;
#endif
#ifdef CONFIG_RTK_REMAP_BBT
#if 0
        start_block = (IMG_BACKUP_ADDR >> block_shift);//virtual block index
printf("start_block %d\n\r",start_block);
		count=0;
        for(i=start_block;i<(APPL_SIZE/block_size)+start_block;i++){ //caculate how many block.
	        addr = (i << block_shift);
//printf("blockv:%d addrv:%d\n\r",i, addr);
		    if(nand_read_ecc(addr, block_size, ptr_data+count*block_size, ptr_oob)){
			    printf("nand_read_ecc addrv :%x error\n", addr);
			    break;
		    }
#ifdef CONFIG_CHECK_HDR
		    if(!find_vimg){
			    magic = *(ptr_data + count*block_size);
			    magic_vimg |= magic << 24; 
			    magic = *(ptr_data + count*block_size + 1);
			    magic_vimg |= magic << 16; 
			    magic = *(ptr_data + count*block_size + 2);			 
			    magic_vimg |= magic << 8;			 
			    magic = *(ptr_data + count*block_size + 3);			 
			    magic_vimg |= magic;
			    if(magic_vimg == MAGIC_SQUASH){//find magic word and start to read!
				    printf("vm1: find magic start_pageB: %d magic_vimg 0x%x\n", addr, magic_vimg);
				    find_vimg=1;
			    }else{
				    printf("vm1: no magic start_pageB: %d magic_vimg 0x%x\n", addr, magic_vimg);
			    }				
		    }
#endif			
			count++;
       }
#endif
#else  //CONFIG_RTK_NAND_BBT
        if(jump_to_backup){
			/*skip backup image searching*/
            goto CHECK_SDK2_PASS;
		}
		for(i=0;i<(APPL_SIZE/block_size);i++){ //caculate how many block.
#ifdef CONFIG_CHECK_HDR	
		    SEARCH_MAGIC_BLOCK:
			if((!find_vimg) && (start_page*page_size >= start_position+search_region)){
				printf("can't find backup image magic word!!! 0x%x\n", start_page*page_size);
                SDKStart2 = IMG_BACKUP_ADDR; //inform kernel we chose which image. 										
				break;			
			}				
#endif
		    do{
				start_page+=ppb;
			}while(nand_checkbad_block(start_page));
//			for(j=0;j<ppb;j++){ //each block have "ppb" pages.
#ifdef CONFIG_CHECK_HDR	
		    magic_vimg = 0;
#endif
#if 0
				if(rtk_read_ecc_page(start_page+j , ptr_data+ (block_size*i) + (j * page_size), ptr_oob, page_size)){
					printf("read ecc page :%d error\n", start_page+j);				
					break;
				}
#endif
				//addr = ((start_page+j) << page_shift);
				if(nand_read_ecc(start_page, block_size, ptr_data+ (block_size*i), ptr_oob)){
					printf("nand_read_ecc page :%d error\n", start_page+j); 			
					break;
				}
#ifdef CONFIG_CHECK_HDR	
			    magic = *(ptr_data + (block_size*i));
			    magic_vimg |= magic << 24; 
 			    magic = *(ptr_data + (block_size*i) + 1);
			    magic_vimg |= magic << 16; 
 			    magic = *(ptr_data + (block_size*i) + 2);			 
			    magic_vimg |= magic << 8; 			 
 			    magic = *(ptr_data + (block_size*i) + 3);			 
			    magic_vimg |= magic;		 
				if(!find_vimg){
					if(magic_vimg == MAGIC_SQUASH){//find magic word and start to read!
						printf("vm2: find magic start_pageB: %d magic_vimg 0x%x\n", start_page, magic_vimg);
						find_vimg=1;
						SDKStart2 = (start_page*page_size); //inform kernel we chose which image.
						goto CHECK_SDK2_PASS;
					}else{
					    printf("vm2: no magic start_pageB: %d magic_vimg 0x%x\n", start_page, magic_vimg);
							goto SEARCH_MAGIC_BLOCK;
					}
				}//find_vimg
#endif				
//			}
		}		
CHECK_SDK2_PASS:
	;
#endif //CONFIG_RTK_NAND_BBT

#else
		printf("ERROR: B-you didn't choose NAND flash in menuconfig\n\r");
#endif

	}
//#endif
		
#ifdef SUPPORT_MULT_UPGRADE
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
		
			if(total_crc32!=crc32_tmp){
				printf("crc32 error %x \n\r",crc32_tmp);
			}
			else {
				total_crc32 =0;
				total_len =0;
				printf("Writing file...\n\r");
				int i=0;
				/*
				 * "conf" is always behind image, so when amd29lvEraseSector will not erase "conf".
				 */
				for( i=0; i<MAX_PARTITION; i++)
				{
					if(! partiton_info_array[i].valid)
						continue;
					if(partiton_info_array[i].type == PARTI_TYPE_IMG) {
						printf("Going to write kernel to flash\n\r");
						writeImage(bParam.load+partiton_info_array[i].addrstart);
						amd29lvEraseSector(0x1F0000, 20); // erase osk router's current setting partition.
						printf("erase flash ok\n\r");
						printf("upgrade kernel successfully!\n\r");
					}
					else if(partiton_info_array[i].type == PARTI_TYPE_BOOT) {
						printf("Going to write bootloader to flash\n\r");
						char *load_buf = bParam.load + partiton_info_array[i].addrstart;
						char *dest_buf = (char *)FLASH_BASE_ADDR;
						unsigned long len = partiton_info_array[i].addrend - partiton_info_array[i].addrstart;
						if(writeImage(load_buf)) {
							printf("No header, assuming raw binary\n\r");
							if(write_file(dest_buf, load_buf, len)!=0)
								printf("write bootloader failed\n\r");
							else
								printf("upgrade bootloader successfully!\n\r");
						}
						else
							printf("upgrade bootloader successfully!\n\r");
					}
					else if(partiton_info_array[i].type == PARTI_TYPE_CONF) {
						char *load_buf = bParam.load + partiton_info_array[i].addrstart;
						char *dest_buf = (char *)(FLASH_BASE_ADDR+0x1f0000);
						unsigned long len = partiton_info_array[i].addrend - partiton_info_array[i].addrstart;
						FLASH_CFG_HDR *cfg_hdr = (FLASH_CFG_HDR *)load_buf;
						if(cfg_hdr->checksum != getCfgChksum((uint16 *)(load_buf+sizeof(FLASH_CFG_HDR)), cfg_hdr->length, 0)) {
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
				gpioConfig(LED_DSL, GPIO_FUNC_OUTPUT);
				gpioConfig(LED_PPP_G, GPIO_FUNC_OUTPUT);
				gpioConfig(LED_PPP_R, GPIO_FUNC_OUTPUT);
				gpioConfig(LED_PWR_R, GPIO_FUNC_OUTPUT);
				gpioConfig(LED_USB, GPIO_FUNC_OUTPUT);
				
				while(1)
				{
					delay_msec(1000);
					gpioClear(LED_DSL);
					gpioClear(LED_PPP_G);
					gpioClear(LED_PPP_R);
					gpioClear(LED_PWR_R);
					gpioClear(LED_USB);
					delay_msec(1000);
					gpioSet(LED_DSL);
					gpioSet(LED_PPP_G);
					gpioSet(LED_PPP_R);
					gpioSet(LED_PWR_R);
					gpioSet(LED_USB);				
				}
			}
		}
	}
#endif
	//reinitialize mem size.
	//*(volatile unsigned int *) 0xb8003304 = bParam.mem_size;
printf("%s--%d!\n\r",__func__,__LINE__);	
#if 1	//for DDR patch ,add by kevinchung
	#if defined(E8B_SUPPORT) && defined(IMAGE_SINGLE_BACKUP_SUPPORT)
	// E8: If normal image is failed, run small image(for TR069 upgrade purpose).
	if((!strcmp(bParam.bootline, "re0")) || 
		((run_gzip_file(bParam.app, bParam.entry)==ERROR) &&
			(run_gzip_file(E8_SMALL_IMAGE_ADDR, bParam.entry)==ERROR)))
	#else
printf("%s--%d!\n\r",__func__,__LINE__);	
	if((!strcmp(bParam.bootline, "re0")) || 
		(run_gzip_file(bParam.app, bParam.entry)==ERROR))
	#endif
#else
	if((!strcmp(bParam.bootline, "re0")) || 
		(DDR_timing_check(bParam.app, bParam.entry)==ERROR))
#endif
	{
		char *mac;
		mac = bParam.mac[0];

#if defined(CONFIG_NAND_FLASH) || defined(CONFIG_SPI_NAND_FLASH)
		if(!jump_to_backup){
			printf("kernel decompress first time fail!\n\r");
			jump_to_backup = 1;
			goto RUN_BACKUP;
		}
#endif		
		//8685s FPGA without lan interface.
		#ifndef CONFIG_FPGA_8685S
		Lan_Initialize(mac);
		#endif
		/*
		 * Decompress failed, try to use web upgrade F/W, wake up httpd.
		 */
		#ifdef CONFIG_FAILRUN_HTTPD
		if(strcmp(bParam.bootline, "re0")) {
			get_param(&bParam);
			#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
		    	rs_httpd();
			#endif //CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
		}
		#elif defined(CONFIG_FAILRUN_BOOTP) || defined(CONFIG_FAILRUN_BOOTM)
		unsigned int image_len;
		char filename[32];
		unsigned char *load_buf = (char*)bParam.load;
		
		if ( bootpReceive (mac, &image_len, 0, 0, filename, load_buf) != 0 )
            printf("bootp ERROR\n\r");
		else {
			#ifdef CONFIG_FAILRUN_BOOTM
			void (*appStart)(void);
			appStart = (void*)bParam.entry;
			if(bParam.load!=bParam.entry)
			{
				if(load_buf[0]==0x1f && load_buf[1]==0x8b)
				{
					printf("Decompress file...\n\r");
					decompress_image((char*)bParam.entry, 1024*1024*5,(char*)bParam.load, image_len);
				}
				else
					memcpy((char*)bParam.entry, (char*)bParam.load, image_len);
			}
			appStart();
			#else
			printf("Writing file...\n\r");
			writeImage(load_buf);
			#endif
		}
		#endif //CONFIG_FAILRUN_HTTPD
	}
#endif /*CONFIG_NO_FLASH*/

run_loader_shell:
	cmd_shell();
	
}



