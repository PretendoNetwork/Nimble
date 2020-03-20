# Pretendo Network Installer/Patcher (for the WiiU)

[![N|Solid](https://camo.githubusercontent.com/b98196c2347796d66fa669e8510749665ec80ba0/68747470733a2f2f692e696d6775722e636f6d2f5839346237574b2e706e67)](https://pretendo.cc)

This is the official Pretendo Installer for the Wii U. It will patch your console's IOSU (ARM microkernel) temporarily (goes away after a reboot) to modify Nintendo URLs to Pretendo URLs.

# Features
- You can see your Serial Number, DeviceId, Factory build date
- URL patching / SSL patch
- change the domain name (up to 12 characters only)

# Dependencies
- [**wut**](https://github.com/devkitPro/wut/) 
- **devkitPro** (PPC only, ARM code is already built)

You need to set **devkitPro\msys\bin** into your PATH
You also need the latest wut version from the github, the one hosted (updated from pacman commands) isn't supported
  
# Credits
- Pretendo Network team
- Rambo6Glaz for making the installer
- Kinnay for the No-SSL patch

# TODO

- Non carcinogenic update detector
