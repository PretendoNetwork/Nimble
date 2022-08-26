#ifndef _TPATCH_WIIU_MENU_H
#define _TPATCH_WIIU_MENU_H

#include <stdio.h>
#include <stdint.h>
#include <gctypes.h>
#include <string.h>

#define WIIU_MENU_TID_J        0x0005001010040000
#define WIIU_MENU_TID_U        0x0005001010040100
#define WIIU_MENU_TID_E        0x0005001010040200

#define WIIU_MENU_TID          WIIU_MENU_TID_J, WIIU_MENU_TID_U, WIIU_MENU_TID_E

void Patch_Wii_U_Menu(uint32_t titleVer, uint64_t titleId);

#endif