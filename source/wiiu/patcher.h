#ifndef _GAME_UTILS_H
#define _GAME_UTILS_H

#include "../utils.h"
#include <vector>

namespace WiiU {
	namespace Patcher {

		typedef struct PatchedFunction {
			
			uint32_t sourceAddr;
			uint32_t sourceInstructions[10];
			uint32_t destAddr;
			void *realFunctionPtr;

		} PatchedFunction;

		/**
		 * @brief  This function writes to the virtual addr (even through mem protection)
		 * @note   
		 * @param  addr: A virtual address
		 * @param  value: A value to write
		 * @retval None
		 */
		void Write32(uint32_t *addr, uint32_t value);

		/**
		 * @brief  A basic memcpy but it goes through mem portection
		 * @note   
		 * @param  dest: Virtual destination address
		 * @param  src: Virtual source address
		 * @param  size: Size to copy from source to dest
		 * @retval None
		 */
		void MemoryCopy(void *dest, void *src, int size);

		/**
		 * @brief  Create a BL (Branch with Link) PowerPC 32-bit instruction
		 * @note   Limited to a 0x03FFFFFC distance between destination and source
		 * @param  dst: Destination of the branch
		 * @param  src: From where we are branching (instruction addr)
		 * @retval The BL instruction
		 */
		unsigned int doBL( unsigned int dst, unsigned int src );

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
		void removePatch(PatchedFunction *ptr);

		/**
		 * @brief  Hooks a function by writing a branch into the code
		 * @note   
		 * @param  source: Virtual source address
		 * @param  destination: Virtual destination address
		 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
		 * @retval PatchedFunction pointer
		 */
		PatchedFunction *hookBranch(uint32_t source, void *destination, uint32_t delta);

		/**
		 * @brief  Hooks a function by writing a branch with link into the code
		 * @note   
		 * @param  source: Virtual source address
		 * @param  destination: Virtual destination address
		 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
		 * @retval PatchedFunction pointer
		 */
		PatchedFunction *hookBranchNoLink(uint32_t source, void *destination, uint32_t delta);

		/**
		 * @brief  Hooks a function by writing a branch, while the real function can still be called
		 * @note   
		 * @param  source: Virtual source address
		 * @param  destination: Virtual destination address
		 * @param  delta: The value to be added at source (e.g, for MK8 it's 0x0C180000)
		 * @param  fptr: Out pointer for the (real) unpatched function
		 * @retval PatchedFunction pointer
		 */
		PatchedFunction *hookFunction(void *source, void *destination, uint32_t delta, void **fptr);

		/**
		 * @brief  Initalize data structures to store to function hooks, etc..
		 * @note   
		 * @retval None
		 */
		void Initialize();
	}
}

#define __SPR_LR	8
#define __SPR_CTR	9

#define _MFMSR(RS) ((31 << 26) | (RS << 21) | (83 << 1))
#define _MTMSR(RS) ((31 << 26) | (RS << 21) | (146 << 1))
#define _MTSPR(RS, SPR) ((31 << 26) | (RS << 21) | (SPR << 16) | (467 << 1))

#define _LWZ(RT, RA, D) ((32 << 26) | (RT << 21) | (RA << 16) | (D & 0xFFFF))
#define _LBZU(RT, RA, D) ((35 << 26) | (RT << 21) | (RA << 16) | (D & 0xFFFF))
#define _STW(RS, RA, D) ((36 << 26) | (RS << 21) | (RA << 16) | (D & 0xFFFF))
#define _STBU(RS, RA, D) ((39 << 26) | (RS << 21) | (RA << 16) | (D & 0xFFFF))
#define _LI(RS, VL) ((14 << 26) | (RS << 21) | (VL & 0xFFFF))
#define _LIS(RS, VL) ((15 << 26) | (RS << 21) | (VL & 0xFFFF))
#define _ANDC(RS, RA, RB) ((31 << 26) | (RS << 21) | (RA << 16) | (RB << 11) | (60 << 1))
#define _ADDI(RT, RA, SI) ((14 << 26) | (RT << 21) | (RA << 16) | (SI & 0xFFFF))
#define _BDNZ(D, AA, LK) ((16 << 26) | (16 << 21) | (((D << 16) >> 16) & 0xFFFF) | (AA << 1) | LK)
#define _ORI(RS, RA, UI) ((24 << 26) | (RS << 21) | (RA << 16) | (UI & 0xFFFF))

#define _BLR() ((19 << 26) | (20 << 21) | (16 << 1))
#define _BRANCH(D, AA, LK) ((18 << 26) | (D & 0x03FFFFFC) | (AA << 1) | LK)
#define _BL(D) ((18 << 26) | (D & 0x03FFFFFC) | (0 << 1) | 1)
#define _BLA(D) ((18 << 26) | (D & 0x03FFFFFC) | (1 << 1) | 1)
#define _BA(D) ((18 << 26) | (D & 0x03FFFFFC) | (1 << 1) | 0)


extern "C" void kern_write(void *addr, uint32_t value);
extern "C" void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len);

/**
 * @brief  Kernel copy data syscall 0x25 (custom syscall patched through kernel read/write primitives)
 * @note   All pointers should be kernel accessible
 * @param  addr: Physical destination addr
 * @param  src: Virtual destination addr
 * @param  len: Size
 * @retval None
 */
extern "C" void SC0x25_KernelCopyData(uint32_t addr, uint32_t src, size_t len);

#endif