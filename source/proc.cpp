/*
-----------------------------------------------
ProcUI wrapper library created by Fangal Airbag
-----------------------------------------------
*/


#include <coreinit/core.h>
#include <coreinit/exit.h>
#include <coreinit/foreground.h>
#include <coreinit/messagequeue.h>
#include <coreinit/systeminfo.h>
#include <coreinit/title.h>
#include <gx2/event.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>

#include "proc.hpp"

#define HBL_TITLE_ID (0x0005000013374842)
#define MII_MAKER_JPN_TITLE_ID (0x000500101004A000)
#define MII_MAKER_USA_TITLE_ID (0x000500101004A100)
#define MII_MAKER_EUR_TITLE_ID (0x000500101004A200)

static bool
isRunning = FALSE;

static bool
fromHBL = FALSE;

static uint32_t
procSaveCallback(void *context)
{
	OSSavesDone_ReadyToRelease();
	return 0;
}

static uint32_t
procHomeButtonDenied(void *context)
{
	return 0;
}

void
ProcInit()
{
	uint64_t titleID = OSGetTitleID();

	if (titleID == HBL_TITLE_ID ||
		titleID == MII_MAKER_JPN_TITLE_ID ||
		titleID == MII_MAKER_USA_TITLE_ID ||
		titleID == MII_MAKER_EUR_TITLE_ID)
	{
		fromHBL = TRUE;
	}

	isRunning = TRUE;

	OSEnableHomeButtonMenu(FALSE);

	ProcUIInitEx(&procSaveCallback, NULL);

	ProcUIRegisterCallback(PROCUI_CALLBACK_HOME_BUTTON_DENIED,
						   &procHomeButtonDenied, NULL, 100);
}

/*
Note:
Need to find out proper quitting for channels/.wuhbs without requiring the Home Button Menu
*/

void
ProcShutdown()
{
	isRunning = FALSE;

	if (fromHBL)
	{
		SYSRelaunchTitle(0, NULL);
	}
}

bool
ProcIsRunning()
{
	ProcUIStatus status;
	
	status = ProcUIProcessMessages(TRUE);
	if (status == PROCUI_STATUS_EXITING) {
		isRunning = FALSE;
	}
	else if (status == PROCUI_STATUS_RELEASE_FOREGROUND) {
		ProcUIDrawDoneRelease();
	}
	
	if (!isRunning)
	{
		ProcUIShutdown();
	}

	return isRunning;
}