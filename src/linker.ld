MEMORY
{
	CODE (rx) : ORIGIN = 0x80000, LENGTH = 128K
	ROM (r) : ORIGIN = 0xa0000, LENGTH = 128K
	RAM (rw) : ORIGIN = 0xc0000, LENGTH = 5M
}

SECTIONS
{
	.text.boot : {
		*(.text.boot) 
	} > CODE
	.text : {
		*(.text)
	} > CODE
	.rodata : {
		*(.rodata)
	} > ROM
	.data : {
		_sdata = .;
		 *(.data) 
		_edata = .;
	} > RAM
	. = ALIGN(0x8);
	
	.bss : {
		bss_begin = .;
		 *(.bss*)
		bss_end = .;
	} > RAM
	
	heap_begin = ORIGIN(RAM) + 1M;
	heap_end = ORIGIN(RAM) + 4M;
}