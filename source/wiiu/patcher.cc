#include "patcher.h"

uint32_t hookJumps[64][10];
WiiU::Patcher::PatchedFunction **pFuncList = NULL;
uint8_t *memcpy_buffer[0x400];

void WiiU::Patcher::Initialize()
{
	memset(hookJumps, 0, sizeof(hookJumps));
	memset(memcpy_buffer, 0, sizeof(memcpy_buffer));
	pFuncList = NULL;
}

void pygecko_memcpy(unsigned char *dst, unsigned char *src, unsigned int len)
{
	if(len>0x400) return;
	memcpy(memcpy_buffer, src, len);
	SC0x25_KernelCopyData((unsigned int)OSEffectiveToPhysical(dst), (unsigned int)&memcpy_buffer, len);
	DCFlushRange(dst, len);
	return;
}

void WiiU::Patcher::Write32(uint32_t *addr, uint32_t value)
{
	pygecko_memcpy((unsigned char *)addr, (unsigned char *)&value, 4);
}

void WiiU::Patcher::MemoryCopy(void *dest, void *src, int size)
{
	pygecko_memcpy((unsigned char *)dest, (unsigned char *)src, size);
}

uint32_t WiiU::Patcher::doBL(uint32_t dst, uint32_t src)
{
	uint32_t newval = (dst - src);
	newval &= 0x03FFFFFC; 
	newval |= 0x48000001;
	return newval;
}

uint32_t WiiU::Patcher::doBLA(uint32_t addr)
{
	return (0x48000003 | (addr & 0x03FFFFFC));
}

WiiU::Patcher::PatchedFunction *WiiU::Patcher::hookBranch(uint32_t source, void *destination, uint32_t delta) 
{
	
	if(!pFuncList) {
		pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
	}

	FIND_ENTRY(pFuncList, 64, NULL);
	R_UNLESS(ret_idx != 64, NULL);
	pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

	pFuncList[ret_idx]->sourceAddr = (source + delta);
	pFuncList[ret_idx]->destAddr = (uint32_t)destination;
	pFuncList[ret_idx]->realFunctionPtr = NULL;
	memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)(source + delta), 10 * sizeof(uint32_t));

	if((uint32_t)destination <= 0x03FFFFFC) {
		Write32((uint32_t*)(source + delta), doBLA((uint32_t)destination));
	} else {
		Write32((uint32_t*)(source + delta), doBL((uint32_t)destination, (source + delta)));
	}

	return pFuncList[ret_idx];

}

WiiU::Patcher::PatchedFunction *WiiU::Patcher::hookBranchNoLink(uint32_t source, void *destination, uint32_t delta)
{
	
	if(!pFuncList) {
		pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
	}

	FIND_ENTRY(pFuncList, 64, NULL);
	R_UNLESS(ret_idx != 64, NULL);
	pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

	pFuncList[ret_idx]->sourceAddr = (source + delta);
	pFuncList[ret_idx]->destAddr = (uint32_t)destination;
	pFuncList[ret_idx]->realFunctionPtr = NULL;
	memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)(source + delta), 10 * sizeof(uint32_t));

	if((uint32_t)destination <= 0x03FFFFFC) {
		Write32((uint32_t*)(source + delta), doBLA((uint32_t)destination) ^ 1);
	} else {
		Write32((uint32_t*)(source + delta), doBL((uint32_t)destination, (source + delta)) ^ 1);
	}

	return pFuncList[ret_idx];

}

WiiU::Patcher::PatchedFunction *WiiU::Patcher::hookFunction(void *source, void *destination, uint32_t delta, void **fptr)
{
	
	if(!pFuncList) {
		pFuncList = (PatchedFunction**)calloc(64, sizeof(PatchedFunction*));
	}

	FIND_ENTRY(pFuncList, 64, NULL);
	R_UNLESS(ret_idx != 64, NULL);
	pFuncList[ret_idx] = (PatchedFunction*)calloc(1, sizeof(PatchedFunction));

	pFuncList[ret_idx]->sourceAddr = ((uint32_t)source + delta);
	pFuncList[ret_idx]->destAddr = (uint32_t)destination;
	memcpy(&pFuncList[ret_idx]->sourceInstructions, (void*)((uint32_t)source + delta), 10 * sizeof(uint32_t));

	delta += 4;
	hookJumps[ret_idx][0] = _STW(3, 1, (int16_t)-32); 							// stw r3, -32(r1)
	hookJumps[ret_idx][1] = _LIS(3, ((uint32_t)((int)source + delta) >> 16)); 	// lis r3, source@h
	hookJumps[ret_idx][2] = _ORI(3, 3, (uint32_t)((int)source + delta));		// ori r3, r3, source@l
	hookJumps[ret_idx][3] = _MTSPR(3, __SPR_CTR);								// mtctr r3
	hookJumps[ret_idx][4] = _LWZ(3, 1, (int16_t)-32);							// lwz r3, -32(r1)
	hookJumps[ret_idx][5] = pFuncList[ret_idx]->sourceInstructions[0];			// base func first instruction (because the real one will be replaced with a BLA)
	hookJumps[ret_idx][6] = 0x4E800420;											// bctr

	DCFlushRange(&hookJumps[ret_idx][0], 10*4);
	ICInvalidateRange(&hookJumps[ret_idx][0], 10*4);

	delta -= 4;
	Write32((uint32_t*)((int)source + delta), doBLA((uint32_t)destination) ^ 1);

	pFuncList[ret_idx]->realFunctionPtr = (void*)&hookJumps[ret_idx][0];
	*fptr = (void*)&hookJumps[ret_idx][0];

	return pFuncList[ret_idx];	
}

