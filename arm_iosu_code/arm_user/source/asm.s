.arm
.align 4

.global GetControlRegister
.type GetControlRegister, %function
GetControlRegister:
    MRC p15, 0, r0, c1, c0, 0
    bx lr

.global SetControlRegister
.type SetControlRegister, %function
SetControlRegister:
    MCR p15, 0, r0, c1, c0, 0
    bx lr
    
.global ClearEntireDCache
.type ClearEntireDCache, %function
ClearEntireDCache:
    MRC p15, 0, r15, c7, c10, 3
    bne ClearEntireDCache
    mov r0, #0
    MCR p15, 0, r0, c7, c10, 4
    bx lr

.global GetCPSR
.type GetCPSR, %function
GetCPSR:
    MRS r0, cpsr
    bx lr

.global SetCPSR
.type SetCPSR, %function
SetCPSR:
    MSR cpsr, r0
    bx lr

