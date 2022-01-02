# NoSSL patch for the WiiU FW version 5.5.5

5.5.5 updated a verification callback in IOS-NSEC, so the function we were patching was shifted a bit.

This code is the patch basically (it's a physical memory write):
```C
*(uint32_t*)(0xE1019F78 - 0xE1000000 + 0x12BC0000) = 0xE3A00001;
```

It's patching:

```C
// OpenSSL 1.0.0f 4 Jan 2012
// @ E1019F50
int ssl_verify_cert_chain(SSL *s,STACK_OF(X509) *sk);
```

***Note:*** The patches from [Inkay](https://github.com/PretendoNetwork/Inkay) are ***not*** included in this version of Nimble.

## Building

### Makefile
To build the rpx, make sure you have `devkitPro`, `devkitPPC`, `devkitARM`, `wut` and `vim` or `vim-common` (Depending on what OS you're running) installed via the dkp-pacman package manager. Run `make` in the same directory as `Makefile` and you should get the file `nimble_patcher.rpx`. For aroma users you can just use `nimble_patcher.wuhb`.

