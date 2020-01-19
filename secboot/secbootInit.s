#define _ASMLANGUAGE
#include "eregdef.h"

	
	.extern C_Entry

secbootInit:  
      	/* Init BSS spaces */
        la      t0, _fbss
        la      t1, _ebss
        beq     t0, t1, 2f
1:      sw      zero, 0(t0)
        nop
        addi    t0, t0, 4
        bne     t0, t1, 1b
        nop
2:
	/*jump to C_Entry in secboot*/
	la 	t0, C_Entry
	jr 	t0
	nop

