

	################################################################################
	#  Noop_Delay  <iteration> <register,default T1>, For All production
	################################################################################
	.macro Noop_Delay iter reg=t1
		li	\reg,\iter
		1:
		subu		\reg,1
		nop	
		bnez		\reg,1b
		nop			
	.endm

	
	################################################################################
	#  Mem_Set_Word <StartAddr> <Size> <Pattern>  - Fill memory with pattern, For All production
	################################################################################
	.macro Mem_Set_Word addr size patt
		#define	CLR_TEMP1	t0
		#define	CLR_TEMP2	t2
		#define	CLR_TEMP3	t3
		li		CLR_TEMP1,\addr				//start address
		addiu	CLR_TEMP2,CLR_TEMP1,\size		//2k bytes, end address
		li		CLR_TEMP3,\patt
		1:	
		sw		CLR_TEMP3,0(CLR_TEMP1)
		addi	CLR_TEMP1,CLR_TEMP1,4
		bne		CLR_TEMP1,CLR_TEMP2,1b
		nop		
		#undef	CLR_TEMP1	
		#undef	CLR_TEMP2	
		#undef	CLR_TEMP3	
	.endm
	

	################################################################################
	#  Detect_RAM_Size_plus2, only supported RTL8676/RTL8676S series model
	#	supported SDRAM above 2Mbytes	
	#	supported DDR1 above 16Mbytes
	#	supported DDR2 above 32Mbytes
	################################################################################
	#	 here we auto detect the sdram size. according Designer's info's on memory controller behavior
	#	 use address range = 0x0000_0000 ~ 0x0FFF_FFFF
	#	input address 0xAFF01754 => physical address = 0x0FF01754
	#	predefine 16bits (bytes) DRAM => DRAM address = 0x0FF0_1754 / 2 = 0x7F80BAA
	#	column address range a0~a9 (0x400) => 0x7F8_0BAA mod 0x400 = 3AA (COL address)
	#	                                                0x7F8_0BAA / 0x400 = 0x1FE02 ( for ROW / BANK)
	#	row address range a0~a13 (0x4000)  => 0x1FE02 mod 0x4000 = 3E02 (ROW address)
	#                                                            0x1FE02 / 0x4000 = 7 (BANK address)
	# we have conclusion on MCR with 64MB setting:
	#	2MB *(0xaFF01754) == *(0xa1F01754)
	#	8MB *(0xaFF01754) == *(0xA7F01554)
	#	16MB *(0xaFF01754) == *(0xA7701354)
	#	32MB *(0xaFF01754) == *(0xA7F01354)
	#	64MB *(0xaFF01754) == *(0xA6E01740)
	#	128MB *(0xaFF01754) == *(0xA7F01754) DDR1
	#	128MB *(0xaFF01754) == *(0xAEE01740) DDR2
	#	256MB *(0xaFF01754) == *(0xAFF01754) DDR2
	#	Auto Detection version: V1.01

#if defined(CONFIG_RTL8676) || defined(CONFIG_RTL8676S)
	.macro Detect_RAM_Size_plus2
		#define	DCR_ADDR		t0
		#define	DCR_VALUE		t1
		#define	DRAM_pattern	t2
		#define	DRAM_CHECK_ADDR	t3
		#define	tRFC			t4
		#define	DRAM_TMP0		t5
		#define	DRAM_TMP1		t6
		#define DRAM_TMP2		t7
		#define DRAM_TMP3		t8
		#define DRAM_size		t9	
			#setup environment, row=16k, col=1k, banks=4 and banks=8.
			li	DRAM_TMP0,0xb800100c
			lw	DRAM_TMP1,0(DRAM_TMP0)
			nop
			or	DRAM_TMP1,0x40000000
			sw	DRAM_TMP1,0(DRAM_TMP0)
			nop
			li	DCR_ADDR,0xb8001004
			li	DCR_VALUE,0x56880000			#set MCR to 256MByte setting temprory
			sw	DCR_VALUE,0(DCR_ADDR)
