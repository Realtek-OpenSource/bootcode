

__romInit:
	.set	noreorder
	mfc0    k0,C0_EPC               # save for nvram
	move    k1,ra                   # save for nvram
	mfc0    gp,C0_SR
	mfc0    t7,C0_CAUSE
	li      v0,SR_BEV
	mtc0    v0,C0_SR                # state unknown on reset
	mtc0    zero,C0_CAUSE           # clear software interrupts
	nop                             # paranoia


__romReboot:
	/* second entry point -- sw reboot inherits a0 start type            */
	.set	noreorder
	li	t0, SR_CU1 | SR_BEV
	nop
	mtc0	t0, C0_SR			/* disable all interrupts fpa on,    */
	nop	
								/* prom exception handlers           */
	mtc0	zero, C0_CAUSE		/* clear all interrupts              */
	nop	
	
	#--- bypass copying if in RAM	
	bal 	1f
	nop
1:
	la		t0, 0x10000000
	nop
	and		t0, ra, t0
	beq		t0, zero, 2f				# decide if ram based on BIT28
	nop

	.set	noreorder