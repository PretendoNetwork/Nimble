#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/gfx.h>
#include <whb/log_udp.h>
#include <whb/sdcard.h>
#include <whb/file.h>

#include <coreinit/memory.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/screen.h>
#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memory.h>
#include <coreinit/ios.h>
#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/context.h>
#include <coreinit/exception.h>
#include <coreinit/internal.h>
#include <coreinit/core.h>
#include <coreinit/title.h>
#include <coreinit/debug.h>
#include <coreinit/memorymap.h>
#include <coreinit/dynload.h>

#include <nsysnet/socket.h>
#include <nsysnet/nssl.h>
#include <nsysnet/socket.h>

#include <sndcore2/core.h>

#include <vpad/input.h>

#include <curl/curl.h>

#include <proc_ui/procui.h>

#include <sysapp/launch.h>

#include <codecvt>
#include <string>
#include <iostream>

#include "patcher.h"
#include "network.h"
#include "screen.h"
#include "IOSU_Exploit.h"
#include "keyboard.h"
#include "led.h"

#include "binary_data/arm_kernel.h"

const char *version = "1.0";

#define IOSU_ALL_URLS   1 << 1
#define IOSU_SSL_PATCH  1 << 2

BOOL alreadyPatched = FALSE;

void ExceptionHandler(const char *type, OSContext *context)
{

	char *symbolname = (char*)OSAllocFromSystem(129, 4);
	memset(symbolname, 0, 129);

	if(OSIsAddressValid(context->srr0) == true)
		OSGetSymbolName(context->srr0, symbolname, 128);
	

	char buffer[0x800];
	__os_snprintf(buffer, 2048,	"Exception: %s - in %s\n"
								"Title ID: %016llX | Core: %d | UPID: %d\n\n"
								""
								"An error has occured.\n"
								"Press the POWER button for 4 seconds to shutdown.\n\n"
								""
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | r%-2d: 0x%08X\n"
								"r%-2d: 0x%08X | r%-2d: 0x%08X | LR : 0x%08X\n"
								"PC : 0x%08X | DAR: 0x%08X | DSISR: 0x%08X\n",
								type, symbolname,
								OSGetTitleID(), OSGetCoreId(), OSGetUPID(),
								0, context->gpr[0], 11, context->gpr[11], 22, context->gpr[22],
								1, context->gpr[1], 12, context->gpr[12], 23, context->gpr[23],
								2, context->gpr[2], 13, context->gpr[13], 24, context->gpr[24],
								3, context->gpr[3], 14, context->gpr[14], 25, context->gpr[25],
								4, context->gpr[4], 15, context->gpr[15], 26, context->gpr[26],
								5, context->gpr[5], 16, context->gpr[16], 27, context->gpr[27],
								6, context->gpr[6], 17, context->gpr[17], 28, context->gpr[28],
								7, context->gpr[7], 18, context->gpr[18], 29, context->gpr[29],
								8, context->gpr[8], 19, context->gpr[19], 30, context->gpr[30],
								9, context->gpr[9], 20, context->gpr[20], 31, context->gpr[31],
								10, context->gpr[10], 21, context->gpr[21], context->lr,
								context->srr0, context->dar, context->dsisr);

	WHBLogPrintf(buffer);
	OSFatal(buffer);

}

BOOL ExcDSI(OSContext *ctx)
{
	ExceptionHandler("DSI", ctx);
	return TRUE;
}

BOOL ExcISI(OSContext *ctx)
{
	ExceptionHandler("ISI", ctx);
	return TRUE;
}

BOOL ExcProgram(OSContext *ctx)
{
	ExceptionHandler("Program", ctx);
	return TRUE;
}

// partial fix for swkbd issue
BOOL sKbdStartedOnce = FALSE;

// menu variables
int menu = 0;
int g_cursor = 0;
uint64_t timeSinceLastPress = 0;
int newMaxOptionCount = 5;

// domain we will patch to
char *currentDomain = (char*)"pretendo.cc\x00";

BOOL exitingToHBL = TRUE;