1:
			lw		DRAM_TMP2,0(DCR_ADDR)
			nop
			and		DRAM_TMP2,0x1				//check DCR_WR_BUSY
			nop
			bnez	DRAM_TMP2,1b
			nop
			li	DRAM_size,0						//test
			li	DRAM_CHECK_ADDR,0xA2301154		//2Mbyte
			sw	$0,0(DRAM_CHECK_ADDR)
			li	DRAM_CHECK_ADDR,0xA6701154		//8Mbyte
			sw	$0,0(DRAM_CHECK_ADDR)
			li	DRAM_CHECK_ADDR,0xA6701354		//16Mbyte
			sw	$0,0(DRAM_CHECK_ADDR)	
			li	DRAM_CHECK_ADDR,0xA6F01354		//32Mbyte
			sw	$0,0(DRAM_CHECK_ADDR)	
			li	DRAM_CHECK_ADDR,0xA6F01754		//64Mbyte
			sw	$0,0(DRAM_CHECK_ADDR)
			li	DRAM_CHECK_ADDR,0xA7F01754		//128Mbyte for DDR1
			sw	$0,0(DRAM_CHECK_ADDR)
			li	DRAM_CHECK_ADDR,0xAEF01754		//128Mbyte for DDR2
			sw	$0,0(DRAM_CHECK_ADDR)
			li	DRAM_CHECK_ADDR,0xAFF01754		//256Mbyte for DDR2
			sw	$0,0(DRAM_CHECK_ADDR)
					
			//write pattern into DRAM
			li	DRAM_CHECK_ADDR,0xA7F01754		//Write pattern into max address of define.
			li	DRAM_pattern,0xAAAA5555
			sw	DRAM_pattern,0(DRAM_CHECK_ADDR)
			
			//check data correct
			li	DRAM_CHECK_ADDR,0xA2301154		//2Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_2MB
			nop
			
			li	DRAM_CHECK_ADDR,0xA6701154		//8Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_8MB
			nop
			
			li	DRAM_CHECK_ADDR,0xA6701354		//16Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_16MB
			nop
			
			li	DRAM_CHECK_ADDR,0xA6F01354		//32Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_32MB
			nop
			
			li	DRAM_CHECK_ADDR,0xA6F01754		//64Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			//beq	DRAM_TMP0,DRAM_pattern,DRAM_64MB
			bne	DRAM_TMP0,DRAM_pattern,Detect_fail
			nop
			
			//write pattern into DRAM
			li	DRAM_CHECK_ADDR,0xAFF01754		//Write pattern into max address of define.
			li	DRAM_pattern,0x5555aaaa
			sw	DRAM_pattern,0(DRAM_CHECK_ADDR)	
			
			li	DRAM_CHECK_ADDR,0xA6F01754		//64Mbyte
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DDR2_DRAM_64MB			
			nop
			
			li	DRAM_CHECK_ADDR,0xA7F01754		//128Mbyte for DDR1
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_128MB_DDR1
			nop

			li	DRAM_CHECK_ADDR,0xAEF01754		//128Mbyte for DDR2
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_128MB_DDR2
			nop	

			li	DRAM_CHECK_ADDR,0xAFF01754		//256Mbyte for DDR2
			lw	DRAM_TMP0,0(DRAM_CHECK_ADDR)
			nop
			beq	DRAM_TMP0,DRAM_pattern,DRAM_256MB_DDR2
			nop				
			b	DRAM_64MB						//set 64MB
			nop			

DRAM_2MB:
			li	DCR_VALUE,0x50000000
			or	DRAM_size,0x2
			b	Set_T_RFC
			nop
DRAM_8MB:
			li	DCR_VALUE, 0x52080000
			or	DRAM_size,0x8
			b	Set_T_RFC
			nop	
Detect_fail:			
DRAM_16MB:
			li	DCR_VALUE, 0x52480000
			or	DRAM_size,0x10
			b	Set_T_RFC
			nop		
DRAM_32MB:
			li	DCR_VALUE, 0x54480000
			or	DRAM_size,0x20
Set_T_RFC:
#ifdef DRAM166			
			li	tRFC,13			//T_RFC=84.34ns
#else
			li	tRFC,10			//T_RFC=82.71ns
#endif	
			b	Reset_DDR2_bank4
			nop		
DRAM_64MB:	
			li	DCR_VALUE, 0x54880000
			or	DRAM_size,0x40
