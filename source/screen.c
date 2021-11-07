#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "screen.h"

#define COLOR_CONSTANT 0x632A5CFF

uint32_t Screen_BufferSizeTV = 0;
uint32_t Screen_BufferSizeDRC = 0;

uint8_t* Screen_TV_Buffer = (uint8_t*)0xF4000000;
uint8_t* Screen_DRC_Buffer = NULL;

int Screen_Y_Position = 0;

void Screen_Initialize()
{
    OSScreenInit();

    Screen_BufferSizeTV = OSScreenGetBufferSizeEx(0);
    Screen_BufferSizeDRC = OSScreenGetBufferSizeEx(1);

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    Screen_DRC_Buffer = Screen_TV_Buffer + Screen_BufferSizeDRC;

    OSScreenSetBufferEx(0, Screen_TV_Buffer);
    OSScreenSetBufferEx(1, Screen_DRC_Buffer);

    OSScreenClearBufferEx(0, COLOR_CONSTANT);
    OSScreenClearBufferEx(1, COLOR_CONSTANT);

    DCFlushRange(Screen_TV_Buffer, Screen_BufferSizeTV);
    DCFlushRange(Screen_DRC_Buffer, Screen_BufferSizeDRC);

    Screen_FlushBuffers();

    Screen_Y_Position = 0;
}

void Screen_Destroy()
{
    OSScreenShutdown();
    for(int i = 0; i < 2; i++)
    {
        OSScreenClearBufferEx(0, 0);
        OSScreenClearBufferEx(1, 0);
        OSScreenFlipBuffersEx(0);
        OSScreenFlipBuffersEx(1);
    }
}

void Screen_DrawText(const char *text)
{
    for(int i = 0; i < 2; i++)
    {
        OSScreenPutFontEx(0, 0, Screen_Y_Position, text);		
        OSScreenPutFontEx(1, 0, Screen_Y_Position, text);
    }
    Screen_Y_Position++;
}

void Screen_DrawTextLine(int line, const char *text)
{
    for(int i = 0; i < 2; i++)
    {
        OSScreenPutFontEx(0, 0, line, text);		
        OSScreenPutFontEx(1, 0, line, text);
    }
}

void Screen_DrawTextf(const char *text, ...)
{
    char *buf1 = (char*)malloc(0x800);
    char *buf2 = (char*)malloc(0x800);

    va_list va;
    va_start(va, text);

    vsnprintf(buf1, 0x800, text, va);
    snprintf(buf2, 0x800, "%s\n", buf1);
    
    Screen_DrawText((const char*)buf2);

    free(buf1);
    free(buf2);

    va_end(va);
}

void Screen_FlushBuffers()
{
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);
}

void Screen_StartFrame()
{
    OSScreenClearBufferEx(0, COLOR_CONSTANT);
    OSScreenClearBufferEx(1, COLOR_CONSTANT);

    DCFlushRange(Screen_TV_Buffer, Screen_BufferSizeTV);
    DCFlushRange(Screen_DRC_Buffer, Screen_BufferSizeDRC);

    Screen_Y_Position = 0;
}

void Screen_EndFrame()
{
    Screen_FlushBuffers();
}