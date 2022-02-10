#include "Wii_U_Menu.h"

char originalOptOutURL[] = "https://wup-o2fgs.cdn.nintendo.net/flags";
char newOptOutURL[] = "https://wup-o2fgs.cdn.pretendo.cc/flags";

/* This appears to be a "Europe" only URL */
void Patch_Wii_U_Menu(uint32_t titleVer, uint64_t titleId) {

    for (uint32_t addr = 0x10000000; addr < 0x18000000; addr += 4) {
        if (!memcmp(originalOptOutURL, (void*)addr, sizeof(originalOptOutURL))) {
            printf("Found WiiU Menu opt-out string at 0x%08X, replacing ...\n", addr);
            memcpy((void*)addr, newOptOutURL,
                   sizeof(newOptOutURL)); // sizeof(new_url) includes the NULL byte terminator
            break;
        }
    }
}