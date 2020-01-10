	.cpu cortex-a53
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"asm-offsets.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.type	main, %function
main:
.LFB112:
	.file 1 "lib/asm-offsets.c"
	.loc 1 23 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 25 0
#APP
@ 25 "lib/asm-offsets.c" 1
	
->GENERATED_GBL_DATA_SIZE #128 (sizeof(struct global_data) + 15) & ~15
@ 0 "" 2
	.loc 1 28 0
@ 28 "lib/asm-offsets.c" 1
	
->GENERATED_BD_INFO_SIZE #32 (sizeof(struct bd_info) + 15) & ~15
@ 0 "" 2
	.loc 1 32 0
	mov	r0, #0
	bx	lr
	.cfi_endproc
.LFE112:
	.size	main, .-main
	.text
.Letext0:
	.file 2 "/home/fae/nas/svn/1295/SPI_bootcode/bootcode/uboot/include/net.h"
	.file 3 "/home/fae/nas/svn/1295/SPI_bootcode/bootcode/uboot/include/common.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0xcd
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF15
	.byte	0x1
	.4byte	.LASF16
	.4byte	.LASF17
	.4byte	.Ldebug_ranges0+0
	.4byte	0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF3
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF9
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF10
	.uleb128 0x4
	.4byte	.LASF18
	.byte	0x4
	.byte	0x2
	.2byte	0x1ee
	.4byte	0x9f
	.uleb128 0x5
	.4byte	.LASF11
	.sleb128 0
	.uleb128 0x5
	.4byte	.LASF12
	.sleb128 1
	.uleb128 0x5
	.4byte	.LASF13
	.sleb128 2
	.uleb128 0x5
	.4byte	.LASF14
	.sleb128 3
	.byte	0
	.uleb128 0x6
	.4byte	.LASF19
	.byte	0x1
	.byte	0x16
	.4byte	0x48
	.4byte	.LFB112
	.4byte	.LFE112-.LFB112
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x7
	.4byte	.LASF20
	.byte	0x3
	.2byte	0x3e9
	.4byte	0x4f
	.4byte	0x200000
	.uleb128 0x8
	.4byte	.LASF21
	.byte	0x2
	.2byte	0x1f6
	.4byte	0x79
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB112
	.4byte	.LFE112-.LFB112
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB112
	.4byte	.LFE112
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF6:
	.ascii	"long long int\000"
.LASF5:
	.ascii	"unsigned int\000"
.LASF13:
	.ascii	"NETLOOP_SUCCESS\000"
.LASF11:
	.ascii	"NETLOOP_CONTINUE\000"
.LASF1:
	.ascii	"long unsigned int\000"
.LASF7:
	.ascii	"long long unsigned int\000"
.LASF15:
	.ascii	"GNU C 4.9.3 20150413 (prerelease) -mlittle-endian -"
	.ascii	"mfpu=neon -mfloat-abi=softfp -mtune=cortex-a53 -mfl"
	.ascii	"oat-abi=soft -marm -mno-thumb-interwork -mabi=aapcs"
	.ascii	"-linux -march=armv8-a -mno-unaligned-access -mtls-d"
	.ascii	"ialect=gnu -g -Os -fuse-tls -fno-common -ffixed-r8 "
	.ascii	"-fno-builtin -ffreestanding -fno-stack-protector -f"
	.ascii	"stack-usage\000"
.LASF20:
	.ascii	"alloc_ptr\000"
.LASF17:
	.ascii	"/home/fae/nas/svn/1295/SPI_bootcode/bootcode/uboot\000"
.LASF0:
	.ascii	"unsigned char\000"
.LASF16:
	.ascii	"lib/asm-offsets.c\000"
.LASF10:
	.ascii	"char\000"
.LASF9:
	.ascii	"long int\000"
.LASF14:
	.ascii	"NETLOOP_FAIL\000"
.LASF12:
	.ascii	"NETLOOP_RESTART\000"
.LASF21:
	.ascii	"net_state\000"
.LASF2:
	.ascii	"short unsigned int\000"
.LASF3:
	.ascii	"signed char\000"
.LASF19:
	.ascii	"main\000"
.LASF18:
	.ascii	"net_loop_state\000"
.LASF4:
	.ascii	"short int\000"
.LASF8:
	.ascii	"sizetype\000"
	.ident	"GCC: (Realtek ASDK-4.9.3 Build 2180) 4.9.3 20150413 (prerelease)"
	.section	.note.GNU-stack,"",%progbits
