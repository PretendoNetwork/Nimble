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

void (*OSScreenInit)(void);
void (*OSScreenShutdown)(void);
uint32_t (*OSScreenGetBufferSizeEx)(uint32_t bufferNum);
int (*OSScreenSetBufferEx)(uint32_t bufferNum, void * addr);
int (*OSScreenClearBufferEx)(uint32_t bufferNum, uint32_t temp);
int (*OSScreenFlipBuffersEx)(uint32_t bufferNum);
int (*OSScreenPutFontEx)(uint32_t bufferNum, uint32_t posX, uint32_t posY, const char * buffer);
int (*OSScreenEnableEx)(uint32_t bufferNum, int enable);
int (*OSScreenPutPixelEx)(uint32_t bufferNum, uint32_t posX, uint32_t posY, uint32_t color);

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

void *__wrap_malloc(size_t size) {
    return memoryAlloc(size);
}

void __wrap_free(void *ptr) {
    memoryFree(ptr);
}

void *__wrap_calloc(size_t nmemb, size_t size) {
    void *ptr = __wrap_malloc(nmemb * size);
    if(ptr) {
        memset(ptr, 0, nmemb * size);
    }
    return ptr;
}

// http://www.beedub.com/Sprite093/src/lib/c/stdlib/realloc.c
void *__wrap_realloc(char *ptr, size_t newSize) {

    void *newPtr;

    if (ptr == 0) {
        return __wrap_malloc(newSize);
    }

    newPtr = __wrap_malloc(newSize);
    memcpy(newPtr, ptr, newSize);
    __wrap_free(ptr);

    return newPtr;
}

void *__wrap_aligned_alloc(size_t alignment, size_t size) {

    if(alignment < 4)
        alignment = 4;

    return memoryAllocAlign(size, alignment);
    
}

size_t __wrap_malloc_usable_size(void *p) {
    return 0x7FFFFFFF;
}

void *__wrap_memalign(size_t alignment, size_t size) {
    return __wrap_aligned_alloc(alignment, size);
}

int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size) {

    if(size == 0)
        *memptr = NULL;
    else
        *memptr = __wrap_aligned_alloc(alignment, size);

    return 0;
}

void *__wrap_pvalloc(size_t size) {
    return __wrap_aligned_alloc(0x1000, size);
}

void *__wrap_valloc(size_t size) {
    return __wrap_aligned_alloc(0x1000, size);
}

void LoadWiiUSymbols() {

    OSDynLoad_Acquire = (int (*)(const char *, uint32_t *))0x0102A3B4;
    OSDynLoad_FindExport = (int (*)(uint32_t, bool, const char *, void **))0x0102B828;

    uint32_t __rpl_coreinit, __rpl_nsysnet;
    OSDynLoad_Acquire("coreinit.rpl", &__rpl_coreinit);
    OSDynLoad_Acquire("nsysnet.rpl", &__rpl_nsysnet);

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

    LOAD_FUNC(__rpl_coreinit, OSScreenInit);
    LOAD_FUNC(__rpl_coreinit, OSScreenShutdown);
    LOAD_FUNC(__rpl_coreinit, OSScreenGetBufferSizeEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenSetBufferEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenClearBufferEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenFlipBuffersEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenPutFontEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenPutPixelEx);
    LOAD_FUNC(__rpl_coreinit, OSScreenEnableEx);

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

    socket_lib_init();

}

#define ROOTRPX_DBAT0U_VAL       0xC00003FF
#define COREINIT_DBAT0U_VAL      0xC20001FF
#define ROOTRPX_DBAT0L_VAL       0x30000012
#define COREINIT_DBAT0L_VAL      0x32000012

void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len)
{
    /*
     * Setup a DBAT access for our 0xC0800000 area and our 0xBC000000 area which hold our variables like GAME_LAUNCHED and our BSS/rodata section
     */
    register unsigned int dbatu0, dbatl0, target_dbat0u = 0, target_dbat0l = 0;
    // setup mapping based on target address
    if ((addr >= 0xC0000000) && (addr < 0xC2000000)) // root.rpx address
    {
        target_dbat0u = ROOTRPX_DBAT0U_VAL;
        target_dbat0l = ROOTRPX_DBAT0L_VAL;
    }
    else if ((addr >= 0xC2000000) && (addr < 0xC3000000))
    {
        target_dbat0u = COREINIT_DBAT0U_VAL;
        target_dbat0l = COREINIT_DBAT0L_VAL;
    }
    // save the original DBAT value
    asm volatile("mfdbatu %0, 0" : "=r" (dbatu0));
    asm volatile("mfdbatl %0, 0" : "=r" (dbatl0));
    asm volatile("mtdbatu 0, %0" : : "r" (target_dbat0u));
    asm volatile("mtdbatl 0, %0" : : "r" (target_dbat0l));
    asm volatile("eieio; isync");

    unsigned char *src_p = (unsigned char*)src;
    unsigned char *dst_p = (unsigned char*)addr;

    unsigned int i;
    for(i = 0; i < len; i++)
    {
        dst_p[i] = src_p[i];
    }

    unsigned int flushAddr = addr & ~31;

    while(flushAddr < (addr + len))
    {
        asm volatile("dcbf 0, %0; sync" : : "r"(flushAddr));
        flushAddr += 0x20;
    }

    /*
     * Restore original DBAT value
     */
    asm volatile("mtdbatu 0, %0" : : "r" (dbatu0));
    asm volatile("mtdbatl 0, %0" : : "r" (dbatl0));
    asm volatile("eieio; isync");

}