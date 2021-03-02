#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern "C" void *memcpy(void *dest, const void * src, size_t n);
extern "C" void *memset(void *str, int c, size_t n);

extern int (*OSDynLoad_Acquire)(const char *dynLoadExeName, uint32_t *outHandle);
extern int (*OSDynLoad_FindExport)(uint32_t handle, bool isFromDataSection, const char * name, void **fpptr);

#define OSFatal ((void (*)(const char* msg))0x01031618)
#define __os_snprintf ((int(*)(char* s, int n, const char * format, ... ))0x0102F160)

#define INADDR_ANY      0
#define INADDR_BROADCAST 0xFFFFFFFF

#define AF_INET         2

#define SHUT_RD         0
#define SHUT_WR         1
#define SHUT_RDWR       2

#define SOCK_STREAM     1
#define SOCK_DGRAM      2

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define SOL_SOCKET      -1
#define SO_REUSEADDR    0x0004
#define SO_BROADCAST    0x0020
#define SO_NONBLOCK     0x1016
#define SO_MYADDR       0x1013
#define SO_RCVTIMEO     0x1006

namespace WiiU {
	namespace Symbols {
	
		/**
		* @brief  It will load our needed OS functions pointer with dynamic linking
		* @note   
		* @retval None
		*/
		void LoadWiiUSymbols();

	}
}

#define C_UNLESS(expr, code) \
	({ \
		if (!(expr)) { \
			code; \
		} \
	})

#define R_UNLESS(expr, res) \
	({ \
		if (!(expr)) { \
			return res; \
		} \
	})


