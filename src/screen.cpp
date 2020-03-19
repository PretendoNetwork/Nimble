#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <coreinit/memory.h>
#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <coreinit/screen.h>
#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memory.h>

#include "screen.h"

#define COLOR_CONSTANT 0x632A5CFF

WiiUScreen::WiiUScreen()
{
	OSScreenInit();

	this->BufferSizeTV = OSScreenGetBufferSizeEx(SCREEN_TV);
	this->BufferSizeDRC = OSScreenGetBufferSizeEx(SCREEN_DRC);

	OSScreenEnableEx(SCREEN_TV, 1);
	OSScreenEnableEx(SCREEN_DRC, 1);

	MEMHeapHandle mem1_heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
	this->TV_Buffer = (void*)MEMAllocFromFrmHeapEx(mem1_heap, this->BufferSizeTV, 4);
	this->DRC_Buffer = (void*)MEMAllocFromFrmHeapEx(mem1_heap, this->BufferSizeDRC, 4);

	OSScreenSetBufferEx(SCREEN_TV, this->TV_Buffer);
	OSScreenSetBufferEx(SCREEN_DRC, this->DRC_Buffer);

	OSScreenClearBufferEx(SCREEN_TV, COLOR_CONSTANT);
	OSScreenClearBufferEx(SCREEN_DRC, COLOR_CONSTANT);

	DCFlushRange(this->TV_Buffer, BufferSizeTV);
	DCFlushRange(this->DRC_Buffer, BufferSizeDRC);

	this->FlushBuffers();

	this->Y_Position = 0;
}

WiiUScreen::~WiiUScreen()
{
	MEMHeapHandle mem1_heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
	MEMFreeByStateToFrmHeap(mem1_heap, 0x000DECAF);

	OSScreenShutdown();
}

void WiiUScreen::DrawText(const char *text)
{
	for(int i = 0; i < 2; i++)
	{

		OSScreenPutFontEx(SCREEN_TV, 0, this->Y_Position, text);		
		OSScreenPutFontEx(SCREEN_DRC, 0, this->Y_Position, text);

	}

	this->Y_Position++;

}

void WiiUScreen::DrawTextLine(int line, const char *text)
{
	for(int i = 0; i < 2; i++)
	{

		OSScreenPutFontEx(SCREEN_TV, 0, line, text);		
		OSScreenPutFontEx(SCREEN_DRC, 0, line, text);

	}

}

void WiiUScreen::DrawTextf(const char *text, ...)
{
	char *buf1 = (char*)OSAllocFromSystem(0x800, 4);
	char *buf2 = (char*)OSAllocFromSystem(0x800, 4);

	va_list va;
	va_start(va, text);

	vsnprintf(buf1, 0x800, text, va);
	snprintf(buf2, 0x800, "%s\n", buf1);
	
	this->DrawText((const char*)buf2);

	OSFreeToSystem(buf1);
	OSFreeToSystem(buf2);

	va_end(va);
}

void WiiUScreen::FlushBuffers()
{
	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}

void WiiUScreen::StartFrame()
{
	OSScreenClearBufferEx(SCREEN_TV, COLOR_CONSTANT);
	OSScreenClearBufferEx(SCREEN_DRC, COLOR_CONSTANT);

	DCFlushRange(this->TV_Buffer, BufferSizeTV);
	DCFlushRange(this->DRC_Buffer, BufferSizeDRC);

	this->Y_Position = 0;
}

void WiiUScreen::EndFrame()
{
	this->FlushBuffers();
}