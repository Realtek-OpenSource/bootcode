#ifndef __IPC_H__
#define __IPC_H__

#include "./bspchip_8685.h"
#include "./memctl_8685.h"
#include "./memctl_8685_func.h"

#define CONFIG_IPC_PLUS

#ifndef BIT
#define BIT(x) (1 << x)
#endif

#if 1
#define CPU_IPC_DESC_TX_BASE 	0xa80014e0
#define CPU_IPC_DESC_RX_BASE 	0xa80014f0
#define CPU_IPC_DESC_TX_ADDR 	0xa80014e4
#define CPU_IPC_DESC_RX_ADDR 	0xa80014f4
#define CPU_IPC_DESC_TX_DATA 	0xa80014e8
#define CPU_IPC_DESC_RX_DATA 	0xa80014f8
#define CPU_IPC_DESC_TX_RSVD 	0xa80014ec
#define CPU_IPC_DESC_RX_RSVD 	0xa80014fc
#define DSP_IPC_DESC_TX_BASE 	0xbfc014f0
#define DSP_IPC_DESC_RX_BASE 	0xbfc014e0
#define DSP_IPC_DESC_TX_ADDR 	0xbfc014f4
#define DSP_IPC_DESC_RX_ADDR 	0xbfc014e4
#define DSP_IPC_DESC_TX_DATA 	0xbfc014f8
#define DSP_IPC_DESC_RX_DATA 	0xbfc014e8
#define DSP_IPC_DESC_TX_RSVD	0xbfc014fc
#define DSP_IPC_DESC_RX_RSVD 	0xbfc014ec
#else
#define CPU_IPC_DESC_TX_BASE 	0xb8004100
#define CPU_IPC_DESC_RX_BASE 	0xb8004110
#define CPU_IPC_DESC_TX_ADDR 	0xb8004104
#define CPU_IPC_DESC_RX_ADDR 	0xb8004114
#define CPU_IPC_DESC_TX_DATA 	0xb8004108
#define CPU_IPC_DESC_RX_DATA 	0xb8004118
#define CPU_IPC_DESC_TX_RSVD 	0xb800410c
#define CPU_IPC_DESC_RX_RSVD 	0xb800411c
#define DSP_IPC_DESC_TX_BASE 	0xb8004110
#define DSP_IPC_DESC_RX_BASE 	0xb8004100
#define DSP_IPC_DESC_TX_ADDR 	0xb8004114
#define DSP_IPC_DESC_RX_ADDR 	0xb8004104
#define DSP_IPC_DESC_TX_DATA 	0xb8004118
#define DSP_IPC_DESC_RX_DATA 	0xb8004108
#define DSP_IPC_DESC_TX_RSVD	0xb800411c
#define DSP_IPC_DESC_RX_RSVD 	0xb800410c
#endif

/*IPC stats*/
#define CMD_READY	BIT(31)
#define DATA_READY	BIT(30)
#define IS_LINK		BIT(29)
#define IS_ERR		BIT(28)

/*IPC command*/
#define IPC_CMD_MASK 	0x0fffffff
#define IPC_W_PATTERN	0x4
#define IPC_V_PATTERN	0x5
#define IPC_MUTEX_TEST	0x6
#define IPC_OVER	0x2
#ifdef CONFIG_IPC_PLUS
#define IPC_READ	0x0
#define IPC_WRITE	0x1	
#define IPC_ERROR	0x3
#endif

#define IPC_DELAY	50000
static inline void delay_nop(int cnt) {
	int i = 0;
	while(i < cnt) {
		__asm__ volatile("nop");
		i++;
	}
}


#endif /*__IPC_H__*/
