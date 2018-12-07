#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>
#include <dynamic_libs/gx2_functions.h>
#include <dynamic_libs/sys_functions.h>
#include <dynamic_libs/vpad_functions.h>
#include <dynamic_libs/padscore_functions.h>
#include <dynamic_libs/socket_functions.h>
#include <dynamic_libs/ax_functions.h>
#include <libutils/fs/sd_fat_devoptab.h>
#include <libutils/system/memory.h>
#include <libutils/utils/utils.h>
#include <libutils/kernel/syscalls.h>
#include <libutils/kernel/kernel_utils.h>

#include "screen.h"


int sy = 0;
unsigned char *DRC_Buffer = (unsigned char *)0xF50A0000;

void deinitScreen()
{
    OSScreenEnableEx(1, 0);
}
void clearScreen() {

    for(int i = 0; i < 2; i++)
    {
        OSScreenClearBufferEx(1, 0x652ab7ff);
        OSScreenFlipBuffersEx(1);
    }

    sy = 0;
}

void initScreen()
{
    OSScreenInit();                                                              
    OSScreenSetBufferEx(1, DRC_Buffer); 
    OSScreenEnableEx(1, 1); // Enable GamePad
    clearScreen();
}
void flipBuffers() {

    int buf_size = OSScreenGetBufferSizeEx(1);
    DCFlushRange((void *)DRC_Buffer, buf_size);
    OSScreenFlipBuffersEx(1);
}

/* *** TODO: actual printf because holy shit *** */
void printf_(const char *buf, u32 val) {

    char buffer[255];
    __os_snprintf(buffer, 255, buf, val);

    for(int i = 0; i < 2; i++)
	{
    	OSScreenPutFontEx(1, 0, sy, buffer);
    	OSScreenFlipBuffersEx(1);
    }

    sy = sy + 1;
}

