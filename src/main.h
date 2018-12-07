//Main.h
#ifndef _MAIN_H_
#define _MAIN_H_

#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/os_defs.h>

#define PRETENDO_PATCHER_VERSION_URL "https://raw.githubusercontent.com/PretendoNetwork/Pretendo-Network-Installer-Wii-U-/master/version.txt"
#define PRETENDO_PATCHER_URL "https://raw.githubusercontent.com/PretendoNetwork/Pretendo-Network-Installer-Wii-U-/master/PretendoInstaller.elf"
#define PRETENDO_PATCHER_FILEPATH "sd:/wiiu/apps/PretendoInstaller/PretendoInstaller.elf"
#define PRETENDO_PATCHER_FOLDER "sd:/wiiu/apps/PretendoInstaller/"

/* Main */
#ifdef __cplusplus
extern "C" {
#endif

//! C wrapper for our C++ functions
int Menu_Main(void);
int dl_file(const char* url);
int write_data(void *buffer, int size, int nmemb, void *userp);

#ifdef __cplusplus
}
#endif

#endif
