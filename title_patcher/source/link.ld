OUTPUT(title_patcher.elf);

/* Tell linker where our application entry is so the garbage collect can work correct */
ENTRY(__entry_menu);

SECTIONS {
    . = 0x00C42080;
    
    .text : {
        *(.text*);
    }

    .rodata : {
        *(.rodata*);
    }

    .data : {
        *(.data*);
        *(.sdata*);
        *(.sdata2*);
    }

    .bss : {
        *(.bss*);
        *(.sbss*);
        *(COMMON);
    }
    __CODE_END = .;

    /DISCARD/ : {
        *(*);
    }
}
