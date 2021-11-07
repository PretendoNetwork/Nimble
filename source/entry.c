#include "symbols.h"
#include "debugger.h"
#include "iosu_kernel.h"
#include "screen.h"

#include "title_patcher.h"
#include "elf_abi.h"

extern void SC_KernelCopyData(void* dst, void* src, size_t size);

uint32_t get_section(uint8_t *data, char *name, uint32_t *size, uint32_t *addr)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)data;
	Elf32_Shdr *shdr = (Elf32_Shdr *) (data + ehdr->e_shoff);

	for(int i = 0; i < ehdr->e_shnum; i++) {
		const char *section_name = ((const char*)data) + shdr[ehdr->e_shstrndx].sh_offset + shdr[i].sh_name;
		if(strcmp(section_name, (const char*)name) == 0) {
			if(addr)
				*addr = shdr[i].sh_addr;
			if(size)
				*size = shdr[i].sh_size;
			return shdr[i].sh_offset;
		}
	}
	return 0;
}

uint8_t* install_section(uint8_t *data, char *name) {
	uint32_t section_addr = 0;
	uint32_t section_len = 0;
	uint32_t section_offset = get_section(data, name, &section_len, &section_addr);
	if(section_offset > 0) {
		uint8_t *section = data + section_offset;
		printf("Installing section %16s @ 0x%08x\n", name, section_addr);
		SC_KernelCopyData((void*)(0xC0000000 + section_addr), section, section_len);
		ICInvalidateRange((void*)section_addr, section_len);
	}

	if(!strcmp(name, ".bss") || !strcmp(name, ".sbss")) {
		memset((void*)section_addr, 0, section_len);
	}

	return (uint8_t*)section_addr;
}

extern void KernelCopyData(uint32_t dst, uint32_t src, size_t size);
extern void kern_write(void *addr, uint32_t value);
int __entry(int argc, char **argv)
{

	/* Initialize symbols and debugger */
	LoadWiiUSymbols();
	Debugger_Start();

	printf("Starting UDP logger\n");

	bool are_we_on_555 = false;
	const char* coreinit_build = (const char*)0x100011A0; // coreinit static .rodata
	if(!strcmp(coreinit_build, "Feb  4 2021")) {
		are_we_on_555 = true;
	}

	Screen_Initialize();

	int err = -1;
	VPADData vpad_data;
	while(true) {

		VPADRead(0, &vpad_data, 1, &err);
		if(vpad_data.btns_d & VPAD_BUTTON_B) {
			Screen_Destroy();
			return 0;
		} else if(vpad_data.btns_d & VPAD_BUTTON_A) {
			break;
		}

		Screen_StartFrame();

		Screen_DrawText("Welcome to the Nimble patcher, provided by the Pretendo team");
		Screen_DrawText("The patcher will redirect Nintendo services to Pretendo");
		Screen_DrawText("It will also apply per-game patches, you DO NOT have to use Inkay");
		Screen_DrawText("Press A to install patches (NoSSL, per game patches etc..)");
		Screen_DrawText("Press B to exit");

		Screen_EndFrame();

	}
	
	Screen_Destroy();
	IOSU_Kernel_Exploit(are_we_on_555, NULL);

	kern_write((void*)(0xFFE84C70 + (0x25 * 4)), (uint32_t)KernelCopyData);
	kern_write((void*)(0xFFE85070 + (0x25 * 4)), (uint32_t)KernelCopyData);
	kern_write((void*)(0xFFEAAA60 + (0x25 * 4)), (uint32_t)KernelCopyData);
	kern_write((void*)(0xFFE85470 + (0x25 * 4)), (uint32_t)KernelCopyData);
	kern_write((void*)(0xFFEAAE60 + (0x25 * 4)), (uint32_t)KernelCopyData);

	Elf32_Ehdr* ehdr = (Elf32_Ehdr*)title_patcher_elf;
	install_section((uint8_t*)title_patcher_elf, ".text");
	install_section((uint8_t*)title_patcher_elf, ".rodata");
	install_section((uint8_t*)title_patcher_elf, ".data");
	install_section((uint8_t*)title_patcher_elf, ".bss");

	printf("Installed hook\n");

	uint32_t inst = 0x48000003 | (ehdr->e_entry & 0x03FFFFFC);
	SC_KernelCopyData((void*)(0xC1000000 + 0x0101c544), &inst, sizeof(inst));
	ICInvalidateRange((void*)0x0101c544, sizeof(inst));

	printf("Returning to next app\n");

	return 0;

}
