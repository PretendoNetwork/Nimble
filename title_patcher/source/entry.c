#include "debugger.h"
#include "symbols.h"
#include <stdint.h>
#include <string.h>

/* Include per-title headers here */

#include "titles/Wii_U_Menu.h"

/* ****************************** */

typedef struct TitlePatch {
    uint64_t m_TitleIDs[3];
    uint32_t m_Version; //! Currently ignored, to be used later
    void (*m_PatchFunction)(uint32_t titleVer, uint64_t titleId);
} TitlePatch;

static TitlePatch titlePatches[] = {
    { .m_TitleIDs = { WIIU_MENU_TID }, .m_Version = 0, .m_PatchFunction = Patch_Wii_U_Menu },
};

static size_t numTitlePatches = sizeof(titlePatches) / sizeof(TitlePatch);

/* ****************************** */

extern void SC_KernelCopyData(void* dst, void* src, size_t size);
extern void kern_write(void* addr, uint32_t value);
extern uint32_t __OSGetTitleVersion();

/* !!!! Make sure the new URL legnth is equal or less than the original length */
char originalDiscoveryURL[] = "discovery.olv.nintendo.net/v1/endpoint";
char newDiscoveryURL[] = "discovery.olv.pretendo.cc/v1/endpoint";

#define INTERNET_BROWSER_JAP 0x000500301001200a
#define INTERNET_BROWSER_USA 0x000500301001210a
#define INTERNET_BROWSER_EUR 0x000500301001220a

#define MII_MAKER_JAP 0x000500101004a000
#define MII_MAKER_USA 0x000500101004a100
#define MII_MAKER_EUR 0x000500101004a200

int _main(uint32_t ret_addr) {

    /*

        Internet Browser calls sub_22A71F8 on entry, this piece of code will crash
        the process:


        // MEMGetBaseHeapHandle(1) == MEM2
        if (MEMGetAllocatableSizeForExpHeapEx(MEMGetBaseHeapHandle(1), 4) <
        0x4B390C ) { PanicCrash();
        }


        Loading nn_olv.rpl inside the Internet Browser takes too much RAM and
        crashes the process because of this call So we directly return and do
        nothing for the internet browser

    */

    LoadWiiUSymbols();

    uint64_t titleId = OSGetTitleID();
    if (titleId == INTERNET_BROWSER_JAP || titleId == INTERNET_BROWSER_USA || titleId == INTERNET_BROWSER_EUR) {
        return ret_addr;
    }

    Debugger_Start();

    /* The point of this block, is to check if we're in Mii Maker because we want to select or create a Mii
     * (for example, creating an account), if this is the case then skip HBL hook and start Mii Maker normally */
    if (titleId == MII_MAKER_JAP || titleId == MII_MAKER_USA || titleId == MII_MAKER_JAP) {

        uint32_t __rpl_sysapp;
        OSDynLoad_Acquire("sysapp", &__rpl_sysapp);

        int (*_SYSGetMiiStudioArgs)(int* outputArgs);
        OSDynLoad_FindExport(__rpl_sysapp, false, "_SYSGetMiiStudioArgs", (void**)&_SYSGetMiiStudioArgs);

        int miiStudioArgs[10];
        memset(miiStudioArgs, 0, sizeof(miiStudioArgs));
        _SYSGetMiiStudioArgs(miiStudioArgs);

        /* This means we're in Mii Maker to choose/create a Mii, do not start sd_loader, do the
         * coreinit entry logic by ourselves */
        if (miiStudioArgs[4] == 2) {

            uint32_t rpxEntry = *(uint32_t*)0x1005E040;
            int rpxArgc = *(int*)0x10013C30;
            char** rpxArgv = *(char***)0x10013C34;

            int (*realGameEntry)(int argc, char** argv) = (int (*)(int argc, char** argv))rpxEntry;
            void (*CafeOS_Exit)(int exitCode) = (void (*)(int))0x0101cdc4;

            CafeOS_Exit(realGameEntry(rpxArgc, rpxArgv));
        }
    }

    printf("\ntitle_patcher: 0x%016llx\n", titleId);

    /* When syscalls will be required for code patching, PLEASE, backup old
     * syscall values then reset them at the end */

    printf("Applying global Miiverse patches\n");

    uint32_t __rpl_nn_olv;
    OSDynLoad_Acquire("nn_olv.rpl", &__rpl_nn_olv);

    /* https://github.com/PretendoNetwork/Inkay/blob/main/src/main.cpp */
    for (uint32_t addr = 0x10000000; addr < 0x18000000; addr += 4) {
        if (!memcmp(originalDiscoveryURL, (void*)addr, sizeof(originalDiscoveryURL))) {
            printf("Found discovery string at 0x%08X, replacing ...\n", addr);
            memcpy((void*)addr, newDiscoveryURL,
                   sizeof(newDiscoveryURL)); // sizeof(newDiscoveryURL) includes the NULL byte terminator
            break;
        }
    }

    printf("Applying per game patches\n");

    /* For each title supported */
    for (size_t i = 0; i < numTitlePatches; i++) {
        /* For all 3 regions */
        for (int j = 0; j < 3; j++) {
            if (titlePatches[i].m_TitleIDs[j] == titleId) {
                printf("Applying game-specific patch to 0x%016llX ...\n", titleId);
                titlePatches[i].m_PatchFunction(__OSGetTitleVersion(), titleId);
                break;
            }
        }
    }

    printf("Exiting title_patcher\n");

    socketclose(__debug_udp_socket);
    socket_lib_finish();
    return ret_addr;
}