void WiiU::Patcher::removePatch(PatchedFunction *ptr)
{
	FIND_ENTRY(pFuncList, 64, ptr);
	R_UNLESS(ret_idx != 64,);
	MemoryCopy((uint32_t*)pFuncList[ret_idx]->sourceAddr, &pFuncList[ret_idx]->sourceInstructions, 10 * sizeof(uint32_t));
	pFuncList[ret_idx] = NULL;

}

void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len)
{
    /*
     * Setup a DBAT access with cache inhibited to write through and read directly from memory
     */
    unsigned int dbatu0, dbatl0, dbatu1, dbatl1;
    // save the original DBAT value
    asm volatile("mfdbatu %0, 0" : "=r" (dbatu0));
    asm volatile("mfdbatl %0, 0" : "=r" (dbatl0));
    asm volatile("mfdbatu %0, 1" : "=r" (dbatu1));
    asm volatile("mfdbatl %0, 1" : "=r" (dbatl1));

    unsigned int target_dbatu0 = 0;
    unsigned int target_dbatl0 = 0;
    unsigned int target_dbatu1 = 0;
    unsigned int target_dbatl1 = 0;

    unsigned char *dst_p = (unsigned char*)addr;
    unsigned char *src_p = (unsigned char*)src;

    // we only need DBAT modification for addresses out of our own DBAT range
    // as our own DBAT is available everywhere for user and supervisor
    // since our own DBAT is on DBAT5 position we don't collide here
    if(addr < 0x00800000 || addr >= 0x01000000)
    {
        target_dbatu0 = (addr & 0x00F00000) | 0xC0000000 | 0x1F;
        target_dbatl0 = (addr & 0xFFF00000) | 0x32;
        asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
        asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
        dst_p = (unsigned char*)((addr & 0xFFFFFF) | 0xC0000000);
    }
    if(src < 0x00800000 || src >= 0x01000000)
    {
        target_dbatu1 = (src & 0x00F00000) | 0xB0000000 | 0x1F;
        target_dbatl1 = (src & 0xFFF00000) | 0x32;

        asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
        asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
        src_p = (unsigned char*)((src & 0xFFFFFF) | 0xB0000000);
    }

    asm volatile("eieio; isync");

    unsigned int i;
    for(i = 0; i < len; i++)
    {
        // if we are on the edge to next chunk
        if((target_dbatu0 != 0) && (((unsigned int)dst_p & 0x00F00000) != (target_dbatu0 & 0x00F00000)))
        {
            target_dbatu0 = ((addr + i) & 0x00F00000) | 0xC0000000 | 0x1F;
            target_dbatl0 = ((addr + i) & 0xFFF00000) | 0x32;
            dst_p = (unsigned char*)(((addr + i) & 0xFFFFFF) | 0xC0000000);

            asm volatile("eieio; isync");
            asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
            asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
            asm volatile("eieio; isync");
        }
        if((target_dbatu1 != 0) && (((unsigned int)src_p & 0x00F00000) != (target_dbatu1 & 0x00F00000)))
        {
            target_dbatu1 = ((src + i) & 0x00F00000) | 0xB0000000 | 0x1F;
            target_dbatl1 = ((src + i) & 0xFFF00000) | 0x32;
            src_p = (unsigned char*)(((src + i) & 0xFFFFFF) | 0xB0000000);

            asm volatile("eieio; isync");
            asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
            asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
            asm volatile("eieio; isync");
        }

        *dst_p = *src_p;

        ++dst_p;
        ++src_p;
    }

    /*
     * Restore original DBAT value
     */
    asm volatile("eieio; isync");
    asm volatile("mtdbatu 0, %0" : : "r" (dbatu0));
    asm volatile("mtdbatl 0, %0" : : "r" (dbatl0));
    asm volatile("mtdbatu 1, %0" : : "r" (dbatu1));
    asm volatile("mtdbatl 1, %0" : : "r" (dbatl1));
    asm volatile("eieio; isync");
}