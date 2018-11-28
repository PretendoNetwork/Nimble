//Main.c
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>
#include <dynamic_libs/gx2_functions.h>
#include <dynamic_libs/sys_functions.h>
#include <dynamic_libs/vpad_functions.h>
#include <dynamic_libs/socket_functions.h>
#include <libutils/system/memory.h>
#include <libutils/utils/logger.h>
#include <libutils/utils/utils.h>
#include <dynamic_libs/os_defs.h>
#include "main.h"
#include "patch_list.h"
#include "exploit.h"
#include "screen.h"

const char *installer_version = "v1.0 ALPHA";
#define EXIT_RELAUNCH_ON_LOAD 0xFFFFFFFD

static int already_done = 0;

int Menu_Main(void)
{


   InitOSFunctionPointers();
   InitSysFunctionPointers();
   InitSocketFunctionPointers();
   InitFSFunctionPointers();

   /* *** Avoid the infinite loading loop ... (with the done flag) *** */

   uint64_t tid = OSGetTitleID();
   if(tid != 0x000500101004A000 + 0x000 && tid != 0x000500101004A000 + 0x100 && tid != 0x000500101004A000 + 0x200)
   {
      return EXIT_RELAUNCH_ON_LOAD;
   }
   else if(already_done)
   {
      return 0;
   }

	InitOSFunctionPointers();
	InitSysFunctionPointers();
	InitSocketFunctionPointers();
   InitFSFunctionPointers();
   InitVPadFunctionPointers();

   SYSLaunchMenu();

   initScreen();
   printf_("Welcome to the Pretendo Installer %s", installer_version);
   printf_("This installer will patch your Console temporarily.");
   printf_("It will modify Nintendo URLs to Pretendo ones.");
   printf_("");
   printf_("Press any button to continue.");

   s32 vpadError = -1;
   VPADData vpad;

   while(1)
   {
      VPADRead(0, &vpad, 1, &vpadError);
      if(!vpadError && (vpad.btns_d | vpad.btns_h)) 
      {
         break;
      }
   }

   clearScreen();

   printf_("Loading patches into RAM...");

   /* *** OSEffectiveToPhysical(0x10000000) = 0x50000000 *** */
   *(int*)0x1080B000 = sizeof(patches) / sizeof(URL_Patch); 
   void *patch_mem = (void*)0x1080C000;
   
   /* *** memcpy() and flush our patches *** */
   memcpy(patch_mem, patches, sizeof(patches));
   DCFlushRange(patch_mem - 0x1000, sizeof(patches) + 0x1000);

   os_sleep(1);

   printf_("Done.");
   printf_("Performing Kernel Exploit and patching IOSU...");

   // TODO: Actual Error Handling because IOS_Open can fail sometimes /shrug
   IOSU_Kernel_Exploit();
   os_sleep(1);

   printf_("Done.");
   printf_("Exiting ...");
   os_sleep(2);

   /* *** set the done flag and exit *** */
   deinitScreen();
   already_done++;
   return EXIT_RELAUNCH_ON_LOAD;
}

