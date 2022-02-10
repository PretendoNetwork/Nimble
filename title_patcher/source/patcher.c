
#include "patcher.h"

uint32_t hookJumps[64][10];
PatchedFunction** pFuncList = NULL;
uint8_t* memcpy_buffer[0x400];

void InitializePatcher() {
    memset(hookJumps, 0, sizeof(hookJumps));
    memset(memcpy_buffer, 0, sizeof(memcpy_buffer));
    pFuncList = NULL;
}

void pygecko_memcpy(unsigned char* dst, unsigned char* src, unsigned int len) {
    if (len > 0x400)
        return;
    memcpy(memcpy_buffer, src, len);
    SC_KernelCopyData(OSEffectiveToPhysical(dst), &memcpy_buffer, len);
    DCFlushRange(dst, len);
    return;
}

void Write32(uint32_t* addr, uint32_t value) {
    pygecko_memcpy((unsigned char*)addr, (unsigned char*)&value, 4);
}

void MemoryCopy(void* dest, void* src, int size) {
    pygecko_memcpy((unsigned char*)dest, (unsigned char*)src, size);
}

uint32_t doBL(uint32_t dst, uint32_t src) {
    uint32_t newval = (dst - src);
    newval &= 0x03FFFFFC;
    newval |= 0x48000001;
    return newval;
}

uint32_t doBLA(uint32_t addr) {
    return (0x48000003 | (addr & 0x03FFFFFC));
}

PatchedFunction* hookBranch(uint32_t source, void* destination, uint32_t delta) {

    if (!pFuncList) {
        pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
    }

    FIND_ENTRY(pFuncList, 64, NULL);
    R_UNLESS(ret_idx != 64, NULL);
    pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

    pFuncList[ret_idx]->sourceAddr = (source + delta);
    pFuncList[ret_idx]->destAddr = (uint32_t)destination;
    pFuncList[ret_idx]->realFunctionPtr = NULL;
    memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)(source + delta), 10 * sizeof(uint32_t));

    if ((uint32_t)destination <= 0x03FFFFFC) {
        Write32((uint32_t*)(source + delta), doBLA((uint32_t)destination));
    } else {
        Write32((uint32_t*)(source + delta), doBL((uint32_t)destination, (source + delta)));
    }

    return pFuncList[ret_idx];
}

PatchedFunction* hookBranchNoLink(uint32_t source, void* destination, uint32_t delta) {

    if (!pFuncList) {
        pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
    }

    FIND_ENTRY(pFuncList, 64, NULL);
    R_UNLESS(ret_idx != 64, NULL);
    pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

    pFuncList[ret_idx]->sourceAddr = (source + delta);
    pFuncList[ret_idx]->destAddr = (uint32_t)destination;
    pFuncList[ret_idx]->realFunctionPtr = NULL;
    memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)(source + delta), 10 * sizeof(uint32_t));

    if ((uint32_t)destination <= 0x03FFFFFC) {
        Write32((uint32_t*)(source + delta), doBLA((uint32_t)destination) ^ 1);
    } else {
        Write32((uint32_t*)(source + delta), doBL((uint32_t)destination, (source + delta)) ^ 1);
    }

    return pFuncList[ret_idx];
}

PatchedFunction* hookFunction(void* source, void* destination, uint32_t delta, void** fptr) {

    if (!pFuncList) {
        pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
    }

    FIND_ENTRY(pFuncList, 64, NULL);
    R_UNLESS(ret_idx != 64, NULL);
    pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

    pFuncList[ret_idx]->sourceAddr = ((uint32_t)source + delta);
    pFuncList[ret_idx]->destAddr = (uint32_t)destination;
    memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)((uint32_t)source + delta), 10 * sizeof(uint32_t));

    delta += 4;
    hookJumps[ret_idx][0] = PowerPC_STW(gpr3, gpr1, -32);                   // stw r3, -32(r1)
    hookJumps[ret_idx][1] = PowerPC_LIS(gpr3, ((int)source + delta) >> 16); // lis r3, source@h
    hookJumps[ret_idx][2] = PowerPC_ORI(gpr3, gpr3, ((int)source + delta)); // ori r3, r3, source@l
    hookJumps[ret_idx][3] = PowerPC_MTSPR(gpr3, SPR_CTR);                   // mtctr r3
    hookJumps[ret_idx][4] = PowerPC_LWZ(gpr3, gpr1, -32);                   // lwz r3, -32(r1)
    hookJumps[ret_idx][5] = PowerPC_MFSPR(gpr13, SPR_LR); // mflr r13 (so you can read LR from the hook function)
    hookJumps[ret_idx][6] = pFuncList[ret_idx]->sourceInstructions[0]; // base func first instruction (because the
                                                                       // real one will be replaced with a BLA)
    hookJumps[ret_idx][7] = 0x4E800420;                                // bctr

    DCFlushRange(&hookJumps[ret_idx][0], 10 * 4);
    ICInvalidateRange(&hookJumps[ret_idx][0], 10 * 4);

    delta -= 4;
    Write32((uint32_t*)((int)source + delta), doBLA((uint32_t)destination) ^ 1);

    pFuncList[ret_idx]->realFunctionPtr = (void*)&hookJumps[ret_idx][0];
    *fptr = (void*)&hookJumps[ret_idx][0];

    return pFuncList[ret_idx];
}

void removePatch(PatchedFunction* ptr) {
    FIND_ENTRY(pFuncList, 64, ptr);
    R_UNLESS(ret_idx != 64, );
    MemoryCopy((uint32_t*)pFuncList[ret_idx]->sourceAddr, &pFuncList[ret_idx]->sourceInstructions,
               10 * sizeof(uint32_t));
    pFuncList[ret_idx] = NULL;
}