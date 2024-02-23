ENTRY(_start)

SECTIONS
{
    _suser = .;
    .text : {
        _s_text = . ;
        *(.text)
        _e_text = . ;
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
    _e_user = .;
}
