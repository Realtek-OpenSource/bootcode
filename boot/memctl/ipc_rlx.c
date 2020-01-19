#include "ipc.h"

void ipc_cpu_init (void) {
        REG32(CPU_IPC_DESC_TX_BASE) = 0xf;
        REG32(CPU_IPC_DESC_RX_BASE) = 0xf;
        REG32(CPU_IPC_DESC_TX_ADDR) = 0x0;
        REG32(CPU_IPC_DESC_RX_ADDR) = 0x0;
        REG32(CPU_IPC_DESC_TX_DATA) = 0x0;
        REG32(CPU_IPC_DESC_RX_DATA) = 0x0;
        REG32(CPU_IPC_DESC_TX_RSVD) = 0x5a5aa5a5;
        REG32(CPU_IPC_DESC_RX_RSVD) = 0xa5a55a5a;

        return;
}

void ipc_cpu_tx_desc_reset(void) {
	REG32(CPU_IPC_DESC_TX_BASE) |= CMD_READY;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(DATA_READY);
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IS_ERR);
	REG32(CPU_IPC_DESC_TX_BASE) |= 0xf;
	REG32(CPU_IPC_DESC_TX_ADDR) = 0x0;
	REG32(CPU_IPC_DESC_TX_DATA) = 0x0;
	
	return;	
}

#ifdef CONFIG_IPC_PLUS
unsigned int ipc_cpu_read(unsigned int addr) {
	unsigned int data;
	
	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_READ;
	REG32(CPU_IPC_DESC_TX_ADDR) = addr;
	REG32(CPU_IPC_DESC_TX_DATA) = 0;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;
	
	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));
	
	/*get the data*/
	data = REG32(CPU_IPC_DESC_RX_DATA);

	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	
	return data;
}

void ipc_cpu_write(unsigned int addr, unsigned int data) {

	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_WRITE;
	REG32(CPU_IPC_DESC_TX_ADDR) = addr;
	REG32(CPU_IPC_DESC_TX_DATA) = data;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;
	
	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));	

	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	
	return;
}

void ipc_cpu_notify_error(void) {

	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_ERROR;
	REG32(CPU_IPC_DESC_TX_ADDR) = 0;
	REG32(CPU_IPC_DESC_TX_DATA) = 0;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;

	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));
	if (REG32(CPU_IPC_DESC_RX_BASE) & IS_ERR) REG32(CPU_IPC_DESC_TX_BASE) |= IS_ERR;	

	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	
	return;
}
#endif

#if 0
void ipc_hw_mutex_test(unsigned int times) {
	unsigned int cnt = 0;
	unsigned int result = 1;

        while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));                                                                                                         
        /*fill up cmd*/       
        REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
        REG32(CPU_IPC_DESC_TX_BASE) |= IPC_MUTEX_TEST;
        REG32(CPU_IPC_DESC_TX_ADDR) = 0;
        REG32(CPU_IPC_DESC_TX_DATA) = times;
        REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
        REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;                                                                                                                                                                
	
	while (cnt < times) {
		while (REG32(0xb8141040) != 0);
	
		puts("[CPU] OC0 get mutex, mutex ID:"); puthex(REG32(0xb8141048)); puts(", times: "); puthex(cnt+1); puts("\n\r");
		
		if (REG32(0xb8141048) == 0x1) {
			cnt++;
			REG32(0xb8141040) = 0;
			//puts("[CPU] OC0 release mutex, mutex ID:"); puthex(REG32(0xb8141048)); puts(", times: "); puthex(cnt+1); puts("\n\r");	
		} else {
			puts("exception in hw mutex test\n\r");
			break;
		}
		
		
	}       
 
        /*waits for server's response*/
        while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));                                                                                                        
        
        if (REG32(CPU_IPC_DESC_RX_ADDR) == 0xcafe0003) {
                //puts("[CPU] DSP has finished _verify_pattern_1\n\r");
                result = REG32(CPU_IPC_DESC_RX_DATA);
                //puts("[CPU] DSP has finished _verify_pattern_1, result=0x"); puthex(result); puts("\n");                                                                                                        

                if (result == 0) puts("[CPU] IPC HW mutex test pass\n\r");                                                                                                                                          
                else {
                        puts("[CPU] IPC HW mutex test fail, addr:");                                                                                                                  
                }
        }else{  
                puts("[CPU] failed to execute IPC HW mutex test\n\r");                                                                                                                                              
        }

        /*reset tx desc*/
        ipc_cpu_tx_desc_reset();       
        return;
}
#endif

void ipc_write_pattern(unsigned int addr, unsigned int len) {
	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_W_PATTERN;
	REG32(CPU_IPC_DESC_TX_ADDR) = addr;
	REG32(CPU_IPC_DESC_TX_DATA) = len;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;
	
	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));
	
	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	return;
}


