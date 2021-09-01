#ifndef _PATCHER_H
#define _PATCHER_H

#include "types.h"

#define IOS_NIM_BA_TO_VA(x) (int)((x - 0xE0000000) + 0x10EC0000)
#define IOS_FPD_BA_TO_VA(x) (int)((x - 0xE3000000) + 0x13640000)

typedef struct URL_Patch
{
	int address;
	char url[80];
} URL_Patch;

static  const URL_Patch url_patches[] = {
	//nim-boss .rodata
	{IOS_NIM_BA_TO_VA(0xE2282550), "http://pushmore.wup.shop.pretendo.cc/pushmore/r/%s"},
	{IOS_NIM_BA_TO_VA(0xE229A0A0), "http://npns-dev.c.app.pretendo.cc/bst.dat"},
	{IOS_NIM_BA_TO_VA(0xE229A0D0), "http://npns-dev.c.app.pretendo.cc/bst2.dat"},
	{IOS_NIM_BA_TO_VA(0xE2281964), "https://tagaya.wup.shop.pretendo.cc/tagaya/versionlist/%s/%s/%s"},
	{IOS_NIM_BA_TO_VA(0xE22819B4), "https://tagaya.wup.shop.pretendo.cc/tagaya/versionlist/%s/%s/latest_version"},
	{IOS_NIM_BA_TO_VA(0xE2282584), "http://pushmo.wup.shop.pretendo.cc/pushmo/d/%s/%u"},
	{IOS_NIM_BA_TO_VA(0xE22825B8), "http://pushmo.wup.shop.pretendo.cc/pushmo/c/%u/%u"},
	{IOS_NIM_BA_TO_VA(0xE2282DB4), "https://ecs.wup.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830A0), "https://ecs.wup.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830E0), "https://nus.wup.shop.pretendo.cc/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE2299990), "nppl.app.pretendo.cc"},
	{IOS_NIM_BA_TO_VA(0xE229A600), "https://pls.wup.shop.pretendo.cc/pls/upload"},
	{IOS_NIM_BA_TO_VA(0xE229A6AC), "https://npvk-dev.app.pretendo.cc/reports"},
	{IOS_NIM_BA_TO_VA(0xE229A6D8), "https://npvk.app.pretendo.cc/reports"},
	{IOS_NIM_BA_TO_VA(0xE229B1F4), "https://npts.app.pretendo.cc/p01/tasksheet/%s/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE229B238), "https://npts.app.pretendo.cc/p01/tasksheet/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE22AB2D8), "https://idbe-wup.cdn.pretendo.cc/icondata/%02X/%016llX.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22AB318), "https://idbe-ctr.cdn.pretendo.cc/icondata/%02X/%016llX.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22AB358), "https://idbe-wup.cdn.pretendo.cc/icondata/%02X/%016llX-%d.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22AB398), "https://idbe-ctr.cdn.pretendo.cc/icondata/%02X/%016llX-%d.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22B3EF8), "https://ecs.c.shop.pretendo.cc"},
	{IOS_NIM_BA_TO_VA(0xE22B3F30), "https://ecs.c.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3F70), "https://ias.c.shop.pretendo.cc/ias/services/IdentityAuthenticationSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FBC), "https://cas.c.shop.pretendo.cc/cas/services/CatalogingSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FFC), "https://nus.c.shop.pretendo.cc/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE229DE0C), "n.app.pretendo.cc"},
	//nim-boss .bss
	{IOS_NIM_BA_TO_VA(0xE24B8A24), "https://nppl.app.pretendo.cc/p01/policylist/1/1/UNK"}, //bit of a hack
	{IOS_FPD_BA_TO_VA(0xE31930D4), "https://%s%saccount.pretendo.cc/v%u/api/"}

};

#endif