int main(int argc, char *argv[])
{

	timeSinceLastPress = OSGetTick();

	WHBProcInit();
	WHBLogUdpInit();
	FSInit();
	AXInit();
	WHBLogPrintf("Hello World!");

	InitDRCStuff();

	/* get the device ID */
	uint32_t deviceID = -1;
	IOS_Ioctl((IOSHandle)IOS_Open("/dev/mcp", (IOSOpenMode)0), 0xD3, 0, 0, &deviceID, 4);

	/* SEEPROM dumper, for internal uses only. */
	IOSU_Exploit((uint8_t*)arm_kernel_bin, arm_kernel_bin_len);

	/* Set exception callbacks */
	OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_DSI, ExcDSI);
	OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_ISI, ExcISI);
	OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_PROGRAM, ExcProgram);

	/* Online check + render loop */
	WiiUScreen *screen = new WiiUScreen();
	screen->DrawText("Welcome to the Pretendo Installer");
	screen->Y_Position++;

	uint8_t* ready = (uint8_t*)OSAllocFromSystem(0x800, 4);
	int PretendoAlive = -1;
	DownloadFile(ready, "http://pretendo.cc", &PretendoAlive);

	while(WHBProcIsRunning()) {
			
		VPADReadError err;
		VPADStatus vpad;

		/* Handle VPAD inputs */
		VPADRead(VPAD_CHAN_0, &vpad, 1, &err);
		uint32_t btn = vpad.hold | vpad.trigger;

		/* Open cursored menu */
		if(btn & VPAD_BUTTON_A && (OSTicksToMilliseconds(OSGetTick() - timeSinceLastPress) > 200))
		{	

			if(menu == 3 && g_cursor == 0 && alreadyPatched == FALSE)
			{

				WHBLogPrintf("test");

				delete screen;
				WHBLogPrintf("%p %s", currentDomain, currentDomain);
				Patch_IOSU_URLs(TRUE, FALSE, currentDomain);
				alreadyPatched = TRUE;
				OSSleepTicks(OSMillisecondsToTicks(200));
				screen = new WiiUScreen();
			}

			if(menu == 3 && g_cursor == 1 && alreadyPatched == FALSE)
			{

				delete screen;
				Patch_IOSU_URLs(TRUE, TRUE, currentDomain);
				alreadyPatched = TRUE;
				OSSleepTicks(OSMillisecondsToTicks(200));
				screen = new WiiUScreen();
			}

			if(menu == 3 && g_cursor == 2 && alreadyPatched == FALSE)
			{

				delete screen;
				Patch_IOSU_URLs(FALSE, TRUE, currentDomain);
				alreadyPatched = TRUE;
				OSSleepTicks(OSMillisecondsToTicks(200));
				screen = new WiiUScreen();		
			}

			if(g_cursor == 1 && menu == 0)
				menu = 1;

			if(g_cursor == 2 && menu == 0)
				menu = 2;	

			if(g_cursor == 4 && menu == 0)
			{
				exitingToHBL = FALSE;
				SYSLaunchMenu();
			}

			if(g_cursor == 0 && menu == 0)
			{
				menu = 3;
				g_cursor = 0;
				newMaxOptionCount = 3;
			}

			if(menu == 0 && g_cursor == 3 && sKbdStartedOnce == FALSE)
			{

				menu = 4;
				delete screen;

				/* partial fix for a bug, you can only use wbkbd once */
				sKbdStartedOnce = TRUE;

				/* get input data */
				currentDomain = GetKeyboardInput();

				/* restart ProcUI because it's broken */
				ProcUIShutdown();
				WHBProcInit();

				/* restart OSScreen too because we used GX2 */
				screen = new WiiUScreen();

				menu = 0;
				g_cursor = 0;

			}

			timeSinceLastPress = OSGetTick();
		}

		/* back to the main menu */
		if(btn & VPAD_BUTTON_B && (OSTicksToMilliseconds(OSGetTick() - timeSinceLastPress) > 200))
		{	
			menu = 0;
			newMaxOptionCount = 5;
			timeSinceLastPress = OSGetTick();
		}


		/* increase cursor pos */
		if(btn & VPAD_BUTTON_UP && (OSTicksToMilliseconds(OSGetTick() - timeSinceLastPress) > 200))
		{
				if(g_cursor > 0)
					g_cursor--;
				timeSinceLastPress = OSGetTick();
		}

		/* decrease cursor pos */
		if(btn & VPAD_BUTTON_DOWN && (OSTicksToMilliseconds(OSGetTick() - timeSinceLastPress) > 200))
		{		
				if(g_cursor < newMaxOptionCount-1)
					g_cursor++;
				timeSinceLastPress = OSGetTick();
		}	


		/* Start render frame */
		screen->StartFrame();

		/* Basic lookup .. waiting for the async thread to answer .. */
		char *text = NULL;
		if(PretendoAlive < 0)
			text = (char*)"Requesting status..";
	
		if(PretendoAlive == 200)
			text = (char*)"Pretendo is up";
		else if(PretendoAlive > 0 && PretendoAlive != 200)
			text = (char*)"Pretendo is downed";

		/*  Basic info printing */
		screen->DrawTextf("Welcome to the Pretendo Installer v%s |  %s", version, text);
		screen->DrawTextLine(16, "Controls: HOME to Exit | DPAD Up/Down | A/B");
		screen->Y_Position++;

		/* Draw Main Menu */
		if(menu == 0)
		{
			screen->DrawTextf("%s Patching menu", (g_cursor == 0) ? "> " : "  ");
			screen->DrawTextf("%s Console information menu", (g_cursor == 1) ? "> " : "  ");
			screen->DrawTextf("%s Credits list", (g_cursor == 2) ? "> " : "  ");
			screen->DrawTextf("%s Change local IP", (g_cursor == 3) ? "> " : "  ");
			screen->DrawTextf("%s Exit to the WiiU Menu", (g_cursor == 4) ? "> " : "  ");
		}

		/* Draw console informations */
		if(menu == 1)
		{

			screen->DrawTextf("SerialNumber: %s%s\nRegion: %s-%s\nFactory date: %02x-%02x-%04x @ %02x:%02x\nDeviceID: %d",(char*)(0xF5FFFC00 + 0x158), 
																									(char*)(0xF5FFFC00 + 0x160), 
																									(char*)(0xF5FFFC00 + 0x150), 
																									(char*)(0xF5FFFC00 + 0x154),
																									*(char*)(0xF5FFFC00 + 0x18B),
																									*(char*)(0xF5FFFC00 + 0x18A),
																									*(short*)(0xF5FFFC00 + 0x188),
																									*(char*)(0xF5FFFC00 + 0x18C),
																									*(char*)(0xF5FFFC00 + 0x18D),
																									deviceID);
		}

		/* Draw Credits */
		if(menu == 2)
		{
			screen->DrawText("Thanks to the Pretendo Team");
			screen->Y_Position++;
			screen->DrawText("Rambo6Glaz for this patcher");
			screen->DrawText("RedDucks for being the lead developer");
			screen->DrawText("Kinnay for the No-SSL patch");
			screen->DrawText("SuperMarioDaBom, superwhiskers, Billy, Jemma");
			screen->DrawText("Oman Computar, jvs, mrexodia, Jip");
			screen->DrawText("kxpler442b_, laurenceN, nybbit, Shadow and Stary");
		}

		/* Draw Main Menu */
		if(menu == 3 && alreadyPatched == FALSE)
		{
			screen->DrawTextf("%s Patch all URLs", (g_cursor == 0) ? "> " : "  ");
			screen->DrawTextf("%s Patch all URLs + SSL Patch", (g_cursor == 1) ? "> " : "  ");
			screen->DrawTextf("%s SSL Patch", (g_cursor == 2) ? "> " : "  ");
		}
		else if(menu == 3 && alreadyPatched == TRUE) 
		{
			screen->DrawText("Already patched, come back later :D");
		}

		/* draw domain */
		if(menu == 0)
		{
			screen->Y_Position++;
			screen->Y_Position++;
			screen->DrawTextf("Patching to: %s", currentDomain);
		}

		/* end the frame (draw it) */
		screen->EndFrame();

	}

	WHBLogPrintf("Exiting ...");

	ChangeDrcLedPattern(DRC_LED_OFF);

	if(exitingToHBL == FALSE)
		ProcUIShutdown();
	else
		WHBProcShutdown();

	WHBLogUdpDeinit();

	return -3;

}