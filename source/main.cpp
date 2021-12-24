#include "patch/iosu_kernel.h"

#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/title.h>
#include <sysapp/launch.h>
#include <vpad/input.h>
#include <sndcore2/core.h>

#include "proc.hpp"
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>
#include <whb/gfx.h>

#define COLOR_CONSTANT 0x632A5CFF

#define HBL_TITLE_ID (0x0005000013374842)
#define MII_MAKER_JPN_TITLE_ID (0x000500101004A000)
#define MII_MAKER_USA_TITLE_ID (0x000500101004A100)
#define MII_MAKER_EUR_TITLE_ID (0x000500101004A200)

int main(int argc, char* args[])
{
    ProcInit();
    
    WHBLogUdpInit();
    WHBLogConsoleInit();

    WHBLogConsoleSetColor(COLOR_CONSTANT);

    WHBLogPrint("Welcome to the Nimble patcher, provided by the Pretendo team");
    WHBLogPrint("The patcher will redirect Nintendo services to Pretendo");
    WHBLogPrint("Press A to install patches (NoSSL etc..)");
    WHBLogPrint("Press B to exit");

    WHBLogConsoleDraw();

    VPADStatus status;
    VPADReadError error = VPAD_READ_SUCCESS;

    uint64_t titleID = OSGetTitleID();

    bool isChannel = true;

    if (titleID == HBL_TITLE_ID ||
        titleID == MII_MAKER_JPN_TITLE_ID ||
        titleID == MII_MAKER_USA_TITLE_ID ||
        titleID == MII_MAKER_EUR_TITLE_ID)
    {
        isChannel = false;
    }

    if (isChannel)
    {
        WHBLogPrintf("Is channel");
        AXInit(); //I hate this, when aroma comes out it can be forgotten thooooo
    }
    else
    {
        WHBLogPrintf("Is not channel");
    }

    while (ProcIsRunning())
    {
        VPADRead(VPAD_CHAN_0, &status, 1, &error);
        
        if (status.trigger & VPAD_BUTTON_A)
        {
            auto r = IOSU_Kernel_Exploit();
            if (r == 0)
            {
                WHBLogPrintf("Nimble patches succeeded!");

                OSSleepTicks(OSTicksToMilliseconds(1500));
                if (isChannel)
                {
                    WHBLogConsoleFree();
                    WHBGfxInit(); //I hate this too
                    SYSLaunchMenu();
                }
                else
                {
                    break;
                }
            }
            else
            {
                WHBLogPrintf("Nimble patches failed.");

                OSSleepTicks(OSTicksToMilliseconds(1500));
                if (isChannel)
                {
                    WHBLogConsoleFree();
                    WHBGfxInit();
                    SYSLaunchMenu();
                }
                else
                {
                    break;
                }
            }
        }
        else if (status.trigger & VPAD_BUTTON_B)
        {
            if (isChannel)
            {
                WHBLogConsoleFree();
                WHBGfxInit(); 
                SYSLaunchMenu();
            }
            else
            {
                break;
            }
        }
    }

    if (!isChannel)
    {
        WHBLogConsoleFree();
    }
    else
    {
        AXQuit();
        WHBGfxShutdown();
    }
    WHBLogUdpDeinit();
    ProcShutdown();

    return 0;
}