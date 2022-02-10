    .global SC_KernelCopyData
SC_KernelCopyData:
    li %r0, 0x2500
    sc
    blr

    .global __OSGetTitleVersion
__OSGetTitleVersion:
    li %r0, 0x7000
    sc
    blr