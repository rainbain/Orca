# Orca Boot
Orca Boot is a part of Orca Wii/GameCube emulator that initalizes the system on boot.

It generates a simple bin file and u-boot script, that is included withen the boot partition, where it can initalize Flipper and call the kernel once it is safe to boot.

Once graphics are implmented, it can also display a splash screen during boot.

## How To Build
You will need a few dependencys. I will assume the system is running Debian or Ubuntu here.
1. `u-boot-tools` to create uboot scripts using mkimage.
2. `gcc-aarch64-linux-gnu` to compile AARCH64 ARM C code and assembly.
3. `cmake` to auto-generate makefiles.
4. `make` to run those make files.

Those can be installed with:

`sudo apt-get install make cmake u-boot-tools gcc-aarch64-linux-gnu cmake make`

Once that is done you can create a build folder withen the project directory,
configure the cmake project, and build it.

```
mkdir build
cd build
cmake ..
make
```

Once this is done you need to copy the following files from your build folder to the SD card.
1. `boot.bin` This is the Orca Boot binary.
2. `boot.scr` This is the uboot script that contains the instructions to load the binary.