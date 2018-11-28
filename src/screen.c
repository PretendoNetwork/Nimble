#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>
#include <dynamic_libs/gx2_functions.h>
#include <dynamic_libs/sys_functions.h>
#include <dynamic_libs/vpad_functions.h>
#include <dynamic_libs/socket_functions.h>
#include <libutils/system/memory.h>
#include <libutils/utils/logger.h>
#include <libutils/utils/utils.h>
#include <dynamic_libs/os_defs.h>
#include "screen.h"

int sy = 0;
unsigned char *DRC_Buffer = (unsigned char *)0xF50A0000;

void deinitScreen()
{
	OSScreenEnableEx(1, 0);
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

void printf_(const char *buf, ...) {

    char buffer[255];
    va_list argptr;
    va_start(argptr, buf);
    __os_snprintf(buffer, 255, buf, argptr);
    va_end(argptr);

    for(int i = 0; i < 2; i++)
	{
    	OSScreenPutFontEx(1, 0, sy, buffer);
    	OSScreenFlipBuffersEx(1);
    }

    sy = sy + 1;
}

void clearScreen() {

	for(int i = 0; i < 2; i++)
	{
    	OSScreenClearBufferEx(1, 0x652ab7ff);
    	OSScreenFlipBuffersEx(1);
    }

    sy = 0;
}