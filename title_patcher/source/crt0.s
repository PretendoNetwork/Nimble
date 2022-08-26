    .globl __entry_menu
__entry_menu:
    mflr %r3
    bl _main
    mtlr %r3
    lis %r11, 0x1006
    blr