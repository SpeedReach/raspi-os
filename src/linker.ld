KernelStackSize = 50000;
KernelHeapSize = 150000;
SECTIONS
{
  . = 0x80000;
  
  .text : {
    _s_text = .;
    *(.text.boot)
    *(.text)
    _e_text = .;
  }

  .rodata : {
    _s_rodata = .;
    *(.rodata)
    _e_rodata = .;
  }

  .data : {
    _s_data = .;
    *(.data)
    _e_data = .;
  }  

  . = ALIGN(0x8);
  .bss : { 
    _s_bss = .;
    *(.bss)
    _e_bss = .;
  }
  
  _e_kernel = .;

  . = . + KernelStackSize;
  _s_stack = .;

  _e_heap = .;
  . = . + KernelHeapSize;
  _s_heap = .;
}