#ifdef DRAM166
	#if defined(CONFIG_RTL8676)			
			li	tRFC,20			//T_RFC=126.51ns
	#elif defined(CONFIG_RTL8676S)		//DRAM CLOCK 175MHz
			li	tRFC,21			//T_RTC=125.71ns
	#else
			#error CONFIG_RTL8676_or_CONFIG_RTL8676S_must_be_selected
	#endif		
#else
	#if defined(CONFIG_RTL8676)			
			li	tRFC,16			//T_RFC=127.82ns
	#elif defined(CONFIG_RTL8676S)		//DRAM CLOCK 150MHz
			li	tRFC,18			//T_RFC=126.67ns
	#else
			#error CONFIG_RTL8676_or_CONFIG_RTL8676S_must_be_selected
	#endif
#endif			
			b	Reset_DDR2_bank4
			nop
DDR2_DRAM_64MB:
			li	DCR_VALUE, 0x54880000
			or	DRAM_size,0x41
#ifdef DRAM166			
			li	tRFC,17			//T_RFC=108.43ns
#else
			li	tRFC,14			//T_RFC=112.78ns
#endif			
			b	Reset_DDR2_bank4
			nop				
DRAM_128MB_DDR1:
			li	DCR_VALUE, 0x56880000
			or	DRAM_size,0x80
#ifdef DRAM166			
			li	tRFC,21			//T_RFC=135.34ns
#else
			li	tRFC,17			//T_RFC=135.34ns
#endif
Reset_DDR2_bank4:
			li	DRAM_TMP3,0xb800100c
			lw	DRAM_TMP2,0(DRAM_TMP3)
			nop
			and	DRAM_TMP2,~0xc0000000		//enable 4 banks
			sw	DRAM_TMP2,0(DRAM_TMP3)
			nop
			b	setting_mcr_plus
			nop	
DRAM_128MB_DDR2:
			li	DCR_VALUE, 0x54880000
			or	DRAM_size,0x81
#ifdef DRAM166			
			li	tRFC,21			//T_RFC=135.34ns
#else
			li	tRFC,17			//T_RFC=135.34ns
#endif				
			b	setting_mcr_plus
			nop		
DRAM_256MB_DDR2:
			li	DCR_VALUE, 0x56880000
			or	DRAM_size,0x101
#ifdef DRAM166			
			li	tRFC,31			//T_RFC=192.77ns
#else
			li	tRFC,26			//T_RFC=203.01ns
#endif					
			b	setting_mcr_plus
			nop				
setting_mcr_plus:
		li	DRAM_TMP0,0xb8001008
		lw	DRAM_TMP1,0(DRAM_TMP0)
		nop
		and	DRAM_TMP1,~0x1f0000			//clean tRFC.
		sll	tRFC,16
		or	DRAM_TMP1,DRAM_TMP1,tRFC
		sw	DRAM_TMP1,0(DRAM_TMP0)		//change tRFC value.
		nop
		sw  DCR_VALUE,0(DCR_ADDR)		//write DCR value.
		nop
		
		//save DRAM size 
		li	DRAM_TMP1,0xa0600030
		sw	DRAM_size,0(DRAM_TMP1)

		#undef	DCR_ADDR
		#undef	DCR_VALUE
		#undef	DRAM_pattern
		#undef	DRAM_CHECK_ADDR
		#undef	tRFC
		#undef	DRAM_TMP0
		#undef	DRAM_TMP1
		#undef DRAM_TMP2
		#undef DRAM_TMP3
		#undef DRAM_size	
	.endm // Detect_SDRAM_Size_plus2
#endif //CONFIG_RTL8676 || CONFIG_RTL8676S
	
	################################################################################
	#  SRAM_Map	 <Bus Address> <Size> <Segment> <Base> for "8676"
	#     Size      (0-128/1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################	
	################################################################################
	#  SRAM_Map	 <Bus Address> <Size> <Segment> <Base> for "8685"
	#     Size      (1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################	
	.macro SRAM_Map addr size segment base
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(\addr | 0x00000001)		//last bit it define enable
		li	SRAM_SIZE,\size					//32k bytes
		li	REG_TEMP,0xb8001300				//unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001304				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004000				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004004				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,\base					//
		li	REG_TEMP,0xb8004008				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)	
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm

