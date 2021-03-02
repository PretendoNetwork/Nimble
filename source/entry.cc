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

	/* Do the kernel exploit */
	WiiU::IOSU_Kernel::Exploit();

	return RETURN_TO_HBL;
}
