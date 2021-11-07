#include "debugger.h"

const char *_END ="\033[0m";
const char *_BOLD= "\033[1m";
const char *_UNDER= "\033[4m";
const char *_REV="\033[7m";

const char *_GREY="\033[30m";
const char *_RED="\033[31m";
const char *_GREEN="\033[32m";
const char *_YELLOW="\033[33m";
const char *_BLUE="\033[34m";
const char *_PURPLE="\033[35m";
const char *_CYAN="\033[36m";
const char *_WHITE="\033[37m";

int __debug_udp_socket = -1;
sockaddr_in __debug_sock_addr;
OSMutex __debug_logger_mutex;

bool ExcDSI(OSContext *ctx)
{
    Exception_ExceptionHandler("DSI", ctx);
    return true;
}

bool ExcISI(OSContext *ctx)
{
    Exception_ExceptionHandler("ISI", ctx);
    return true;
}

bool ExcProgram(OSContext *ctx)
{
    Exception_ExceptionHandler("Program", ctx);
    return true;
}

void Debugger_Start()
{

    int broadcastEnable = 1;

    __debug_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(__debug_udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    memset(&__debug_sock_addr, 0, sizeof(sockaddr_in));
    __debug_sock_addr.sin_family = AF_INET;
    __debug_sock_addr.sin_port = 4405;
    __debug_sock_addr.sin_addr.s_addr = INADDR_BROADCAST;

    OSInitMutexEx(&__debug_logger_mutex, "Debug Log I/O Mutex");

    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_DSI, (void*)ExcDSI);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_ISI, (void*)ExcISI);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_PROGRAM, (void*)ExcProgram);

}

void Debugger_Log(const char *txt)
{

    OSLockMutex(&__debug_logger_mutex);

    // conveniently "stolen" from libutils lol
    int len = strlen(txt);
    int ret;
    while (len > 0)
    {
        int block = len < 1400 ? len : 1400;
        ret = sendto(__debug_udp_socket, txt, block, 0, &__debug_sock_addr, sizeof(sockaddr_in));
        if(ret < 0)
            break;

        len -= ret;
        txt += ret;
    }

    OSUnlockMutex(&__debug_logger_mutex);

}

void Debugger_Logf(const char *fmt, ...)
{

    va_list va;
    char *buf1 = (char*)malloc(0x800);
    char *buf2 = (char*)malloc(0x800);
    
    va_start(va, fmt);

    vsnprintf(buf1, 0x800, fmt, va);
    __os_snprintf(buf2, 0x800, "%s", buf1);
    
    Debugger_Log(buf2);
    free(buf1);
    free(buf2);

    va_end(va);

}

void Exception_ExceptionHandler(const char *type, OSContext *context)
{

    char *symbolname = (char*)malloc(129);

    bool valid = OSIsAddressValid(context->srr0);
    C_UNLESS(!valid, OSGetSymbolName(context->srr0, symbolname, 128));
    C_UNLESS(valid, memcpy(symbolname, "No symbol detected", 19));
    
    char *buffer = (char*)malloc(2049);
    __os_snprintf(buffer, 2048,	"Exception: %s - in %s\n"
                                "Title ID: %016llX | Core: %d | UPID: %d\n\n"
                                ""
                                "An error has occured.\n"
                                "Press the POWER button for 4 seconds to shutdown.\n\n"
                                ""
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
                                "r%-2d: 0x%08X | r%-2d: 0x%08X | LR : 0x%08X\n"
                                "PC : 0x%08X | DAR: 0x%08X | DSISR: 0x%08X\n",
                                type, symbolname,
                                OSGetTitleID(), OSGetCoreId(), OSGetUPID(),
                                0, context->gpr[0], 11, context->gpr[11], 22, context->gpr[22],
                                1, context->gpr[1], 12, context->gpr[12], 23, context->gpr[23],
                                2, context->gpr[2], 13, context->gpr[13], 24, context->gpr[24],
                                3, context->gpr[3], 14, context->gpr[14], 25, context->gpr[25],
                                4, context->gpr[4], 15, context->gpr[15], 26, context->gpr[26],
                                5, context->gpr[5], 16, context->gpr[16], 27, context->gpr[27],
                                6, context->gpr[6], 17, context->gpr[17], 28, context->gpr[28],
                                7, context->gpr[7], 18, context->gpr[18], 29, context->gpr[29],
                                8, context->gpr[8], 19, context->gpr[19], 30, context->gpr[30],
                                9, context->gpr[9], 20, context->gpr[20], 31, context->gpr[31],
                                10, context->gpr[10], 21, context->gpr[21], context->lr,
                                context->srr0, context->dar, context->dsisr);

    Debugger_Log(buffer);
    OSFatal(buffer);

}

int __wrap_printf(const char* fmt, ...) {

    va_list va;
    char *buf1 = (char*)malloc(0x800);
    
    va_start(va, fmt);
    int ret = vsnprintf(buf1, 0x800, fmt, va);

    Debugger_Log(buf1);
    free(buf1);

    va_end(va);
    return ret;

}

int __wrap_puts(const char* txt) {
    Debugger_Log(txt);
    Debugger_Log("\n");
    return strlen(txt);
}