void rlx_sync(void){        
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

unsigned int ipc_verify_pattern(unsigned int addr, unsigned int len) {
	unsigned int result = 0xffffffff;
	unsigned int count = 0x10000;
	//unsigned int rx_base_temp = 0;

	//puts("[CPU] issue ipc_write_pattern to DSP\n\r");
	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));
	//delay_nop(500000);

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_V_PATTERN;
	REG32(CPU_IPC_DESC_TX_ADDR) = addr;
	REG32(CPU_IPC_DESC_TX_DATA) = len;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;

#if 0
	puts("[CPU] 0xCAB01234 ");//puthex(REG32(CPU_IPC_DESC_RX_BASE));puts("\n");
	puts("This is a test. ");
	puts("To check the timing");
	puts("\n\r");
	//rx_base_temp = REG32(CPU_IPC_DESC_RX_BASE);	
#endif
	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));	

#if 1
	while(count--){

		/*get the data*/
		if (REG32(CPU_IPC_DESC_RX_ADDR) == 0xcafe0002) {
			//puts("[CPU] DSP has finished _verify_pattern_1\n\r");
			result = REG32(CPU_IPC_DESC_RX_DATA);
			//puts("[CPU] DSP has finished _verify_pattern_1, result=0x"); puthex(result); puts("\n");
			break;
		}else{
			rlx_sync();	
			if(count==1){
				;		
			}
		}
	}
	
#else
	/*get the data*/
	if (REG32(CPU_IPC_DESC_RX_ADDR) == 0xcafe0002) {
		//puts("[CPU] DSP has finished _verify_pattern_1\n\r");
		result = REG32(CPU_IPC_DESC_RX_DATA);
		//puts("[CPU] DSP has finished _verify_pattern_1, result=0x"); puthex(result); puts("\n");
	}
	else {
		puts("[CPU] DSP failed to finish _verify_pattern_1\n\r");	
	}
#endif
	
	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	return result;
}

int ipc_end_session(void) {
	int ret = 0;

	/*make sure both CPU and CPU are ready*/
	while (!(REG32(CPU_IPC_DESC_TX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY));

	/*fill up cmd*/
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(IPC_CMD_MASK);
	REG32(CPU_IPC_DESC_TX_BASE) |= IPC_OVER;
	REG32(CPU_IPC_DESC_TX_ADDR) = 0;
	REG32(CPU_IPC_DESC_TX_DATA) = 0;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(CMD_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= DATA_READY;
	
	/*waits for server's response*/
	while (!(REG32(CPU_IPC_DESC_RX_BASE) & CMD_READY) || !(REG32(CPU_IPC_DESC_RX_BASE) & DATA_READY));
	if (!(REG32(CPU_IPC_DESC_RX_BASE) & IS_LINK)) {
		REG32(CPU_IPC_DESC_TX_BASE) &= ~(IS_LINK);
		puts("[RLX5281] IPC disconnected\n\r");
	}
	else {
		/*disconnect failed*/
		puts("[RLX5281] IPC disconnect cmd failed\n\r");
		ret = -1;
	}
	/*reset tx desc*/
	ipc_cpu_tx_desc_reset();
	
	return ret;
}

volatile void ipc_cpu_client(void) {
	int i;	
	unsigned int data;
	unsigned int addr;

	REG32(CPU_IPC_DESC_TX_BASE) |= CMD_READY;
	REG32(CPU_IPC_DESC_TX_BASE) &= ~(DATA_READY);
	REG32(CPU_IPC_DESC_TX_BASE) |= IS_LINK;
		
	//delay_nop(500000);
	/*waits for MIPS link ack*/
	puts("[RLX5281] waits for MIPS booting up\n\r");
	while(!(REG32(CPU_IPC_DESC_RX_BASE) & IS_LINK));
	puts("[RLX5281] IPC between RLX and MIPS has been established\n\r");
	delay_nop(500000);
#if 0
	ipc_write_pattern(0x80b00000, 0x10000);
	data = ipc_verify_pattern(0x80b00000, 0x10000);
		
	puts("[CPU] ipc test, veryify pattern result: 0x");
	puthex(data);
	puts("\n\r");
	
	for (i = 0; i < 20; i++) {
		addr = 0xa0b00000 + i*4;
		data = ipc_cpu_read(addr);
		puts("[CPU] read addr: 0x");
		puthex(addr);
		puts(" data: 0x");
		puthex(data);
		puts("\n\r");

		data = 0xcafe0000 | i;
		//delay_nop(IPC_DELAY);
		ipc_cpu_write(addr, data);
		puts("[CPU] write addr: 0x");
		puthex(addr);
		puts(" data: 0x");
		puthex(data);
		puts("\n\r");
		//delay_nop(IPC_DELAY);
		data = ipc_cpu_read(addr);
		puts("[CPU] read addr: 0x");
		puthex(addr);
		puts(" data: 0x");
		puthex(data);
		puts("\n\r");
	}
	
	ipc_end_session();
#endif
	return;
}

