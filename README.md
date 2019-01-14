# Pretendo Network Installer/Patcher (for the WiiU)

[![N|Solid](https://camo.githubusercontent.com/b98196c2347796d66fa669e8510749665ec80ba0/68747470733a2f2f692e696d6775722e636f6d2f5839346237574b2e706e67)](https://pretendo.cc)



This is the official Pretendo Installer for the Wii U. It will patch your console's IOSU (ARM microkernel) temporarily (goes away after a reboot) to modify Nintendo URLs to Pretendo URLs.

# Features
- Pretty lightweight
- Contains vocabulary that might not be understood
- Uses magic to do what it does

# Dependencies
- [dynamic_libs](https://github.com/Maschell/dynamic_libs/)
- [libutils](https://github.com/Maschell/libutils/)
- devkitPro (PPC and ARM)

You need to set **devkitPro\msys\bin** into your PATH for **xxd.exe** usage.
  
# Credits
- Pretendo Network team
- NexoCube for making the installer
- dimok789 for the infinite loading patch
- Kinnay for the No-SSL patch

# TODO

- Add an option to run it with all URL patches w/out SSL Patch
- Add an option to run it with all URL patches w/ SSL Patch
- Add an option to run it with no URL patches w/ SSL patch
- Make a working auto-updater (old-one supported HTTPS & Proxies but couldn't write to the SD Card)