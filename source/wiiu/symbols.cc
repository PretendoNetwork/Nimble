#include "symbols.h"

int (*OSDynLoad_Acquire)(const char *dynLoadExeName, uint32_t *outHandle);
int (*OSDynLoad_FindExport)(uint32_t handle, bool isFromDataSection, const char * name, void **fpptr);

void __load_from_module(uint32_t module, const char *symbol, void **f) {
	OSDynLoad_FindExport(module, false, symbol, (void**)f);
	C_UNLESS(*f, OSFatal(symbol));
}

void* (*memoryAllocAlign)(size_t align, size_t size) = (void *(*)(size_t, size_t))0x1048d10;
void* (*memoryAlloc)(size_t size) = (void *(*)(size_t))0x01048C64;
void* (*memoryFree)(void *ptr) = (void *(*)(void*))0x01048dbc;

void (*FSInit)(void);
int (*FSAddClient)(FSClient *c, int unk1);
int (*FSInitCmdBlock)(FSCmdBlock *blk);
int (*FSOpenFile)(FSClient *client, FSCmdBlock *block, const char *path, const char *mode, int *handle, uint32_t flags);
int (*FSCloseFile)(FSClient *client, FSCmdBlock *block, int handle, uint32_t unk1);
int (*FSOpenDir)(FSClient *client, FSCmdBlock *block, const char *dirPath, int *dirHandle, uint32_t flags);
int (*FSReadDir)(FSClient *client, FSCmdBlock *block, int dirHandle, FSDirEntry *returnedDirEntry, int errHandling);
int (*FSCloseDir)(FSClient *client, FSCmdBlock *block, int dirHandle, int unk1);
int (*FSGetStat)(FSClient *client, FSCmdBlock *block, const char *path, FSStat *stat, uint32_t flags);
int (*FSGetStatFile)(FSClient *client, FSCmdBlock *block, int handle, FSStat *stat, uint32_t flags);
int (*FSWriteFile)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t size, size_t count, int handle, uint32_t unk1, uint32_t flags);
int (*FSReadFile)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t count, size_t size, int handle, uint32_t unk1, uint32_t flags);
int (*FSReadFileWithPos)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t count, size_t size, uint32_t pos, int handle, uint32_t unk1, uint32_t flags);
int (*FSSetPosFile)(FSClient *client, FSCmdBlock *block, int handle, uint32_t pos, uint32_t flags);
int (*FSGetMountSource)(FSClient *client, FSCmdBlock *cmd, int type /* 0 = sd */, char *out, uint32_t flags);
int (*FSMount)(FSClient *client, FSCmdBlock *cmd, char *source, const char *target, uint32_t bytes, uint32_t flags);
int (*FSGetPosFile)(FSClient *client, FSCmdBlock *block, int fileHandle, uint32_t *pos, uint32_t flags);

int (*IOS_Open)(const char *devNode, int mode);
int (*IOS_Close)(int handle);
int (*IOS_Ioctl)(int handle, uint32_t methodId, void *bufIn, size_t sizeIn, void *bufIo, size_t sizeIo);

bool (*OSCreateThread)(OSThread *thread, void* entry, int argc, char *argv, void *stack, uint32_t stackSize, int priority, uint16_t attributes);
bool (*OSJoinThread)(OSThread *thread, int *threadResult);
int (*OSResumeThread)(OSThread *thread);
OSThread *(*OSGetCurrentThread)(void);
void (*OSExitThread)(int result);
void (*OSSetThreadName)(OSThread *thread, const char *name);
void (*OSSleepTicks)(int64_t ticks);

void *(*__OSPhysicalToEffectiveCached)(uint32_t addr);
void *(*OSEffectiveToPhysical)(void *addr);
void (*DCInvalidateRange)(void *addr, uint32_t size);
void (*DCFlushRange)(void *addr, uint32_t size);
void (*DCStoreRange)(void *addr, uint32_t size);
void (*ICInvalidateRange)(void *addr, uint32_t size);
void (*OSMemoryBarrier)();

int (*socket_lib_init)(void);
int (*socket_lib_finish)(void);
int (*socket)(int domain, int type, int protocol);
int (*socketclose)(int s);
int (*connect)(int s, void *addr, int addrlen);
int (*sendto)(int s, const void *buffer, int size, int flags, const sockaddr_in *dest, int dest_len);
int (*setsockopt)(int s, int level, int optname, void *optval, int optlen);

