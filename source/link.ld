OUTPUT(gamehacking.elf);

ENTRY(__entry);

SECTIONS {
    . = 0x00802000;
    .text : {
        *(.kernel_code*);
        *(.text*);
        KEEP(*(.kernel_code*));
    }
    .rodata : {
        *(.rodata*);
    }
    .data : {
        *(.data*);

        __sdata_start = .;
        *(.sdata*);
        __sdata_end = .;

        __sdata2_start = .;
        *(.sdata2*);
        __sdata2_end = .;
    }
    .bss : {
        __bss_start = .;
        *(.bss*);
        *(.sbss*);
        *(COMMON);
        __bss_end = .;
    }
    __CODE_END = .;

    /DISCARD/ : {
        *(*);
    }
}