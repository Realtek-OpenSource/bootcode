/*
 * Realtek 1295 common configuration settings
 *
 */

#ifndef __CONFIG_RTK_RTD1295_COMMON_H
#define __CONFIG_RTK_RTD1295_COMMON_H

/* Display CPU and Board Info */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SKIP_LOWLEVEL_INIT	1

#undef CONFIG_USE_IRQ				/* no support for IRQs */

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_REVISION_TAG		1

/*
 * Size of malloc() pool
 * Total Size Environment - 128k
 * Malloc - add 256k
 */
#define CONFIG_ENV_SIZE			(128 << 10)
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (256 << 10))

/*
 * Hardware drivers
 */

/*
 * serial port - NS16550 compatible
 */
#define V_NS16550_CLK				27000000

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		V_NS16550_CLK

#define UART1_BASE					0x9801B200
#define UART0_BASE					0x98007800
#define CONFIG_CONS_INDEX			1
#define CONFIG_SYS_NS16550_COM1     UART0_BASE

#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, 9600, 19200, 38400, 57600, 115200}

#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

/* Flash */
#define CONFIG_SYS_NO_FLASH	1

/*
 * Environment setup
 */

#define CONFIG_BOOTDELAY	0

#define CONFIG_ENV_OVERWRITE

#define CONFIG_BOOTCOMMAND \
	"bootr"

#define CONFIG_KERNEL_LOADADDR	0x03000000
#define CONFIG_ROOTFS_LOADADDR	0x02200000
#define CONFIG_LOGO_LOADADDR	0x02002000      //reserved ~2M
#define CONFIG_FDT_LOADADDR	0x01FF2000      //reserved 64K
#define CONFIG_FW_LOADADDR	0x01b00000  //reserved 4M

#define CONFIG_EXTRA_ENV_SETTINGS                   \
   "kernel_loadaddr=0x03000000\0"                  \
   "fdt_loadaddr=0x01FF2000\0"                  \
   "fdt_high=0xffffffffffffffff\0"                  \
   "rootfs_loadaddr=0x02200000\0"                   \
   "mtd_part=mtdparts=rtk_nand:\0"                  \

/* Pass open firmware flat tree */
#define CONFIG_CMD_BOOTI
#define CONFIG_GZIP_DECOMPRESS_KERNEL_ADDR	0x0c000000	// GZIPED kernel decompress addr
#define CONFIG_GZIP_KERNEL_MAX_LEN		0x01400000	// Set MAX size to 20M after decompressed
//#define CONFIG_ARMV8_SWITCH_TO_EL1
#define CONFIG_OF_LIBFDT    		1
#define CONFIG_OF_STDOUT_VIA_ALIAS	1

/* Console */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE

/*
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER	/* use "hush" command parser */
#define CONFIG_SYS_CBSIZE		640

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)

/*
 * memtest setup
 */
#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + (32 << 20))

/* Default load address */
#define CONFIG_SYS_LOAD_ADDR		0x03000000

/* Use General purpose timer 2 */
#define CONFIG_SYS_TIMER		0     //FPGA
#define CONFIG_SYS_HZ			1000

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
//stack size is setup in linker script 1MB
#ifdef CONFIG_USE_IRQ
	#define CONFIG_STACKSIZE_IRQ	(4 << 10)	/* IRQ stack */
	#define CONFIG_STACKSIZE_FIQ	(4 << 10)	/* FIQ stack */
#endif

/*
 * SDRAM Memory Map
 * Even though we use two CS all the memory
 * is mapped to one contiguous block
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_RAM_DCU1_SIZE	0x20000000		//512MB

/* GIC-400 setting */
#define CONFIG_GICV2
#define GICD_BASE			0xff011000      // FIXME, all these register should be reviewed
#define GICC_BASE			0xff012000      // FIXME, all these register should be reviewed



//if the relocation is enabled, the address is used to be the stack at very beginning.
#define CONFIG_SYS_INIT_SP_ADDR     0x00100000


// 1:cache disable   0:enable
#if 0
	#define CONFIG_SYS_ICACHE_OFF
	#define CONFIG_SYS_DCACHE_OFF
#else
	#define CONFIG_SYS_NONCACHED_MEMORY
	#define CONFIG_SYS_NONCACHED_START	0x20000000
	#define CONFIG_SYS_NONCACHED_SIZE	0x20000000
	#define CONFIG_CMD_CACHE
#endif

#define CONFIG_SYS_CACHELINE_SIZE	64

/*
 * rm include/autoconf.mk ---- prevent use old CONFIG_SYS_TEXT_BASE
 * make CONFIG_SYS_TEXT_BASE
 */
#ifndef CONFIG_SYS_TEXT_BASE
	#define CONFIG_SYS_TEXT_BASE		0x00020000
#endif

/* ENV related config options */
#define CONFIG_ENV_IS_NOWHERE

#define CONFIG_SYS_PROMPT       		"Realtek> "

/* Library support */
#define CONFIG_LZMA
#define CONFIG_LZO

#ifdef CONFIG_CMD_NET
	/* Eth Net */
	#define CONFIG_CMD_PING
	#define CONFIG_CMD_TFTPPUT
	#define CONFIG_RTL8168
	#define CONFIG_TFTP_BLOCKSIZE		400

	/* Network setting */
	#define CONFIG_ETHADDR				00:10:20:30:40:50
	#define CONFIG_IPADDR				192.168.100.1
	#define CONFIG_GATEWAYIP			192.168.100.254
	#define CONFIG_SERVERIP				192.168.100.2
	#define CONFIG_NETMASK				255.255.255.0
#endif


/********* RTK CONFIGS ************/
#define CONFIG_BSP_REALTEK
#define CONFIG_NO_RELOCATION
#define CONFIG_HEAP_ADDR	0x07880000


#endif /* __CONFIG_RTK_RTD1295_COMMON_H */

