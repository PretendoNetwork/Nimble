/*
 *   Copyright (C) 2022 GaryOderNichts
 *   Copyright (C) 2022 Pretendo Network contributors
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "syscalls.h"

#include <coreinit/cache.h>
#include <coreinit/memorymap.h>

#define KERN_SYSCALL_TBL_1          0xFFE84C70 // unknown
#define KERN_SYSCALL_TBL_2          0xFFE85070 // works with games
#define KERN_SYSCALL_TBL_3          0xFFE85470 // works with loader
#define KERN_SYSCALL_TBL_4          0xFFEAAA60 // works with home menu
#define KERN_SYSCALL_TBL_5          0xFFEAAE60 // works with browser (previously KERN_SYSCALL_TBL)

// see syscalls_asm.s
void SCKernelCopyData(uint32_t addr, uint32_t src, uint32_t len);
void SC_KernelCopyData(uint32_t addr, uint32_t src, uint32_t len);

void KernelWriteU32(uint32_t addr, uint32_t value)
{
    ICInvalidateRange(&value, 4);
    DCFlushRange(&value, 4);

    uint32_t dst = OSEffectiveToPhysical(addr);
    uint32_t src = OSEffectiveToPhysical((uint32_t) &value);

    SC_KernelCopyData(dst, src, 4);

    DCFlushRange((void *) addr, 4);
    ICInvalidateRange((void *) addr, 4);
}

/* Write a 32-bit word with kernel permissions */
void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value)
{
    asm volatile (
            "li 3,1\n"
            "li 4,0\n"
            "mr 5,%1\n"
            "li 6,0\n"
            "li 7,0\n"
            "lis 8,1\n"
            "mr 9,%0\n"
            "mr %1,1\n"
            "li 0,0x3500\n"
            "sc\n"
            "nop\n"
            "mr 1,%1\n"
            :
            :    "r"(addr), "r"(value)
            :    "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
    "11", "12"
            );
}

void KernelSetup()
{
    kern_write((void *) (KERN_SYSCALL_TBL_1 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_2 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_3 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_4 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_5 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
}