void (*OSInitMutexEx)(OSMutex *mutex, const char *name);
bool (*OSLockMutex)(OSMutex *mutex);
bool (*OSUnlockMutex)(OSMutex *mutex);

bool (*OSIsAddressValid)(uint32_t addr);
bool (*OSGetSymbolName)(uint32_t addr, char *outSymbolName, int maxSz);
uint64_t (*OSGetTitleID)(void);
int (*OSGetCoreId)(void);
int (*OSGetUPID)(void);
bool (*OSSetExceptionCallbackEx)(OSExceptionMode mode, OSExceptionType exceptionType, void *callback);

bool (*SYSCheckTitleExists)(uint64_t titleId);
void (*SYSLaunchMenu)(void);
void (*SYSLaunchTitle)(uint64_t);

int (* VPADRead)(int chan, VPADData *buffer, size_t buffer_size, int *error);

int (*SAVEOpenFile)(FSClient *client, FSCmdBlock *block, uint8_t accountSlotNo, const char *path, const char *mode, int *fileHandle, int errHandling);

ProcUIStatus (*ProcUIProcessMessages)(bool block);

void (*GX2WaitForVsync)(void);


/*
* https://www.gnu.org/software/libc/manual/html_node/Replacing-malloc.html
*/

extern "C" void *__wrap_malloc(size_t size) {
	return memoryAlloc(size);
}

extern "C" void __wrap_free(void *ptr) {
	memoryFree(ptr);
}

extern "C" void *__wrap_calloc(size_t nmemb, size_t size) {
	void *ptr = __wrap_malloc(nmemb * size);
	if(ptr) {
		memset(ptr, 0, nmemb * size);
	}
	return ptr;
}

// http://www.beedub.com/Sprite093/src/lib/c/stdlib/realloc.c
extern "C" void *__wrap_realloc(char *ptr, size_t newSize) {

	void *newPtr;

	if (ptr == 0) {
		return __wrap_malloc(newSize);
	}

	newPtr = __wrap_malloc(newSize);
	memcpy(newPtr, ptr, newSize);
	__wrap_free(ptr);

	return newPtr;
}

extern "C" void *__wrap_aligned_alloc(size_t alignment, size_t size) {

	if(alignment < 4)
		alignment = 4;

	return memoryAllocAlign(size, alignment);
	
}

extern "C" size_t __wrap_malloc_usable_size(void *p) {
	return 0x7FFFFFFF;
}

extern "C" void *__wrap_memalign(size_t alignment, size_t size) {
	return __wrap_aligned_alloc(alignment, size);
}

extern "C" int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size) {

	if(size == 0)
		*memptr = NULL;
	else
		*memptr = __wrap_aligned_alloc(alignment, size);

	return 0;
}

extern "C" void *__wrap_pvalloc(size_t size) {
	return __wrap_aligned_alloc(0x1000, size);
}

extern "C" void *__wrap_valloc(size_t size) {
	return __wrap_aligned_alloc(0x1000, size);
}

