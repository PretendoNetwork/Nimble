#include "wiiu/symbols.h"
#include "wiiu/debugger.h"
#include "wiiu/iosu_kernel.h"
#include "wiiu/patcher.h"


extern "C" void kern_write(void *addr, uint32_t value);
extern "C" int __entry(int argc, char **argv)
{

	/* Initialize symbols and debugger */
	WiiU::Symbols::LoadWiiUSymbols();
	WiiU::Debugger::Start();

	bool are_we_on_555 = false;
	const char* coreinit_build = (const char*)0x100011A0;
	if(!strcmp(coreinit_build, "Feb  4 2021")) {
		are_we_on_555 = true;
	}

	/* Do the kernel exploit */
	WiiU::IOSU_Kernel::Exploit(are_we_on_555);

	/* If we're still alive, everything went well */
	OSScreenInit();
	OSScreenEnableEx(0, 1);
	OSScreenEnableEx(1, 1);

	uint8_t* scBuffer = (uint8_t*)0xF4000000;
	uint32_t tvSize = OSScreenGetBufferSizeEx(0);

	OSScreenSetBufferEx(0, scBuffer);
	OSScreenSetBufferEx(1, scBuffer + tvSize);

	OSScreenClearBufferEx(0, 0);
	OSScreenClearBufferEx(1, 0);

	OSScreenPutFontEx(0, 0, 0, "NoSSL patch applied");
	OSScreenPutFontEx(0, 0, 1, "URLs redirected to Pretendo Network");
	OSScreenPutFontEx(0, 0, 2, "Exiting to loader in 3s");
	OSScreenPutFontEx(0, 0, 4, "Restart your WiiU to remove patches");

	OSScreenPutFontEx(1, 0, 0, "NoSSL patch applied");
	OSScreenPutFontEx(1, 0, 1, "URLs redirected to Pretendo Network");
	OSScreenPutFontEx(1, 0, 2, "Exiting to loader in 3s");
	OSScreenPutFontEx(1, 0, 4, "Restart your WiiU to remove patches");

	OSScreenFlipBuffersEx(0); OSScreenFlipBuffersEx(1);

	OSSleepTicks(OSSecondsToTicks(3));

	return RETURN_TO_HBL;
}
