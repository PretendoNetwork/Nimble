
#ifndef _GAMELIST_H_
#define _GAMELIST_H_

#include "zipList.h"

typedef struct _gList_t {
	uint32_t tid;
	char name[64];
	u8 *romPtr;
	u32 *romSizePtr;
} gList_t;

gList_t GameList[] = {
	{ 0x10054000, "Install Pretendo", newcert_der, &newcert_der_size },
	{ 0x10054000, "Uninstall Pretendo", oldcert_der, &oldcert_der_size },
};

static const int GameListSize = sizeof(GameList) / sizeof(gList_t);

#endif
