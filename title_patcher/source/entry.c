#include <string.h>
#include <stdint.h>
#include "symbols.h"
#include "debugger.h"

/* Include per-title headers here */

#include "titles/Wii_U_Menu.h"

/* ****************************** */

typedef struct TitlePatch
{
    uint64_t m_TitleIDs[3];
    uint32_t m_Version; //! Currently ignored, to be used later
    void (*m_PatchFunction)(uint32_t titleVer, uint64_t titleId);
} TitlePatch;

static TitlePatch titlePatches[] = {
    {.m_TitleIDs = {WIIU_MENU_TID}, .m_Version = 0, .m_PatchFunction = Patch_Wii_U_Menu},
};

static size_t numTitlePatches = sizeof(titlePatches)/sizeof(TitlePatch);

/* ****************************** */

extern void SC_KernelCopyData(void* dst, void* src, size_t size);
extern void kern_write(void *addr, uint32_t value);
extern uint32_t __OSGetTitleVersion();

void set_syscalls(int id, uint32_t func) {
    kern_write((void*)(0xFFE84C70 + (id * 4)), func);
    kern_write((void*)(0xFFE85070 + (id * 4)), func);
    kern_write((void*)(0xFFEAAA60 + (id * 4)), func);
    kern_write((void*)(0xFFE85470 + (id * 4)), func);
    kern_write((void*)(0xFFEAAE60 + (id * 4)), func);
}

/* !!!! Make sure the new URL legnth is equal or less than the original length !!!! */
char original_url[] = "discovery.olv.nintendo.net/v1/endpoint";
char new_url[] =      "discovery.olv.pretendo.cc/v1/endpoint";

int _main(uint32_t ret_addr) {

    /* 

        Internet Browser calls sub_22A71F8 on entry, this piece of code will crash the process:


        // MEMGetBaseHeapHandle(1) == MEM2
        if (MEMGetAllocatableSizeForExpHeapEx(MEMGetBaseHeapHandle(1), 4) < 0x4B390C ) {
            PanicCrash();
        }
        

        Loading nn_olv.rpl inside the Internet Browser takes too much RAM and crashes the process because of this call
        So we directly return and do nothing for the internet browser
    
    */

    LoadWiiUSymbols();

    uint64_t titleId = OSGetTitleID();
    if(titleId == 0x000500301001220a
        || titleId == 0x000500301001210a
        || titleId == 0x000500301001200a)
    {
        return ret_addr;
    }

    Debugger_Start();

    if(titleId == 0x000500101004a000
        || titleId == 0x000500101004a100
        || titleId == 0x000500101004a200)
    {
        int (*_SYSGetMiiStudioArgs)(int* r3);
        uint32_t __rpl_sysapp;
        OSDynLoad_Acquire("sysapp", &__rpl_sysapp);
        OSDynLoad_FindExport(__rpl_sysapp, false, "_SYSGetMiiStudioArgs", (void**)&_SYSGetMiiStudioArgs);

        int args[10];
        memset(args, 0, sizeof(args));
        _SYSGetMiiStudioArgs(args);

        /* We're in Mii Maker to choose/create a Mii, do not start sd_loader, do the coreinit entry by ourselves */
        if(args[4] == 2) {

            uint32_t rpxEntry = *(uint32_t*)0x1005E040;
            int rpxArgc = *(int*)0x10013C30;
            char **rpxArgv = *(char***)0x10013C34;

            int (*realGameEntry)(int argc, char **argv) = (int (*)(int argc, char **argv))rpxEntry;
            void (*CafeOS_Exit)(int exitCode) = (void (*)(int))0x0101cdc4; 
    
            CafeOS_Exit(realGameEntry(rpxArgc, rpxArgv));
    
        }
    }

    printf("\ntitle_patcher: 0x%016llx\n", titleId);

    /* When syscalls will be required for code patching, PLEASE, backup old syscall values then reset them at the end */

    printf("Applying global Miiverse patches\n");

    uint32_t __rpl_nn_olv;
    OSDynLoad_Acquire("nn_olv.rpl", &__rpl_nn_olv);

    /* https://github.com/PretendoNetwork/Inkay/blob/main/src/main.cpp */
    for (uint32_t addr = 0x10000000; addr < 0x20000000; addr += 4) {
        if (!memcmp(original_url, (void*)addr, sizeof(original_url))) {
            printf("Found discovery string at 0x%08X, replacing ...\n", addr);
            memcpy((void*)addr, new_url, sizeof(new_url)); // sizeof(new_url) includes the NULL byte terminator
            break;
        }
    }

    printf("Applying per game patches\n");

    for(size_t i = 0; i < numTitlePatches; i++) {
        for(int j = 0; j < 3; j++) {
            if(titlePatches[i].m_TitleIDs[j] == titleId) {
                printf("Applying game-specific patch to 0x%016llX ...\n", titleId);
                titlePatches[i].m_PatchFunction(__OSGetTitleVersion(), titleId);
                goto end;
            }
        }
    }

    printf("Exiting title_patcher\n");

    socketclose(__debug_udp_socket);
    socket_lib_finish();

end:
    return ret_addr;

}
