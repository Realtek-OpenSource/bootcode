#include "ipc.h"

//#define IPC_DSP_DBG

#ifdef IPC_DSP_DBG
#define DBG_PUTS 	puts
#define DBG_PUTHEX	puthex
#else
#define DBG_PUTS
#define DBG_PUTHEX
#endif

#ifdef MIPS_SYNC
void mips_sync(void){
        __asm__ volatile(
                        ".set  push\n\t"
                        ".set  noreorder\n\t"
						"sync\n\t"
                        ".set pop\n"
                        "nop"
                        : /* no output */
                        : /* no input */
						: "memory"
                                );
}
#endif

void ipc_dsp_tx_desc_reset(void) {
	REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
	REG32(DSP_IPC_DESC_TX_BASE) &= ~(DATA_READY);
	REG32(DSP_IPC_DESC_TX_BASE) &= ~(IS_ERR);
	REG32(DSP_IPC_DESC_TX_BASE) |= 0xf;
	REG32(DSP_IPC_DESC_TX_ADDR) = 0x0;
	REG32(DSP_IPC_DESC_TX_DATA) = 0x0;
	
	return;	
}

#if 0
unsigned int hw_mutex_test(unsigned int times) {
	int ret = 0;
	unsigned int cnt = 0;
	
	while (cnt < times) {
		while( REG32(0xb8141044) != 0);

		//OC1 get mutex;
		if (REG32(0xb8141048) == 0x2) {
			cnt++;
			//OC1 release mutex;
			REG32(0xb8141044) = 0;
		}
		else ret = 1; //something is wrong...
	}

	return ret;
}
#endif

