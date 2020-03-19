#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/gfx.h>
#include <whb/log_udp.h>

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
#include <coreinit/messagequeue.h>

#include <sndcore2/core.h>

#include <vpad/input.h>

#include <curl/curl.h>

#include <nsysnet/socket.h>
#include <nsysnet/nssl.h>
#include <nsysnet/socket.h>

#include <nn/swkbd.h>

#include <proc_ui/procui.h>

#include "keyboard.h"

// UTF-16 LE
uint16_t *ToUTF16(const char *utf8)
{	

	int rsz = strlen(utf8) + 1;
	int sz = (strlen(utf8) + 1)*2;

	uint16_t *yeet = (uint16_t*)MEMAllocFromDefaultHeap(sz);
	memset(yeet, 0, sz);

	for (int i = 0; i < rsz; ++i)
		yeet[i] = utf8[i];

	return yeet;

}

char * GetKeyboardInput()
{

	WHBGfxInit();

	// Create FSClient for swkbd
	FSClient *fsClient = (FSClient *)MEMAllocFromDefaultHeap(sizeof(FSClient));
	FSAddClient(fsClient, 0);

	// Create swkbd
	nn::swkbd::CreateArg createArg;
	createArg.regionType = nn::swkbd::RegionType::Europe;
	createArg.workMemory = MEMAllocFromDefaultHeap(nn::swkbd::GetWorkMemorySize(0));
	createArg.fsClient = fsClient;
	if (!nn::swkbd::Create(createArg)) {
		WHBLogPrintf("nn::swkbd::Create failed");
		WHBProcShutdown();
		return NULL;
	}

	// Show the keyboard
	nn::swkbd::AppearArg appearArg;
	appearArg.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;
	appearArg.inputFormArg.unk_0x0C = (uint32_t)ToUTF16("Enter the domain name.\nString size is limited to 12 characters.");
	appearArg.inputFormArg.maxTextLength = 12;
	if (!nn::swkbd::AppearInputForm(appearArg)) {
		WHBLogPrintf("nn::swkbd::AppearInputForm failed");
		WHBProcShutdown();
		return NULL;
	}

	nn::swkbd::MuteAllSound(FALSE);

	while(WHBProcIsRunning())
	{
		VPADStatus vpadStatus;
		VPADRead(VPAD_CHAN_0, &vpadStatus, 1, nullptr);
		VPADGetTPCalibratedPoint(VPAD_CHAN_0, &vpadStatus.tpNormal, &vpadStatus.tpNormal);

		nn::swkbd::ControllerInfo controllerInfo;
		controllerInfo.vpad = &vpadStatus;
		controllerInfo.kpad[0] = nullptr;
		controllerInfo.kpad[1] = nullptr;
		controllerInfo.kpad[2] = nullptr;
		controllerInfo.kpad[3] = nullptr;
		nn::swkbd::Calc(controllerInfo);

		if (nn::swkbd::IsNeedCalcSubThreadFont()) {
			nn::swkbd::CalcSubThreadFont();
		}

		if (nn::swkbd::IsNeedCalcSubThreadPredict()) {
			nn::swkbd::CalcSubThreadPredict();
		}

		if (nn::swkbd::IsDecideOkButton(nullptr)) {
			nn::swkbd::DisappearInputForm();
			break;
		}

		WHBGfxBeginRender();

		WHBGfxBeginRenderTV();
		WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		nn::swkbd::DrawTV();
		WHBGfxFinishRenderTV();

		WHBGfxBeginRenderDRC();
		WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		nn::swkbd::DrawDRC();
		WHBGfxFinishRenderDRC();

		WHBGfxFinishRender();

	}

	const char16_t *str = nn::swkbd::GetInputFormString();
	char *logStr = (char*)MEMAllocFromDefaultHeap(128);

	if (!str) {
		WHBLogPrint("nn::swkbd::GetInputFormString returned NULL");
	} else
	{
		// Quick hack to get from a char16_t str to char for our log function
		
		logStr[0] = 0;

		for (int i = 0; i < 128; ++i) {
			if (!str[i]) {
				logStr[i] = 0;
				break;
			}

			if (str[i] > 0x7F) {
				logStr[i] = '?';
			} else
			{
				logStr[i] = str[i];
			}
		}

	WHBLogPrintf("Input string: %s", logStr);
	}

	nn::swkbd::Destroy();
	MEMFreeToDefaultHeap(createArg.workMemory);

	FSDelClient(fsClient, 0);
	MEMFreeToDefaultHeap(fsClient);

	WHBGfxShutdown();

	return logStr;

}