#ifndef NIMBLE_SYSCALLS_H
#define NIMBLE_SYSCALLS_H

#include <stdint.h>

void KernelSetup();
void KernelWriteU32(uint32_t addr, uint32_t value);
void kern_write(void *addr, uint32_t value);

#endif //NIMBLE_SYSCALLS_H
