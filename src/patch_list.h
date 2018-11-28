#ifndef _PATCH_LIST_H_
#define _PATCH_LIST_H_

#include <gctypes.h>

#define IOS_NIM_BA_TO_VA(x) (int)((x - 0xE0000000) + 0x10EC0000)
#define IOS_FPD_BA_TO_VA(x) (int)((x - 0xE3000000) + 0x13640000)

typedef struct URL_Patch
{

	int address;
	char url[80];

} URL_Patch;

static URL_Patch patches[] = {

	{IOS_NIM_BA_TO_VA(0xE2282550), "http://pushmore.wup.shop.pretendo.cc/pushmore/r/%s"},
	{IOS_NIM_BA_TO_VA(0xE229A0A0), "http://npns-dev.c.app.pretendo.cc/bst.dat"},
	{IOS_NIM_BA_TO_VA(0xE229A0D0), "http://npns-dev.c.app.pretendo.cc/bst2.dat"},
	{IOS_NIM_BA_TO_VA(0xE2281964), "http://tagaya.wup.shop.pretendo.cc/tagaya/versionlist/%s/%s/%s"},
	{IOS_NIM_BA_TO_VA(0xE22819B4), "http://tagaya.wup.shop.pretendo.cc/tagaya/versionlist/%s/%s/latest_version"},
	{IOS_NIM_BA_TO_VA(0xE2282584), "http://pushmo.wup.shop.pretendo.cc/pushmo/d/%s/%u"},
	{IOS_NIM_BA_TO_VA(0xE22825B8), "http://pushmo.wup.shop.pretendo.cc/pushmo/c/%u/%u"},
	{IOS_NIM_BA_TO_VA(0xE2282DB4), "http://ecs.wup.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830A0), "http://ecs.wup.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22830E0), "http://nus.wup.shop.pretendo.cc/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE2299990), "nppl.app.pretendo.cc"},
	{IOS_NIM_BA_TO_VA(0xE229A600), "http://pls.wup.shop.pretendo.cc/pls/upload"},
	{IOS_NIM_BA_TO_VA(0xE229A6AC), "http://npvk-dev.app.pretendo.cc/reports"},
	{IOS_NIM_BA_TO_VA(0xE229A6D8), "http://npvk.app.pretendo.cc/reports"},
	{IOS_NIM_BA_TO_VA(0xE229B1F4), "http://npts.app.pretendo.cc/p01/tasksheet/%s/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE229B238), "http://npts.app.pretendo.cc/p01/tasksheet/%s/%s/%s?c=%s&l=%s"},
	{IOS_NIM_BA_TO_VA(0xE22AB2D8), "http://idbe-wup.cdn.pretendo.cc/icondata/%02X/%016llX.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22AB358), "http://idbe-wup.cdn.pretendo.cc/icondata/%02X/%016llX-%d.idbe"},
	{IOS_NIM_BA_TO_VA(0xE22B3EF8), "http://ecs.c.shop.pretendo.cc"},
	{IOS_NIM_BA_TO_VA(0xE22B3F30), "http://ecs.c.shop.pretendo.cc/ecs/services/ECommerceSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3F70), "http://ias.c.shop.pretendo.cc/ias/services/IdentityAuthenticationSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FBC), "http://cas.c.shop.pretendo.cc/cas/services/CatalogingSOAP"},
	{IOS_NIM_BA_TO_VA(0xE22B3FFC), "http://nus.c.shop.pretendo.cc/nus/services/NetUpdateSOAP"},
	{IOS_NIM_BA_TO_VA(0xE229DE0C), "n.app.pretendo.cc"},
	{IOS_FPD_BA_TO_VA(0xE31930D4), "https://%s%saccount.pretendo.cc/v%u/api/"}


};

#endif