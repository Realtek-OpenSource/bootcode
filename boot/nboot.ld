SECTIONS {
	.text 0x80000000 : {
		*(.text)
	}
	.data : {
		_fdata = ABSOLUTE(.);
		*(.data) *(.rodata) *(.rodata*)
		_gp = ABSOLUTE(.);
		*(.sdata)
		*(.reginfo)
		_edata = ABSOLUTE(.);
	}
	.bss : {
		_fbss = ABSOLUTE(.);
		*(.bss) *(.sbss)
		*(.scommon) *(.common) *(COMMON)
		. = ALIGN(4);
		_ebss = ABSOLUTE(.);
	}
}

