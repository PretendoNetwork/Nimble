#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "patches.h"

extern uint32_t GetControlRegister(void);
extern void SetControlRegister(uint32_t newControlRegister);
extern uint32_t GetCPSR(void);
extern void SetCPSR(uint32_t newCPSR);
extern void ClearEntireDCache(void);

#define CPSR_IRQ_BIT	(1 << 7)
#define CPSR_FIQ_BIT	(1 << 6)

#define CR_MMU_BIT 		(1 << 0)
#define CR_DCACHE_BIT	(1 << 2)
#define CR_ICACHE_BIT	(1 << 12)

typedef enum IOSU_ProcessID {
	IOS_KERNEL = 0,
	IOS_MCP = 1,
	IOS_BSP = 2,
	IOS_CRYPTO = 3,
	IOS_USB = 4,
	IOS_FS = 5,
	IOS_PAD = 6,
	IOS_NET = 7,
	IOS_ACP = 8,
	IOS_NSEC = 9,
	IOS_AUXIL = 10,
	IOS_NIM_BOSS = 11,
	IOS_FPD = 12,	
	IOS_TEST = 13,
	COS_KERNEL = 14,
	COS_ROOT = 15,
	COS_02 = 16,
	COS_03 = 17,
	COS_OVERLAY = 18,
	COS_HBM = 19,
	COS_ERROR = 20,
	COS_MASTER = 21,
} IOSU_ProcessID;

typedef enum IOSU_PermGroup {
	BSP = 1,
	DK = 3,
	USB_CLS_DRV = 9,
	UHS = 12,
	FS = 11,
	MCP = 13,
	NIM = 14,
	ACT = 15,
	FPD = 16,
	BOSS = 17,
	ACP = 18,
	PDM = 29,
	AC = 20,
	NDM = 21,
	NSEC = 22,
} IOSU_PermGroup;

typedef struct ios_thread_t {
	unsigned int cpsr;
	unsigned int gpr[14];
	unsigned int lr;
	unsigned int pc;
	void *threadQueueNext;
	int maxPriority;
	int priority;
	int state;
	int pid;
	int id;
	unsigned int flags;
	int exitValue;
	void *joinQueue;
	void *threadQueue;
	char unk1[56];
	void *stackPointer;
	char unk2[8];
	void *sysStackAddr;
	void *userStackAddr;
	unsigned int userStackSize;
	void *threadLocalStorage;
	unsigned int profileCount;
	unsigned int profileTime;
} ios_thread_t;

typedef struct __attribute__((packed)) CRYPTO_KeyHandle
{

	uint8_t valid;
	uint8_t desc;
	int16_t index;
	uint32_t permissions;
	uint32_t unk;
	uint8_t unk_[4];

} CRYPTO_KeyHandle; //.crypto.bss:0402bb2c - total size 0x800 bytes

typedef struct __attribute__((packed)) CRYPTO_KeyData
{
    uint8_t valid;
    uint8_t data[32];
    uint8_t pad;
    uint16_t next;
} CRYPTO_KeyData; //crypto.bss:0402972c - total size 0x2400

/* just static data @ 0x101001DC - 0x68 (from 5.5.X fw.img, same address) */
static const char usbStackPivotDamageFix[] = {
	0xE5,0x8D,0xE0,0x04, 0xE5,0x8D,0xC0,0x08, 0xE5,0x8D,0x40,0x0C, 0xE5,0x8D,0x60,0x10,
	0xEB,0x00,0xB2,0xFD, 0xEA,0xFF,0xFF,0xC9, 0x10,0x14,0x03,0xF8, 0x10,0x62,0x4D,0xD3,
	0x10,0x14,0x50,0x00, 0x10,0x14,0x50,0x20, 0x10,0x14,0x00,0x00, 0x10,0x14,0x00,0x90,
	0x10,0x14,0x00,0x70, 0x10,0x14,0x00,0x98, 0x10,0x14,0x00,0x84, 0x10,0x14,0x03,0xE8,
	0x10,0x14,0x00,0x3C, 0x00,0x00,0x01,0x73, 0x00,0x00,0x01,0x76, 0xE9,0x2D,0x4F,0xF0,
	0xE2,0x4D,0xDE,0x17, 0xEB,0x00,0xB9,0x92, 0xE3,0xA0,0x10,0x00, 0xE3,0xA0,0x20,0x03,
	0xE5,0x9F,0x0E,0x68, 0xEB,0x00,0xB3,0x20,
};

