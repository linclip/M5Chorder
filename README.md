# M5Chorder
BLE Chord Pad App

This is a M5Stack app which provides functions to play chords via Bluetooth Low Energy MIDI.

## Note
Currently it works only when connected to iOS.  

It uses 1.3M byte of program memory.  
If you use M5Stack 4MB-flash model, change partition to Huge App (3MB No OTA).

Contributors welcome!

# How to Install

## Using SDUpdater(or LovyanLauncher) (recommended)
1. Install [SDUpdater](https://github.com/tobozo/M5Stack-SD-Updater) or [LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher) into your M5Stack.
2. Prepare a SD Card and copy files as the following.
3. Insert the SD into the M5Stack and execute.

## Using PlatformIO
1. Install Visual Studio Code.
2. Install PlatformIO.
3. Clone the repository;
4. Open repository folder in VSCode and run "PlatformIO: Upload".

## Using esptool (untested)
1. Install esptool (or download M5Burner and use esptool.py in it).
2. Download M5Chorder_1.0.0_bin.zip from [Releases](releases/).
3. Run the following.

``` sh
# In case of 16MB-flash model and default 16MB partition
esptool.py --chip esp32 --port /dev/tty.SLAB_USBtoUART --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_freq 40m 0x10000 M5Chorder.bin
```