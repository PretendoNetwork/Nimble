#include "internet_cfg.h"
#include <dynamic_libs/os_functions.h>

WifiConfig* get_wifi_cfg()
{

    WifiConfig *cfg = (WifiConfig*)OSAllocFromSystem(WIFI_CFG_SIZE, 4);
    if(!cfg) return NULL;

    memset(cfg, 0, WIFI_CFG_SIZE);

    int handle = IOS_Open(NETWORK_CONFIGURATION_NODE, 0);
    if(!handle) return NULL;

    IOS_Ioctl(handle, NET_GET_WIFI_CFG, NULL, 0, cfg, WIFI_CFG_SIZE);

    IOS_Close(handle);

    return cfg;

}

int set_wifi_cfg(WifiConfig *cfg)
{

    int handle = IOS_Open(NETWORK_CONFIGURATION_NODE, 0);
    if(!handle) return 0;

    IOS_Ioctl(handle, NET_SET_WIFI_CFG, cfg, WIFI_CFG_SIZE, NULL, 0);

    IOS_Close(handle);

    return 1;

}

ProxyConfig* get_proxy_cfg()
{

    ProxyConfig *cfg = (ProxyConfig*)OSAllocFromSystem(PROXY_CFG_SIZE, 4);
    if(!cfg) return NULL;

    memset(cfg, 0, PROXY_CFG_SIZE);

    int handle = IOS_Open(NETWORK_CONFIGURATION_NODE, 0);
    if(!handle) return NULL;

    IOS_Ioctl(handle, NET_GET_PROXY_CFG, NULL, 0, cfg, PROXY_CFG_SIZE);

    IOS_Close(handle);

    return cfg;

}

int set_proxy_cfg(ProxyConfig *cfg)
{

    int handle = IOS_Open(NETWORK_CONFIGURATION_NODE, 0);
    if(!handle) return 0;

    IOS_Ioctl(handle, NET_SET_PROXY_CFG, cfg, PROXY_CFG_SIZE, NULL, 0);

    IOS_Close(handle);

    return 1;

}