/* just static data @ 0x08120000 (from 5.5.X fw.img, same address) */
static const char kernelDamageFixData[] = {
	0xe9,0x2d,0x40,0x30, 0xe1,0xa0,0x40,0x00, 0xeb,0x00,0x39,0xda, 0xe5,0x9f,0x30,0x1c,
	0xe1,0xa0,0x50,0x00, 0xe1,0xa0,0x00,0x04, 0xe1,0x2f,0xff,0x33, 0xe1,0xa0,0x40,0x00,
	0xe1,0xa0,0x00,0x05, 0xeb,0x00,0x39,0xd8, 0xe1,0xa0,0x00,0x04, 0xe8,0xbd,0x80,0x30,
	0xff,0xff,0xe3,0xec, 0xe9,0x2d,0x40,0x30
};

size_t strlen_(const char *str) {
    const char *s;
    for (s = str; *s; ++s);
    return (s - str);
}

void memcpy_(void *where, const void *from, uint32_t size) {

	uint8_t *where_ = (uint8_t*)where;
	uint8_t *from_ = (uint8_t*)from;

	for (uint32_t i = 0; i < size; i++)
		where_[i] = from_[i];

}

int _main() {

	void(*invalidate_icache)() = (void(*)())0x0812DCF0;
	void(*invalidate_dcache)(unsigned int, unsigned int) = (void(*)())0x08120164;
	int (*read_otp_internal)(int index, void* out_buf, uint32_t size) = (int (*)(int, void*, uint32_t ))0x08120248;

	/* Dump the OTP */
	read_otp_internal(0, (void*)0x01FFF000, 0x400);


	uint32_t savedControlRegister = GetControlRegister();
	uint32_t newControlRegister = savedControlRegister;

	uint32_t savedCPSR = GetCPSR();
	uint32_t newCPSR = savedCPSR;

	ClearEntireDCache();

	/* Mask IRQ, FIQ and disable MMU, ICache, DCache */
	newCPSR |= CPSR_IRQ_BIT;
	newCPSR |= CPSR_FIQ_BIT;
	SetCPSR(newCPSR);

	newControlRegister &= ~CR_MMU_BIT;
	newControlRegister &= ~CR_DCACHE_BIT;
	newControlRegister &= ~CR_ICACHE_BIT;
	SetControlRegister(newControlRegister);

	/* Patch kernel panic handler */
	*(uint32_t*)0x08129A24 = 0xE12FFF1E;

	/* Fix syscall 0x4F (the one we patched to our jump func ) */
	memcpy_((void*)0x08120000, kernelDamageFixData, 0x38);

	/* https://wiiubrew.org/wiki/IOSU#IPC */
	/* Write the IPC request struct ptr in MEM1, so we can reply (svc 0x49) later on. */
	*(uint32_t**)0x0012F000 = *(uint32_t**)0x1016AD18; // Always in the 0x08148xxx range

	/* Fix damage done by the stack pivot gagdet "construction" */
	memcpy_((void*)0x10100174, usbStackPivotDamageFix, 0x68);

	/* copy the code to memory, the userland ROP chain will jump to it */
	memcpy_((void*)0x101312D0, (void*)0x00148004, *(uint32_t*)0x00148000);

	/* No SSL patch */
	/* 5.5.5 */
	if(*(uint32_t*)0xE1019F78 == 0xE1A00004) {
		*(uint32_t*)(0xE1019F78 - 0xE1000000 + 0x12BC0000) = 0xE3A00001;
	} else {
		*(uint32_t*)(0xE1019E84 - 0xE1000000 + 0x12BC0000) = 0xE3A00001;
	}

	/* URL patch */
	for(int i = 0; i < sizeof(url_patches)/sizeof(URL_Patch); i++) {
		memcpy_((void*)(url_patches[i].address),
				(void*)&url_patches[i].url,
				strlen_((const char*)&url_patches[i].url) + 1
				);
	}

	/* Reset CR, and CPSR back to what they were */
	SetControlRegister(savedControlRegister);
	invalidate_dcache(0x081298BC, 0x4001);
	invalidate_icache();
	SetCPSR(savedCPSR);

	int (*internal_setClientCapabilites)(IOSU_ProcessID processId, IOSU_PermGroup featureId, uint64_t mask) = (int(*)(IOSU_ProcessID, IOSU_PermGroup, uint64_t))0x081260a8;
	internal_setClientCapabilites(COS_MASTER, FS, 0xFFFFFFFFFFFFFFFF);
	internal_setClientCapabilites(COS_MASTER, MCP, 0xFFFFFFFFFFFFFFFF);
	internal_setClientCapabilites(COS_MASTER, ACT, 0xFFFFFFFFFFFFFFFF);
	internal_setClientCapabilites(COS_MASTER, FPD, 0xFFFFFFFFFFFFFFFF);
	internal_setClientCapabilites(COS_MASTER, NSEC, 0xFFFFFFFFFFFFFFFF);

	return 0;
}
