#ifndef __PTC_H
#define __PTC_H

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

#include <sysapp/launch.h>

#include <vpad/input.h>

#include <nn/swkbd.h>

#include <proc_ui/procui.h>

void Patch_IOSU_URLs(BOOL URLs, BOOL SSL, char *domain);

#define IOS_NIM_BA_TO_VA(x) (int)((x - 0xE0000000) + 0x10EC0000)
#define IOS_FPD_BA_TO_VA(x) (int)((x - 0xE3000000) + 0x13640000)

typedef struct URL_Patch
{

	int address;
	char url[80];

} URL_Patch;

static  const URL_Patch url_patches[25] = {

	{IOS_NIM_BA_TO_VA(0xE2282550), "http://pushmore.wup.shop.ABCD/pushmore/r/%s"},
	{IOS_NIM_BA_TO_VA(0xE229A0A0), "http://npns-dev.c.app.ABCD/bst.dat"},
	{IOS_NIM_BA_TO_VA(0xE229A0D0), "http://npns-dev.c.app.ABCD/bst2.dat"},
	{IOS_NIM_BA_TO_VA(0xE2281964), "http://tagaya.wup.shop.ABCD/tagaya/versionlist/%s/%s/%s"},
	{IOS_NIM_BA_TO_VA(0xE22819B4), "http://tagaya.wup.shop.ABCD/tagaya/versionlist/%s/%s/latest_version"},
	{IOS_NIM_BA_TO_VA(0xE2282584), "http://pushmo.wup.shop.ABCD/pushmo/d/%s/%u"},
	{IOS_NIM_BA_TO_VA(0xE22825B8), "http://pushmo.wup.shop.ABCD/pushmo/c/%u/%u"},
	{IOS_NIM_BA_TO_VA(0xE2282DB4), "http://ecs.wup.shop.ABCD/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830A0), "http://ecs.wup.shop.ABCD/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830E0), "http://nus.wup.shop.ABCD/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE2299990), "nppl.app.ABCD"},
	{IOS_NIM_BA_TO_VA(0xE229A600), "http://pls.wup.shop.ABCD/pls/upload"},
	{IOS_NIM_BA_TO_VA(0xE229A6AC), "http://npvk-dev.app.ABCD/reports"},
	{IOS_NIM_BA_TO_VA(0xE229A6D8), "http://npvk.app.ABCD/reports"},
	{IOS_NIM_BA_TO_VA(0xE229B1F4), "http://npts.app.ABCD/p01/tasksheet/%s/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE229B238), "http://npts.app.ABCD/p01/tasksheet/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE22AB2D8), "http://idbe-wup.cdn.ABCD/icondata/%02X/%016llX.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22AB358), "http://idbe-wup.cdn.ABCD/icondata/%02X/%016llX-%d.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22B3EF8), "http://ecs.c.shop.ABCD"},
	{IOS_NIM_BA_TO_VA(0xE22B3F30), "http://ecs.c.shop.ABCD/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3F70), "http://ias.c.shop.ABCD/ias/services/IdentityAuthenticationSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FBC), "http://cas.c.shop.ABCD/cas/services/CatalogingSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FFC), "http://nus.c.shop.ABCD/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE229DE0C), "n.app.ABCD"},
	{IOS_FPD_BA_TO_VA(0xE31930D4), "https://%s%saccount.ABCD/v%u/api/"}


};

#endif