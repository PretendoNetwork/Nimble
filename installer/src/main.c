/*
 * Copyright (C) 2016-2017 FIX94
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <iosuhax.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/gx2_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "system/memory.h"
#include "common/common.h"
#include "main.h"
#include "exploit.h"
#include "gameList.h"
#include "../payload/wupserver_bin.h"

static const char *sdCardVolPath = "/vol/storage_sdcard";
#ifdef CB
static const char *systemXmlPath = "/vol/system/config/system.xml";
static const char *syshaxXmlPath = "/vol/system/config/syshax.xml";
#endif
//just to be able to call async
void someFunc(void *arg)
{
	(void)arg;
}

static int mcp_hook_fd = -1;
int MCPHookOpen()
{
	//take over mcp thread
	mcp_hook_fd = MCP_Open();
	if(mcp_hook_fd < 0)
		return -1;
	IOS_IoctlAsync(mcp_hook_fd, 0x62, (void*)0, 0, (void*)0, 0, someFunc, (void*)0);
	//let wupserver start up
	sleep(1);
	if(IOSUHAX_Open("/dev/mcp") < 0)
		return -1;
	return 0;
}

void MCPHookClose()
{
	if(mcp_hook_fd < 0)
		return;
	//close down wupserver, return control to mcp
	IOSUHAX_Close();
	//wait for mcp to return
	sleep(1);
	MCP_Close(mcp_hook_fd);
	mcp_hook_fd = -1;
}

void println_noflip(int line, const char *msg)
{
	OSScreenPutFontEx(0,0,line,msg);
	OSScreenPutFontEx(1,0,line,msg);
}

void println(int line, const char *msg)
{
	int i;
	for(i = 0; i < 2; i++)
	{	//double-buffered font write
		println_noflip(line,msg);
		OSScreenFlipBuffersEx(0);
		OSScreenFlipBuffersEx(1);
	}
}

typedef struct _parsedList_t {
	uint32_t tid;
	char name[64];
	char path[64];
	u8 *romPtr;
	u32 romSize;
} parsedList_t;

int fsa_read(int fsa_fd, int fd, void *buf, int len)
{
	int done = 0;
	uint8_t *buf_u8 = (uint8_t*)buf;
	while(done < len)
	{
		size_t read_size = len - done;
		int result = IOSUHAX_FSA_ReadFile(fsa_fd, buf_u8 + done, 0x01, read_size, fd, 0);
		if(result < 0)
			return result;
		else
			done += result;
	}
	return done;
}

int fsa_write(int fsa_fd, int fd, void *buf, int len)
{
	int done = 0;
	uint8_t *buf_u8 = (uint8_t*)buf;
	while(done < len)
	{
		size_t write_size = len - done;
		int result = IOSUHAX_FSA_WriteFile(fsa_fd, buf_u8 + done, 0x01, write_size, fd, 0);
		if(result < 0)
			return result;
		else
			done += result;
	}
	return done;
}

int availSort(const void *c1, const void *c2)
{
	return strcmp(((parsedList_t*)c1)->name,((parsedList_t*)c2)->name);
}

void printhdr_noflip()
{
	println_noflip(0,"Pretendo Installer by SuperMarioDaBom");
	println_noflip(1,"Based upon Haxchi installer originally by FIX94 tweaked by jam1garner");
}

int Menu_Main(void)
{
	InitOSFunctionPointers();
	InitSysFunctionPointers();
	InitVPadFunctionPointers();
	VPADInit();
	memoryInitialize();

	// Init screen
	OSScreenInit();
	int screen_buf0_size = OSScreenGetBufferSizeEx(0);
	int screen_buf1_size = OSScreenGetBufferSizeEx(1);
	uint8_t *screenBuffer = (uint8_t*)MEMBucket_alloc(screen_buf0_size+screen_buf1_size, 0x100);
	OSScreenSetBufferEx(0, screenBuffer);
	OSScreenSetBufferEx(1, (screenBuffer + screen_buf0_size));
	OSScreenEnableEx(0, 1);
	OSScreenEnableEx(1, 1);
	OSScreenClearBufferEx(0, 0);
	OSScreenClearBufferEx(1, 0);

	int mcp_handle = MCP_Open();
	int count = MCP_TitleCount(mcp_handle);
	int listSize = count*0x61;
	char *tList = memalign(32, listSize); //cant be in MEMBucket
	memset(tList, 0, listSize);
	int recievedCount = count;
	MCP_TitleList(mcp_handle, &recievedCount, tList, listSize);
	MCP_Close(mcp_handle);

	int gAvailCnt = 0;
	parsedList_t *gAvail = (parsedList_t*)MEMBucket_alloc(recievedCount*sizeof(parsedList_t), 4);
	memset(gAvail, 0, recievedCount*sizeof(parsedList_t));

	int i, j;
	for(i = 0; i < recievedCount; i++)
	{
		char *cListElm = tList+(i*0x61);
		if(memcmp(cListElm+0x56,"mlc",4) != 0 || *(uint32_t*)(cListElm) != 0x0005001B)
			continue;
		for(j = 0; j < GameListSize; j++)
		{
			if(*(uint32_t*)(cListElm+4) == GameList[j].tid)
			{
				gAvail[gAvailCnt].tid = GameList[j].tid;
				memcpy(gAvail[gAvailCnt].name, GameList[j].name, 64);
				memcpy(gAvail[gAvailCnt].path, cListElm+0xC, 64);
				gAvail[gAvailCnt].romPtr = GameList[j].romPtr;
				gAvail[gAvailCnt].romSize = *(GameList[j].romSizePtr);
				gAvailCnt++;
				break;
			}
		}
	}

	int vpadError = -1;
	VPADData vpad;

	if(gAvailCnt == 0)
	{
		printhdr_noflip();
		println_noflip(2,"An error has occurred. The certificate title wasn't found.");
		println_noflip(3,"Please disconnect all usb devices, and try again.");
		println_noflip(4,"If the error persists, please tell SuperMarioDaBom.");
		println_noflip(5,"Press any button to return to Homebrew Launcher.");
		OSScreenFlipBuffersEx(0);
		OSScreenFlipBuffersEx(1);
		while(1)
		{
			usleep(25000);
			VPADRead(0, &vpad, 1, &vpadError);
			if(vpadError != 0)
				continue;
			if(vpad.btns_d | vpad.btns_h)
				break;
		}
		OSScreenEnableEx(0, 0);
		OSScreenEnableEx(1, 0);
		MEMBucket_free(screenBuffer);
		memoryRelease();
		return EXIT_SUCCESS;
	}

	qsort(gAvail,gAvailCnt,sizeof(parsedList_t),availSort);

	u32 redraw = 1;
	s32 PosX = 0;
	s32 ScrollX = 0;

	s32 ListMax = gAvailCnt;
	if( ListMax > 13 )
		ListMax = 13;

	u32 UpHeld = 0, DownHeld = 0;
	while(1)
	{
		usleep(25000);
		VPADRead(0, &vpad, 1, &vpadError);
		if(vpadError != 0)
			continue;

		if((vpad.btns_d | vpad.btns_h) & VPAD_BUTTON_HOME)
		{
			OSScreenEnableEx(0, 0);
			OSScreenEnableEx(1, 0);
			MEMBucket_free(screenBuffer);
			memoryRelease();
			return EXIT_SUCCESS;
		}
		if( vpad.btns_h & VPAD_BUTTON_DOWN )
		{
			if(DownHeld == 0 || DownHeld > 10)
			{
				if( PosX + 1 >= ListMax )
				{
					if( PosX + 1 + ScrollX < gAvailCnt)
						ScrollX++;
					else {
						PosX	= 0;
						ScrollX = 0;
					}
				} else {
					PosX++;
				}
				redraw = 1;
			}
			DownHeld++;
		}
		else
			DownHeld = 0;

		if( vpad.btns_h & VPAD_BUTTON_UP )
		{
			if(UpHeld == 0 || UpHeld > 10)
			{
				if( PosX <= 0 )
				{
					if( ScrollX > 0 )
						ScrollX--;
					else {
						PosX	= ListMax - 1;
						ScrollX = gAvailCnt - ListMax;
					}
				} else {
					PosX--;
				}
				redraw = 1;
			}
			UpHeld++;
		}
		else
			UpHeld = 0;

		if( vpad.btns_d & VPAD_BUTTON_A )
			break;

		if( redraw )
		{
			OSScreenClearBufferEx(0, 0);
			OSScreenClearBufferEx(1, 0);
			printhdr_noflip();
			println_noflip(2,"Please select what you'd like to do.");
			// Starting position.
			int gamelist_y = 4;
			for (i = 0; i < ListMax; ++i, gamelist_y++)
			{
				const parsedList_t *cur_gi = &gAvail[i+ScrollX];
				char printStr[64];
				sprintf(printStr,"%c %s", i == PosX ? '>' : ' ', cur_gi->name);
				println_noflip(gamelist_y, printStr);
			}
			OSScreenFlipBuffersEx(0);
			OSScreenFlipBuffersEx(1);
			redraw = 0;
		}
	}
	const parsedList_t *SelectedGame = &gAvail[PosX + ScrollX];
	for(j = 0; j < 2; j++)
	{
		OSScreenClearBufferEx(0, 0);
		OSScreenClearBufferEx(1, 0);
		printhdr_noflip();
		println_noflip(2,"You have chosen to do the following:");
		println_noflip(3,SelectedGame->name);

		println_noflip(4,"This will install/uninstall Pretendo.");
		println_noflip(5,"If you are sure press A, else press HOME.");

		OSScreenFlipBuffersEx(0);
		OSScreenFlipBuffersEx(1);
		usleep(25000);
	}
	while(1)
	{
		usleep(25000);
		VPADRead(0, &vpad, 1, &vpadError);
		if(vpadError != 0)
			continue;
		//user aborted
		if((vpad.btns_d | vpad.btns_h) & VPAD_BUTTON_HOME)
		{
			OSScreenEnableEx(0, 0);
			OSScreenEnableEx(1, 0);
			MEMBucket_free(screenBuffer);
			memoryRelease();
			return EXIT_SUCCESS;
		}
		//lets go!
		if(vpad.btns_d & VPAD_BUTTON_A)
			break;

	}

	int line = 6;

	int fsaFd = -1;
	int sdMounted = 0;
	int sdFd = -1, mlcFd = -1, slcFd = -1;

	//open up iosuhax
	int res = IOSUHAX_Open(NULL);
	if(res < 0)
		res = MCPHookOpen();
	if(res < 0)
	{
		println(line++,"Doing IOSU Exploit...");
		*(volatile unsigned int*)0xF5E70000 = wupserver_bin_len;
		memcpy((void*)0xF5E70020, &wupserver_bin, wupserver_bin_len);
		DCStoreRange((void*)0xF5E70000, wupserver_bin_len + 0x40);
		IOSUExploit();
		//done with iosu exploit, take over mcp
		if(MCPHookOpen() < 0)
		{
			println(line++,"MCP hook could not be opened!");
			goto prgEnd;
		}
	}

	//mount with full permissions
	fsaFd = IOSUHAX_FSA_Open();
	if(fsaFd < 0)
	{
		println(line++,"FSA could not be opened!");
		goto prgEnd;
	}

	int ret = IOSUHAX_FSA_Mount(fsaFd, "/dev/sdcard01", sdCardVolPath, 2, (void*)0, 0);
	if(ret < 0)
	{
		println(line++,"Failed to mount SD!");
		goto prgEnd;
	}
	else
		sdMounted = 1;
	char path[256];
	sprintf(path,"%s/content/scerts/CACERT_NINTENDO_CA_G3.der",SelectedGame->path);
	if(IOSUHAX_FSA_OpenFile(fsaFd, path, "rb", &mlcFd) < 0)
	{
		println(line++,"Unable to find the certificate! Please try again. If this");
		println(line++,"error continues to appear, please tell SuperMarioDaBom.");
		goto prgEnd;
	}
	else
		IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
	if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
	{
		println(line++,"Writing data...");
		fsa_write(fsaFd, mlcFd, SelectedGame->romPtr, SelectedGame->romSize);
		IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
		mlcFd = -1;
	}

	char sdHaxchiPath[256];

	println(line++,"Done performing installation / uninstallation of Pretendo!");

prgEnd:
	if(tList) //cant be in MEMBucket
		free(tList);
	if(gAvail)
		MEMBucket_free(gAvail);
	//close down everything fsa related
	if(fsaFd >= 0)
	{
		if(slcFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
		if(mlcFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
		if(sdFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		if(sdMounted)
			IOSUHAX_FSA_Unmount(fsaFd, sdCardVolPath, 2);
		if(mcp_hook_fd >= 0)
		{
			if(IOSUHAX_FSA_FlushVolume(fsaFd, "/vol/storage_mlc01") == 0)
				println(line++, "Flushed NAND Cache!");
		}
		IOSUHAX_FSA_Close(fsaFd);
	}
	//close out iosuhax
	if(mcp_hook_fd >= 0)
		MCPHookClose();
	else
		IOSUHAX_Close();
	sleep(5);
	//will do IOSU reboot
	OSForceFullRelaunch();
	SYSLaunchMenu();
	OSScreenEnableEx(0, 0);
	OSScreenEnableEx(1, 0);
	MEMBucket_free(screenBuffer);
	memoryRelease();
	return EXIT_RELAUNCH_ON_LOAD;
}
