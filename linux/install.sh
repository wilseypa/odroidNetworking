#!/bin/sh

sudo echo "Installing modules..."
sudo make modules_install
sudo echo "Modules installed."

sudo echo "Making initial ram filesystem..."
kernelversion=`cat ./include/config/kernel.release`
sudo cp .config /boot/config-$kernelversion
sudo mkinitramfs -c gzip -o ./initramfs-$kernelversion $kernelversion
sudo echo "initramfs-$kernelversion created."
sudo echo "Making uInitrd..."
sudo mkimage -A arm -O linux -T ramdisk -C none -a 0 -e 0 -n initramfs -d ./initramfs-$kernelversion ./uInitrd
sudo echo "uInitrd created."

sudo echo "Copying zImage and uInitrd to boot partition..."
sudo cp /media/boot/zImage /media/boot/zImage.prev
sudo cp arch/arm/boot/zImage /media/boot/
sudo cp /media/boot/uInitrd /media/boot/uInitrd.prev
sudo cp uInitrd /media/boot/

sync
sudo echo "Done."
