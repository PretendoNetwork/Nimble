#ifndef _TPATCH_MK8_H
#define _TPATCH_MK8_H

#include <stdio.h>
#include <stdint.h>
#include <gctypes.h>
#include <string.h>

#include "patcher.h"

#define MARIO_KART_8_TID_J 0x000500001010EB00
#define MARIO_KART_8_TID_U 0x000500001010EC00
#define MARIO_KART_8_TID_E 0x000500001010ED00

#define MARIO_KART_8_TID MARIO_KART_8_TID_J, MARIO_KART_8_TID_U, MARIO_KART_8_TID_E

void Patch_Mario_Kart_8(uint32_t titleVer, uint64_t titleId);

#endif