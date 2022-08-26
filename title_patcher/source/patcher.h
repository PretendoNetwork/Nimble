#ifndef _GAME_UTILS_PT_H
#define _GAME_UTILS_PT_H

#include <malloc.h>

#include "symbols.h"
#include "ppc.h"

extern void SC_KernelCopyData(void* dst, void* src, size_t size);

typedef struct PatchedFunction {

    uint32_t sourceAddr;
    uint32_t sourceInstructions[10];
    uint32_t destAddr;
    void* realFunctionPtr;

} PatchedFunction;

/**
 * @brief  This function writes to the virtual addr (even through mem
 * protection)
 * @note
 * @param  addr: A virtual address
 * @param  value: A value to write
 * @retval None
 */
void Write32(uint32_t* addr, uint32_t value);

/**
 * @brief  A basic memcpy but it goes through mem portection
 * @note
 * @param  dest: Virtual destination address
 * @param  src: Virtual source address
 * @param  size: Size to copy from source to dest
 * @retval None
 */
void MemoryCopy(void* dest, void* src, int size);

/**
 * @brief  Create a BL (Branch with Link) PowerPC 32-bit instruction
 * @note   Limited to a 0x03FFFFFC distance between destination and source
 * @param  dst: Destination of the branch
 * @param  src: From where we are branching (instruction addr)
 * @retval The BL instruction
 */
unsigned int doBL(unsigned int dst, unsigned int src);

/**
 * @brief  Create a BLA (Branch Absolute with Link) PowerPC 32-bit instruction
 * @note   Limited to the 0-0x03FFFFFC range
 * @param  addr: The address to jump to
 * @retval The BLA instruction
 */
uint32_t doBLA(uint32_t addr);

/**
 * @brief  Remove a patch from our global patch array
 * @note
 * @param  ptr: PatchedFunction pointer returned by one of the hooking function
 * @retval None
 */
void removePatch(PatchedFunction* ptr);

/**
 * @brief  Hooks a function by writing a branch into the code
 * @note
 * @param  source: Virtual source address
 * @param  destination: Virtual destination address
 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
 * @retval PatchedFunction pointer
 */
PatchedFunction* hookBranch(uint32_t source, void* destination, uint32_t delta);

/**
 * @brief  Hooks a function by writing a branch with link into the code
 * @note
 * @param  source: Virtual source address
 * @param  destination: Virtual destination address
 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
 * @retval PatchedFunction pointer
 */
PatchedFunction* hookBranchNoLink(uint32_t source, void* destination, uint32_t delta);

/**
 * @brief  Hooks a function by writing a branch, while the real function can
 * still be called
 * @note
 * @param  source: Virtual source address
 * @param  destination: Virtual destination address
 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
 * @param  fptr: Out pointer for the (real) unpatched function
 * @retval PatchedFunction pointer
 */
PatchedFunction* hookFunction(void* source, void* destination, uint32_t delta, void** fptr);

/**
 * @brief  Initalize data structures to store to function hooks, etc..
 * @note
 * @retval None
 */
void InitializePatcher();

#define HOOK_BRANCH_LINK(source, dest) hookBranch((uint32_t)source, (void*)hook_##dest, 0);
#define HOOK_BRANCH(source, dest) hookBranchNoLink((uint32_t)source, (void*)hook_##dest, 0);
#define HOOK_FUNC(source, dest) hookFunction((void*)source, (void*)hook_##dest, 0, (void**)&real_##dest);

#define DECL_HOOK(res, name, ...)    \
    res (*real_##name)(__VA_ARGS__); \
    res hook_##name(__VA_ARGS__)

#define DECL_HOOK_BRANCH(res, name, ...) res hook_##name(__VA_ARGS__)

#endif