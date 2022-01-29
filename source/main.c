/*
 *   Copyright (C) 2021 GaryOderNichts
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <iosuhax.h>

#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include "patches.h"

uint32_t strlen_(const char* str) {
    const char* s;
    for (s = str; *s; ++s);
    return (s - str);
}

//thanks @Gary#4139 :p
static void write_string(uint32_t addr, const char* str)
{
    int len = strlen(str) + 1;
    int remaining = len % 4;
    int num = len - remaining;

    for (int i = 0; i < (num / 4); i++) {
        IOSUHAX_kern_write32(addr + i * 4, *(uint32_t*)(str + i * 4));
    }

    if (remaining > 0) {
        uint8_t buf[4];
        IOSUHAX_kern_read32(addr + num, (uint32_t*)&buf, 1);

        for (int i = 0; i < remaining; i++) {
            buf[i] = *(str + num + i);
        }

        IOSUHAX_kern_write32(addr + num, *(uint32_t*)&buf);
    }
}

int main(int argc, char** argv)
{
    WHBLogUdpInit();
    
    WHBLogPrintf("Hello world from Nimble!");

    VPADStatus status;
    VPADReadError error = VPAD_READ_SUCCESS;
    VPADRead(VPAD_CHAN_0, &status, 1, &error);

    if (status.hold & VPAD_BUTTON_ZL)
    {
        WHBLogPrintf("Nimble patches skipped.");
    }
    else
    {
        if (IOSUHAX_Open(NULL) >= 0) {
            /* NoSSL patch */
            IOSUHAX_kern_write32(0xE1019F78, 0xE3A00001); // mov r0, #1

            /* URL patch */
            for (int i = 0; i < sizeof(url_patches) / sizeof(URL_Patch); i++) {
                write_string(url_patches[i].address, url_patches[i].url);
            }

            IOSUHAX_Close();
            WHBLogPrintf("Nimble patches succeeded!");
        }
        else
        {
            WHBLogPrintf("Nimble patches failed!");
        }
    }
    
    WHBLogUdpDeinit();
    
    return 0;
}