#if defined(CONFIG_BOOT_MIPS) || defined(CONFIG_MIPS)
	################################################################################
	#  DSP_SRAM_Map	 <Bus Address> <Size> <Segment> <Base> for "8685"
	#     Size      (1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################	
	.macro DSP_SRAM_Map addr size segment base
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(\addr | 0x00000001)		//last bit it define enable
		li	SRAM_SIZE,\size					//32k bytes
		li	REG_TEMP,0xb8001340				//unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001344				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004040				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004044				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,\base					//
		li	REG_TEMP,0xb8004048				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)	
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm

	################################################################################
	#  DSP_SRAM_Unmap	 <Bus Address> <Size> <Segment> <Base> for "8685"
	#     Size      (1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################	
	.macro DSP_SRAM_Unmap addr size segment base
		/////////  DSP SRAM unmap /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(\addr | 0x00000001)		//last bit it define enable
		li	SRAM_SIZE,\size					//32k bytes
		li	REG_TEMP,0xb8001340				//unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001344				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm
#endif

	################################################################################
	#  UN_SRAM_Map	<Segment> <Base=0>
	#     
	#     Segment   (0/1/2/3) -> (0x00/0x10/0x20/0x30)	
	################################################################################	
	.macro UN_SRAM_Map segment
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(0x00000000)		//last bit it define enable
		li	SRAM_SIZE,(0x00000000)					//set size to zero
		li	REG_TEMP,0xb8001300				      //unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001304				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004000				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004004				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,(0x00000000)					//
		li	REG_TEMP,0xb8004008				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm

#if defined(CONFIG_BOOT_MIPS) || defined(CONFIG_MIPS)
	################################################################################
	#  DSP_UN_SRAM_Map	<Segment> <Base=0>
	#     
	#     Segment   (0/1/2/3) -> (0x00/0x10/0x20/0x30)	
	################################################################################	
	.macro DSP_UN_SRAM_Map segment
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(0x00000000)		//last bit it define enable
		li	SRAM_SIZE,(0x00000000)					//set size to zero
		li	REG_TEMP,0xb8001340				      //unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001344				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004040				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004044				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,(0x00000000)					//
		li	REG_TEMP,0xb8004048				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm
#endif
	
	################################################################################
	#  UN_SRAM_Map_1	<Segment> <Base=0>
	#     
	#     Segment   (0/1/2/3) -> (0x00/0x10/0x20/0x30)	
	################################################################################	
	.macro UN_SRAM_Map_1 segment
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(0x00000000)		//last bit it define enable
		li	SRAM_SIZE,(0x00000000)					//set size to zero
		li	REG_TEMP,0xb8001340				      //unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001344				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
	.endm

	################################################################################
	#  Mem_Init_RTL8676
	################################################################################
#if defined(CONFIG_RTL8676)
	.macro Mem_Init_RTL8676

		// SWR parameter change
		li		t1,0xb8000218
		li		t0,0x845555e7
		sw		t0,0(t1)
		nop
		li		t1,0xb800021c
		li		t0,0x0003a11e
		sw		t0,0(t1)
		nop		
		Noop_Delay 1500
		//
		li		t1,0xb8000200			//load System Clock Frequency Control Register	
		li		t0,0x77001200			//clear OCP and DRAM CLK fields
		li		t2,0xb8000308			//PCM is bonded if bit24=1 or bit25=1 in 0xb8000308
		lw 		t3,0(t2)
		nop
		srl 		t3,t3,24
		andi		t3,t3,0x3
		bne		t3,0x0,PCM_OCP_CLK
		nop
NORMAL_OCP_CLK:
		ori		t0,t0,0x15			//OCP=560MHz (0x1A), 460(0x15)
		b		DRAM_CLK
		nop
PCM_OCP_CLK:
		ori		t0,t0,0x1D			//OCP=620MHz (0x1D)
DRAM_CLK:
#ifdef DRAM166
		li		t2,0x080000			//Set DRAM=166MHz
		or		t0,t0,t2
#else
		li		t2,0x060000			//Set DRAM=133MHz
		or		t0,t0,t2
