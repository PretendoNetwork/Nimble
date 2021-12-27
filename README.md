# RPX port of Nimble

This port makes the Nimble patcher an rpx.

***Note:*** The patches from [Inkay](https://github.com/PretendoNetwork/Inkay) are ***not*** included in this version of Nimble.

## Building


### RPX
To build the rpx, make sure you have `devkitPro`, `devkitPPC`, `wut` and `vim` or `vim-common` (Depending on what OS you're running) installed via the dkp-pacman package manager. Run `make` in the same directory as `Makefile` and you should get the file `nimble_patcher.rpx`.

### Channel

To create the channel version follow the same steps above to get `nimble_patcher.rpx`. Once done go to the folder named `code` inside the channel directory and place your rpx inside. Download NUSPacker.jar from [here](https://bitbucket.org/timogus/nuspacker/src/master/) and place it next to `channel` and create a folder named `WUP` in that same directory, afterwards run the following command in a terminal or cmd window (Replace the zeros with the Wii U common key)

```shell
java -jar NUSPacker.jar -in channel -out WUP -encryptKeyWith 00000000000000000000000000000000
```

Afterwards you may place the contents from the `WUP` folder into the `install` folder on your SD card and then install via WUP Installer.

### WUHB (Aroma)

To build the wuhb, just follow the rpx building instructions but instead of running `make` run `make -f Makefile.Aroma`. You should get the file `nimble_patcher_aroma.wuhb`.
