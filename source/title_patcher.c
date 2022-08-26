#include "title_patcher.h"

#include <coreinit/cache.h>
#include <string.h>
#include <stdint.h>

#include "syscalls.h"
#include "elf_abi.h"
#include "title_patcher.elf.h"

/**
 * Installs an ELF by reading the program headers. Must be EM_PPC.
 * @param elf Pointer to the ELF in memory.
 * @return ELF entrypoint on success, 0 on a bad elf.
 */
static uint32_t install_elf(void* elf) {
    Elf32_Ehdr* ehdr = elf;

    // Check for a sensible ELF
    const char ident[16] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x02, 0x01, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (memcmp(ehdr->e_ident, ident, sizeof(ident)) != 0)
        return 0;

    if (ehdr->e_machine != EM_PPC)
        return 0;

    if (ehdr->e_phnum == 0)
        return 0;

    if (ehdr->e_phentsize != 32)
        return 0;

    // Parse and load program headers
    const void* phdrs = elf + ehdr->e_phoff;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        const Elf32_Phdr* phdr = phdrs + ehdr->e_phentsize * i;

        if (phdr->p_type == PT_LOAD) {
            void* va = (void*)phdr->p_vaddr;
            memcpy(va, elf + phdr->p_offset, phdr->p_filesz);
            // filesz < memsz when a BSS is present, so zero it
            memset(va + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
            DCFlushRange(va, phdr->p_memsz);
            ICInvalidateRange(va, phdr->p_memsz);
        }
    }

    return ehdr->e_entry;
}

/**
 * Installs the embedded title patcher ELF and adds the coreinit hook.
 * @return 0 on success, -1 on invalid ELF
 */
int install_title_patcher() {
    uint32_t entry = install_elf((void*)title_patcher_elf);
    if (!entry) return -1;

    // Install coreinit hook
    uint32_t inst = 0x48000003 | (entry & 0x03FFFFFC);
    KernelWriteU32(0x0101c544, inst);

    return 0;
}