#endif
		sw		t0,0(t1)
		nop
		Noop_Delay 500
		nop

		##############################################
		# reg b800_1200: bit31-29,0:RAM clk/2, 1: RAM clk/4, 2: RAM clk/6, 3: RAM clk/8
		# Set default spi clock as RAM clk/6, e.g.200Mhz/6 = 33.333Mhz
		# bit26-22: deselect time
		# CS# deselect time min 100n for MX25L3205D, so set 19 to guarantee all device access normally.
		and		t2,t0,0x0F0000
		blt		t2,0x70000,set_div4		//DRAM freq < 150MHz, div 4, deselect value set to 0x10
		nop
		li		t0,0x5cc00000				//DRAM freq >= 150MHz, div 6, deselect value set to 0x13
		b		set_div
		nop
set_div4:
		li		t0,0x3c000000			//
set_div:		
		li		t1,0xb8001200
		sw		t0,0(t1)
		nop	
		Noop_Delay 100
		###############################################
		
		nop
		
		li		t1,0xb8000300
		li		t0,0x4
		sw		t0,0(t1)
		nop
		
		###############################################
		li		t0,0xb8000308		//Pin Status Register
		lw		t1,0(t0)
		nop
		and		t1,t1,~0x20000000	//bit29:Switch setting (0:master, 1:slave)
		sw		t1,0(t0)
		nop	
		
		//0xb800030c, Enable IP / IP selection Register,
		
	.endm
#endif //CONFIG_RTL8676
	
	################################################################################
	#  Mem_Init_RTL8676S
	################################################################################
#if defined(CONFIG_RTL8676S)
	.macro Mem_Init_RTL8676S
                //SWR parameters adjust for Core power
                li              t5, 0xb8000218
                li              t6, 0xb800021c
                li              t3,0x63330000                   //addition RL6333
                li              t0,0xb8000224
                lw              t1,0(t0)
                nop
                beq             t1,t3,RTL8676S_SWR
                nop
                //SWR parameters adjust for Core power
                li              t0, 0x89caaadc
                sw              t0,0(t5)
                li              t0, 0x00000082
                sw              t0,0(t6)
                b               MOVDD_BJT_CTRL
                nop
RTL8676S_SWR:
                li              t0, 0x89caaad5                  //1.21v
                sw              t0,0(t5)
                li              t0, 0x00000082                  //default
                sw              t0,0(t6)

MOVDD_BJT_CTRL:
                Noop_Delay 3000
                nop

		//MOVDD BJT CTRL patch.
		li		t1, 0xb8000214
		li		t0, 0x00026c00
		sw		t0,0(t1)
		Noop_Delay 300
		nop
		
		//Detect DRAM type and Set parameters.
		li		t0,0xb8000308
		lw		t2,0(t0)
		nop
		li		t3,0x200
		and		t3,t2,t3
		beqz		t3,SDRAM_SET
		nop
DDR1_SET:		
		li		t3,0x10000
		li		t1,0x42FFFFFF				//n31=0, n25=1, n24=0, n1..n0=> ESMT=00, Etron=01.For ETH n30..n26=0b100 00
		and 		t2,t1,t2
		li		t1,0x42000000				//n25=1, n30=1
		or		t2,t1,t2
		sw		t2,0(t0)
		b		SYS_CLK_SET
		nop
SDRAM_SET:
		li		t1,0xC1FFFFFF				//n31=1, n25=0, n24=1, n1..n0=> ESMT=00, Etron=01.For ETH n30..n26=0b100 00
		and 		t2,t1,t2
		li		t1,0xC1000000				//n31=1, n24=1, n30=1
		or		t2,t1,t2
		sw		t2,0(t0)		
SYS_CLK_SET:
		//
		li		t1,0xb8000200			//load System Clock Frequency Control Register	
		//li		t0,0x77000e12			//clear OCP and DRAM CLK fields, LX=200MHz, OCP=500MHz
		li		t0,0x77000e10			//clear OCP and DRAM CLK fields, LX=200MHz, OCP=450MHz		
#ifdef DRAM166
		//li		t2,0x050000			//Set DRAM=175MHz
		li		t2,0x040000			//Set DRAM=150MHz
		//li		t2,0x030000			//Set DRAM=125MHz
		//li		t2,0x020000			//Set DRAM=100MHz
		add		t2,t2,t3				//if DDR, DRAM=175MHz, elase 150MHz
		or		t0,t0,t2
#else
		li		t2,0x030000			//Set DRAM=125MHz
		//li		t2,0x020000			//Set DRAM=100MHz, for T176 QA board use.
		add		t2,t2,t3				//if DDR, DRAM=150MHz, elase 125MHz
		or		t0,t0,t2