#ifdef __cplusplus
extern "C" {
#endif

typedef struct in_addr {
	uint32_t s_addr;
} in_addr;

typedef struct sockaddr_in {
	int16_t sin_family;
	uint16_t sin_port;
	in_addr sin_addr;
	char sin_zero[8];
} sockaddr_in;

#define VPAD_BUTTON_A        0x8000
#define VPAD_BUTTON_B        0x4000
#define VPAD_BUTTON_X        0x2000
#define VPAD_BUTTON_Y        0x1000
#define VPAD_BUTTON_LEFT     0x0800
#define VPAD_BUTTON_RIGHT    0x0400
#define VPAD_BUTTON_UP       0x0200
#define VPAD_BUTTON_DOWN     0x0100
#define VPAD_BUTTON_ZL       0x0080
#define VPAD_BUTTON_ZR       0x0040
#define VPAD_BUTTON_L        0x0020
#define VPAD_BUTTON_R        0x0010
#define VPAD_BUTTON_PLUS     0x0008
#define VPAD_BUTTON_MINUS    0x0004
#define VPAD_BUTTON_HOME     0x0002
#define VPAD_BUTTON_SYNC     0x0001
#define VPAD_BUTTON_STICK_R  0x00020000
#define VPAD_BUTTON_STICK_L  0x00040000
#define VPAD_BUTTON_TV       0x00010000

#define VPAD_STICK_R_EMULATION_LEFT     0x04000000
#define VPAD_STICK_R_EMULATION_RIGHT    0x02000000
#define VPAD_STICK_R_EMULATION_UP       0x01000000
#define VPAD_STICK_R_EMULATION_DOWN     0x00800000

#define VPAD_STICK_L_EMULATION_LEFT     0x40000000
#define VPAD_STICK_L_EMULATION_RIGHT    0x20000000
#define VPAD_STICK_L_EMULATION_UP       0x10000000
#define VPAD_STICK_L_EMULATION_DOWN     0x08000000

//! Own definitions
#define VPAD_BUTTON_TOUCH               0x00080000
#define VPAD_MASK_EMULATED_STICKS       0x7F800000
#define VPAD_MASK_BUTTONS               ~VPAD_MASK_EMULATED_STICKS

typedef enum VPADTPResolution {
    VPAD_TP_1920x1080,
    VPAD_TP_1280x720,
    VPAD_TP_854x480
} VPADTPResolution;

typedef enum VPADGyroZeroDriftMode {
    VPAD_GYRO_ZERODRIFT_LOOSE,
    VPAD_GYRO_ZERODRIFT_STANDARD,
    VPAD_GYRO_ZERODRIFT_TIGHT
} VPADGyroZeroDriftMode;

typedef struct {
    float x,y;
} Vec2D;

typedef struct {
    float x,y,z;
} Vec3D;

typedef struct {
    Vec3D X,Y,Z;
} VPADDir;

typedef struct {
    uint16_t x, y;               /* Touch coordinates */
    uint16_t touched;            /* 1 = Touched, 0 = Not touched */
    uint16_t invalid;            /* 0 = All valid, 1 = X invalid, 2 = Y invalid, 3 = Both invalid? */
} VPADTPData;

typedef struct {
    int16_t offsetX;
    int16_t offsetY;
    float scaleX;
    float scaleY;
} VPADTPCalibrationParam;

typedef struct {
    uint32_t btns_h;                  /* Held buttons */
    uint32_t btns_d;                  /* Buttons that are pressed at that instant */
    uint32_t btns_r;                  /* Released buttons */
    Vec2D lstick, rstick;        /* Each contains 4-byte X and Y components */
    Vec3D acc;                   /* Status of DRC accelerometer */
    float acc_magnitude;           /* Accelerometer magnitude */
    float acc_variation;           /* Accelerometer variation */
    Vec2D acc_vertical;          /* Vertical */
    Vec3D gyro;                  /* Gyro data */
    Vec3D angle;                 /* Angle data */
    int8_t error;                    /* Error */
    VPADTPData tpdata;           /* Normal touchscreen data */
    VPADTPData tpdata1;          /* Modified touchscreen data 1 */
    VPADTPData tpdata2;          /* Modified touchscreen data 2 */
    VPADDir dir;                 /* Orientation in three-dimensional space */
    int headphone;               /* Set to TRUE if headphones are plugged in, FALSE otherwise */
    Vec3D mag;                   /* Magnetometer data */
    uint8_t volume;                   /* 0 to 255 */
    uint8_t battery;                  /* 0 to 6 */
    uint8_t mic;                      /* Microphone status */
    uint8_t unk_volume;               /* One less than volume */
    uint8_t paddings[7];
} VPADData;


#define OSTimerClockSpeed ((248625000) / 4)

#define OSSecondsToTicks(val)        ((uint64_t)(val) * (uint64_t)OSTimerClockSpeed)
#define OSMillisecondsToTicks(val)  (((uint64_t)(val) * (uint64_t)OSTimerClockSpeed) / 1000ull)
#define OSMicrosecondsToTicks(val)  (((uint64_t)(val) * (uint64_t)OSTimerClockSpeed) / 1000000ull)
#define OSNanosecondsToTicks(val)   (((uint64_t)(val) * ((uint64_t)OSTimerClockSpeed) / 31250ull) / 32000ull)

#define OSTicksToSeconds(val)        ((uint64_t)(val) / (uint64_t)OSTimerClockSpeed)
#define OSTicksToMilliseconds(val)  (((uint64_t)(val) * 1000ull) / (uint64_t)OSTimerClockSpeed)
#define OSTicksToMicroseconds(val)  (((uint64_t)(val) * 1000000ull) / (uint64_t)OSTimerClockSpeed)
#define OSTicksToNanoseconds(val)   (((uint64_t)(val) * 32000ull) / ((uint64_t)OSTimerClockSpeed / 31250ull))

#define OS_THREAD_ATTRIB_AFFINITY_CPU0 	1 << 0
#define OS_THREAD_ATTRIB_AFFINITY_CPU1 	1 << 1
#define OS_THREAD_ATTRIB_AFFINITY_CPU2 	1 << 2
#define OS_THREAD_ATTRIB_AFFINITY_ANY 	OS_THREAD_ATTRIB_AFFINITY_CPU0 | OS_THREAD_ATTRIB_AFFINITY_CPU1 | OS_THREAD_ATTRIB_AFFINITY_CPU2
#define OS_THREAD_ATTRIB_DETACHED 		1 << 3
#define OS_THREAD_ATTRIB_STACK_USAGE 	1 << 5

typedef enum OSExceptionMode
{
   OS_EXCEPTION_MODE_SYSTEM               = 0,
   OS_EXCEPTION_MODE_THREAD               = 1,
   OS_EXCEPTION_MODE_GLOBAL               = 2,
   OS_EXCEPTION_MODE_THREAD_ALL_CORES     = 3,
   OS_EXCEPTION_MODE_GLOBAL_ALL_CORES     = 4,
} OSExceptionMode;

typedef enum OSExceptionType
{
   OS_EXCEPTION_TYPE_SYSTEM_RESET         = 0,
   OS_EXCEPTION_TYPE_MACHINE_CHECK        = 1,
   OS_EXCEPTION_TYPE_DSI                  = 2,
   OS_EXCEPTION_TYPE_ISI                  = 3,
   OS_EXCEPTION_TYPE_EXTERNAL_INTERRUPT   = 4,
   OS_EXCEPTION_TYPE_ALIGNMENT            = 5,
   OS_EXCEPTION_TYPE_PROGRAM              = 6,
   OS_EXCEPTION_TYPE_FLOATING_POINT       = 7,
   OS_EXCEPTION_TYPE_DECREMENTER          = 8,
   OS_EXCEPTION_TYPE_SYSTEM_CALL          = 9,
   OS_EXCEPTION_TYPE_TRACE                = 10,
   OS_EXCEPTION_TYPE_PERFORMANCE_MONITOR  = 11,
   OS_EXCEPTION_TYPE_BREAKPOINT           = 12,
   OS_EXCEPTION_TYPE_SYSTEM_INTERRUPT     = 13,
   OS_EXCEPTION_TYPE_ICI                  = 14,
} OSExceptionType;

typedef struct OSContext {

	uint64_t tag;
	uint32_t gpr[32];
	uint32_t cr;
	uint32_t lr;
	uint32_t ctr;
	uint32_t xer;
	uint32_t srr0;
	uint32_t srr1;
	uint32_t dsisr;
	uint32_t dar;
	char a[12];
	uint32_t fpscr;
	double fpr[32];
	uint16_t spinLockCount;
	uint16_t state;
	uint32_t gqr[8];
	char b[4];
	double psf[32];
	uint64_t coretime[3];
	uint64_t starttime;
	uint32_t error;
	char c[4];
	uint32_t pmc1;
	uint32_t pmc2;
	uint32_t pmc3;
	uint32_t pmc4;
	uint32_t mmcr0;
	uint32_t mmcr1;

} OSContext;

typedef bool (*OSExceptionCallbackFn)(OSContext *context);

typedef struct OSThread {

    OSContext context;

    uint32_t txtTag;
    uint8_t state;
    uint8_t attr;

    short threadId;
    int suspend;
    int priority;

    char _[0x394 - 0x330 - 8];
    void *thread_next;
	void *thread_prev;

    void *stackBase;
    void *stackEnd;

    void * entryPoint;

    char _3A0[0x6A0 - 0x3A0];

} OSThread;

typedef struct OSThreadQueue {

	OSThread *head;
	OSThread *tail;
	void *parent;
	uint32_t unk;

} OSThreadQueue;

typedef struct OSMutex {

	uint32_t tag;
	const char *name;
	uint32_t unk;
	OSThreadQueue queue;
	OSThread *owner;
	int32_t count;
	void *next;
	void *prev;

} OSMutex;

typedef enum FSMode
{
   FS_MODE_READ_OWNER                   = 0x400,
   FS_MODE_WRITE_OWNER                  = 0x200,
   FS_MODE_EXEC_OWNER                   = 0x100,

   FS_MODE_READ_GROUP                   = 0x040,
   FS_MODE_WRITE_GROUP                  = 0x020,
   FS_MODE_EXEC_GROUP                   = 0x010,

   FS_MODE_READ_OTHER                   = 0x004,
   FS_MODE_WRITE_OTHER                  = 0x002,
   FS_MODE_EXEC_OTHER                   = 0x001,
} FSMode;

typedef struct FSClient {
	char unk[0x1700];
} FSClient;

typedef struct FSCmdBlock {
	char unk[0xA80];
} FSCmdBlock;

typedef struct  __attribute__((__packed__)) FSStat {
	uint32_t flags;
	FSMode mode;
	uint32_t owner;
	uint32_t group;
	uint32_t size;
	uint32_t allocSize;
	uint64_t quotaSize;
	uint32_t entryId;
	int64_t created;
	int64_t modified;
	char unk[0x30];
} FSStat;

typedef enum ProcUICallbackType {

	PROCUI_CALLBACK_ACQUIRE,
	PROCUI_CALLBACK_RELEASE,
	PROCUI_CALLBACK_EXIT,
	PROCUI_CALLBACK_NET_IO_START,
	PROCUI_CALLBACK_NET_IO_STOP,
	PROCUI_CALLBACK_HOME_BUTTON_DENIED,

} ProcUICallbackType;

typedef enum ProcUIStatus {

	PROCUI_STATUS_IN_FOREGROUND,
	PROCUI_STATUS_IN_BACKGROUND,
	PROCUI_STATUS_RELEASE_FOREGROUND,
	PROCUI_STATUS_EXITING,

} ProcUIStatus;

typedef struct FSDirEntry {
	FSStat stat;
	char name[256];
} FSDirEntry;



extern void* (*memoryAlloc)(size_t size);
extern void* (*memoryFree)(void *ptr);

extern void (*FSInit)(void);
extern int (*FSAddClient)(FSClient *c, int unk1);
extern int (*FSInitCmdBlock)(FSCmdBlock *block);
extern int (*FSOpenFile)(FSClient *client, FSCmdBlock *block, const char *path, const char *mode, int *handle, uint32_t flags);
extern int (*FSCloseFile)(FSClient *client, FSCmdBlock *block, int handle, uint32_t unk1);
extern int (*FSOpenDir)(FSClient *client, FSCmdBlock *block, const char *dirPath, int *dirHandle, uint32_t flags);
extern int (*FSReadDir)(FSClient *client, FSCmdBlock *block, int dirHandle, FSDirEntry *returnedDirEntry, int errHandling);
extern int (*FSCloseDir)(FSClient *client, FSCmdBlock *block, int dirHandle, int unk1);
extern int (*FSGetStat)(FSClient *client, FSCmdBlock *block, const char *path, FSStat *stat, uint32_t flags);
extern int (*FSGetStatFile)(FSClient *client, FSCmdBlock *block, int handle, FSStat *stat, uint32_t flags);
extern int (*FSWriteFile)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t size, size_t count, int handle, uint32_t unk1, uint32_t flags);
extern int (*FSReadFile)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t count, size_t size, int handle, uint32_t unk1, uint32_t flags);
extern int (*FSReadFileWithPos)(FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t count, size_t size, uint32_t pos, int handle, uint32_t unk1, uint32_t flags);
extern int (*FSSetPosFile)(FSClient *client, FSCmdBlock *block, int handle, uint32_t pos, uint32_t flags);
extern int (*FSGetMountSource)(FSClient *client, FSCmdBlock *cmd, int type /* 0 = sd */, char *out, uint32_t flags);
extern int (*FSMount)(FSClient *client, FSCmdBlock *cmd, char *source, const char *target, uint32_t bytes, uint32_t flags);
extern int (*FSGetPosFile)(FSClient *client, FSCmdBlock *block, int fileHandle, uint32_t *pos, uint32_t flags);

