/*
  * Help text for boot loader commands
  *
  */
  
#ifndef INCLUDE_HELP_H
#define INCLUDE_HRLP_H

#define HELP_APP			"app [app addr]\n\r"
#ifdef CONFIG_SPANSION_16M_FLASH
#define HELP_BANK			"bank\n\r"
#endif
#define HELP_BOOTLINE		"bootline\n\r"
#define HELP_BOOTP			"bootp\n\r"
#ifdef CONFIG_MAC0_LBK
#define HELP_CALI			"cali [0/1] [1/2/3/4/5/6/7]\n\r"\
							"first parameter:0:DQS0 first test, 1:DQS1 first test, SDRAM for 0:up_count 1:down_count\n\r"\
							"second parameter:[n0]=1 for TFTP, [n1]=1 for decompress, [n2]=1 for MAC LBK  \n\r"\	
							"Example:\n\r"\
							"  cali 0 4\n\r\n\r"
#endif
#define HELP_D				"d [addr] <len>\n\r"
#define HELP_ENTRY			"entry [address]\n\r"
#define HELP_FERASE			"ferase [addr] <len>\n\r"
#define HELP_FWRITE			"fwrite [flash address] [dram address]\n\r"
#define HELP_FLASHSIZE		"flashsize [256(k)/128(k)/1(M)/2(M)/4(M)/8(M)/16(M)]\n\r"
#ifdef CONFIG_GDM_SCAN
#define HELP_GDMA_SCAN		"gdma_scan [0/1/2....16] [memory size] [times]\n\r"
#endif
#define HELP_HELP			"help\n\r"						
#define HELP_INFO			"info\n\r"
#define HELP_LOAD			"load [load buffer addr]\n\r"
#define HELP_MAC			"mac [\"clear\"/\"osk\"/mac address]\n\r"
#define HELP_MEMSIZE		"memsize ROW[2k/4k/8k/16k] COL[256/512/1k/2k/4k] BANK[2/4]\n\r"
#define HELP_MIISEL			"miisel [0(Int. PHY)/1(Ext. PHY)]\n\r"

#define HELP_MULTICAST		"multicast\n\r"
#ifdef CONFIG_NAND_FLASH
#define HELP_NAND			"nand [operation] [page] \n\r"
#endif
#ifdef CONFIG_SPI_NAND_FLASH
#define HELP_SPI_NAND			"nand [operation] [page] \n\r"
#endif
#define HELP_R				"r [addr]\n\r"
#define HELP_REBOOT			"reboot\n\r"
#define HELP_RESETCFG		"restcfg\n\r"
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
#define HELP_ROOT			"root\n\r"
#endif
#define HELP_RUN			"run [app addr] [entry addr]\n\r"
#define HELP_SDRAM			"sdram\n\r"
#define HELP_TT				"tt\n\r"
#define HELP_TFTP			"tftp [ip] [server ip] [file name]\n\r"
#define HELP_TFTPM			"tftpm [ip] [server ip] [file name]\n\r"
#define HELP_W				"w [addr] [val]\n\r"
#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
#define HELP_WEB			"web\n\r"
#endif
#define HELP_XMODEM			"xmodem [address]\n\r"
#if 0 //shrink bootloader size, remove unnecessary code
#define HELP_DMEMDMA		"dmemdma\n\r"
#define HELP_FLASHCHECK		"flashcheck\n\r"
#define HELP_FLASHCONTENT	"flashcontent\n\r"
#define HELP_IMEMDMA		"imemdma\n\r"
#define HELP_MEMCPY			"memcpy\n\r"
#define HELP_R16			"r16 [addr]\n\r"
#define HELP_R8				"r8 [addr]\n\r"
#define HELP_READTEST		"readtest\n\r"
#define HELP_RUNX			"runx [func]\n\r"
#define HELP_SRAM			"sram\n\r"
#define HELP_TFTPX			"tftpx [ip] [server ip] [?] [?]\n\r"
#define HELP_W16			"w16 [addr] [val]\n\r"
#define HELP_W8				"w8 [addr] [val]\n\r"
#endif
#ifdef CONFIG_PCIE_HOST
#define HELP_HRST			"hrst\n\r"
#define HELP_PCIE_R			"pcie_r [addr] [slot]\n\r"
#define HELP_PCIE_W			"pcie_w [addr] [val] [slot]\n\r"
#define HELP_PCIE_RST		"pcie_rst\n\r"
#define HELP_PCIE_D_LBK		"Pcie_D_LBK\n\r"
#define HELP_PCIE_LBK		"Pcie_LBK\n\r"
#endif

#ifdef CONFIG_RTL8685_MEMCTL_CHK
#define HELP_CALI_8685	"cali_8685\n\r"
#define HELP_DIAG_8685	"diag_8685 [freq]\n\r"
#endif
#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
#define HELP_FREQ_8685S	"freq_8685s \n\r"
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */
#endif //INCLUDE_HELP_H

#if defined(CONFIG_MEM_TEST)
#if defined(CONFIG_RTL8676S)
#define HELP_DRAM_TEST	"dram_test [timeout(sec)] [round]\n\r"
#define HELP_FLASH_TEST	"flash_test [timeout(sec)] [round]\n\r"
#else
#define HELP_DRAM_TEST		"dram_test [timeout(sec)] [round], dram_test without prefetch\n\r"
#define HELP_DRAM_TEST_DP	"dram_test_dp [timeout(sec)] [round], dram_test with data prefetch\n\r"
#define HELP_DRAM_TEST_IP	"dram_test_ip [timeout(sec)] [round], dram_test with inst. prefetch\n\r"
#define HELP_DRAM_TEST_BP	"dram_test_bp [timeout(sec)] [round], dram_test with both data and inst. prefetch\n\r"
#define HELP_DRAM_TEST_ALL	"dram_test_all [timeout(sec)] [round], dram_test with all mode\n\r"
#endif
#endif

#ifdef CONFIG_L2C_TEST
#define HELP_L2C_MODE	"l2c [mode], mode: 0x0(WT), 0x2(UC), 0x3(WB), 0x4(CWBE), 0x5(CWB), 0x7(UCA)\n\r"
#endif

#ifdef CONFIG_BTG_TEST
#define HELP_BTG_TEST	"btg help, to show the manual of BTG test\n\r"
#endif

#define HELP_MEMSET "memset <addr> <char val> <size>\n\r"
#define HELP_MEMCMP "memcmp <addr1> <addr2> <size>\n\r"
#define HELP_DMEM "dram <addr>\n\r"
