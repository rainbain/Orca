# Software
Contained here is the software that must run on the FPGA's embedded hardware. Creating a bootable SD card with the required software contains many steps, and will not be fully documented here, but this should contain all the information needed to get a system running.

Here I have only included the Vivado XSA file needed to generate the project, as the Vivado project is not yet ready to be packaged.

## Setting Up
Setting up the software requires building petalinux, building OrcaBoot, preparing a SD card and its rootfs, and building Orca on the booted system.

I have not had time to test these instructions yet, it is recommended to use Ubuntu as the build system, as petalinux can be a bit odd. I recommend doing it on a fresh system due to this.

### Building petalinux
Orca uses the petalinx kernel from Xilinx to create the kernel image needed to run the system. Here we will create a whole petalinux build, including the lengthy rootfs build. Saddly I am so far unsure how to only build the kernel and device tree, as that is all we will be needing from it.

#### Install Petalinux
Follow the guide from xilinx here `https://docs.xilinx.com/r/en-US/ug1144-petalinux-tools-reference-guide/Installation-Steps` where one will need to download the installer and install its dependencys.

You bash, not dash, when following these sets as petalinux only supports bash. It will be easyest to follow these setups from the Software folder.

1. Setup a petalinux enviroment with its setting script: `source <PETALINUX INSTALL DIR>/settings.sh` This will also display any missing dependencys or errors.
2. Download the KR260 board support package. That is on xilinx's download page at `https://www.xilinx.com/support/download.html`
3. Create the project with `petalinux-create -s <KRIA_KR_260_BSP>` and change directory into the created project folder.
4. Configure the project with the Vivado hardware config `petalinux-config --get-hw-description ../` The directoy `../` is where it will look for the XSA file. From the config you will want to check `Image Packing Configuration` and make the root filesystem type is `EXT4` and that the device node is `/dev/sda2`. From there you can save and exit letting it finish creating all the config files needed.
5. Configure the kernel for UIO with `petalinux-config -c kernel`. Navigate to Device Drivers -> Userspace I/O drivers and enable `Userspace I/O platform driver with generic IRQ handling`, `Userspace platform driver with generic irq and dynamic memory`, and `Xilinx AI Engine driver`. They should display `<*>` when enabled. At this point save and exit.
6. Copy the device tree in the Software directory to `<petalinux-project>/project-spec/meta-user/recipes-bsp/device-tree/files/`
7. Build the project `petalinux-build`. This will take a long time, I recommend getting some coffie and check for errors every once in a while.

At this point your project should contain `images/linux/system.dtb` and `images/linux/Image` that we will put on the SD card lator.

#### Build OrcaBoot
Follow the settings from the OrcaBoot directorys `README.md` file.

#### Preparing the SD Card
Here we will format and create a SD card with the boot partition and rootfs from the files we just generated in previouse steps!

1. Find out what SD card directoy it is with `lsblk` Mine is /dev/sdb. Please don't format a drive you don't want to loose the contence of.
2. Run fdisk on the drive. `sudo fdisk /dev/sdb`. With this we will want to create 2 partitions. The first is a primary partition, number 1 default sector, and +256M in size to hold the kernel. Then make it a `W95 FAT32` type with the `t` command and hex code `b`. Then create the rootfs partition as a primary parition, number 2, default sector, and default size to use the remaining space on disk. Then write changes to disk and exit.
3. You can check your paritions are there with `sudo fdisk -l /dev/sdb`
4. Create the FAT file system on the boot partition. `sudo mkfs -t vfat -n BOOT /dev/sdb1`
5. Create the EXT4 root file system on the rootfs partition. `sudo mkfs -t ext4 -L ROOTFS /dev/sdb2`
6. Mount the boot partition, to in this case the `/mnt` folder. `sudo mount /dev/sdb1 /mnt`
7. Copy the base system. `sudo cp OrcaBoot/build/{boot.bin,boot.scr} <PETALINUX_PROJECT_FOLDER>/images/linux/{Image,system.dtb} design_1_wrapper.bin /mnt/`
8. Unmount and mount the rootfs. `sudo umount /mnt` then `sudo mount /dev/sdb2 /mnt`
9. At this point we will use `qemu-debootstrap` as compared to the traditional debian bootstrap as it finish up a few things on the system. Feel free to add or remove any packages here: `sudo qemu-debootstrap --arch=arm64 --include=build-essential,cmake,make,curl,htop,autotools-dev,git,sudo,net-tools,python3,nfs-common,openssh-server,tcpdump buster /mnt http://ftp.us.debian.org/debian`. I learned about this intresting method at (`http://lucasbrasilino.com/posts/Custom-debian-for-zynq-ultrascale+-mpsoc-platform/`)
10. Set the root password `sudo bash -c "echo 'root:orca' | chpasswd -c SHA512 -R /mnt"` in this case the root password will be `orca`. I recommend changing that.
11. Eject the SD card. `sudo eject /mnt/`

At this point you should be ready to boot. Put the SD card in the KR260, connect to its serial port at 115200 BPS, and power it on. If all goes well you will be able to log in with root.

#### Configuring the host system.
This will include the steps I always follow to setup a new system, you can follow whatever steps you need fit.
1. Manually configure the network interfaces.
2. Create a new user.
3. Add it to the sudoers.
4. Add that new user to be open over SSH.
5. Log into that new user.

#### Put Orca on that system.
At this point you can transfer the Orca folder to the host system, and follow its build instructions in its `README.MD` file. If all goes well you should see the fan turn down to a more appropriate speed.