extern int (*IOS_Open)(const char *devNode, int mode);
extern int (*IOS_Close)(int handle);
extern int (*IOS_Ioctl)(int handle, uint32_t methodId, void *bufIn, size_t sizeIn, void *bufIo, size_t sizeIo);

extern bool (*OSCreateThread)(OSThread *thread, void *entry, int argc, char *argv, void *stack, uint32_t stackSize, int priority, uint16_t attributes);
extern bool (*OSJoinThread)(OSThread *thread, int *threadResult);
extern int (*OSResumeThread)(OSThread *thread);
extern OSThread *(*OSGetCurrentThread)(void);
extern void (*OSExitThread)(int result);
extern void (*OSSetThreadName)(OSThread *thread, const char *name);
extern void (*OSSleepTicks)(int64_t ticks);

extern void *(*__OSPhysicalToEffectiveCached)(uint32_t addr);
extern void *(*OSEffectiveToPhysical)(void *addr);
extern void (*DCInvalidateRange)(void *addr, uint32_t size);
extern void (*DCFlushRange)(void *addr, uint32_t size);
extern void (*DCStoreRange)(void *addr, uint32_t size);
extern void (*ICInvalidateRange)(void *addr, uint32_t size);
extern void (*OSMemoryBarrier)();

extern int (*socket_lib_init)(void);
extern int (*socket_lib_finish)(void);
extern int (*socket)(int domain, int type, int protocol);
extern int (*socketclose)(int s);
extern int (*connect)(int s, void *addr, int addrlen);
extern int (*sendto)(int s, const void *buffer, int size, int flags, const sockaddr_in *dest, int dest_len);
extern int (*setsockopt)(int s, int level, int optname, void *optval, int optlen);

