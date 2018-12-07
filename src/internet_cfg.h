#ifndef ICFG_H
#define ICFG_H

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/* Main */
#ifdef __cplusplus
extern "C" {
#endif


typedef struct WifiConfig
{
    char unk0[0x4-0x0];
    char ssid[0x20];
    uint16_t ssid_len;
    char unk26[0x2c-0x26];
    uint16_t key_len;
    char key[0x40];
    char unk6e[0x70-0x6e];
} WifiConfig;

typedef struct ProxyConfig
{
    uint16_t using_proxy; // 0x0000 or 0x0001
    uint16_t port;
    int using_auth; // not sure about that
    char ip[0x80];
    char username[0x80];
    char password[0x80];
    char unknown[0x40];
} ProxyConfig;

#define PROXY_CFG_SIZE 0x1c8
#define WIFI_CFG_SIZE  0x70


#define NET_GET_WIFI_CFG 0x0C
#define NET_SET_WIFI_CFG 0x11

#define NET_GET_PROXY_CFG 0x26
#define NET_SET_PROXY_CFG 0x25

#define NETWORK_CONFIGURATION_NODE (char*)"/dev/net/ifmgr/ncl"

WifiConfig* get_wifi_cfg();
int set_wifi_cfg(WifiConfig *cfg);
ProxyConfig* get_proxy_cfg();
int set_proxy_cfg(ProxyConfig *cfg);

#ifdef __cplusplus
}
#endif

#endif