#endif
		sw		t0,0(t1)
		nop
		Noop_Delay 500
		nop
#if 1
		##############################################
		# reg b800_1200: bit31-29,0:RAM clk/2, 1: RAM clk/4, 2: RAM clk/6, 3: RAM clk/8
		# Set default spi clock as RAM clk/6, e.g.200Mhz/6 = 33.333Mhz
		# bit26-22: deselect time
		# CS# deselect time min 100n for MX25L3205D, so set 19 to guarantee all device access normally.
		and		t2,t0,0x0F0000
		blt		t2,0x70000,set_div4		//DRAM freq < 150MHz, div 4, deselect value set to 0x10
		nop
		li		t0,0x5ac00000				//DRAM freq >= 150MHz, div 6, deselect value set to 0x13
		b		set_div
		nop
set_div4:
		li		t0,0x3ac00000			//
set_div:		
		li		t1,0xb8001200
		sw		t0,0(t1)
		nop	
		Noop_Delay 100
		###############################################
		
		nop
		
		li		t1,0xb8000300
		li		t0,0x4
		sw		t0,0(t1)
		nop
#endif		
	.endm
#endif //CONFIG_RTL8676S	
	
	#define NAND_CHECK_READY()      \
123:;							\
	la		s0, NAND_CTRL_BASE+0x4; \
	lw    	t0, 0(s0);       \
	and   	t0, t0, 0x80000000;      \
	bne   	t0, 0x80000000, 123b;    \
	nop
	
	
	################################################################################
	#  NAND Controller Registers	
	################################################################################
	#define NandRegCtrl			(NAND_CTRL_BASE+0x04)
	#define NandRegRW			(NAND_CTRL_BASE+0x10)
	#define NandRegFlashAddr	(NAND_CTRL_BASE+0x18)
	#ifndef CONFIG_RTL8685	
	#define NandRegRAMAddr		(NAND_CTRL_BASE+0x1C)
	#define NandRegTagAddr		(NAND_CTRL_BASE+0x54)
	#else
	#define NandRegRAMAddr		(NAND_CTRL_BASE+0x20)
	#define NandRegTagAddr		(NAND_CTRL_BASE+0x24)
	#endif	
		
	################################################################################
	#  NAND_Ready  
	#     Loop until NAND controller is ready
	################################################################################
	.macro NAND_Reay
	11:
		la		t7, NandRegCtrl
		lw		t7, 0(t7)
		nop
		srl		t7, 31
		beq		zero, t7, 11b
		nop
	.endm
	
	################################################################################
	#  NAND_Copy_2048 <page_from> <page_to> <Ram> <Tag> 
	#     Copy n x 2k page from NAND Flash address to RAM/Tag
	################################################################################			
	.macro NAND_Copy_2048 PageFrom PageTo RamAddr TagAddr
		la		t5, 0xc00fffff
		la		t4, NandRegCtrl
		sw		t5, 0(t4)
		
		li		t1, \PageFrom * 0x1000
		li		t2, \RamAddr
		li		t3, \TagAddr
		li		t4, \PageTo * 0x1000
	
21:		
		li		t0, 4
20:		
		la		t5, NandRegFlashAddr	# initialize variables
		sw		t1, 0(t5)
		la		t6, NandRegRAMAddr
		sw		t2, 0(t6)
		la		t5, NandRegTagAddr
		sw		t3, 0(t5)
		la		t6, NandRegRW
		li		t5, 0x5b				# start write
		sw		t5, 0(t6)
		NAND_Reay
		
		addi	t0, t0, -1
		addi	t1, t1, 0x210
		addi	t2, t2, 0x200
		addi	t3, t3, 0x10
		bne		zero, t0, 20b
		nop
		
		addi	t1, t1, (0x1000 - 0x840)				
		bne		t1, t4, 21b
		nop		
	.endm
	
	################################################################################
	#  NAND_Copy_512 <page_from> <page_to> <Ram> <Tag> 
	#  		Copy n x 512B page from NAND Flash address to RAM/Tag
	################################################################################
	.macro NAND_Copy_512 PageFrom PageTo RamAddr TagAddr
		la		t5, 0xc00fffff
		la		t4, NandRegCtrl
		sw		t5, 0(t4)
		
		
		li		t2, \RamAddr
		li		t3, \TagAddr
		li		t1, \PageFrom * 0x200
		li		t0, \PageTo * 0x200
		