extern void (*OSInitMutexEx)(OSMutex *mutex, const char *name);
extern bool (*OSLockMutex)(OSMutex *mutex);
extern bool (*OSUnlockMutex)(OSMutex *mutex);

extern bool (*OSIsAddressValid)(uint32_t addr);
extern bool (*OSGetSymbolName)(uint32_t addr, char *outSymbolName, int maxSz);
extern uint64_t (*OSGetTitleID)(void);
extern int (*OSGetCoreId)(void);
extern int (*OSGetUPID)(void);
extern bool (*OSSetExceptionCallbackEx)(OSExceptionMode mode, OSExceptionType exceptionType, void *callback);

extern bool (*SYSCheckTitleExists)(uint64_t titleId);
extern void (*SYSLaunchMenu)(void);
extern void (*SYSLaunchTitle)(uint64_t);

extern int (* VPADRead)(int chan, VPADData *buffer, size_t buffer_size, int *error);

extern int (*SAVEOpenFile)(FSClient *client, FSCmdBlock *block, uint8_t accountSlotNo, const char *path, const char *mode, int *fileHandle, int errHandling);

extern ProcUIStatus (*ProcUIProcessMessages)(bool block);

extern void (*GX2WaitForVsync)(void);

#ifdef __cplusplus
}
#endif

#define LOAD_FUNC(mod, x) ((__load_from_module(mod, # x, (void**)&x)))

#endif