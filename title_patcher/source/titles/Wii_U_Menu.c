#include "Wii_U_Menu.h"

char o2_original_url[] = "https://wup-o2fgs.cdn.nintendo.net/flags";
char o2_new_url[] =      "https://wup-o2fgs.cdn.pretendo.cc/flags";

void Patch_Wii_U_Menu(uint32_t titleVer, uint64_t titleId) {

    for (uint32_t addr = 0x10000000; addr < 0x20000000; addr += 4) {
        if (!memcmp(o2_original_url, (void*)addr, sizeof(o2_original_url))) {
            printf("Found WiiU Menu opt-out string at 0x%08X, replacing ...\n", addr);
			memcpy((void*)addr, o2_new_url, sizeof(o2_new_url)); // sizeof(new_url) includes the NULL byte terminator
            break;
        }
	}

}