/*server waits for the client's commnd and response to it*/
volatile void ipc_dsp_server (void) {
	int session_over = 0;
	unsigned int addr;
	unsigned int data;
	unsigned int cmd;
	int i;

	REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
	REG32(DSP_IPC_DESC_TX_BASE) &= ~(DATA_READY);
	
	DBG_PUTS("[DSP] Server starts...\n\r");

	/*waits for client link*/
	while (!(REG32(DSP_IPC_DESC_RX_BASE) & IS_LINK));
		//delay_nop(IPC_DELAY);
	DBG_PUTS("[DSP] CPU has connected to DSP\n\r");
	
	/*reply link ack to client*/
	REG32(DSP_IPC_DESC_TX_BASE) |= IS_LINK;
	
	while (1) {
		//waits for the client's command
#if 0
		while (!(REG32(DSP_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(DSP_IPC_DESC_RX_BASE) & CMD_READY));
			//delay_nop(IPC_DELAY);
		DBG_PUTS("[DSP] Both CPU and DSP are ready, wait DSP's CMD\n\r");
#endif
		while((REG32(DSP_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(DSP_IPC_DESC_RX_BASE) & DATA_READY));
			//delay_nop(IPC_DELAY);
			
		REG32(DSP_IPC_DESC_TX_BASE) &= ~(CMD_READY);	
		
		cmd =  (REG32(DSP_IPC_DESC_RX_BASE) & IPC_CMD_MASK);
		if (cmd == IPC_W_PATTERN) {
				DBG_PUTS("[DSP] receives cmd IPC_W_PATTERN\n\r");
				_write_pattern_1(REG32(DSP_IPC_DESC_RX_ADDR), REG32(DSP_IPC_DESC_RX_DATA));
				
				/*DSP reply the result to CPU*/
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_W_PATTERN;
				REG32(DSP_IPC_DESC_TX_ADDR) = 0xcafe0001;
				REG32(DSP_IPC_DESC_TX_DATA) = 0;
				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;

		}
		else if (cmd == IPC_V_PATTERN) {
				DBG_PUTS("[DSP] receives cmd IPC_V_PATTERN\n\r");
				
				data = _verify_pattern_1(REG32(DSP_IPC_DESC_RX_ADDR), REG32(DSP_IPC_DESC_RX_DATA));
				//puts("[DSP] finished _verify_pattern_1, data=0x"); puthex(data); puts("\n");
				
				/*DSP reply the result to CPU*/
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_V_PATTERN;
				REG32(DSP_IPC_DESC_TX_ADDR) = 0xcafe0002;
				REG32(DSP_IPC_DESC_TX_DATA) = data;
				while (REG32(DSP_IPC_DESC_TX_DATA) != data) {
					delay_nop(IPC_DELAY);
					REG32(DSP_IPC_DESC_TX_DATA) = data;
				}

				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;
				
				#ifdef MIPS_SYNC
				mips_sync();
				#endif
		}
#if 0
                else if (cmd == IPC_MUTEX_TEST) {                
                                DBG_PUTS("[DSP] receives mutex test\n\r");
                                data = hw_mutex_test(REG32(DSP_IPC_DESC_RX_DATA));                                                                                                     
                                                               
                                /*DSP reply the result to CPU*/
                                REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
                                REG32(DSP_IPC_DESC_TX_BASE) |= IPC_MUTEX_TEST;
                                REG32(DSP_IPC_DESC_TX_ADDR) = 0xcafe0003;
                                REG32(DSP_IPC_DESC_TX_DATA) = data;
                                REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
                                REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;                                                                                                                                        
                        
                }
#endif
		else if (cmd == IPC_OVER) {
				DBG_PUTS("[DSP] receives session over\n\r");
				session_over = 1;
	
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_OVER;
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IS_LINK);
				REG32(DSP_IPC_DESC_TX_ADDR) = 0;
				REG32(DSP_IPC_DESC_TX_DATA) = 0;
				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;
				
		}
#ifdef CONFIG_IPC_PLUS
		else if (cmd == IPC_READ) {
				DBG_PUTS("[DSP] CPU read, addr: 0x");
				DBG_PUTHEX(REG32(DSP_IPC_DESC_RX_ADDR));
				
				/*DSP read data from addr*/
				data = REG32(REG32(DSP_IPC_DESC_RX_ADDR));
				DBG_PUTS(" data: 0x");
				DBG_PUTHEX(data);
				DBG_PUTS("\n\r\n\r");			
	
				/*DSP reply the result to CPU*/
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_READ;
				REG32(DSP_IPC_DESC_TX_ADDR) = REG32(DSP_IPC_DESC_RX_ADDR);
				REG32(DSP_IPC_DESC_TX_DATA) = data;
				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;
		}
		else if	(cmd == IPC_WRITE) {
				DBG_PUTS("[DSP] CPU write, data: 0x");
				DBG_PUTHEX(REG32(DSP_IPC_DESC_RX_DATA));
				DBG_PUTS(" to addr: 0x");
				DBG_PUTHEX(REG32(DSP_IPC_DESC_RX_ADDR));
				DBG_PUTS("\n\r");
				/* DSP write data to addr */
				addr = REG32(DSP_IPC_DESC_RX_ADDR);
				data = REG32(DSP_IPC_DESC_RX_DATA);
				REG32(addr) = data;
				data = REG32(addr);

				DBG_PUTS("[DSP] data in addr: 0x");
				DBG_PUTHEX(addr);
				DBG_PUTS(" after write: 0x");
				DBG_PUTHEX(data);
				DBG_PUTHEX("\n\r\n\r");

				/*DSP reply the result to CPU*/
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_WRITE;
				REG32(DSP_IPC_DESC_TX_ADDR) = REG32(DSP_IPC_DESC_RX_ADDR);
				REG32(DSP_IPC_DESC_TX_DATA) = REG32(DSP_IPC_DESC_RX_DATA);
				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;
	
		}
		else if (cmd == IPC_ERROR) {	
				DBG_PUTS("[DSP] CPU finds error in DSP response\n\r");
				
				REG32(DSP_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
				REG32(DSP_IPC_DESC_TX_BASE) |= IPC_ERROR;
				REG32(DSP_IPC_DESC_TX_BASE) |= IS_ERR;
				REG32(DSP_IPC_DESC_TX_ADDR) = 0;
				REG32(DSP_IPC_DESC_TX_DATA) = 0;
				REG32(DSP_IPC_DESC_TX_BASE) |= CMD_READY;
				REG32(DSP_IPC_DESC_TX_BASE) |= DATA_READY;
		}
#endif
		else {
				DBG_PUTS("[DSP] invalid command!!!\n\r");
		} 

		if (session_over) {
			//puts("[DSP] IPC session is over\n\r");
			//while(REG32(0xa8000008) == 0);	
			return;
		}

		while (!(REG32(DSP_IPC_DESC_RX_BASE) & CMD_READY));
		ipc_dsp_tx_desc_reset();
	}	
	
	return;

}