void WiiU::Symbols::LoadWiiUSymbols() {

	OSDynLoad_Acquire = (int (*)(const char *, uint32_t *))0x0102A3B4;
	OSDynLoad_FindExport = (int (*)(uint32_t, bool, const char *, void **))0x0102B828;

	uint32_t __rpl_coreinit, __rpl_nsysnet, __rpl_sysapp, __rpl_vpad, __rpl_nn_save, __rpl_proc_ui, __rpl_gx2;
	OSDynLoad_Acquire("coreinit.rpl", &__rpl_coreinit);
	OSDynLoad_Acquire("nsysnet.rpl", &__rpl_nsysnet);
	OSDynLoad_Acquire("sysapp.rpl", &__rpl_sysapp);
	OSDynLoad_Acquire("vpad.rpl", &__rpl_vpad);
	OSDynLoad_Acquire("nn_save.rpl", &__rpl_nn_save);
	OSDynLoad_Acquire("proc_ui.rpl", &__rpl_proc_ui);
	OSDynLoad_Acquire("gx2.rpl", &__rpl_gx2);

	LOAD_FUNC(__rpl_coreinit, FSInit);
	LOAD_FUNC(__rpl_coreinit, FSAddClient);
	LOAD_FUNC(__rpl_coreinit, FSInitCmdBlock);
	LOAD_FUNC(__rpl_coreinit, FSOpenFile);
	LOAD_FUNC(__rpl_coreinit, FSCloseFile);
	LOAD_FUNC(__rpl_coreinit, FSOpenDir);
	LOAD_FUNC(__rpl_coreinit, FSReadDir);
	LOAD_FUNC(__rpl_coreinit, FSCloseDir);
	LOAD_FUNC(__rpl_coreinit, FSGetStat);
	LOAD_FUNC(__rpl_coreinit, FSGetStatFile);
	LOAD_FUNC(__rpl_coreinit, FSWriteFile);
	LOAD_FUNC(__rpl_coreinit, FSReadFile);
	LOAD_FUNC(__rpl_coreinit, FSReadFileWithPos);
	LOAD_FUNC(__rpl_coreinit, FSSetPosFile);
	LOAD_FUNC(__rpl_coreinit, FSGetMountSource);
	LOAD_FUNC(__rpl_coreinit, FSMount);
	LOAD_FUNC(__rpl_coreinit, FSGetPosFile);

	LOAD_FUNC(__rpl_coreinit, IOS_Open);
	LOAD_FUNC(__rpl_coreinit, IOS_Close);
	LOAD_FUNC(__rpl_coreinit, IOS_Ioctl);

	LOAD_FUNC(__rpl_coreinit, OSCreateThread);
	LOAD_FUNC(__rpl_coreinit, OSJoinThread);
	LOAD_FUNC(__rpl_coreinit, OSResumeThread);
	LOAD_FUNC(__rpl_coreinit, OSGetCurrentThread);
	LOAD_FUNC(__rpl_coreinit, OSExitThread);
	LOAD_FUNC(__rpl_coreinit, OSSetThreadName);
	LOAD_FUNC(__rpl_coreinit, OSSleepTicks);

	LOAD_FUNC(__rpl_coreinit, __OSPhysicalToEffectiveCached);
	LOAD_FUNC(__rpl_coreinit, OSEffectiveToPhysical);
	LOAD_FUNC(__rpl_coreinit, DCInvalidateRange);
	LOAD_FUNC(__rpl_coreinit, DCFlushRange);
	LOAD_FUNC(__rpl_coreinit, DCStoreRange);
	LOAD_FUNC(__rpl_coreinit, ICInvalidateRange);
	LOAD_FUNC(__rpl_coreinit, OSMemoryBarrier);

	LOAD_FUNC(__rpl_nsysnet, socket_lib_init);
	LOAD_FUNC(__rpl_nsysnet, socket_lib_finish);
	LOAD_FUNC(__rpl_nsysnet, socket);
	LOAD_FUNC(__rpl_nsysnet, socketclose);
	LOAD_FUNC(__rpl_nsysnet, connect);
	LOAD_FUNC(__rpl_nsysnet, sendto);
	LOAD_FUNC(__rpl_nsysnet, setsockopt);

	LOAD_FUNC(__rpl_coreinit, OSInitMutexEx);
	LOAD_FUNC(__rpl_coreinit, OSLockMutex);
	LOAD_FUNC(__rpl_coreinit, OSUnlockMutex);

	LOAD_FUNC(__rpl_coreinit, OSSetExceptionCallbackEx);

	LOAD_FUNC(__rpl_coreinit, OSIsAddressValid);
	LOAD_FUNC(__rpl_coreinit, OSGetSymbolName);
	LOAD_FUNC(__rpl_coreinit, OSGetTitleID);
	LOAD_FUNC(__rpl_coreinit, OSGetCoreId);
	LOAD_FUNC(__rpl_coreinit, OSGetUPID);

	LOAD_FUNC(__rpl_sysapp, SYSCheckTitleExists);
	LOAD_FUNC(__rpl_sysapp, SYSLaunchMenu);
	LOAD_FUNC(__rpl_sysapp, SYSLaunchTitle);

	LOAD_FUNC(__rpl_vpad, VPADRead);

	LOAD_FUNC(__rpl_nn_save, SAVEOpenFile);

	LOAD_FUNC(__rpl_proc_ui, ProcUIProcessMessages);

	LOAD_FUNC(__rpl_gx2, GX2WaitForVsync);

	socket_lib_init();
}