30:
		la		t4, NandRegFlashAddr	# initialize variables
		sw		t1, 0(t4)
		la		t5, NandRegRAMAddr
		sw		t2, 0(t5)
		la		t4, NandRegTagAddr
		sw		t3, 0(t4)
		la		t5, NandRegRW
		li		t4, 0x5b				# start write
		sw		t4, 0(t5)
		NAND_Reay
		
		addi	t1, t1, 0x200
		addi	t2, t2, 0x200
		addi	t3, t3, 0x10
		
		bne		t0, t1, 30b
		nop		
	.endm
	
	################################################################################
	#  Copy data to IMEM  
	#    src - can be any address
	#    dst - MUST be a cachable address (i.e. 0x80000000)
	#   size - length to copy
	#   c3base, c3top are CP3 BASE,TOP register ($0, $1 for IMEM0)
	#  Corresponding IMEM will be enabled after this call
	################################################################################
	.macro CopyToIMEM  dst src size c3base c3top c3sel
		li		k0, \dst		// set IMEM0 range (base-top)	
		li		k1, 0x0ffffc00	// must be physical address
		and		k0, k0, k1
		add		k1, k0, \size - 1
		mtc3	k0, \c3base
		mtc3	k1, \c3top
		
		mtc0	zero, $20		// Enable IMEM
		li		k0, 0x40		
		mtc0	k0, $20, \c3sel
		
		li		t0, \src
		li		t1, \dst
		li		t2, \size
1:
		lw		k0, 0(t0)
		lw		k1, 4(t0)
		mtc0	k0, $28
		mtc0	k1, $29
		add		t0, t0, 8
		cache	0xa, 0(t1)
		sub		t2, t2, 8
		add		t1, t1, 8
		bne		$0, t2, 1b
		nop				
	.endm

	################################################################################
	#  Copy data from IMEM
	#    src - MUST be a cachable address (i.e. 0x80000000)
	#    dst - can be any address
	#   size - length to copy
	#   This macro assumes IMEM is already enabled.
	################################################################################
	.macro CopyFromIMEM  dst src size c3sel								
		li		t0, \src
		li		t1, \dst
		li		t2, \size
1:
		cache	0x6, 0(t0)
		sub		t2, t2, 8
		add		t0, t0, 8
		nop
		nop
		nop
		mfc0	k0, $28
		mfc0	k1, $29
		sw		k0, 0(t1)
		sw		k1, 4(t1)
		add		t1, t1, 8				
		bne		$0, t2, 1b
		nop				
	.endm
	
	.macro CopyToIMEM0 dst src size
		CopyToIMEM \dst, \src, \size, $0, $1, 0
	.endm
	
	.macro CopyToIMEM1 dst src size
		CopyToIMEM \dst, \src, \size, $2, $3, 1
	.endm
	
	################################################################################
	#  rlx5281  ICache invalidate & 
	#			DCache invalidate and flush		
	################################################################################
	.macro rlx5281_cache_flush
		mtc0	zero, $20
		nop
		li		t0, 0x202
		mtc0	t0, $20
		nop
		mtc0	zero, $20
		nop
	.endm
	
	################################################################################
	#  rlx5281  DCache Write-Allocation Enable		
	################################################################################
	.macro rlx5281_dcache_wa_en
		mfc0    t0, $20
        li      t1, 0xFFFF7F7F
        and     t0, t0, t1
        mtc0    t0, $20
        nop
        li      t1, 0x00000080 # bit 7 of COP0 reg 20, select 0
        or      t0, t0, t1
        mtc0    t0, $20
        nop
	.endm
	
	################################################################################
	#  rlx5281  DCache Write-Allocation Disable		
	################################################################################
	.macro rlx5281_dcache_wa_dis
		/* Turn off cache Write allocation */
        mfc0    t0, $20
        li      t1, 0xFFFF7F7F
        and     t0, t0, t1
        mtc0    t0, $20
        nop
        li      t1, 0x00008000 # bit 15 of COP0 reg 20, select 0
        or      t0, t0, t1
        mtc0    t0, $20
        nop
	.endm
	
